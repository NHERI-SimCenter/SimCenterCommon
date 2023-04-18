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
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

#include "SimCenterUQInputSensitivity.h"
#include <SimCenterUQResultsSensitivity.h>
#include <RandomVariablesContainer.h>


#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>

#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>


#include <QStackedWidget>
#include <MonteCarloInputSimWidget.h>
#include <ImportSamplesWidget.h>

#include <LatinHypercubeInputWidget.h>

SimCenterUQInputSensitivity::SimCenterUQInputSensitivity(QWidget *parent)
: UQ_Engine(parent),uqSpecific(0)
{
    mLayout = new QVBoxLayout();

    //
    // create layout for selection box for method type to layout
    //

    QHBoxLayout *methodLayout= new QHBoxLayout;
    QLabel *label1 = new QLabel("Method");

    samplingMethod = new QComboBox();
    samplingMethod->addItem(tr("Monte Carlo"));
    samplingMethod->addItem(tr("Import Data Files"));
    samplingMethod->setMaximumWidth(200);
    samplingMethod->setMinimumWidth(200);

    toolTip = new QPushButton("?");
   // toolTip->setStyleSheet({ " background-color: white; border-style: solid; border-width:1px; border-radius:50px; border-color: red; max-width:10px;  max-height:10px; min-width:10px; min-height:10px;" });
    toolTip->setStyleSheet({ " border-style: solid; border-width:0px; border-radius:50px; max-width:10px;  max-height:10px; min-width:10px; min-height:10px;" });
    QLabel *toolTipString = new QLabel();
    connect(toolTip, &QPushButton::pressed, this, [=]()  {
        toolTipString ->setText("Import comma-separated (.csv), tab-separated (.txt), or binary files (.bin). The binary files should be float type and row-major order.");
    });
    connect(toolTip, &QPushButton::released, this, [=]()  {
        toolTipString ->setText("");
    });
    toolTip->hide();
    methodLayout->addWidget(label1);
    methodLayout->addWidget(samplingMethod,1);
    methodLayout->addWidget(toolTip,2);
    methodLayout->addWidget(toolTipString,3);
    methodLayout->addStretch(5);

    mLayout->addLayout(methodLayout);

    //
    // qstacked widget to hold all widgets
    //

    theStackedWidget = new QStackedWidget();

    theMC = new MonteCarloInputSimWidget();
    theStackedWidget->addWidget(theMC);

    theSamples = new ImportSamplesWidget();
    theStackedWidget->addWidget(theSamples);

    theCurrentMethod = theMC;
    mLayout->addWidget(theStackedWidget);





    //
    // Set sensitivity group
    //

    advancedLayout= new QHBoxLayout;
    //advancedLayout->setMargin(0);
    advancedLayout->setAlignment(Qt::AlignTop);

    advancedLabel = new QLabel(QString("Advanced option for global sensitivity analysis"));
    advancedLabel->setStyleSheet("font-weight: bold; color: gray");
    advancedCheckBox = new QCheckBox();
    advancedLayout->addWidget(advancedCheckBox,0);
    advancedLayout->addWidget(advancedLabel,1);

    advancedLayoutWrap= new QWidget;
    QGridLayout *advancedLayout2= new QGridLayout(advancedLayoutWrap);

    QFrame * lineB = new QFrame;
    lineB->setFrameShape(QFrame::HLine);
    lineB->setFrameShadow(QFrame::Sunken);
    lineB->setMaximumWidth(300);
    advancedLayout2->addWidget(lineB,0,0,1,-1);

    //advancedLayout2->setMargin(0);
    QLabel *label3 = new QLabel(QString("Get group Sobol indicies"));
    RVvarList = new QLineEdit();
    RVvarList->setPlaceholderText("(optional) e.g. {RV_name1},{RV_name1,RV_name2,RV_name6}");
    RVvarList->setMaximumWidth(420);
    RVvarList->setMinimumWidth(420);
    advancedLayout2->addWidget(label3,1,0);
    advancedLayout2->addWidget(RVvarList,1,1,1,2);
    //advancedLayout2->setRowStretch(3,1);
    //advancedLayout2->setColumnStretch(2,1);

    //advancedLayout2->setMargin(0);
    QLabel *label4 = new QLabel(QString("Perform PCA with QoI"));
    performPCA = new QComboBox();
    performPCA->addItem("Automatic");
    performPCA->addItem("Yes");
    performPCA->addItem("No");
    performPCA->setMinimumWidth( performPCA->minimumSizeHint().width()*2);
    performPCA->setMaximumWidth( performPCA->minimumSizeHint().width()*2);
    PCAautoText = new QLabel("(PCA is performed if the number of QoI is greater than 15)");
    PCAvarRatio = new QLineEdit();
    PCAvarRatio->setVisible(false);
    PCAvarRatio->setPlaceholderText("(default:0.99) Portion of variance explained");
    //PCAvarRatio->setMaximumWidth(420);
    //PCAvarRatio->setMinimumWidth(420);
    advancedLayout2->addWidget(label4,2,0);
    advancedLayout2->addWidget(performPCA,2,1);
    advancedLayout2->addWidget(PCAautoText,2,2);
    advancedLayout2->addWidget(PCAvarRatio,2,2);

    advancedLayout2->setRowStretch(4,1);
    advancedLayout2->setColumnStretch(3,1);

    advancedLayoutWrap ->setVisible(false);

    mLayout->addLayout(advancedLayout);
    mLayout->addWidget(advancedLayoutWrap);

    //void InputWidgetEDP::setDefaultGroup(bool tog)
    //{
    //    if (tog) {
    //        theGroupEdit->setDisabled(0);
    //	RandomVariablesContainer *theRVs=RandomVariablesContainer::getInstance();
    //        QStringList rvNames = theRVs->getRandomVariableNames();
    //        if (rvNames.count()>0) {
    //            QString rvNameString;
    //            for (QString eleName : rvNames)
    //            {
    //                rvNameString.push_back("{"+eleName+"},");
    //            }
    //            //int pos = rvNameString.lastIndexOf(QChar(','));
    //            rvNameString.truncate(rvNameString.lastIndexOf(QChar(',')));
    //            theGroupEdit->setText(rvNameString);
    //        }
    //    } else {
    //        theGroupEdit->setDisabled(1);
    //        theGroupEdit->setText("");
    //    }


    mLayout->addStretch(2);
    mLayout->setStretch(5,1);

    this->setLayout(mLayout);

    connect(samplingMethod, SIGNAL(currentTextChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(advancedCheckBox,SIGNAL(toggled(bool)),this,SLOT(showAdvancedOptions(bool)));

    connect(performPCA, &QComboBox::currentTextChanged, this, [=](QString selec)  {
        if (selec==QString("Yes"))
            PCAvarRatio -> setVisible(true);
        else
            PCAvarRatio->setVisible(false);

        if (selec==QString("Automatic"))
            PCAautoText -> setVisible(true);
        else
            PCAautoText->setVisible(false);
    });
}

void SimCenterUQInputSensitivity::onTextChanged(QString text)
{
    if (text=="Monte Carlo") {
      theStackedWidget->setCurrentIndex(0);
      theCurrentMethod = theMC;
      toolTip ->hide();
    }
    else if (text=="Import Data Files") {
      theStackedWidget->setCurrentIndex(1);
      theCurrentMethod = theSamples;
      toolTip ->show();
    }
    setRV_Defaults();
}



SimCenterUQInputSensitivity::~SimCenterUQInputSensitivity()
{

}

int 
SimCenterUQInputSensitivity::getMaxNumParallelTasks(void){
  return theCurrentMethod->getNumberTasks();
}

void SimCenterUQInputSensitivity::clear(void)
{
    theCurrentMethod->clear();
    advancedCheckBox->setChecked(false);
}

bool
SimCenterUQInputSensitivity::outputToJSON(QJsonObject &jsonObject)
{
    // testing
    bool result = true;

    QJsonObject uq;
    uq["method"]=samplingMethod->currentText();
    theCurrentMethod->outputToJSON(uq);

    jsonObject["samplingMethodData"]=uq;

    jsonObject["advancedOptions"] = advancedCheckBox->isChecked();
    if (advancedCheckBox->isChecked()) {
        jsonObject["RVsensitivityGroup"] = RVvarList->text();
        jsonObject["performPCA"] = performPCA->currentText();
        if (performPCA->currentText()=="Yes") {
            if (PCAvarRatio->text().isEmpty()){
                jsonObject["PCAvarianceRatio"] = 0.99;
            } else {
                jsonObject["PCAvarianceRatio"] = PCAvarRatio->text().toDouble();
            }
        } else if (performPCA->currentText()=="No") {
            jsonObject["PCAvarianceRatio"] = "N/A";
        } else {
            jsonObject["PCAvarianceRatio"] = 0.99;
        }

    } else {
        jsonObject["RVsensitivityGroup"] = ""; // empty
        jsonObject["performPCA"] = "Automatic"; // default
        jsonObject["PCAvarianceRatio"] = 0.99; // default
    }



    return result;
}


bool
SimCenterUQInputSensitivity::inputFromJSON(QJsonObject &jsonObject)
{
  bool result = false;
  this->clear();

  //
  // get sampleingMethodData, if not present it's an error
  //

  if (jsonObject.contains("samplingMethodData")) {
      QJsonObject uq = jsonObject["samplingMethodData"].toObject();
      if (uq.contains("method")) {

          QString method =uq["method"].toString();
          int index = samplingMethod->findText(method);
          if (index == -1) {
              return false;
          }
          samplingMethod->setCurrentIndex(index);

          //
          // For version competibility... This should be removed in the figure 08/12/2022
          //
          // ***
          if (jsonObject.contains("RVdataGroup")) {
             uq["RVdataGroup"] = jsonObject["RVdataGroup"].toObject();
          }

          result = theCurrentMethod->inputFromJSON(uq);
          if (result == false)
              return result;

      }
    }




   if (jsonObject.contains("advancedOptions")) {
      advancedCheckBox->setChecked(jsonObject["advancedOptions"].toBool());
  }

   if (jsonObject.contains("RVsensitivityGroup")) {
      RVvarList->setText(jsonObject["RVsensitivityGroup"].toString());
  }

   if (jsonObject.contains("performPCA")) {
      performPCA->setCurrentText(jsonObject["performPCA"].toString());
      if (performPCA->currentText()=="Yes") {
          PCAvarRatio->setText(QString::number(jsonObject["PCAvarianceRatio"].toDouble()));
      }
  }

  return result;
}


void SimCenterUQInputSensitivity::showAdvancedOptions(bool tog)
{
    if (tog) {
        advancedLabel->setStyleSheet("font-weight: bold; color: black");
        RandomVariablesContainer *theRVs = RandomVariablesContainer::getInstance();
        RVvarList->setText(theRVs->getAllRVString());
        advancedLayoutWrap->setVisible(true);
    } else {
        advancedLabel->setStyleSheet("font-weight: bold; color: gray");
        RVvarList->setText("");
        advancedLayoutWrap->setVisible(false);
    }
}


int SimCenterUQInputSensitivity::processResults(QString &filenameResults, QString &filenameTab) {
     Q_UNUSED(filenameResults);
    Q_UNUSED(filenameTab);
    return 0;
}

UQ_Results *
SimCenterUQInputSensitivity::getResults(void) {
    return new SimCenterUQResultsSensitivity(RandomVariablesContainer::getInstance());
}

void
SimCenterUQInputSensitivity::setRV_Defaults(void) {
  RandomVariablesContainer *theRVs = RandomVariablesContainer::getInstance();
  QString engineType("SimCenterUQ");
    QString classType;
  if (samplingMethod->currentText()=="Monte Carlo") {
      classType = "Uncertain";
      theRVs->setDefaults(engineType, classType, Normal);
  }
  else if (samplingMethod->currentText()=="Import Data Files") {
      classType = "NA";
      theRVs->setDefaults(engineType, classType, Uniform);
  }
}

QString
SimCenterUQInputSensitivity::getMethodName(void){
  return QString("sensitivity");
}
