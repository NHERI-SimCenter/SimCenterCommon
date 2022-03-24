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

#include "SimCenterAppMulti.h"

#include "sectiontitle.h"

// Qt headers
#include <QTabWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QLineEdit>
#include <QDebug>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

SimCenterAppMulti::SimCenterAppMulti(QString label,
                                     QString appNam,
                                     SimCenterAppWidget *theProto,
                                     QWidget *parent)
  :SimCenterAppWidget(parent), thePrototype(theProto), appName(appNam), tabLabel(label)
{
    theTabs = new QTabWidget();

    QGridLayout *layout = new QGridLayout();
    QPushButton *addButton = new QPushButton("Add");
    QPushButton *removeButton = new QPushButton("Remove");
    layout->addWidget(addButton, 0,0);
    layout->addWidget(removeButton, 0,1);
    layout->addWidget(theTabs,1,0,1,3);

    this->setLayout(layout);
    this->addTab();

    connect(addButton, &QPushButton::clicked, this, [=](){
      this->addTab();
    });

    connect(removeButton, &QPushButton::clicked, this, [=](){
      this->removeCurrentTab();
    });    
}



SimCenterAppMulti::~SimCenterAppMulti()
{

}

int
SimCenterAppMulti::addTab() {

    SimCenterAppWidget *theNewSelection = thePrototype->getClone();
    if (theNewSelection != 0) {
        qDebug() << "Prototype failed to return a valid copy";
    }

    QWidget *newWidget = new QWidget();
    QGridLayout *newLayout = new QGridLayout();
    newWidget->setLayout(newLayout);

    QLineEdit *newBelief = new QLineEdit;
    newBelief->setText(QString::number(1.0));
    newLayout->addWidget(new QLabel("Belief:"), 0,0);
    newLayout->addWidget(newBelief, 0,1);

    if (theNewSelection != 0) {
        newLayout->addWidget(theNewSelection,1,0,1,4);
    }


    theBeliefs.append(newBelief);
    theModels.append(theNewSelection);

    QString label = tabLabel + QString("-") + QString::number(theBeliefs.count());
    theTabs->addTab(newWidget,label);


    newLayout->setColumnStretch(3,1);

    return 0;
}

bool SimCenterAppMulti::outputToJSON(QJsonObject &jsonObject)
{
   jsonObject["modelToRun"]=QString("RV.")+appName;

   QJsonArray modelArray;
   QJsonArray beliefArray;
   int numFEM = theModels.size();
   for (int i=0; i<numFEM; i++) {
       QJsonObject modelData;
       SimCenterAppWidget *theWidget = theModels.at(i);
       QLineEdit *theBelief = theBeliefs.at(i);
       beliefArray.append(QJsonValue(theBelief->text().toDouble()));
       theWidget->outputToJSON(modelData);
       modelArray.append(modelData);
   }
   jsonObject.insert(QString("models"),modelArray);
   jsonObject.insert(QString("beliefs"), beliefArray);
   return true;
}


bool SimCenterAppMulti::inputFromJSON(QJsonObject &jsonObject)
{

    if (jsonObject.contains("models")) {
        QJsonArray modelObjects = jsonObject["models"].toArray();
        int length = modelObjects.count();
        if (length == theModels.count()) {
            for (int i=0; i<length; i++) {
                QJsonObject modelObj = modelObjects.at(i).toObject();
                theModels.at(i)->inputFromJSON(modelObj);
            }
            return true;
        } else {
            errorMessage("SimCenterAppMulti - inputFromJson, input arrays and existing tabs do not match!");
            return false;
        }
        errorMessage("SimCenterAppMulti - inputFromJson, no models section");
        return false;
    }
    // won't get here, but to stop compiler warning
    return true;
}


bool SimCenterAppMulti::outputAppDataToJSON(QJsonObject &jsonObject)
{
    bool result = true;
    jsonObject["Application"] = appName;
    QJsonObject dataObj;

    dataObj["modelToRun"]=QString("RV.")+appName;

    QJsonArray modelArray;
    QJsonArray beliefArray;
    int numModel = theModels.size();
    for (int i=0; i<numModel; i++) {
        QJsonObject modelData;
        SimCenterAppWidget *theWidget = theModels.at(i);
        QLineEdit *theBelief = theBeliefs.at(i);
        beliefArray.append(QJsonValue(theBelief->text().toDouble()));
        bool res = theWidget->outputAppDataToJSON(modelData);
        if (res == false)
            result = false;
        modelArray.append(modelData);
    }
    dataObj.insert(QString("models"),modelArray);
    dataObj.insert(QString("beliefs"), beliefArray);
    jsonObject["ApplicationData"] = dataObj;

    return true;
}


bool SimCenterAppMulti::inputAppDataFromJSON(QJsonObject &jsonObject)
{

    this->clear();

    if (jsonObject.contains("ApplicationData")) {
        QJsonObject dataObject = jsonObject["ApplicationData"].toObject();

        if (dataObject.contains("models") && dataObject.contains("beliefs")) {
            QJsonArray modelObjects = dataObject["models"].toArray();
            QJsonArray beliefObjects = dataObject["beliefs"].toArray();

            int length = modelObjects.count();
            QString msg = QString("REading ") + QString::number(length);
            errorMessage(msg);
            for (int i=0; i<length; i++) {
                this->addTab();
                QJsonObject modelObject = modelObjects.at(i).toObject();
                double val = beliefObjects.at(i).toDouble();
                theBeliefs.at(i)->setText(QString::number(val));
                theModels.at(i)->inputAppDataFromJSON(modelObject);
            }
            return true;
        } else {
            errorMessage("SimCenterAppMulti - could not read models or beliefs sections");
            return false;
        }

    }
    errorMessage("SimCenterAppMulti - could not read ApplicationData section");
    return false;
}


bool SimCenterAppMulti::copyFiles(QString &destDir)
{
    bool result = true;
    int numModel = theModels.size();
    for (int i=0; i<numModel; i++) {
        SimCenterAppWidget *theWidget = theModels.at(i);
        bool res = theWidget->copyFiles(destDir);
        if (res != true) result  = false;
    }
    return result;
}


bool SimCenterAppMulti::outputCitation(QJsonObject &jsonObj)
{
        return true;
}


void SimCenterAppMulti::clear(void)
{
    theTabs->clear();
    theModels.clear();
    theBeliefs.clear();
}

int
SimCenterAppMulti::removeCurrentTab() {
    int index = theTabs->currentIndex();
    QWidget *theWidgetToBeRemoved = theTabs->currentWidget();

    // remove tab and the widgets from theBeielfs and theSelections;
    theTabs->removeTab(index);
    delete theWidgetToBeRemoved;
    theBeliefs.removeAt(index);
    theModels.removeAt(index);

    for (int i=index; i<theBeliefs.count(); i++) {
        QString label = tabLabel + QString("-") + QString::number(i+1);
        theTabs->setTabText(i,label);
    }

    return 0;
}

