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
#include <sectiontitle.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>


#include <QStackedWidget>
#include <MonteCarloInputWidget.h>
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
    samplingMethod->setMaximumWidth(200);
    samplingMethod->setMinimumWidth(200);

    methodLayout->addWidget(label1);
    methodLayout->addWidget(samplingMethod,2);
    methodLayout->addStretch(4);

    mLayout->addLayout(methodLayout);

    //
    // qstacked widget to hold all widgets
    //

    theStackedWidget = new QStackedWidget();

    theMC = new MonteCarloInputWidget();
    theStackedWidget->addWidget(theMC);
    theCurrentMethod = theMC;
    mLayout->addWidget(theStackedWidget);

    //
    // Import paired data
    //

    pairedRVLayout= new QHBoxLayout;
    pairedRVLayout->setMargin(0);
    pairedRVLayout->setAlignment(Qt::AlignTop);

    pairedRVLabel = new QLabel();
    pairedRVLabel->setText(QString("Resample RVs from correlated dataset"));
    pairedRVLabel->setStyleSheet("font-weight: bold; color: gray");
    pairedRVCheckBox = new QCheckBox();
    pairedRVLayout->addWidget(pairedRVCheckBox,0);
    pairedRVLayout->addWidget(pairedRVLabel,1);

    pairedRVLayoutWrap= new QWidget;
    QGridLayout *pairedRVLayout2= new QGridLayout(pairedRVLayoutWrap);

    QFrame * lineA = new QFrame;
    lineA->setFrameShape(QFrame::HLine);
    lineA->setFrameShadow(QFrame::Sunken);
    lineA->setMaximumWidth(300);
    pairedRVLayout2->addWidget(lineA,0,0,1,-1);

    pairedRVLayout2->setMargin(0);
    QLabel *label2 = new QLabel(QString("RV data groups"));
    RVdataList = new QLineEdit();
    RVdataList->setPlaceholderText("e.g. {RV_name1,RV_name2},{RV_name5,RV_name6,RV_name8}");
    RVdataList->setMaximumWidth(420);
    RVdataList->setMinimumWidth(420);
    pairedRVLayout2->addWidget(label2,1,0);
    pairedRVLayout2->addWidget(RVdataList,1,1);

    //pairedRVLayout2->setRowStretch(2,1);
    pairedRVLayout2->setColumnStretch(2,1);
    //pairedRVLayout2->setSpacing(0);
    pairedRVLayoutWrap ->setVisible(false);

    mLayout->addLayout(pairedRVLayout);
    mLayout->addWidget(pairedRVLayoutWrap);

    //
    // Set sensitivity group
    //

    groupRVLayout= new QHBoxLayout;
    groupRVLayout->setMargin(0);
    groupRVLayout->setAlignment(Qt::AlignTop);

    groupRVLabel = new QLabel(QString("Advanced option for global sensitivity analysis"));
    groupRVLabel->setStyleSheet("font-weight: bold; color: gray");
    groupRVCheckBox = new QCheckBox();
    groupRVLayout->addWidget(groupRVCheckBox,0);
    groupRVLayout->addWidget(groupRVLabel,1);

    groupRVLayoutWrap= new QWidget;
    QGridLayout *groupRVLayout2= new QGridLayout(groupRVLayoutWrap);

    QFrame * lineB = new QFrame;
    lineB->setFrameShape(QFrame::HLine);
    lineB->setFrameShadow(QFrame::Sunken);
    lineB->setMaximumWidth(300);
    groupRVLayout2->addWidget(lineB,0,0,1,-1);

    groupRVLayout2->setMargin(0);
    QLabel *label3 = new QLabel(QString("Get group Sobol indicies"));
    RVvarList = new QLineEdit();
    RVvarList->setPlaceholderText("e.g. {RV_name1},{RV_name1,RV_name2,RV_name6}");
    RVvarList->setMaximumWidth(420);
    RVvarList->setMinimumWidth(420);
    groupRVLayout2->addWidget(label3,1,0);
    groupRVLayout2->addWidget(RVvarList,1,1);
    groupRVLayout2->setRowStretch(3,1);
    groupRVLayout2->setColumnStretch(2,1);

    groupRVLayoutWrap ->setVisible(false);

    mLayout->addLayout(groupRVLayout);
    mLayout->addWidget(groupRVLayoutWrap);

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

    //connect(samplingMethod, SIGNAL(currentTextChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(pairedRVCheckBox,SIGNAL(toggled(bool)),this,SLOT(showDataOptions(bool)));
    connect(groupRVCheckBox,SIGNAL(toggled(bool)),this,SLOT(showAdvancedOptions(bool)));

}

void SimCenterUQInputSensitivity::onMethodChanged(QString text)
{
  if (text=="LHS") {
    //theStackedWidget->setCurrentIndex(0);
    //theCurrentMethod = theLHS;
  }
  else if (text=="Monte Carlo") {
    theStackedWidget->setCurrentIndex(0);
    theCurrentMethod = theMC;  
  }
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
    pairedRVCheckBox->setChecked(false);
    groupRVCheckBox->setChecked(false);
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

    if (pairedRVCheckBox->isChecked()) {
        jsonObject["RVdataGroup"] = RVdataList->text();
    } else {
        jsonObject["RVdataGroup"] = ""; // empty
    }

    if (groupRVCheckBox->isChecked()) {
        jsonObject["RVsensitivityGroup"] = RVvarList->text();
    } else {
        jsonObject["RVsensitivityGroup"] = ""; // empty
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
          result = theCurrentMethod->inputFromJSON(uq);
          if (result == false)
              return result;

      }
    }

   if (jsonObject.contains("RVdataGroup")) {
      RVdataList->setText(jsonObject["RVdataGroup"].toString());
      if ((RVdataList->text()).isEmpty()) {
          pairedRVCheckBox->setChecked(false);
      } else {
          pairedRVCheckBox->setChecked(true);
      }
  }

   if (jsonObject.contains("RVsensitivityGroup")) {
      RVvarList->setText(jsonObject["RVsensitivityGroup"].toString());
      if ((RVvarList->text()).isEmpty()) {
          groupRVCheckBox->setChecked(false);
      } else {
          groupRVCheckBox->setChecked(true);
      }
  }


  return result;
}


void SimCenterUQInputSensitivity::showDataOptions(bool tog)
{
    if (tog) {
        pairedRVLabel->setStyleSheet("font-weight: bold; color: black");
        RandomVariablesContainer *theRVs = RandomVariablesContainer::getInstance();
        RVdataList->setText(theRVs->getRVStringDatasetDiscrete());
        pairedRVLayoutWrap->setVisible(true);
    } else {
        pairedRVLabel->setStyleSheet("font-weight: bold; color: gray");
        RVdataList->setText("");
        pairedRVLayoutWrap->setVisible(false);
    }
}

void SimCenterUQInputSensitivity::showAdvancedOptions(bool tog)
{
    if (tog) {
        groupRVLabel->setStyleSheet("font-weight: bold; color: black");
        RandomVariablesContainer *theRVs = RandomVariablesContainer::getInstance();
        RVvarList->setText(theRVs->getAllRVString());
        groupRVLayoutWrap->setVisible(true);
    } else {
        groupRVLabel->setStyleSheet("font-weight: bold; color: gray");
        RVvarList->setText("");
        groupRVLayoutWrap->setVisible(false);
    }
}


int SimCenterUQInputSensitivity::processResults(QString &filenameResults, QString &filenameTab) {
    return 0;
}

UQ_Results *
SimCenterUQInputSensitivity::getResults(void) {
    return new SimCenterUQResultsSensitivity(RandomVariablesContainer::getInstance());
}

void
SimCenterUQInputSensitivity::setRV_Defaults(void) {
  RandomVariablesContainer *theRVs = RandomVariablesContainer::getInstance();
  QString classType("Uncertain");
  QString engineType("SimCenterUQ");  

  theRVs->setDefaults(engineType, classType, Normal);
}

QString
SimCenterUQInputSensitivity::getMethodName(void){
  return QString("sensitivity");
}
