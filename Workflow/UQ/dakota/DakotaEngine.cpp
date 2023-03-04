// Written: fmckenna

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

// Written: fmckenna

#include "DakotaEngine.h"
#include <DakotaResultsSampling.h>
#include <GoogleAnalytics.h>

#include <QStackedWidget>
#include <QComboBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QSpacerItem>

#include <QDebug>

#include <DakotaInputSampling.h>
#include <DakotaInputReliability.h>
#include <DakotaInputSensitivity.h>
#include <DakotaInputCalibration.h>
#include <DakotaInputBayesianCalibration.h>
#include <DakotaInputOptimization.h>
#include <QAbstractItemView>


DakotaEngine::DakotaEngine(UQ_EngineType type, QWidget *parent)
: UQ_Engine(parent), theCurrentEngine(0)
{

    QVBoxLayout *layout = new QVBoxLayout();

    bool doSensitivity = true;
    bool doReliability = true;
    bool doCalibration = true;        

    if (type == ForwardOnly) {
      doSensitivity = false;
      doReliability = false;
      doCalibration = false;              
    } else if (type == ForwardReliabilitySensitivity) {
      doCalibration = false;
    }
    
    //
    // the selection part
    //

    QHBoxLayout *theSelectionLayout = new QHBoxLayout();
    QLabel *label = new QLabel();
    label->setText(QString("Dakota Method Category"));
    theEngineSelectionBox = new QComboBox();
    theEngineSelectionBox->addItem(tr("Forward Propagation"));
    if (doReliability == true)
      theEngineSelectionBox->addItem(tr("Reliability Analysis"));
    if (doSensitivity == true)
      theEngineSelectionBox->addItem(tr("Sensitivity Analysis"));
    if (doCalibration == true) {
      //theEngineSelectionBox->addItem(tr("Parameters Estimation")); // Deterministic
      //theEngineSelectionBox->addItem(tr("Inverse Problem")); // Bayesian
      theEngineSelectionBox->addItem(tr("Deterministic Calibration"));
      theEngineSelectionBox->addItem(tr("Bayesian Calibration"));
      theEngineSelectionBox->addItem(tr("Optimization"));
    }
    
    //theEngineSelectionBox->setMinimumWidth(600);
    int width = theEngineSelectionBox->minimumSizeHint().width();
    theEngineSelectionBox->view()->setMinimumWidth(width);

    theSelectionLayout->addWidget(label);
    theSelectionLayout->addWidget(theEngineSelectionBox, 1);


    //theSelectionLayout->addStretch();
    //theSelectionLayout->addWidget(new QSpacerItem(20,5));
    parallelCheckBox = new QCheckBox("Parallel Execution  ");
    parallelCheckBox->setChecked(true);

    removeWorkdirCheckBox = new QCheckBox("Save Working dirs");
    removeWorkdirCheckBox->setChecked(true);
    theSelectionLayout->addWidget(parallelCheckBox);
    theSelectionLayout->addWidget(removeWorkdirCheckBox);
    theSelectionLayout->addStretch(2);


    layout->addLayout(theSelectionLayout);

    //
    // create the stacked widget
    //

    theStackedWidget = new QStackedWidget();

    //
    // create the individual widgets add to stacked widget
    //

    theSamplingEngine = new DakotaInputSampling();
    theReliabilityEngine = new DakotaInputReliability();
    theCalibrationEngine = new DakotaInputCalibration();
    theBayesianCalibrationEngine = new DakotaInputBayesianCalibration();
    theSensitivityEngine = new DakotaInputSensitivity();
    theOptimizationEngine = new DakotaInputOptimization();

    theStackedWidget->addWidget(theSamplingEngine);
    theStackedWidget->addWidget(theReliabilityEngine);
    theStackedWidget->addWidget(theSensitivityEngine);    
    theStackedWidget->addWidget(theCalibrationEngine);
    theStackedWidget->addWidget(theBayesianCalibrationEngine);
    theStackedWidget->addWidget(theOptimizationEngine);

    layout->addWidget(theStackedWidget);
    this->setLayout(layout);
    theCurrentEngine=theSamplingEngine;

    connect(theEngineSelectionBox, SIGNAL(currentTextChanged(QString)), this, SLOT(engineSelectionChanged(QString)));


    //connect(theEngineSelectionBox, &QComboBox::currentTextChanged, this,
    //      SLOT(engineSelectionChanged(QString&)));

    // connect(theSamplingEngine, SIGNAL(onNumModelsChanged(int)), this, SLOT(numModelsChanged(int)));

    theCurrentEngine = theSamplingEngine;
}

DakotaEngine::~DakotaEngine()
{

}


void DakotaEngine::engineSelectionChanged(const QString &arg1)
{
    // UQ_Engine *theOldEngine = theCurrentEngine;
  
    if ((arg1 == QString("Sampling")) || (arg1 == QString("Forward Propagation"))) {
      
      theStackedWidget->setCurrentIndex(0);
      theCurrentEngine = theSamplingEngine;
      
    } else if ((arg1 == QString("Reliability")) || (arg1 == QString("Reliability Analysis"))) {
      
      theStackedWidget->setCurrentIndex(1);
      theCurrentEngine = theReliabilityEngine;
      
    } else if ((arg1 == QString("Sensitivity")) || (arg1 == QString("Sensitivity Analysis"))) {
      
      theStackedWidget->setCurrentIndex(2);
      theCurrentEngine = theSensitivityEngine;
      
    } else if ((arg1 == QString("Calibration"))
               || (arg1 == QString("Parameters Estimation"))
               || (arg1 == QString("Parameter Estimation"))
               || (arg1 == QString("Deterministic Calibration"))) {

      theStackedWidget->setCurrentIndex(3);
      theCurrentEngine = theCalibrationEngine;
      
    } else if ((arg1 == QString("Bayesian Calibration")) || (arg1 == QString("Inverse Problem"))) {
      theStackedWidget->setCurrentIndex(4);
      theCurrentEngine = theBayesianCalibrationEngine;
      
    } else if ((arg1 == QString("Optimization"))) {
               theStackedWidget->setCurrentIndex(5);
               theCurrentEngine = theOptimizationEngine;
    } else {
      qDebug() << "ERROR .. DakotaEngine selection .. type unknown: " << arg1;
    }

    // emit signal if engine changed
    //if (theCurrentEngine != theOldEngine)
    emit onUQ_EngineChanged("Dakota");
}


int
DakotaEngine::getMaxNumParallelTasks(void) {
    return theCurrentEngine->getMaxNumParallelTasks();
}

bool
DakotaEngine::outputToJSON(QJsonObject &jsonObject) {
    //TODO4: change this when version number changes from 3 to 4
    QString uqMethod = theEngineSelectionBox->currentText();
    if (uqMethod == QString("Bayesian Calibration")) {
        uqMethod = QString("Inverse Problem");
    } else if (uqMethod == QString("Deterministic Calibration")) {
        uqMethod = QString("Parameters Estimation");
    }
    jsonObject["uqType"] = uqMethod;
    jsonObject["parallelExecution"]=parallelCheckBox->isChecked();
    jsonObject["saveWorkDir"]=removeWorkdirCheckBox->isChecked();

    return theCurrentEngine->outputToJSON(jsonObject);
}

bool
DakotaEngine::inputFromJSON(QJsonObject &jsonObject) {
    bool result = false;

    QString uqMethod = jsonObject["uqType"].toString();
    
    if (uqMethod == QString("Inverse Problem")) {
        uqMethod = QString("Bayesian Calibration");
    } else if (uqMethod == QString("Parameters Estimation")) {
        uqMethod = QString("Deterministic Calibration");
    } 

    //emit onUQ_methodUpdated(uqMethod);

    bool doParallel = true;
    if (jsonObject.contains("parallelExecution"))
        doParallel = jsonObject["parallelExecution"].toBool();

    parallelCheckBox->setChecked(doParallel);


    int index = theEngineSelectionBox->findText(uqMethod);
    
    theEngineSelectionBox->setCurrentIndex(index);
    
    this->engineSelectionChanged(uqMethod);
    if (theCurrentEngine != 0)
        result = theCurrentEngine->inputFromJSON(jsonObject);
    else 
        result = false; // don't emit error as one should have been generated

    return result;
}


bool
DakotaEngine::outputAppDataToJSON(QJsonObject &jsonObject)
{
    jsonObject["Application"] = "Dakota-UQ";
    QJsonObject dataObj;
    jsonObject["ApplicationData"] = dataObj;

    return true;
}

bool
DakotaEngine::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    Q_UNUSED(jsonObject);
    return true;
}

void
DakotaEngine::setRV_Defaults() {
    return theCurrentEngine->setRV_Defaults();
}

UQ_Results *
DakotaEngine::getResults(void) {
    return theCurrentEngine->getResults();
}

QString
DakotaEngine::getProcessingScript() {
    return QString("parseDAKOTA.py");
}

void
DakotaEngine::numModelsChanged(int newNum) {
    emit onNumModelsChanged(newNum);
}

QString
DakotaEngine::getMethodName() {
    return theCurrentEngine->getMethodName();
}

bool
DakotaEngine::fixMethod(QString Methodname) {
    int res = theEngineSelectionBox->findText(Methodname);
    if (res == -1) {
        return false;
    } else {
        theEngineSelectionBox->setCurrentIndex(res);
        theEngineSelectionBox->hide();
        return true;
    }
}
bool
DakotaEngine::copyFiles(QString &fileDir) {
    QString googleString=QString("UQ-DAKOTA-") + this->getMethodName();
    GoogleAnalytics::ReportAppUsage(googleString);

    return theCurrentEngine->copyFiles(fileDir);
}
