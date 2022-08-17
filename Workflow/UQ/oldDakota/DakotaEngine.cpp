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

#include <QStackedWidget>
#include <QComboBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>

#include <QDebug>

#include <DakotaInputSampling.h>
#include <DakotaInputReliability.h>
#include <DakotaInputSensitivity.h>
//#include <DakotaInputCalibration.h>
//#include <DakotaInputBayesianCalibration.h>

DakotaEngine::DakotaEngine(RandomVariablesContainer *theRVs, UQ_EngineType type, QWidget *parent)
: UQ_Engine(parent), theCurrentEngine(0)
{

    QVBoxLayout *layout = new QVBoxLayout();

    //
    // the selection part
    //

    QHBoxLayout *theSelectionLayout = new QHBoxLayout();
    QLabel *label = new QLabel();
    label->setText(QString("Dakota Method Catagory"));
    theEngineSelectionBox = new QComboBox();
    theEngineSelectionBox->addItem(tr("Forward Propagation"));
    if (type == ForwardReliabilitySensivity) {
        theEngineSelectionBox->addItem(tr("Sensitivity Analysis"));
        theEngineSelectionBox->addItem(tr("Reliability Analysis"));
    }
    theEngineSelectionBox->setMinimumWidth(600);

    theSelectionLayout->addWidget(label);
    theSelectionLayout->addWidget(theEngineSelectionBox);
    //    theSelectionLayout->addStretch();
    
    theSelectionLayout->addWidget(new QLabel("        Parallel Execution"));
    parallelCheckBox = new QCheckBox();
    parallelCheckBox->setChecked(true);
    theSelectionLayout->addWidget(parallelCheckBox);
    theSelectionLayout->addStretch();
    
    layout->addLayout(theSelectionLayout);

    //
    // create the stacked widget
    //

    theStackedWidget = new QStackedWidget();

    //
    // create the individual widgets add to stacked widget
    //

    theSamplingEngine = new DakotaInputSampling(theRVs);
    theReliabilityEngine = new DakotaInputReliability(theRVs);
    theSensitivityEngine = new DakotaInputSensitivity(theRVs);

    theStackedWidget->addWidget(theSamplingEngine);
    theStackedWidget->addWidget(theSensitivityEngine);
    theStackedWidget->addWidget(theReliabilityEngine);

    layout->addWidget(theStackedWidget);
    this->setLayout(layout);
    theCurrentEngine=theSamplingEngine;

    connect(theEngineSelectionBox, SIGNAL(currentIndexChanged(QString)), this,
          SLOT(engineSelectionChanged(QString)));

    theCurrentEngine = theSamplingEngine;
}

DakotaEngine::~DakotaEngine()
{

}


void DakotaEngine::engineSelectionChanged(const QString &arg1)
{
    UQ_Engine *theOldEngine = theCurrentEngine;

    if ((arg1 == QString("Sampling")) || (arg1 == QString("Forward Propagation"))) {
      theStackedWidget->setCurrentIndex(0);
      theCurrentEngine = theSamplingEngine;   
    } else if ((arg1 == QString("Reliability")) || (arg1 == QString("Reliability Analysis"))) {
      theStackedWidget->setCurrentIndex(2);
      theCurrentEngine = theReliabilityEngine;
    } else if ((arg1 == QString("Sensitivity")) || (arg1 == QString("Sensitivity Analysis"))) {
      theStackedWidget->setCurrentIndex(1);
      theCurrentEngine = theSensitivityEngine;
    } else {
      qDebug() << "ERROR .. DakotaEngine selection .. type unknown: " << arg1;
    }

    // emit signal if engine changed
    if (theCurrentEngine != theOldEngine)
        emit onUQ_EngineChanged();
}


int
DakotaEngine::getMaxNumParallelTasks(void) {
    return theCurrentEngine->getMaxNumParallelTasks();
}

bool
DakotaEngine::outputToJSON(QJsonObject &jsonObject) {

    jsonObject["uqType"] = theEngineSelectionBox->currentText();
    jsonObject["parallelExecution"]=parallelCheckBox->isChecked();
    
    return theCurrentEngine->outputToJSON(jsonObject);
}

bool
DakotaEngine::inputFromJSON(QJsonObject &jsonObject) {
    bool result = false;

    if (jsonObject.contains("uqType")) {
        QString selection = jsonObject["uqType"].toString();

        int index = theEngineSelectionBox->findText(selection);
        theEngineSelectionBox->setCurrentIndex(index);
        this->engineSelectionChanged(selection);

    } else {// for backward compatability .. forward was default
        theEngineSelectionBox->setCurrentIndex(0);
        this->engineSelectionChanged(tr("Sampling"));
    }


    bool doParallel = true;
    if (jsonObject.contains("parallelExecution"))
        doParallel = jsonObject["parallelExecution"].toBool();

    parallelCheckBox->setChecked(doParallel);
    
    if (theCurrentEngine != 0)
        result = theCurrentEngine->inputFromJSON(jsonObject);
    else
        result = false; // don't emit error as one should have been generated

    return result;
}

bool
DakotaEngine::outputAppDataToJSON(QJsonObject &jsonObject)
{
    jsonObject["Application"] = "Dakota-UQ1";
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

int
DakotaEngine::processResults(QString &filenameResults, QString &filenameTab) {
    return theCurrentEngine->processResults(filenameResults, filenameTab);
}

RandomVariablesContainer *
DakotaEngine::getParameters() {
    return theCurrentEngine->getParameters();
}

UQ_Results *
DakotaEngine::getResults(void) {
    return theCurrentEngine->getResults();
}

QString
DakotaEngine::getProcessingScript() {
    return QString("parseDAKOTA.py");
}
