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

#include "SimCenterEventAppSelection.h"
#include "SectionTitle.h"
#include <GoogleAnalytics.h>

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
#include <QScrollArea>
#include <QList>

// A class acting for secondary level RDT menu items
// whose display is dependent on the selection in GI
// of assets selected. What is actually displayed? that
// functionality is performed by the SecondaryComponentSelection object

// NOTE: The GI interacts through the connection
// of signals from checkboxes with slot in WorkflowAppRDT. 
// That method calls each SimCenterEventAppSelection with a show or a hide


SimCenterEventAppSelection::SimCenterEventAppSelection(QString label, QString appName, QString eventT, QWidget *parent)
  :SimCenterAppWidget(parent), currentIndex(-1), theCurrentSelection(NULL), jsonKeyword(appName), eventType(eventT), viewableStatus(false)
{
    this->initializeWidget(label);
}



void
SimCenterEventAppSelection::initializeWidget(QString label) {

    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *topLayout = new QHBoxLayout;

    topLayout->setSpacing(0);
    layout->setSpacing(0);
    topLayout->setContentsMargins(0,0,0,0);
    layout->setContentsMargins(0,0,0,0);

    selectionText = new SectionTitle();
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

    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    theStackedWidget = new QStackedWidget();

    sa->setWidget(theStackedWidget);

    layout->addLayout(topLayout);
    layout->addWidget(sa);

    //  layout->addWidget(theStackedWidget);
    //  layout->addStretch();

    this->setLayout(layout);

    connect(theSelectionCombo, SIGNAL(currentTextChanged(QString)),
            this, SLOT(selectionChangedSlot(QString)));
}



SimCenterEventAppSelection::~SimCenterEventAppSelection()
{

}


bool SimCenterEventAppSelection::outputToJSON(QJsonObject &jsonObject)
{
    if (theSelectionCombo->isEnabled() == false) {
        return true; // disabled
    }
    
    QJsonArray eventArray;
    QJsonObject data;
    if (theCurrentSelection != NULL) {
      if (theCurrentSelection->outputToJSON(data) == false) {
	return false;
      } else {
	eventArray.append(data);
	jsonObject[jsonKeyword] = eventArray;	
	return true;
      }
    }

    return false;
}


bool SimCenterEventAppSelection::inputFromJSON(QJsonObject &jsonObject)  
{
    if (theSelectionCombo->isEnabled() == false) {
        return true; // disabled
    }
    
    if (theCurrentSelection == NULL) {
      errorMessage("SimCenter Logic Error - report bug - SimCenterEventAppSelection::inputFrom JSON .. no current app!");
      return false; 
    }

    QString key;
    if (jsonObject.contains(jsonKeyword))
      key = jsonKeyword;
    else {
      this->errorMessage(QString("No Application key: ") + jsonKeyword + QString(" found in json"));
      return false;
    }

    //QJsonObject theApplicationObject = jsonObject[key].toObject();    
    QJsonValue theValue = jsonObject[key];
    if (theValue.isArray()) {
      QJsonArray theEvents = theValue.toArray();
      theValue = theEvents.at(0);      
    }

    if (theValue.isNull()) {
      this->errorMessage(QString("No Event in Array "));
      return false;
    }
    QJsonObject theEvent = theValue.toObject();

    return theCurrentSelection->inputFromJSON(theEvent);
}


bool SimCenterEventAppSelection::outputAppDataToJSON(QJsonObject &jsonObject)
{
    QJsonArray eventArray;  
    QJsonObject data;
    if (theCurrentSelection != NULL) {
      if (theSelectionCombo->isEnabled()) {
	if (theCurrentSelection->outputAppDataToJSON(data) == false) {
	  return false;
	} else {
	  if(data.isEmpty()) {
	    data["Application"] = "None";
	  }
	  if (! data.contains("EventClassification"))
	    data["EventClassification"]=eventType;
	  eventArray.append(data);
	  jsonObject[jsonKeyword] = eventArray;
	  return true;
        }
      }
    }
    
    return false;
}


bool SimCenterEventAppSelection::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    // qDebug() << __PRETTY_FUNCTION__<< " " << jsonKeyword;
  
    if (theSelectionCombo->isEnabled() == false) {
        return true; // disabled
    }
    
    QString key;
    bool found = false;
      
    if (jsonObject.contains(jsonKeyword)) {
        key = jsonKeyword;
        found = true;
    } else {
        qDebug() << jsonKeyword << " " << jsonObject;
	return false;
    }

    // 2 cases: 1 it is an array
    //          2 it is an object with just app name and app data


    QJsonValue theValue = jsonObject[key];
    if (theValue.isArray()) {
      QJsonArray theEvents = theValue.toArray();
      theValue = theEvents.at(0);
      if (theValue.isNull()) {
	return false;
      }      
    } 
    
    QJsonObject theApplicationObject = theValue.toObject();
    
    if (theApplicationObject.contains("Application")) {
      QJsonValue theName = theApplicationObject["Application"];
      QString appName = theName.toString();
      
      // qDebug() << __PRETTY_FUNCTION__<< " " << jsonKeyword << " " << appName;

      int index = theApplicationNames.indexOf(appName);
      
      if (index != -1) {
	theSelectionCombo->setCurrentIndex(index);
	theCurrentSelection=theComponents.at(index);
	return theCurrentSelection->inputAppDataFromJSON(theApplicationObject);
      } else {
	QString message = jsonKeyword +  QString(" found unknown application: ") + appName;
	this->errorMessage(message);
      }
      
    } else {
      errorMessage("SimCenterEventAppSelection - InvalidInput - no Application section in Event Object data");
      return false;
    }

    return false; // error message
}



bool SimCenterEventAppSelection::copyFiles(QString &destDir)
{
  if (theCurrentSelection != NULL) {
    QString textForAnalytics = jsonKeyword.replace("-", "_") + QString("_") + theApplicationNames.at(currentIndex);
    GoogleAnalytics::ReportAppUsage(textForAnalytics);    
    return theCurrentSelection->copyFiles(destDir);
  } else
    return false;
}


void SimCenterEventAppSelection::clear(void)
{
    foreach (auto&& comp, theComponents) {
        comp->clear();
    }
}

void SimCenterEventAppSelection::clearSelections(void)
{
    foreach (auto&& comp, theComponents) {
        comp->clear();
    }
    theComboNames.clear();
    theApplicationNames.clear();
    theSelectionCombo->clear();
}

bool
SimCenterEventAppSelection::addComponent(QString text, QString appName, SimCenterAppWidget *theComponent)
{
    if (theComboNames.indexOf(text) == -1) {
        theComboNames.append(text);
        theApplicationNames.append(appName);
        theComponents.append(theComponent);
        theStackedWidget->addWidget(theComponent);

        theSelectionCombo->addItem(text);

        return true;
    } else {
        QString message = jsonKeyword +  QString(" application already exists: ") + text + " (" + appName + ")";
        this->errorMessage(message);
    }

    return false;
}

SimCenterAppWidget *
SimCenterEventAppSelection::getComponent(QString text)
{
    if (theComboNames.indexOf(text) != -1)
        return theComponents.at(theComboNames.indexOf(text));
    else 
        return NULL;
}


bool
SimCenterEventAppSelection::selectComponent(const QString text)
{
    auto idx = theComboNames.indexOf(text);

    if (idx != -1)
    {
        theSelectionCombo->setCurrentIndex(idx);

        return true;
    }
    else
        return false;
}


bool
SimCenterEventAppSelection::displayComponent(QString text)
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
SimCenterEventAppSelection::hideHeader()
{
//    selectionText->toPlainText();
//    selectionText->setMinimumWidth(0);
    selectionText->hide();
    theSelectionCombo->hide();
}


void
SimCenterEventAppSelection::selectionChangedSlot(const QString &selectedText)
{
    //
    // get stacked widget to display current, if of course it exists
    //

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
SimCenterEventAppSelection::setCurrentlyViewable(bool status) {
    viewableStatus = status;
    if (theCurrentSelection != 0)
        theCurrentSelection->setCurrentlyViewable(viewableStatus);
    else {
      QString message = QString("SimCenterEventAppSelection: ") + jsonKeyword +  QString(" no current selection! ");
      errorMessage(message);
    }
}

SimCenterAppWidget *
SimCenterEventAppSelection::getCurrentSelection(void) {
    return theCurrentSelection;
}


QString
SimCenterEventAppSelection::getCurrentSelectionName(void)
{
    auto idx = theComponents.indexOf(theCurrentSelection);

    if(idx == -1)
        return QString();

    return theApplicationNames.at(idx);
}


void SimCenterEventAppSelection::removeItem(QString itemName) {
    theSelectionCombo->removeItem(theSelectionCombo->findText(itemName));
};


QString SimCenterEventAppSelection::getComboName(int index) {
    return theComboNames[index];
}

QString SimCenterEventAppSelection::getCurrentComboName(void) {
    auto idx = theComponents.indexOf(theCurrentSelection);

    QString comboName;
    if(idx != -1)
        comboName = theComboNames[idx];
    else
        comboName = QString("");

    return comboName;
}

int SimCenterEventAppSelection::count() {
    return theSelectionCombo->count();
}

void
SimCenterEventAppSelection::setSelectionsActive(bool visibility) {
    if (visibility == false) {
        theStackedWidget->setVisible(false);
        theSelectionCombo->setEnabled(false);
    } else {
        theStackedWidget->setVisible(true);
        theSelectionCombo->setEnabled(true);
    }
}

/*
void
SimCenterEventAppSelection::setOldKeyName(QString oldKeyword) {
  jsonKeywordOld = oldKeyword;
}
*/

bool
SimCenterEventAppSelection::outputCitation(QJsonObject &citation)
{

  QJsonObject appSpecificCitation;
  theCurrentSelection->outputCitation(appSpecificCitation);
  if (!appSpecificCitation.isEmpty()) {
    citation.insert(theApplicationNames[currentIndex], appSpecificCitation);
  }
  return true;
}
