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
// Additional edits: Michael Gardner

#include "SectionTitle.h"
#include "UQ_EngineSelection.h"
#include <GoogleAnalytics.h>
#include <NoneEngine.h>
#include <QJsonObject>
#include <QJsonArray>

#include <QDebug>
#include <DakotaEngine.h>
#include <SimCenterUQEngine.h>
#include <UCSD_Engine.h>
#include <UQpyEngine.h>
#include <UQ_JsonEngine.h>
#include <JsonConfiguredUQEngine.h>
#include <UQpyEngine.h>
#include <NoneEngine.h>

UQ_EngineSelection::UQ_EngineSelection(bool includeNone,
				       QString assetType,
				       UQ_EngineType type,
				       QWidget *parent)
  :SimCenterAppSelection(QString("UQ Application"), assetType, QString("UQ_Method"), QString(), parent),
    theCurrentEngine(0), includeNoneOption(includeNone), typeOption(type)
{
  this->initialize();
}

UQ_EngineSelection::UQ_EngineSelection(UQ_EngineType type,
				       QWidget *parent)
  :SimCenterAppSelection(QString("UQ Engine"), QString("UQ"), QString("UQ_Method"), QString(), parent),
    theCurrentEngine(0), includeNoneOption(false), typeOption(type)
{
  this->initialize();
}

void
UQ_EngineSelection::initialize()
{
    //
    // In fact, we select the method first.
    //

    SectionTitle * selectionText = new SectionTitle();
    selectionText->setMinimumWidth(250);
    selectionText->setText("UQ Method");

    theMethodCombo = new QComboBox();

    if (typeOption == ForwardOnly) {
        theMethodCombo->addItem("Forward Propagation");
    } else if (typeOption == ForwardSurrogate) {
        theMethodCombo->addItem("Forward Propagation");
        theMethodCombo->addItem("Surrogate Modeling");
    } else if (typeOption == ForwardReliabilitySensitivity) {
        theMethodCombo->addItem("Forward Propagation");
        theMethodCombo->addItem("Reliability Analysis");
        theMethodCombo->addItem("Sensitivity Analysis");
    } else if (typeOption == ForwardReliabilitySensitivitySurrogate) {
        theMethodCombo->addItem("Forward Propagation");
        theMethodCombo->addItem("Reliability Analysis");
        theMethodCombo->addItem("Sensitivity Analysis");
        theMethodCombo->addItem("Surrogate Modeling");
    } else  if (typeOption == All) {
        theMethodCombo->addItem("Forward Propagation");
        theMethodCombo->addItem("Reliability Analysis");
        theMethodCombo->addItem("Sensitivity Analysis");
        theMethodCombo->addItem("Deterministic Calibration");
        theMethodCombo->addItem("Bayesian Calibration");
        theMethodCombo->addItem("Optimization");
        theMethodCombo->addItem("Surrogate Modeling");
        theMethodCombo->addItem("CustomUQ");
    } else {
        theMethodCombo->addItem("Forward Propagation");
    }

    if (includeNoneOption) {
        theMethodCombo->addItem("None");
    } 
    

    theMethodCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    connect(theMethodCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(updateEngineComboDisp(QString)));

    QHBoxLayout * theMethodLayout = new QHBoxLayout();
    theMethodLayout -> addWidget(selectionText);
    theMethodLayout -> addWidget(theMethodCombo);
    theMethodLayout -> addStretch();

    auto *myLayout = dynamic_cast<QVBoxLayout*> (this->layout());
    myLayout->insertLayout(0,theMethodLayout);


    //
    // Then we select the engine
    //

    QLabel * engineLabel = new QLabel(QString("  UQ Engine"));
    theEngineComboDisp = new QComboBox();
    engineLabel->setMinimumWidth(200);
    theEngineComboDisp->setMinimumWidth(500);
    QHBoxLayout * theEngineLayout = new QHBoxLayout();
    theEngineLayout -> addWidget(engineLabel);
    theEngineLayout -> addWidget(theEngineComboDisp);
    theEngineLayout -> addStretch();
    myLayout->insertLayout(1,theEngineLayout);

    connect(theEngineComboDisp, &QComboBox::currentTextChanged, this, [=](QString engineName){
              this->selectComponent(engineName);
          });
    
    //connect(theEngineComboDisp, SIGNAL(currentTextChanged(QString)), this, SLOT(selectComponent(QString)));
    //connect(theEngineComboDisp, SIGNAL(currentTextChanged(QString)), this, SLOT(engineSelectionChanged(QString)));
    connect(this, SIGNAL(selectionChangedSignal(QString)), this, SLOT(engineSelectionChanged(QString)));
    //
    // This is the all the list of engines
    //

    this ->hideHeader();

    theDakotaEngine = new DakotaEngine(typeOption);
    theSimCenterUQEngine = new SimCenterUQEngine(typeOption);
    theCustomEngine = new UQ_JsonEngine(typeOption);
    theUCSD_Engine = new UCSD_Engine(typeOption);
    theUQpyEngine = new UQpyEngine(typeOption);

    this->addComponent(QString("Dakota"), QString("Dakota-UQ"), theDakotaEngine);
    this->addComponent(QString("SimCenterUQ"), QString("SimCenter-UQ"), theSimCenterUQEngine);
    this->addComponent(QString("CustomUQ"), QString("Custom-UQ"), theCustomEngine);
    
    if (includeNoneOption) {
      theNoneEngine = new NoneEngine(this);
      this->addComponent(QString("None"), QString("None"), theNoneEngine);
    } else
      theNoneEngine = 0;
    
    
    if (typeOption == All)
    {
      this->addComponent(QString("UCSD-UQ"), QString("UCSD-UQ"), theUCSD_Engine);
      //this->addComponent(QString("UQpy"), QString("UQpy"), theUQpyEngine);
    }


    theCurrentEngine=theDakotaEngine;

    connect(theDakotaEngine, SIGNAL(onUQ_MethodUpdated(QString)), this, SLOT(methodSelectionChanged(QString)));
    connect(theSimCenterUQEngine, SIGNAL(onUQ_MethodUpdated(QString)), this, SLOT(methodSelectionChanged(QString)));
    connect(theCustomEngine, SIGNAL(onUQ_MethodUpdated(QString)), this, SLOT(methodSelectionChanged(QString)));
    connect(theUCSD_Engine, SIGNAL(onUQ_MethodUpdated(QString)), this, SLOT(methodSelectionChanged(QString)));
    connect(theNoneEngine, SIGNAL(onUQ_MethodUpdated(QString)), this, SLOT(methodSelectionChanged(QString)));    

    connect(theDakotaEngine, SIGNAL(onUQ_EngineChanged(QString)), this, SLOT(engineSelectionChanged(QString)));
    connect(theSimCenterUQEngine, SIGNAL(onUQ_EngineChanged(QString)), this, SLOT(engineSelectionChanged(QString)));
    connect(theCustomEngine, SIGNAL(onUQ_EngineChanged(QString)), this, SLOT(engineSelectionChanged(QString)));
    connect(theUCSD_Engine, SIGNAL(onUQ_EngineChanged(QString)), this, SLOT(engineSelectionChanged(QString)));
    connect(theNoneEngine, SIGNAL(onUQ_EngineChanged(QString)), this, SLOT(engineSelectionChanged(QString)));    
    

//    connect(theDakotaEngine, SIGNAL(onUQ_MethodUpdated(QString)), theMethodCombo, SLOT(setCurrentText(QString)));
//    connect(theSimCenterUQEngine, SIGNAL(onUQ_MethodUpdated(QString)), theMethodCombo, SLOT(setCurrentText(QString)));
//    connect(theCustomEngine, SIGNAL(onUQ_MethodUpdated(QString)), theMethodCombo, SLOT(setCurrentText(QString)));
//    connect(theUCSD_Engine, SIGNAL(onUQ_MethodUpdated(QString)), theMethodCombo, SLOT(setCurrentText(QString)));

//    connect(theDakotaEngine, SIGNAL(onUQ_EngineChanged(QString)), this, SLOT(methodSelectionChanged(QString)));
//    connect(theSimCenterUQEngine, SIGNAL(onUQ_EngineChanged(QString)), this, SLOT(methodSelectionChanged(QString)));
//    connect(theCustomEngine, SIGNAL(onUQ_EngineChanged(QString)), this, SLOT(methodSelectionChanged(QString)));
//    connect(theUCSD_Engine, SIGNAL(onUQ_EngineChanged(QString)), this, SLOT(methodSelectionChanged(QString)));



//    connect(theCurrentEngine, SIGNAL(onUQ_MethodUpdated(QString)), theMethodCombo, SLOT(setCurrentText(QString)));
//    connect(theCurrentEngine, SIGNAL(onUQ_EngineChanged(QString)), theEngineComboDisp, SLOT(setCurrentText(QString)));

    // connect queryEVT
    connect(theSimCenterUQEngine, SIGNAL(queryEVT()), this, SLOT(relayQueryEVT()));

    this->updateEngineComboDisp();
}

void UQ_EngineSelection::methodSelectionChanged(QString MethodName) {
    theMethodCombo->setCurrentText(MethodName);
    theCurrentEngine->setRV_Defaults();
    //engineSelectionChanged(MethodName);
}

void UQ_EngineSelection::engineSelectionChanged(QString arg1)
{
    emit queryEVT();

    engineName =  arg1;
    if (arg1 == "Dakota" || arg1 == "Dakota-UQ") {
        theCurrentEngine = theDakotaEngine;
    } else if (arg1 == "SimCenterUQ" || arg1 == "SimCenterUQ-UQ") {
        theCurrentEngine = theSimCenterUQEngine;
    } else if (arg1 == "CustomUQ" || arg1 == "Custom-UQ") {
      theCurrentEngine = theCustomEngine;      
    } else if (arg1 == "UCSD-UQ") {
      theCurrentEngine = theUCSD_Engine;
    } else if (arg1 == "None") {
      theCurrentEngine = theNoneEngine;
    } else {
      qDebug() << "ERROR .. UQ_EngineSelection selection .. type unknown: " << arg1;
    }

    theCurrentEngine->setRV_Defaults();

    connect(theCurrentEngine,SIGNAL(onNumModelsChanged(int)), this, SLOT(numModelsChanged(int)));

    QString currentEngine = this->getCurrentComboName();
    int idx = theEngineComboDisp->findText(currentEngine);
    if (idx!=-1) {
        theEngineComboDisp->setCurrentIndex(idx);
    } else {
        theEngineComboDisp->setCurrentIndex(0);
    }

    /* FMK
    if (thePreviousEngine->getMethodName() == "surrogate")
    {
        theEDPs->setGPQoINames(QStringList({}) );// remove GP QoIs
        theRVs->setGPVarNamesAndValues(QStringList({}));// remove GP RVs
        theFemWidget->setFEMforGP("reset");// reset FEM
    }
    */
    
    //thePreviousEngine = theCurrentEngine;
}


void UQ_EngineSelection::updateEngineComboDisp(const QString methodName)
{

  //this->createComboBox();
  //QString currentEngine = theEngineComboDisp->currentText();
  theEngineComboDisp->clear();
  
  if (methodName == "None") {
    theEngineComboDisp->addItem("None"); // Display it on the combobox
    theEngineComboDisp->setCurrentIndex(0);
    return;
  }
  
    QString currentEngine = this->getCurrentComboName();
    int numItems = this->count();
    for(int i=0; i<numItems;i++) {
        QString engineName = this->getComboName(i);
	if (engineName!=QString("None")) {
            auto myWidget = dynamic_cast<UQ_Engine*> (this->getComponent(engineName));
            if (myWidget->fixMethod(methodName)){
                theEngineComboDisp->addItem(engineName); // Display it on the combobox
               // this->removeItem(engineName);
            }
	}
    }

    int idx = theEngineComboDisp->findText(currentEngine);
    if (idx!=-1) {
        theEngineComboDisp->setCurrentIndex(idx);
    } else {
        theEngineComboDisp->setCurrentIndex(0);
    }

}

void
UQ_EngineSelection::setRV_Defaults() {
    return theCurrentEngine->setRV_Defaults();
}


UQ_Results *
UQ_EngineSelection::getResults(void) {
    return theCurrentEngine->getResults();
}

int
UQ_EngineSelection::getNumParallelTasks() {
    return theCurrentEngine->getMaxNumParallelTasks();
}


void
UQ_EngineSelection::relayQueryEVT(void) {
    emit queryEVT();
}

void
UQ_EngineSelection::setEventType(QString type) {
    theSimCenterUQEngine->setEventType(type);
}
