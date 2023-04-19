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
// That method calls each SimCenterAppSelection with a show or a hide


SimCenterAppSelection::SimCenterAppSelection(QString label, QString appName, QWidget *parent)
    :SimCenterAppWidget(parent), currentIndex(-1), theCurrentSelection(NULL), jsonKeyword(appName), viewableStatus(false)
{
    this->initializeWidget(label);
}



SimCenterAppSelection::SimCenterAppSelection(QString label, QString appName, QString oldAppName, QString typeOfAsset, QWidget *parent)
    :SimCenterAppWidget(parent), currentIndex(-1), theCurrentSelection(NULL), jsonKeyword(appName), jsonKeywordOld(oldAppName), assetType(typeOfAsset), viewableStatus(false)
{
    this->initializeWidget(label);
}

SimCenterAppSelection::SimCenterAppSelection(QString label, QString appName, QList<QString> xKeys, QString typeOfAsset, QWidget *parent)
    :SimCenterAppWidget(parent), currentIndex(-1), theCurrentSelection(NULL), jsonKeyword(appName), assetType(typeOfAsset), viewableStatus(false)
{
    for (int i=0; i<xKeys.length(); i++) {
        QString copy(xKeys.at(i));
        extraKeys.append(copy);
    }
  this->initializeWidget(label);
}

void
SimCenterAppSelection::initializeWidget(QString label) {

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



SimCenterAppSelection::~SimCenterAppSelection()
{

}


bool SimCenterAppSelection::outputToJSON(QJsonObject &jsonObject)
{

    if (theSelectionCombo->isEnabled() == false) {
        return true; // disabled
    }

    QJsonObject data;
    if (theCurrentSelection != NULL) {
        if (theCurrentSelection->outputToJSON(data) == false) {
            return false;
        } else {
            jsonObject[jsonKeyword] = data;
            for (int i=0; i<extraKeys.length(); i++) {
                jsonObject[extraKeys.at(i)] = data;
            }
            return true;
        }
    }

    return false;
}


bool SimCenterAppSelection::inputFromJSON(QJsonObject &jsonObject)
{
    if (theSelectionCombo->isEnabled() == false) {
        return true; // disabled
    }

    QString key;
    if (jsonObject.contains(jsonKeyword))
        key = jsonKeyword;
    else if (jsonObject.contains(jsonKeywordOld))
        key = jsonKeywordOld;
    else {
        return true; // Stevan has this condition for some reason
    }

    QJsonObject theApplicationObject = jsonObject[key].toObject();

    if (theCurrentSelection != NULL)
        return theCurrentSelection->inputFromJSON(theApplicationObject);

    //
    // if get here an error
    //

    QString message = QString("AppSelection: " ) + jsonKeyword + QString(" no app found");
    errorMessage(message);
    return false;

}


bool SimCenterAppSelection::outputAppDataToJSON(QJsonObject &jsonObject)
{
    QJsonObject data;
    if (theCurrentSelection != NULL) {
        if (theSelectionCombo->isEnabled()) {
            if (theCurrentSelection->outputAppDataToJSON(data) == false) {
                return false;
            } else {
                if(!data.isEmpty())
                {
                    if(assetType.isEmpty()) {
                        jsonObject[jsonKeyword] = data;
                    for (int i=0; i<extraKeys.length(); i++)
                        jsonObject[extraKeys.at(i)] = data;
                    } else {
                        auto assetObj = jsonObject[jsonKeyword].toObject();

                        assetObj.insert(assetType, data);

                        jsonObject.insert(jsonKeyword,assetObj);
                    }
                }
                return true;
            }
        } else {
            data["Application"] = "None";
            jsonObject[jsonKeyword] = data;
            return true;
        }
    }

    return false;
}


bool SimCenterAppSelection::inputAppDataFromJSON(QJsonObject &jsonObject)
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
    } else if (jsonObject.contains(jsonKeywordOld)) {
        key = jsonKeywordOld;
        found = true;
    }
    else {
        qDebug() << jsonKeyword << " " << jsonKeywordOld << " " << jsonObject;
    }

    if (found) {

        QJsonObject theApplicationObject = jsonObject[key].toObject();

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

            // To accomodate multiple assets
            if(theApplicationObject.contains(assetType))
            {
                auto theAssetObject = theApplicationObject[assetType].toObject();

                if(!theAssetObject.contains("Application"))
                {
                    QString message = assetType +  QString(" needs to contain an application field");
                    this->errorMessage(message);
                    return false;
                }

                QJsonValue theName = theAssetObject["Application"];
                QString appName = theName.toString();

                // qDebug() << __PRETTY_FUNCTION__<< " " << jsonKeyword << " " << appName;

                int index = theApplicationNames.indexOf(appName);

                if (index != -1) {
                    theSelectionCombo->setCurrentIndex(index);
                    theCurrentSelection=theComponents.at(index);
                    return theCurrentSelection->inputAppDataFromJSON(theAssetObject);
                } else {
                    QString message = jsonKeyword +  QString(" found unknown application: ") + appName;
                    this->errorMessage(message);
                }
            }
            else{
                QString message = QString("SimCenterAppSelection could not find Application field in JSON");
                this->errorMessage(message);
            }
        }
    } else {

        // Check if 'None' is an option
        if (theApplicationNames.indexOf("None") != -1) {

            int index = theApplicationNames.indexOf("None");
            theSelectionCombo->setCurrentIndex(index);
            theCurrentSelection=theComponents.at(index);
            return true;
        }
        else
        {
            // Application is not found in the input file, do not input json, return
            QString message = QString("Applications does not contain a field: ") + jsonKeyword;
            this->errorMessage(message);
            return false;
        }
    }

    return false; // error message
}



bool SimCenterAppSelection::copyFiles(QString &destDir)
{
  if (theCurrentSelection != NULL) {
    QString textForAnalytics = jsonKeyword + QString("-") + theApplicationNames.at(currentIndex);
    GoogleAnalytics::ReportAppUsage(textForAnalytics);    
    return theCurrentSelection->copyFiles(destDir);
  } else
    return false;
}


void SimCenterAppSelection::clear(void)
{
    foreach (auto&& comp, theComponents) {
        comp->clear();
    }
}

void SimCenterAppSelection::clearSelections(void)
{
    foreach (auto&& comp, theComponents) {
        comp->clear();
    }
    theComboNames.clear();
    theApplicationNames.clear();
    theSelectionCombo->clear();
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
        QString message = jsonKeyword +  QString(" application already exists: ") + text + " (" + appName + ")";
        this->errorMessage(message);
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
SimCenterAppSelection::selectComponent(const QString text)
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
SimCenterAppSelection::hideHeader()
{
//    selectionText->toPlainText();
//    selectionText->setMinimumWidth(0);
    selectionText->hide();
    theSelectionCombo->hide();
}


void
SimCenterAppSelection::selectionChangedSlot(const QString &selectedText)
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
SimCenterAppSelection::setCurrentlyViewable(bool status) {
    viewableStatus = status;
    if (theCurrentSelection != 0)
        theCurrentSelection->setCurrentlyViewable(viewableStatus);
    else {
      QString message = QString("SimCenterAppSelection: ") + jsonKeyword +  QString(" no current selection! ");
      errorMessage(message);
    }
}

SimCenterAppWidget *
SimCenterAppSelection::getCurrentSelection(void) {
    return theCurrentSelection;
}


QString
SimCenterAppSelection::getCurrentSelectionName(void)
{
    auto idx = theComponents.indexOf(theCurrentSelection);

    if(idx == -1)
        return QString();

    return theApplicationNames.at(idx);
}


void SimCenterAppSelection::removeItem(QString itemName) {
    theSelectionCombo->removeItem(theSelectionCombo->findText(itemName));
};


QString SimCenterAppSelection::getComboName(int index) {
    return theComboNames[index];
}

QString SimCenterAppSelection::getCurrentComboName(void) {
    auto idx = theComponents.indexOf(theCurrentSelection);

    QString comboName;
    if(idx != -1)
        comboName = theComboNames[idx];
    else
        comboName = QString("");

    return comboName;
}

int SimCenterAppSelection::count() {
    return theSelectionCombo->count();
}

void
SimCenterAppSelection::setSelectionsActive(bool visibility) {
    if (visibility == false) {
        theStackedWidget->setVisible(false);
        theSelectionCombo->setEnabled(false);
    } else {
        theStackedWidget->setVisible(true);
        theSelectionCombo->setEnabled(true);
    }
}
