
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

#include "SimCenterUQInputSurrogate.h"
#include "SimCenterUQResultsSurrogate.h"
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
#include <SurrogateDoEInputWidget.h>
#include <SurrogateNoDoEInputWidget.h>
#include <SurrogateMFInputWidget.h>
#include <SimCenterIntensityMeasureWidget.h>

SimCenterUQInputSurrogate::SimCenterUQInputSurrogate(QWidget *parent)
  : UQ_Engine(parent),uqSpecific(0)
{

    layout = new QVBoxLayout();
    mLayout = new QVBoxLayout();

    //
    // create layout for input selection box
    //

    QHBoxLayout *methodLayout1= new QHBoxLayout;
    inpMethod = new QComboBox();
    inpMethod->addItem(tr("Sampling and Simulation"));
    inpMethod->addItem(tr("Import Data File"));
    inpMethod->addItem(tr("Import Multi-fidelity Data File"));
    inpMethod->setMaximumWidth(250);
    inpMethod->setMinimumWidth(250);

    methodLayout1->addWidget(new QLabel("Training Dataset"));
    methodLayout1->addWidget(inpMethod,2);

    methodLayout1->addStretch();

    mLayout->addLayout(methodLayout1);

    //
    // input selection widgets
    //

    theStackedWidget = new QStackedWidget();

    theDoE = new SurrogateDoEInputWidget();
    theStackedWidget->addWidget(theDoE);
    //FMK    theData = new SurrogateNoDoEInputWidget(theParameters,theFemWidget,theEdpWidget);
    theData = new SurrogateNoDoEInputWidget();    
    theStackedWidget->addWidget(theData);
    //FMK    theMultiFidelity = new SurrogateMFInputWidget(theParameters,theFemWidget,theEdpWidget);
    theMultiFidelity = new SurrogateMFInputWidget();
    
    theStackedWidget->addWidget(theMultiFidelity);
    theStackedWidget->setCurrentIndex(0);
    theInpCurrentMethod = theDoE;

    mLayout->addWidget(theStackedWidget);

    //
    //
    //

    layout->addLayout(mLayout);

    this->setLayout(layout);

    connect(inpMethod, SIGNAL(currentTextChanged(QString)), this, SLOT(onIndexChanged(QString)));
    connect(this, SIGNAL(eventTypeChanged(QString)), this, SLOT(onEventTypeChanged(QString)));
}

void SimCenterUQInputSurrogate::onIndexChanged(const QString &text)
{
    if (text=="Sampling and Simulation") {
        theStackedWidget->setCurrentIndex(0);
        theInpCurrentMethod = theDoE;
	//FMK        theFemWidget->setFEMforGP("GPmodel"); // reset FEM
    }
    else if (text=="Import Data File") {
        delete theData;
        theData = new SurrogateNoDoEInputWidget();
        theStackedWidget->insertWidget(1,theData);
        theStackedWidget->setCurrentIndex(1);
        theInpCurrentMethod = theData;
    }
    else if (text=="Import Multi-fidelity Data File") {
        delete theMultiFidelity;
        theMultiFidelity = new SurrogateMFInputWidget();
        theStackedWidget->insertWidget(2,theMultiFidelity);

        theStackedWidget->setCurrentIndex(2);
        theInpCurrentMethod = theMultiFidelity;
        // FMK theFemWidget->setFEMforGP("GPdata");
    }
    setRV_Defaults();

    //theParameters->setGPVarNamesAndValues(QStringList({}));// remove GP RVs
}


SimCenterUQInputSurrogate::~SimCenterUQInputSurrogate()
{

}

int
SimCenterUQInputSurrogate::getMaxNumParallelTasks(void){
  return theInpCurrentMethod->getNumberTasks();
}

void SimCenterUQInputSurrogate::clear(void)
{
    inpMethod->setCurrentIndex(0);
    //theInpCurrentMethod->clear();
}

void SimCenterUQInputSurrogate::numModelsChanged(int numModels) {
    emit onNumModelsChanged(numModels);
}

bool
SimCenterUQInputSurrogate::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    QJsonObject uq;
    uq["method"]=inpMethod->currentText();
    theInpCurrentMethod->outputToJSON(uq);

    jsonObject["surrogateMethodInfo"]=uq;

    return result;
}


bool
SimCenterUQInputSurrogate::inputFromJSON(QJsonObject &jsonObject)
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
SimCenterUQInputSurrogate::outputAppDataToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    jsonObject["Application"] = "SimCenterUQ";
    QJsonObject uq;
    uq["method"]=inpMethod->currentText();
    theInpCurrentMethod->outputToJSON(uq);
    jsonObject["ApplicationData"] = uq;

    return result;
}


bool
SimCenterUQInputSurrogate::inputAppDataFromJSON(QJsonObject &jsonObject)
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


UQ_Results *
SimCenterUQInputSurrogate::getResults(void) {
    qDebug() << "RETURNED SimCenterUQRESULTSSURROGATE";
    return new SimCenterUQResultsSurrogate(RandomVariablesContainer::getInstance());
}

void
SimCenterUQInputSurrogate::setRV_Defaults(void) {
  
  theInpCurrentMethod->setRV_Defaults();
  //RandomVariablesContainer *theRVs =  RandomVariablesContainer::getInstance();
  //QString classType("Uniform");
  //QString engineType("SimCenterUQ");

  //theRVs->setDefaults(engineType, classType, Normal);
}

bool
SimCenterUQInputSurrogate::copyFiles(QString &fileDir) {
    return theInpCurrentMethod->copyFiles(fileDir);
}

QString
SimCenterUQInputSurrogate::getMethodName(void){
    //if (inpMethod->currentIndex()==0){
    //    return QString("surrogateModel");
    //} else if (inpMethod->currentIndex()==1){
    //    return QString("surrogateData");
    //}
    return QString("surrogate");
}

void SimCenterUQInputSurrogate::setEventType(QString type) {
    typeEVT = type;
    emit eventTypeChanged(typeEVT);
    theDoE->setEventType(typeEVT);
}


void
SimCenterUQInputSurrogate::onEventTypeChanged(QString typeEVT) {
    if (typeEVT.compare("EQ") ==0 ) {
        // an earthquake event type
        inpMethod->removeItem(1) ; //remove option "Import from data"
    } else {
        // not an earthquake event, inactivate ground motion intensity widget
    }
}


bool SimCenterUQInputSurrogate::outputCitation(QJsonObject &jsonObject)
{
    QJsonArray citations;

    QJsonObject paper1;
    paper1.insert("citation",QString("GPy. (2012-present). GPy: A Gaussian process framework in Python. Retrieved from http://github.com/SheffieldML/GPy."));
    paper1.insert("description",QString("Surrogate modeling functionality of quoFEM is built upon the GPy library, an open-source Python framework for Gaussian process modeling developed in the Sheffield machine learning group."));

    QJsonObject paper2;
    paper2.insert("citation",QString("Kyprioti, A.P., Zhang, J., and Taflanidis, A.A. (2020). Adaptive design of experiments for global Kriging metamodeling through cross-validation information. Structural and Multidisciplinary Optimization, 1-23."));
    paper2.insert("description",QString("If the user selected the IMSEw or MMSEw DoE Options under the 'Advanced Option for Gaussian Process Model' in the SimCenterUQ engine, the algorithm outlined in this paper is utilized."));

    QJsonObject paper3;
    paper3.insert("citation",QString("Yi, S.R. and Taflanidis, A.A., (2023). Computationally Efficient Adaptive Design of Experiments for Global Metamodeling through Integrated Error Approximation and Multicriteria Search Strategies. Journal of Engineering Mechanics, 149(8), p.04023050."));
    paper3.insert("description",QString("If the user selected the Pareto DoE Option under the 'Advanced Option for Gaussian Process Model' in the SimCenterUQ engine, the algorithm outlined in this paper is utilized."));

    QJsonObject paper4;
    paper4.insert("citation",QString("Kyprioti, A.P. and Taflanidis, A.A., (2021). Kriging metamodeling for seismic response distribution estimation. Earthquake Engineering & Structural Dynamics, 50(13), pp.3550-3576."));
    paper4.insert("description",QString("If the user selected the Heteroscedastic 'Nugget Variances' option under the 'Advanced Option for Gaussian Process Model' in the SimCenterUQ engine, the algorithm outlined in this paper is utilized."));

    citations.append(paper1);
    citations.append(paper2);
    citations.append(paper3);
    citations.append(paper4);

    jsonObject.insert("citations", citations);

    return true;
}
