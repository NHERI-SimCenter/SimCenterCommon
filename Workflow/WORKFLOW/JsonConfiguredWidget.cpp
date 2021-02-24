/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS 
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written: Michael Gardner

#include <exception>
#include <iostream>
#include <string>
#include <unordered_map>

#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include <RandomVariablesContainer.h>

#include "JsonConfiguredWidget.h"

JsonConfiguredWidget::JsonConfiguredWidget(
    RandomVariablesContainer *random_variables, QWidget *parent)
    : SimCenterWidget(parent),
      theRVInputWidget(random_variables) {
  QVBoxLayout * layout = new QVBoxLayout();

  // Add file selection widget
  QHBoxLayout * theConfigFileLayout = new QHBoxLayout();
  QLabel * configFileLabel = new QLabel();
  configFileLabel->setText(tr("Configuration Input File"));
  theConfigFile = new QLineEdit();
  theConfigFile->setToolTip(tr("User-provided JSON file specifying input layout"));
  
  QPushButton * chooseFile = new QPushButton();
  chooseFile->setText(tr("Choose"));
  connect(chooseFile, &QPushButton::clicked, this, &JsonConfiguredWidget::chooseConfigFile);  
  
  theConfigFileLayout->addWidget(configFileLabel);
  theConfigFileLayout->addWidget(theConfigFile);
  theConfigFileLayout->addWidget(chooseFile);

  theWidget = new QWidget(); 

  layout->addLayout(theConfigFileLayout);
  layout->addWidget(theWidget);
  layout->addStretch();
  this->setLayout(layout);
}

bool JsonConfiguredWidget::inputFromJSON(QJsonObject& rvObject) {
  theConfigFile->setText(rvObject["Config File"].toString());

  auto oldWidget = theWidget;
  auto newWidget = generateWidgetLayout(rvObject["Parameters"].toArray(), true);
  
  this->layout()->replaceWidget(theWidget, newWidget);
  theWidget = newWidget;
  oldWidget->deleteLater();  

  return true;
}

bool JsonConfiguredWidget::outputToJSON(QJsonObject &rvObject) {
  // Output the stacked widget to JSON
  rvObject.insert("Config File", theConfigFile->text());
  rvObject.insert("Parameters", widgetToJson(theWidget));
  return true;
}

void JsonConfiguredWidget::chooseConfigFile() {
  QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"), "C://", "All files (*.*)");
  theConfigFile->setText(fileName);

  // Initialize the widget based on chosen configuration
  initialize(fileName);
}

JsonWidget::Type JsonConfiguredWidget::getEnumIndex(const QString& inputString) const {
  static std::unordered_map<std::string, JsonWidget::Type> stringToEnum{
      {"ComboBox", JsonWidget::Type::ComboBox},
      {"RVLineEdit", JsonWidget::Type::RVLineEdit},
      {"FileInput", JsonWidget::Type::FileInput},
      {"LineEdit", JsonWidget::Type::LineEdit},
      {"DoubleSpinBox", JsonWidget::Type::DoubleSpinBox},
      {"SpinBox", JsonWidget::Type::SpinBox}};

  auto enumValue = stringToEnum.find(inputString.toStdString());

  if (enumValue != stringToEnum.end()) {
    return enumValue->second;
  } else {
    qDebug() << "ERROR: JsonConfiguredWidget::getEnumIndex: Input string not "
                "recognized, check input JSON config file!";
    throw std::invalid_argument("ERROR: JsonConfiguredWidget::getEnumIndex: "
                                "Check input JSON configuration!");
  }
}

void JsonConfiguredWidget::initialize(const QString& configFile) {
  QFile configuration(configFile);
  QJsonObject inputConfig;
  
  if (configuration.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QJsonDocument fileValues = QJsonDocument::fromJson(configuration.readAll());    
    configuration.close();
    inputConfig = fileValues.object();
  } else {
    qDebug() << "ERROR: JsonConfiguredWidget::initialize: Failed to open input "
                "configuration file!";
  }

  auto oldWidget = theWidget;
  auto newWidget = generateWidgetLayout(inputConfig["Parameters"].toArray());
  
  this->layout()->replaceWidget(theWidget, newWidget);
  theWidget = newWidget;
  oldWidget->deleteLater();  
}

QWidget *
JsonConfiguredWidget::generateWidgetLayout(const QJsonArray &inputArray,
                                           bool set) const {

  auto generatedWidget = new QWidget();
  QVBoxLayout * layout = new QVBoxLayout();  

  for (auto const& value : inputArray) {
    switch (getEnumIndex(value["type"].toString())) {
    case JsonWidget::Type::ComboBox: {
      auto comboBox = generateComboBox(value);
      if (set) {
	auto jsonObject = value.toObject();	
	comboBox->inputFromJSON(jsonObject);
      }
      layout->addWidget(comboBox);
      break;      
    }

    case JsonWidget::Type::RVLineEdit: {
      auto rvLineEdit = generateRVLineEdit(value);
      if (set) {
	auto jsonObject = value.toObject();
	rvLineEdit->inputFromJSON(jsonObject);
      }
      layout->addWidget(rvLineEdit);
      break;      
    }

    case JsonWidget::Type::FileInput: {
      auto fileInput = generateFileInput(value);
      if (set) {
	auto jsonObject = value.toObject();
	fileInput->inputFromJSON(jsonObject);
      }
      layout->addWidget(fileInput);
      break;	
      }

    case JsonWidget::Type::LineEdit: {
      auto lineEdit = generateLineEdit(value);
      if (set) {
	auto jsonObject = value.toObject();
	lineEdit->inputFromJSON(jsonObject);
      }
      layout->addWidget(lineEdit);
      break;            
    }

    case JsonWidget::Type::DoubleSpinBox: {
      auto doubleSpinBox = generateDoubleSpinBox(value);
      if (set) {
	auto jsonObject = value.toObject();
	doubleSpinBox->inputFromJSON(jsonObject);
      }
      layout->addWidget(doubleSpinBox);
      break;            
    }

    case JsonWidget::Type::SpinBox: {
      auto spinBox = generateSpinBox(value);
      if (set) {
	auto jsonObject = value.toObject();
	spinBox->inputFromJSON(jsonObject);
      }
      layout->addWidget(spinBox);
      break;            
    }      
      
    default:
        break;
    }
  }

  layout->addStretch();
  generatedWidget->setLayout(layout);
  
  return generatedWidget;
}

SimCenterWidget *
JsonConfiguredWidget::generateComboBox(const QJsonValue &inputValue) const {
  return new SimCenterComboBox(inputValue);
}

SimCenterWidget *
JsonConfiguredWidget::generateRVLineEdit(const QJsonValue &inputValue) const {
  return new SimCenterRVLineEdit(theRVInputWidget, inputValue);
}

SimCenterWidget *
JsonConfiguredWidget::generateFileInput(const QJsonValue &inputValue) const {
  return new SimCenterFileInput(inputValue);
}

SimCenterWidget *
JsonConfiguredWidget::generateLineEdit(const QJsonValue &inputValue) const {
  return new SimCenterLineEdit(inputValue);
}

SimCenterWidget *
JsonConfiguredWidget::generateDoubleSpinBox(const QJsonValue &inputValue) const {
  return new SimCenterDoubleSpinBox(inputValue);
}

SimCenterWidget *
JsonConfiguredWidget::generateSpinBox(const QJsonValue &inputValue) const {
  return new SimCenterSpinBox(inputValue);
}

QJsonArray
JsonConfiguredWidget::widgetToJson(QWidget *inputWidget) {

  QJsonArray paramsArray;

  auto widgetList = inputWidget->findChildren<QWidget *>();

  for (auto& child : widgetList) {
    if (qobject_cast<SimCenterWidget *>(child)) {
      QJsonObject widgetJson;
      qobject_cast<SimCenterWidget *>(child)->outputToJSON(widgetJson);
      paramsArray.push_back(widgetJson);
    }
  }

  return paramsArray;
}
