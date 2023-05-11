
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

#include "SimCenterUQInputPLoM.h"
#include "SimCenterUQResultsPLoM.h"
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
#include <PLoMInputWidget.h>
#include <PLoMSimuWidget.h>
//#include <InputWidgetEDP.h>

SimCenterUQInputPLoM::SimCenterUQInputPLoM(QWidget *parent)
    : UQ_Engine(parent),uqSpecific(0)
{

    layout = new QVBoxLayout();
    mLayout = new QVBoxLayout();

    //
    // create layout for input selection box
    //

    QHBoxLayout *methodLayout1= new QHBoxLayout;
    inpMethod = new QComboBox();
    inpMethod->addItem(tr("Import Data File"));
    inpMethod->addItem(tr("Sampling and Simulation"));
    //inpMethod->addItem(tr("Import Multi-fidelity Data File"));
    inpMethod->setMaximumWidth(250);
    inpMethod->setMinimumWidth(250);

    methodLayout1->addWidget(new QLabel("Training Dataset"));
    methodLayout1->addWidget(inpMethod,2);

    methodLayout1->addStretch();

    mLayout->addLayout(methodLayout1);

    // communicate with EVT panel to display corresponding items in theAffiliateVariableComboBox
    typeEVT = "None"; // default is none (for quoFEM)

    //
    // input selection widgets
    //

    theStackedWidget = new QStackedWidget();

    //theData = new PLoMInputWidget(theParameters,theFemWidget,theEdpWidget);
    theData = new PLoMInputWidget();
    theStackedWidget->addWidget(theData);
    theSimu = new PLoMSimuWidget();
    theStackedWidget->addWidget(theSimu);
    //theMultiFidelity = new SurrogateMFInputWidget(theParameters,theFemWidget,theEdpWidget);
    //theStackedWidget->addWidget(theMultiFidelity);

    theStackedWidget->setCurrentIndex(0);

    theInpCurrentMethod = theData;

    mLayout->addWidget(theStackedWidget);
    mLayout->addStretch();
    layout->addLayout(mLayout);

    this->setLayout(layout);

    connect(inpMethod, SIGNAL(currentTextChanged(QString)), this, SLOT(onIndexChanged(QString)));
    connect(theSimu, SIGNAL(queryEVT()), this, SLOT(relayQueryEVT()));
}

void SimCenterUQInputPLoM::onIndexChanged(const QString &text)
{
    if (text=="Import Data File") {
        theStackedWidget->setCurrentIndex(0);
        theInpCurrentMethod = theData;
    }

    else if (text=="Sampling and Simulation") {
        //theDoE = new SurrogateDoEInputWidget();
        //theStackedWidget->insertWidget(1,theSimu);
        theStackedWidget->setCurrentIndex(1);
        theInpCurrentMethod = theSimu;
        //theFemWidget->setFEMforGP("GPmodel"); // reset FEM (TODO: CHANGE TO PLOM)
    }
    /***
    else if (text=="Import Multi-fidelity Data File") {
        delete theMultiFidelity;
        theMultiFidelity = new SurrogateMFInputWidget(theParameters,theFemWidget,theEdpWidget);
        theStackedWidget->insertWidget(2,theMultiFidelity);

        theStackedWidget->setCurrentIndex(2);
        theInpCurrentMethod = theMultiFidelity;
        theFemWidget->setFEMforGP("GPdata");
    }
    ***/
    //theParameters->setGPVarNamesAndValues(QStringList({}));// remove GP RVs
}


SimCenterUQInputPLoM::~SimCenterUQInputPLoM()
{

}

int
SimCenterUQInputPLoM::getMaxNumParallelTasks(void){
  return theInpCurrentMethod->getNumberTasks();
}

void SimCenterUQInputPLoM::clear(void)
{

}

void SimCenterUQInputPLoM::numModelsChanged(int numModels) {
    emit onNumModelsChanged(numModels);
}

bool
SimCenterUQInputPLoM::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    QJsonObject uq;
    uq["method"]=inpMethod->currentText();
    theInpCurrentMethod->outputToJSON(uq);

    jsonObject["surrogateMethodInfo"]=uq;

    return result;
}


bool
SimCenterUQInputPLoM::inputFromJSON(QJsonObject &jsonObject)
{
  bool result = false;
  this->clear();

  //
  // get sampleingMethodData, if not present it's an error
  //

  if (jsonObject.contains("surrogateMethodInfo")) {
      QJsonObject uq = jsonObject["surrogateMethodInfo"].toObject();
      if (uq.contains("method")) {

          QString method =uq["method"].toString();
          int index = inpMethod->findText(method);
          if (index == -1) {
              return false;
          }
          inpMethod->setCurrentIndex(index);
          result = theInpCurrentMethod->inputFromJSON(uq);
          if (result == false)
              return result;

      }
  }

  return result;
}

bool
SimCenterUQInputPLoM::outputAppDataToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    jsonObject["Application"] = "SimCenterUQ-UQ";
    QJsonObject uq;
    uq["method"]=inpMethod->currentText();
    theInpCurrentMethod->outputToJSON(uq);
    jsonObject["ApplicationData"] = uq;

    return result;
}


bool
SimCenterUQInputPLoM::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    bool result = false;
    this->clear();
    //
    // get sampleingMethodData, if not present it's an error

    if (jsonObject.contains("ApplicationData")) {
        QJsonObject uq = jsonObject["ApplicationData"].toObject();

        if (uq.contains("method")) {
          QString method = uq["method"].toString();
          int index = inpMethod->findText(method);

          if (index == -1) {
              errorMessage(QString("ERROR: Unknown Method") + method);
              return false;
          }
          inpMethod->setCurrentIndex(index);
          return theInpCurrentMethod->inputFromJSON(uq);
        }

    } else {
        errorMessage("ERROR: Surrogate Input Widget - no \"surrogateMethodData\" input");
        return false;
    }

    return result;
}



int SimCenterUQInputPLoM::processResults(QString &filenameResults, QString &filenameTab) {
    Q_UNUSED(filenameResults);
    Q_UNUSED(filenameTab);

    return 0;
}

UQ_Results *
SimCenterUQInputPLoM::getResults(void) {
    qDebug() << "RETURNED SimCenterUQResultsPLoM";
    // KZ: need to get RandomVariablesContainer instance
    RandomVariablesContainer *theRandomVariables = RandomVariablesContainer::getInstance();
    return new SimCenterUQResultsPLoM(theRandomVariables);
}

/***
RandomVariablesContainer *
SimCenterUQInputPLoM::getParameters(void) {
  QString classType("Uncertain");
  theRandomVariables =  new RandomVariablesContainer(classType,tr("SimCenterUQ"));
  return theRandomVariables;
}
***/

void
SimCenterUQInputPLoM::setRV_Defaults(void) {

  RandomVariablesContainer *theRVs =  RandomVariablesContainer::getInstance();
  QString classType("Uncertain");
  QString engineType("SimCenterUQ");

  theRVs->setDefaults(engineType, classType, Normal);
}

bool
SimCenterUQInputPLoM::copyFiles(QString &fileDir) {
    return theInpCurrentMethod->copyFiles(fileDir);
}

QString
SimCenterUQInputPLoM::getMethodName(void){
    //if (inpMethod->currentIndex()==0){
    //    return QString("surrogateModel");
    //} else if (inpMethod->currentIndex()==1){
    //    return QString("surrogateData");
    //}
    return QString("PLoM");
}

void SimCenterUQInputPLoM::setEventType(QString type) {
    theSimu->setEventType(type);
}
