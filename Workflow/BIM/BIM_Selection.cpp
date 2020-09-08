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

#include "BIM_Selection.h"

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

#include <InputWidgetOpenSeesAnalysis.h>

BIM_Selection::BIM_Selection(RandomVariablesContainer *theRVs, QWidget *parent)
    : SimCenterAppWidget(parent), theCurrentSelection(0)
{
    QVBoxLayout *layout = new QVBoxLayout();

    //
    // the selection part
    //

    QHBoxLayout *theSelectionLayout = new QHBoxLayout();
    //    QLabel *label = new QLabel();
    SectionTitle *label = new SectionTitle();
    label->setText(QString("BIM Application"));
    label->setMinimumWidth(200);

    theSelectionBox = new QComboBox();
    theSelectionBox->addItem(tr("Basic"));

    theSelectionLayout->addWidget(label);
    QSpacerItem *spacer = new QSpacerItem(50,10);
    theSelectionLayout->addItem(spacer);
    theSelectionLayout->addWidget(theSelectionBox);
    theSelectionLayout->addStretch();
    layout->addLayout(theSelectionLayout);

    //
    // create the stacked widget
    //

    theStackedWidget = new QStackedWidget();

    //
    // create the individual widgets add to stacked widget
    //

    theOpenSeesApplication = new InputWidgetOpenSeesAnalysis(theRVs);
    theStackedWidget->addWidget(theOpenSeesApplication);

    layout->addWidget(theStackedWidget);
    this->setLayout(layout);
    theCurrentSelection=theOpenSeesApplication;

    connect(theSelectionBox, SIGNAL(currentIndexChanged(QString)), this,
            SLOT(engineSelectionChanged(QString)));
}

BIM_Selection::~BIM_Selection()
{

}


bool
BIM_Selection::outputToJSON(QJsonObject &jsonObject)
{
    QJsonObject fem;
    theCurrentSelection->outputToJSON(fem);
    jsonObject["Simulation"]=fem;

    return true;
}


bool
BIM_Selection::inputFromJSON(QJsonObject &jsonObject) {

    if (jsonObject.contains("Simulation")) {
        QJsonObject femObject = jsonObject["Simulation"].toObject();

        if (theCurrentSelection != 0) {
            return theCurrentSelection->inputFromJSON(femObject);
        }
    }

    return false;
}

void BIM_Selection::selectionChanged(const QString &arg1)
{
    //
    // switch stacked widgets depending on text
    // note type output in json and name in pull down are not the same and hence the ||
    //

    if (arg1 == "OpenSees") {
        theStackedWidget->setCurrentIndex(0);
        theCurrentSelection = theOpenSeesApplication;
        emit onSelectionChanged();
    }

    else {
        qDebug() << "ERROR .. BIM_Selection selection .. type unknown: " << arg1;
    }
}


void
BIM_Selection::selectionChanged(void){
    emit onSelectionChanged();
}

bool
BIM_Selection::outputAppDataToJSON(QJsonObject &jsonObject)
{
    QJsonObject appsUQ;
    theCurrentSelection->outputAppDataToJSON(appsUQ);
    jsonObject["Simulation"]=appsUQ;

    return true;
}


bool
BIM_Selection::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    // get name from "Application" key

    if (jsonObject.contains("Simulation")) {
        QJsonObject theObject = jsonObject["Simulation"].toObject();

        QString type;

        if (theObject.contains("Application")) {
            QJsonValue theName = theObject["Application"];
            type = theName.toString();

            // based on application name value set engine type
            int index = 0;
            if ((type == QString("OpenSees")) ||
                (type == QString("OpenSees-Simulation"))) {
                index = 0;
            } else {
                emit sendErrorMessage("BIM_Selection - no valid type found");
                return false;
            }

            theSelectionBox->setCurrentIndex(index);

            // invoke inputAppDataFromJSON on new type
            if (theCurrentSelection != 0) {
                return theCurrentSelection->inputAppDataFromJSON(theObject);
            }
        } else {
            emit sendErrorMessage("BIM_Selection - no Application key found");
            return false;
        }

    } else {
        emit sendErrorMessage("BIM_Selection: failed to find BIM application");
        return false;
    }


    return false;
}

bool
BIM_Selection::copyFiles(QString &destDir) {

  if (theCurrentSelection != 0) {
    return  theCurrentSelection->copyFiles(destDir);
  }
  
  return false;
}



