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

#include "UQ_EngineSelection.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QStackedWidget>
#include <QComboBox>

#include <QPushButton>
#include <QJsonObject>
#include <QJsonArray>

#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <sectiontitle.h>

#include <DakotaEngine.h>
#include <SimCenterUQEngine.h>
#include <UCSD_Engine.h>
#include <UQpyEngine.h>
#include <UQ_JsonEngine.h>
#include <JsonConfiguredUQEngine.h>
//#include <filterEngine.h>
#include <sectiontitle.h>

UQ_EngineSelection::UQ_EngineSelection(UQ_EngineType type,
				       QWidget *parent)
  : SimCenterAppWidget(parent),
    theCurrentEngine(0)
{
    QVBoxLayout *layout = new QVBoxLayout();

    //
    // the selection part
    //

    QHBoxLayout *theSelectionLayout = new QHBoxLayout();
    //theSelectionLayout->setMargin(10);
    SectionTitle *title=new SectionTitle();
    title->setText(tr("UQ Engine"));

   // label->setText(QString("    UQ Engine   "));
    theEngineSelectionBox = new QComboBox();
    theEngineSelectionBox->addItem(tr("Dakota"));
    // theEngineSelectionBox->addItem(tr("UQpy"));
    theEngineSelectionBox->addItem(tr("SimCenterUQ"));    
    theEngineSelectionBox->addItem(tr("CustomUQ"));
    
    if (type == All)
    {
      theEngineSelectionBox->addItem(tr("UCSD-UQ"));
//      theEngineSelectionBox->addItem(tr("UQpy"));
    }

    theEngineSelectionBox->setItemData(0, "Dakota engine", Qt::ToolTipRole);
    theEngineSelectionBox->setItemData(1, "SimCenterUQ engine", Qt::ToolTipRole);
    theEngineSelectionBox->setItemData(2, "Custom UQ Engine", Qt::ToolTipRole);        
    theEngineSelectionBox->setItemData(3, "UCSD UQ engine", Qt::ToolTipRole);
//    theEngineSelectionBox->setItemData(4, "uqPY engine", Qt::ToolTipRole);

    theSelectionLayout->addWidget(title,1);
    theSelectionLayout->addWidget(theEngineSelectionBox,3);
    theSelectionLayout->addStretch(6);
    layout->addLayout(theSelectionLayout);

    //
    // create the stacked widget
    //

    theStackedWidget = new QStackedWidget();

    //
    // create the individual widgets add to stacked widget
    //

    theDakotaEngine = new DakotaEngine(type);
    theSimCenterUQEngine = new SimCenterUQEngine(type);
    theCustomEngine = new UQ_JsonEngine(type);
    theUCSD_Engine = new UCSD_Engine(type);
//    theUQpyEngine = new JsonConfiguredUQEngine(type);

    theStackedWidget->addWidget(theDakotaEngine);
    theStackedWidget->addWidget(theSimCenterUQEngine);
    theStackedWidget->addWidget(theCustomEngine);
    theStackedWidget->addWidget(theUCSD_Engine);

    layout->addWidget(theStackedWidget);
    this->setLayout(layout);
    layout->setMargin(0);

    theCurrentEngine=theDakotaEngine;
    thePreviousEngine=theCurrentEngine;

    connect(theEngineSelectionBox, SIGNAL(currentIndexChanged(QString)), this,
            SLOT(engineSelectionChanged(QString)));

    connect(theDakotaEngine, SIGNAL(onUQ_EngineChanged()), this, SLOT(enginesEngineSelectionChanged()));
    connect(theSimCenterUQEngine, SIGNAL(onUQ_EngineChanged()), this, SLOT(enginesEngineSelectionChanged()));
    connect(theCustomEngine, SIGNAL(onUQ_EngineChanged()), this, SLOT(enginesEngineSelectionChanged()));
    connect(theUCSD_Engine, SIGNAL(onUQ_EngineChanged()), this, SLOT(enginesEngineSelectionChanged()));

    // connect(theCustomEngine, SIGNAL(onNumModelsChanged(int)), this, SLOT(numModelsChanged(int)));    
    // connect(theCustomEngine, SIGNAL(onUQ_EngineChanged()), this, SLOT(enginesEngineSelectionChanged()));

    // connect queryEVT
    connect(theSimCenterUQEngine, SIGNAL(queryEVT()), this, SLOT(relayQueryEVT()));
}

UQ_EngineSelection::~UQ_EngineSelection()
{

}


bool
UQ_EngineSelection::outputToJSON(QJsonObject &jsonObject)
{
    QJsonObject uq;
    uq["uqEngine"]=theEngineSelectionBox->currentText();
    theCurrentEngine->outputToJSON(uq);
    jsonObject["UQ_Method"]=uq;

    return true;
}


bool
UQ_EngineSelection::inputFromJSON(QJsonObject &jsonObject) {
    if (jsonObject.contains("UQ_Method")) {
        QJsonObject uqObject = jsonObject["UQ_Method"].toObject();
        if (uqObject.contains("uqEngine")) {
            QString engine =uqObject["uqEngine"].toString();
            int index = theEngineSelectionBox->findText(engine);
            if (index == -1) {
                qDebug() << "UQ_EngineSelection::inputFromJson - Unknown Engine : " << engine;
                return false;
            }
            theEngineSelectionBox->setCurrentIndex(index);
        } else { // old format which just contained info for Dakota
            theEngineSelectionBox->setCurrentIndex(0);
        }
        if (theCurrentEngine != 0) {
            return theCurrentEngine->inputFromJSON(uqObject);
        }
    }

    return false;
}

void UQ_EngineSelection::engineSelectionChanged(const QString &arg1)
{
    //
    // switch stacked widgets depending on text
    // note type output in json and name in pull down are not the same and hence the ||
    //

    if (arg1 == "Dakota") {
      
        theStackedWidget->setCurrentIndex(0);
        theCurrentEngine = theDakotaEngine;
        emit onUQ_EngineChanged(true);
	
    } else if (arg1 == "SimCenterUQ" || arg1 == "SimCenterUQ-UQ") {
      
        theStackedWidget->setCurrentIndex(1);
        theCurrentEngine = theSimCenterUQEngine;
        emit onUQ_EngineChanged(true);
	
    } else if (arg1 == "CustomUQ") {

      theStackedWidget->setCurrentIndex(2);
      theCurrentEngine = theCustomEngine;
      emit onUQ_EngineChanged(false);
      
    } else if (arg1 == "UCSD-UQ") {

      theStackedWidget->setCurrentIndex(3);
      theCurrentEngine = theUCSD_Engine;
      emit onUQ_EngineChanged(true);

    } else {
      qDebug() << "ERROR .. UQ_EngineSelection selection .. type unknown: " << arg1;
    }
    theCurrentEngine->setRV_Defaults();

    //connect(theCurrentEngine, SIGNAL(onUQ_EngineChanged()), this, SLOT(enginesEngineSelectionChanged()));
    connect(theCurrentEngine,SIGNAL(onNumModelsChanged(int)), this, SLOT(numModelsChanged(int)));


    // theCurrentEngine->setRV_Defaults();
    
    /* FMK
    if (thePreviousEngine->getMethodName() == "surrogate")
    {
        theEDPs->setGPQoINames(QStringList({}) );// remove GP QoIs
        theRVs->setGPVarNamesAndValues(QStringList({}));// remove GP RVs
        theFemWidget->setFEMforGP("reset");// reset FEM
    }
    */
    
    thePreviousEngine = theCurrentEngine;
}


void
UQ_EngineSelection::enginesEngineSelectionChanged(){
    qDebug() << "UQ_EngineSelection::enginesSelectionChanged()";
    theCurrentEngine->setRV_Defaults();
    emit onUQ_EngineChanged(true);
}

bool
UQ_EngineSelection::outputAppDataToJSON(QJsonObject &jsonObject)
{
    QJsonObject appsUQ;
    theCurrentEngine->outputAppDataToJSON(appsUQ);
    jsonObject["UQ"]=appsUQ;

    return true;
}

bool
UQ_EngineSelection::inputAppDataFromJSON(QJsonObject &jsonObject)
{

    if (jsonObject.contains("UQ")) {
        QJsonObject theObject = jsonObject["UQ"].toObject();

        QString type;

        if (theObject.contains("Application")) {
            QJsonValue theName = theObject["Application"];
            type = theName.toString();

            // based on application name value set engine type
            int index = 0;
            if ((type == QString("Dakota")) ||
                    (type == QString("DakotaEngine")) ||
                    (type == QString("Dakota-UQ"))) {
                index = 0;
            } else if ((type == QString("SimCenterUQ")) || (type == QString("SimCenter-UQ"))) {
                index = 1;
                // } else if ((type == QString("UQpy")) || (type == QString("UQpyEngine"))) {
                //   index = 2;
            } else if ((type == QString("CustomUQ")) || type == QString("CustomUQEngine")  || type == QString("Other-UQ") || type == QString("")) {
                index = 2;
            } else if ((type == QString("UCSD-UQ"))) {
                index = 3;
            } else {
                QString theErrorMessage = QString("ERROR: UQ_Engine Selection - type: ") + type + QString(" not valid");
                errorMessage(theErrorMessage);
                return false;
            }

            theEngineSelectionBox->setCurrentIndex(index);

            // invoke inputAppDataFromJSON on new type
            if (theCurrentEngine != 0) {
                return theCurrentEngine->inputAppDataFromJSON(theObject);
            }
        } else {
            errorMessage("ERROR: UQ_EngineSelection - no Application key found");
            return false;
        }

    } else {
        errorMessage("ERROR: UQ_EngineSelection: failed to find UQ application");
        return false;
    }


    return false;
}


bool
UQ_EngineSelection::copyFiles(QString &destDir) {
  if (theCurrentEngine != 0) {
    return  theCurrentEngine->copyFiles(destDir);
  }
  
  return false;
}

void
UQ_EngineSelection::setRV_Defaults() {
    return theCurrentEngine->setRV_Defaults();
}

UQ_Results *
UQ_EngineSelection::getResults(void) {
    return theCurrentEngine->getResults();
}

void
UQ_EngineSelection::clear(void) {
    return theCurrentEngine->clear();
}

int
UQ_EngineSelection::getNumParallelTasks() {
    return theCurrentEngine->getMaxNumParallelTasks();
}

void
UQ_EngineSelection::numModelsChanged(int newNum) {
    emit onNumModelsChanged(newNum);
}

void
UQ_EngineSelection::relayQueryEVT(void) {
    emit queryEVT();
}

void
UQ_EngineSelection::setEventType(QString type) {
    theSimCenterUQEngine->setEventType(type);
}
