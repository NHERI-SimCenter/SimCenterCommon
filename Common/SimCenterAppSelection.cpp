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

#include "SimCenterAppSelection.h"
#include "sectiontitle.h"

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
// That method calls each SimCenterAppSelection with a show or a hide


SimCenterAppSelection::SimCenterAppSelection(QString label, QString appName, QWidget *parent)
    :SimCenterAppWidget(parent), currentIndex(-1), theCurrentSelection(NULL), selectionApplicationType(appName), viewableStatus(false)
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

  connect(theSelectionCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(selectionChangedSlot(QString)));
}


SimCenterAppSelection::~SimCenterAppSelection()
{

}


bool SimCenterAppSelection::outputToJSON(QJsonObject &jsonObject)
{ 
    QJsonObject data;
    if (theCurrentSelection != NULL)
        if (theCurrentSelection->outputToJSON(data) == false) {
            return false;
        } else {
            jsonObject[selectionApplicationType] = data;
            return true;
        }
    return false;
}


bool SimCenterAppSelection::inputFromJSON(QJsonObject &jsonObject)
{
    if (jsonObject.contains(selectionApplicationType)) {

        QJsonObject theApplicationObject = jsonObject[selectionApplicationType].toObject();

        if (theCurrentSelection != NULL)
            return theCurrentSelection->inputFromJSON(theApplicationObject);
        else
            return false;
    }

    return false;
}


bool SimCenterAppSelection::outputAppDataToJSON(QJsonObject &jsonObject)
{
    QJsonObject data;
    if (theCurrentSelection != NULL)
        if (theCurrentSelection->outputAppDataToJSON(data) == false) {
            return false;
        } else {
            jsonObject[selectionApplicationType] = data;
            return true;
        }

    return false;
}


bool SimCenterAppSelection::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    // qDebug() << __PRETTY_FUNCTION__<< " " << selectionApplicationType;

    if (jsonObject.contains(selectionApplicationType)) {

        QJsonObject theApplicationObject = jsonObject[selectionApplicationType].toObject();
        if (theApplicationObject.contains("Application")) {
            QJsonValue theName = theApplicationObject["Application"];
            QString appName = theName.toString();

            // qDebug() << __PRETTY_FUNCTION__<< " " << selectionApplicationType << " " << appName;

            int index = theApplicationNames.indexOf(appName);

            if (index != -1) {
                theSelectionCombo->setCurrentIndex(index);
                theCurrentSelection=theComponents.at(index);
                return theCurrentSelection->inputAppDataFromJSON(theApplicationObject);
            } else {
                QString message = selectionApplicationType +  QString(" found unknown application: ") + appName;
                emit sendErrorMessage(message);
            }
        } else {
            QString message = QString("SimCenterAppSelection could not find Application field in JSON");
            emit sendErrorMessage(message);
        }
    } else {

        // Check if 'None' is an option
        int index = theApplicationNames.indexOf("None");
        if (index != -1) {
            theSelectionCombo->setCurrentIndex(index);
            theCurrentSelection=theComponents.at(index);
            return true;
        }
        else
        {
            QString message = QString("Applications does not contain a field: ") + selectionApplicationType;
            emit sendErrorMessage(message);
        }
    }

    return false; // error message
}



bool SimCenterAppSelection::copyFiles(QString &destDir)
{
    if (theCurrentSelection != NULL)
        return theCurrentSelection->copyFiles(destDir);
    else
        return false;
}


void SimCenterAppSelection::clear(void)
{
    foreach (auto&& comp, theComponents) {
        comp->clear();
    }
}


bool
SimCenterAppSelection::addComponent(QString text, QString appName, SimCenterAppWidget *theComponent)
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
        emit sendErrorMessage(message);
    }

    return false;
}

SimCenterAppWidget *
SimCenterAppSelection::getComponent(QString text)
{
    if (theComboNames.indexOf(text) != -1)
        return theComponents.at(theComboNames.indexOf(text));
    else
        return NULL;
}


bool
SimCenterAppSelection::displayComponent(QString text)
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
SimCenterAppSelection::selectionChangedSlot(const QString &selectedText)
{
    //
    // get stacked widget to display current if of course it exists
    //

    // qDebug() << this->objectName() << " slotChanged() " << viewableStatus;

    int index = theComboNames.indexOf(selectedText);

    if (index != -1 && index != currentIndex) {
        theStackedWidget->setCurrentIndex(index);
        currentIndex = index;
        if (theCurrentSelection != 0)
            theCurrentSelection->setCurrentlyViewable(false);
        theCurrentSelection = theComponents.at(index);
        theCurrentSelection->setCurrentlyViewable(viewableStatus);
        theStackedWidget->setCurrentIndex(index);
        emit selectionChangedSignal(selectedText);
    }
}

void
SimCenterAppSelection::setCurrentlyViewable(bool status) {
    viewableStatus = status;
    theCurrentSelection->setCurrentlyViewable(viewableStatus);
    // qDebug() << this->objectName() << " setViewable " << viewableStatus;
}

SimCenterAppWidget *
SimCenterAppSelection::getCurrentSelection(void) {
  return theCurrentSelection;
}
