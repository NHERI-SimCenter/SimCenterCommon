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
#include <unordered_map>

#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include <RandomVariablesContainer.h>

#include "JsonConfiguredWidget.h"

JsonConfiguredWidget::JsonConfiguredWidget(
    RandomVariablesContainer *random_variables const QString &configFile,
    QWidget *parent)
    : QWidget(parent),
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
  theConfigFileLayout->addWidget(configFileLabel);
  theConfigFileLayout->addWidget(theConfigFile);
  theConfigFileLayout->addWidget(chooseFile);
  connect(chooseFile, SIGNAL(clicked(bool)), this, SLOT(chooseConfigFile()));

  theStackedWidget = new QStackedWidget();
}

bool JsonConfiguredWidget::inputFromJSON(QJsonObject& rvObject) {}

bool JsonConfiguredWidget::outputToJSON(QJsonObject& rvObject) {}

void JsonConfiguredWidget::chooseConfigFile() {
  QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"), "C://", "All files (*.*)");
  theConfigFile->setText(fileName);

  // Initialize the widget based on chosen configuration
  initialize(fileName);
}

JsonWidget::Type JsonConfiguredWidget::getEnumIndex(const QString& inputString) const {
  static std::unordered_map<QString, JsonWidget::Type> stringToEnum{
      {"ComboBox", JsonWidget::Type::ComboBox},
      {"RVLineEdit", JsonWidget::Type::RVLineEdit},
      {"FileInput", JsonWidget::Type::FileInput},
      {"ForkComboBox", JsonWidget::Type::ForkComboBox}};

  auto enumValue = stringToEnum.find(inputString);

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
  
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QJsonDocument fileValues = QJsonDocument::fromJson(file.readAll().toUtf8());    
    file.close();
    inputConfig = fileValues.object();
  } else {
    qDebug() << "ERROR: JsonConfiguredWidget::initialize: Failed to open input "
                "configuration file!";
  }

  auto oldStackedWidget = theStackedWidget;
  theStackedWidget = generateStackedWidget(inputConfig);
  delete oldStackedWidget;

  // CONTINUE HERE WHEN DONE WITH WIDGET GENERATION FUNCTIONS
}

QStackedWidget* JsonConfiguredWidget::generateStackedWidget(
    const QJsonObject &inputObject) {

  auto stackedWidget = new QStackedWidget();
  
  try {
    switch (getEnumIndex(inputObject["type"])) {
    case JsonWidget::Type::ComboBox:
      stackedWidget->addWidget(generateComboBox(inputObject));
      return stackedWidget;

    case JsonWidget::Type::RVLineEdit:
      stackedWidget->addWidget(generateRVLineEdit(inputObject));
      return stackedWidget;
      
    case JsonWidget::Type::FileInput: {
      stackedWidget->addWidget(generageFileInput(inputObject));
      return stackedWidget;
    }
      
    case JsonWidget::Type::ForkComboBox:
      stackedWidget->addWidget(generateStackedWidget(inputObject));
      return stackedWidget;
    }
  } catch (const std::exception &e) {
    std::cout << "\nERROR: In JsonConfiguredWidget::generateStackedWidget: An "
                 "exception was caught while generating widgets based on the "
                 "input configuration file: "
              << e.what();
  }
}

SimCenterWidget *
JsonConfiguredWidget::generateComboBox(const QJsonValue &inputObject) const {
  return new SimCenterComboBox(inputObject);
}

SimCenterWidget *
JsonConfiguredWidget::generateRVLineEdit(const QJsonObject &inputObject) const {
  return new SimCenterLineEditRV(theRVInputWidget, inputObject);
}

SimCenterWidget *
JsonConfiguredWidget::generateFileInput(const QJsonObject &inputObject) const {
  return new SimCenterFileInput(inputObject);
}
