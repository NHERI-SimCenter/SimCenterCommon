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

#include "SectionTitle.h"

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
#include <QDoubleValidator>

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
    
    if (theNewSelection == 0) {
      errorMessage(appName+ QString(" failed to return a valid widget when trying to add tab"));
      return -1;
    }

    QWidget *newWidget = new QWidget();
    QGridLayout *newLayout = new QGridLayout();
    newWidget->setLayout(newLayout);

    QLineEdit *newBelief = new QLineEdit;
    newBelief->setText(QString::number(1.0));
    newLayout->addWidget(new QLabel("Belief:"), 0,0);
    newLayout->addWidget(newBelief, 0,1);

    QDoubleValidator *beliefValidator = new QDoubleValidator;
    beliefValidator->setBottom(0.0);
    newBelief->setValidator(beliefValidator);

    if (theNewSelection != 0) {
        newLayout->addWidget(theNewSelection,1,0,1,4);
    }

    QLabel *newTotalBelief = new QLabel;
    newTotalBelief->setText(QString("Out of ") + QString::number(getTotalBelief()));
    newLayout->addWidget(newTotalBelief, 0, 2);

    theBeliefs.append(newBelief);
    theModels.append(theNewSelection);
    theTotalBeliefs.append(newTotalBelief);

    updateTotalBelief();

    QString label = tabLabel + QString("-") + QString::number(theBeliefs.count());
    theTabs->addTab(newWidget,label);


    newLayout->setColumnStretch(3,1);


    connect(newBelief, &QLineEdit::textChanged, this, [=](){
        this->updateTotalBelief();
      });

    return 0;
}

bool SimCenterAppMulti::outputToJSON(QJsonObject &jsonObject)
{
   jsonObject["modelToRun"]=QString("RV.")+appName;

   QJsonArray modelArray;
   int numModels = theModels.size();
   for (int i=0; i<numModels; i++) {
       QJsonObject data;
       QJsonObject modelData;
       QJsonObject appData;

       QLineEdit *theBelief = theBeliefs.at(i);
       data.insert(QString("belief"), QJsonValue(theBelief->text().toDouble()));

       SimCenterAppWidget *theWidget = theModels.at(i);
       theWidget->outputToJSON(modelData);
       // get the first item in the modelData
       QJsonObject::const_iterator iter = modelData.constBegin();
       // replace its key with "data" and write to json file
       QJsonValue val = iter.value();
       data.insert(QString("data"), val);

       theWidget->outputAppDataToJSON(appData);
       // get the first item in the appData
       QJsonObject::const_iterator it = appData.constBegin();
       QJsonObject sourceObjAppData = it.value().toObject();
       // loop over the items in the first item of appData
       QJsonObject::iterator iterAppData = sourceObjAppData.begin();
       while (iterAppData != sourceObjAppData.end()) {
           data.insert(iterAppData.key(), iterAppData.value());
           ++iterAppData;
       }
       modelArray.append(data);
   }
   jsonObject.insert(QString("models"),modelArray);
   return true;
}


bool SimCenterAppMulti::inputFromJSON(QJsonObject &jsonObject)
{
    if (jsonObject.contains("models")) {
        QJsonArray modelObjects = jsonObject["models"].toArray();
        int length = modelObjects.count();

        for (int i=0; i<length; i++) {
            this->addTab();

            QJsonObject appDataObj;
            appDataObj.insert(QString("Application"), modelObjects.at(i)["Application"]);
            appDataObj.insert(QString("ApplicationData"), modelObjects.at(i)["ApplicationData"]);
            QJsonObject appObj;
            appObj[tabLabel] = appDataObj;
            theModels.at(i)->inputAppDataFromJSON(appObj);

            QJsonObject modelDataObj = modelObjects.at(i)["data"].toObject();

            QJsonObject modelObj;
            modelObj[tabLabel] = modelDataObj;
            theModels.at(i)->inputFromJSON(modelObj);

            double belief = modelObjects.at(i)["belief"].toDouble();
            theBeliefs.at(i)->setText(QString::number(belief));

        }
        return true;
    } else {
        errorMessage("SimCenterAppMulti - inputFromJson, no models section");
        return false;
    }
}


bool SimCenterAppMulti::outputAppDataToJSON(QJsonObject &jsonObject)
{
    bool result = true;
    jsonObject["Application"] = "MultiModel";
    QJsonObject appTypeObj;
    QString applicationType;
    QJsonObject modelData;
    SimCenterAppWidget *theWidget = theModels.at(0);
    bool res = theWidget->outputAppDataToJSON(modelData);
    if (res == false)
        result = false;
    applicationType = modelData.keys().at(0);
    appTypeObj.insert(QString("appKey"),applicationType);
    jsonObject.insert(QString("ApplicationData"), appTypeObj);

    return true; // FMK may need to keep instead of using result
}


bool SimCenterAppMulti::inputAppDataFromJSON(QJsonObject &jsonObject)
{

    this->clear();

    if (jsonObject.contains("ApplicationData")) {
        QString appKey = jsonObject["ApplicationData"].toObject()["appKey"].toString();
        return true;

    } else {
        errorMessage("SimCenterAppMulti - could not read ApplicationData section");
        return false;
    }
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
    Q_UNUSED(jsonObj);
        return true;
}


void SimCenterAppMulti::clear(void)
{
    theTabs->clear();
    theModels.clear();
    theBeliefs.clear();
    theTotalBeliefs.clear();
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
    theTotalBeliefs.removeAt(index);

    for (int i=index; i<theBeliefs.count(); i++) {
        QString label = tabLabel + QString("-") + QString::number(i+1);
        theTabs->setTabText(i,label);
    }

    updateTotalBelief();

    return 0;
}

double
SimCenterAppMulti::getTotalBelief() {
    double total = 0.0;
    for (int i=0; i<theBeliefs.count(); i++) {
        QLineEdit *theBelief = theBeliefs.at(i);
        total += theBelief->text().toDouble();
    }
    return total;
}

void
SimCenterAppMulti::updateTotalBelief(void) {
    int numModels = theModels.size();
    for (int i=0; i<numModels; i++) {
        QLabel *theTotalBelief = theTotalBeliefs.at(i);
        theTotalBelief->setTextFormat(Qt::RichText);
        theTotalBelief->setText(QString("Out of ")
                                + QString::number(getTotalBelief(), 'g', 15)
                                + QString(" (i. e. , <b> ")
                                + QString::number(theBeliefs.at(i)->text().toDouble()
                                                  /getTotalBelief()*100, 'g', 6)
                                + QString("%</b>)"));
    }
}
