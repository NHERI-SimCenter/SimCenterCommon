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

// Written by: fmk

#include "SimCenterAppEventSelection.h"
#include "SectionTitle.h"

// Qt headers
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QJsonArray>

#include <QLineEdit>
#include <QListWidget>
#include <QDebug>
#include <QComboBox>
#include <QStackedWidget>


// A class acting for secondary level RDT menu items
// whose display is dependent on the selection in GI
// of assets selected. What is actually displayed? that
// functionality is performed by the SecondaryComponentSelection object

// NOTE: The GI interacts through the connection
// of signals from checkboxes with slot in WorkflowAppRDT. 
// That method calls each SimCenterAppEventSelection with a show or a hide


SimCenterAppEventSelection::SimCenterAppEventSelection(QString label, QString appName, QWidget *parent)
    :SimCenterAppWidget(parent), currentIndex(-1), theCurrentSelection(NULL), selectionApplicationType(appName), currentEventType("Earthquake")
{
    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *topLayout = new QHBoxLayout;

    SectionTitle *selectionText = new SectionTitle();
    selectionText->setMinimumWidth(250);
    selectionText->setText(label);

    theSelectionCombo = new QComboBox();
    theSelectionCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    topLayout->addWidget(selectionText);
    topLayout->addWidget(theSelectionCombo, 1);
    topLayout->addStretch(1);

#ifdef _WIN32
    theSelectionCombo->setMaximumHeight(25);
#endif

    theStackedWidget = new QStackedWidget();

    layout->addLayout(topLayout);
    layout->addWidget(theStackedWidget);
    layout->addStretch();

    this->setLayout(layout);

    connect(theSelectionCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(selectionChangedSlot(QString)));
}


SimCenterAppEventSelection::~SimCenterAppEventSelection()
{

}


bool SimCenterAppEventSelection::outputToJSON(QJsonObject &jsonObject)
{ 
    // here we will loop over all events
    if (theCurrentSelection != NULL)
    {
        jsonObject["type"]=currentEventType;
        return theCurrentSelection->outputToJSON(jsonObject);
    }
    else
        return false;
}


bool SimCenterAppEventSelection::inputFromJSON(QJsonObject &jsonObject)
{
    if (theCurrentSelection != NULL)
        return theCurrentSelection->inputFromJSON(jsonObject);
    else
        return false;
}


bool SimCenterAppEventSelection::outputAppDataToJSON(QJsonObject &jsonObject)
{

    QJsonArray data;

    // here we will loop over all events in future, but for now one object in array

    if (theCurrentSelection != NULL) {
        QJsonObject dataEvent;
        dataEvent["EventClassification"]=currentEventType;
        if (theCurrentSelection->outputAppDataToJSON(dataEvent) == false)
            return false;
        else {
            data.append(dataEvent);
            jsonObject[selectionApplicationType] = data;
            return true;
        }
    }

    return false;
}


bool SimCenterAppEventSelection::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    if (jsonObject.contains(selectionApplicationType))
    {

        QJsonArray theAppArray = jsonObject[selectionApplicationType].toArray();

        foreach(auto&& obj, theAppArray)
        {

            QJsonObject theApplicationObject = obj.toObject();
            if (theApplicationObject.contains("Application"))
            {
                QJsonValue theName = theApplicationObject["Application"];
                QString appName = theName.toString();

                int index = theApplicationNames.indexOf(appName);

                if (index != -1)
                {
                    theSelectionCombo->setCurrentIndex(index);
                    theCurrentSelection=theComponents.at(index);
                    return theCurrentSelection->inputAppDataFromJSON(theApplicationObject);
                }
                else
                {
                    QString message = selectionApplicationType +  QString(" found unknown application: ") + appName;
                    this->errorMessage(message);
                }
            }
            else
            {
                QString message = QString("SimCenterAppEventSelection could not find Application field in JSON");
                this->errorMessage(message);
            }
        }
    }
    else {
        QString message = QString("Applications does not contain a field: ") + selectionApplicationType;
        this->errorMessage(message);
    }

    return false; // error message
}



bool SimCenterAppEventSelection::copyFiles(QString &destDir)
{
    if (theCurrentSelection != NULL)
        return theCurrentSelection->copyFiles(destDir);
    else
        return false;
}


bool
SimCenterAppEventSelection::addComponent(QString text, QString appName, SimCenterAppWidget *theComponent)
{
    if (theComboNames.indexOf(text) == -1) {
        theComboNames.append(text);
        theApplicationNames.append(appName);
        theComponents.append(theComponent);
        theStackedWidget->addWidget(theComponent);

        theSelectionCombo->addItem(text);

        return true;
    } else {
        QString message = selectionApplicationType +  QString(" found unknown application: ") + appName;
        this->errorMessage(message);
    }

    return false;
}

SimCenterAppWidget *
SimCenterAppEventSelection::getComponent(QString text)
{
    if (theComboNames.indexOf(text) != -1)
        return theComponents.at(theComboNames.indexOf(text));
    else
        return NULL;
}


bool
SimCenterAppEventSelection::displayComponent(QString text)
{
    //
    // find index of text in list and display corresponding widget if index found
    //

    int index = theComboNames.indexOf(text);

    if (index != -1 && index != currentIndex) {
        theSelectionCombo->setCurrentIndex(index);
        theStackedWidget->setCurrentIndex(index);
        return true;
    }

    return false;
}

void
SimCenterAppEventSelection::selectionChangedSlot(const QString &selectedText)
{
    //
    // get stacked widget to display current if of course it exists
    //

    int index = theComboNames.indexOf(selectedText);

    if (index != -1 && index != currentIndex) {
        theStackedWidget->setCurrentIndex(index);
        currentIndex = index;
        theCurrentSelection = theComponents.at(index);
        theStackedWidget->setCurrentIndex(index);
    }
}

void
SimCenterAppEventSelection::currentEventTypeChanged(QString &eventType) {
    currentEventType = eventType;
}

void
SimCenterAppEventSelection::newHazard(QString &eventType){
    Q_UNUSED(eventType);

}


void
SimCenterAppEventSelection::clear(void)
{
    foreach (auto&& comp, theComponents) {
        comp->clear();
    }
}


