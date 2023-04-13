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

// Written: fmckenna, kuanshi

#include "CustomPy.h"
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <QFileInfo>
#include <LineEditRV.h>
#include <ReadWriteRVJSON.h>
#include <string>
#include <sstream>
#include <iostream>
//using namespace std;
#include <QGridLayout>

#include <OpenSeesParser.h>
#include <RandomVariablesContainer.h>

//#include <InputWidgetParameters.h>

CustomPy::CustomPy(bool includeC, QWidget *parent)
  : SimCenterAppWidget(parent), responseNodes(0)
{
  includeCentroid = includeC;
  centroidNodes = NULL;
    responseNodes = NULL;
    layout = new QGridLayout();

    QLabel *label1 = new QLabel();
    label1->setText("Input Script");

   // QHBoxLayout *fileName1Layout = new QHBoxLayout();
    file1 = new QLineEdit;
    QPushButton *chooseFile1 = new QPushButton();
    chooseFile1->setText(tr("Choose"));
    connect(chooseFile1,SIGNAL(clicked()),this,SLOT(chooseFileName1()));
    layout->addWidget(label1,0,0);
    layout->addWidget(file1,0,1);
    layout->addWidget(chooseFile1,0,2);

    int rowCount = 1;
    if (includeCentroid == true) {
      QLabel *label2a = new QLabel();
      label2a->setText("Centroid Nodes:");
      centroidNodes = new QLineEdit;
      
      layout->addWidget(label2a,rowCount,0);
      layout->addWidget(centroidNodes,rowCount++,1);
    }

    QLabel *label2 = new QLabel();
    label2->setText("Response Nodes:");
    responseNodes = new QLineEdit;

    layout->addWidget(label2,rowCount,0);
    layout->addWidget(responseNodes,rowCount++,1);


    QLabel *label3 = new QLabel();
    label3->setText("Spatial Dimension:");
    ndm = new QLineEdit();
    ndm->setText("2");
    ndm->setMaximumWidth(50);
    layout->addWidget(label3,rowCount,0);
    layout->addWidget(ndm,rowCount++,1);

    QLabel *label4 = new QLabel();
    label4->setText("# DOF at Nodes:");
    ndf = new QLineEdit();
    ndf->setText("3");
    ndf->setMaximumWidth(50);
    layout->addWidget(label4,rowCount,0);
    layout->addWidget(ndf,rowCount++,1);

    RandomVariablesContainer *randomVariables = RandomVariablesContainer::getInstance();    
    dampingRatio = new LineEditRV(randomVariables);
    dampingRatio->setToolTip(tr("damping ratio, .02 = 2% damping"));
    dampingRatio->setText("0.02");    
    dampingRatio->setMaximumWidth(100);
    dampingRatio->setMinimumWidth(100);        
    layout->addWidget(new QLabel("Damping Ratio"), rowCount, 0);
    layout->addWidget(dampingRatio, rowCount++, 1);    
    
    ndf->setValidator(new QIntValidator());
    ndm->setValidator(new QIntValidator());
    ndm->setMaximumWidth(100);
    ndm->setMinimumWidth(100);
    ndf->setMaximumWidth(100);
    ndf->setMinimumWidth(100);            

    QWidget *dummyR = new QWidget();
    layout->addWidget(dummyR,5,0);
    layout->setRowStretch(6,1);
    //layout->setColumnStretch(3,1);

    this->setLayout(layout);

}

CustomPy::~CustomPy()
{
    // remove old random variables
    QStringList names;
    for (int i=0; i<varNamesAndValues.size()-1; i+=2) {
        names.append(varNamesAndValues.at(i));
    }
    RandomVariablesContainer *theRandomVariablesContainer=RandomVariablesContainer::getInstance();
    theRandomVariablesContainer->removeRandomVariables(names);
}


void CustomPy::clear(void)
{

}



bool
CustomPy::outputToJSON(QJsonObject &jsonObject)
{
    // just need to send the class type here.. type needed in object in case user screws up
    jsonObject["type"]="CustomPyInput";
    int numCentroidNodes = 0;
    QJsonArray nodeTags;
    if (centroidNodes != NULL) {
        std::string nodeString = centroidNodes->text().toStdString();
        std::string s1(nodeString); // this line is needed as nodeString cannot be passed directly to the line below!
        std::stringstream nodeStream(s1);
        int nodeTag;
        while (nodeStream >> nodeTag) {
            nodeTags.append(QJsonValue(nodeTag));
            if (nodeStream.peek() == ',')
                nodeStream.ignore();
            numCentroidNodes++;
        }
    }
    
    QJsonArray responseNodeTags;
    std::string nodeString = responseNodes->text().toStdString();
    std::string s1(nodeString); // this line is needed as nodeString cannot be passed directly to the line below!
    std::stringstream nodeStream(s1);
    int nodeTag;
    int numStories = -1;
    while (nodeStream >> nodeTag) {
        responseNodeTags.append(QJsonValue(nodeTag));
        if (numCentroidNodes == 0)
            nodeTags.append(QJsonValue(nodeTag));
        if (nodeStream.peek() == ',') {
            nodeStream.ignore();
        } else
            numStories++;
    }
    writeLineEditRV(jsonObject,"dampingRatio", dampingRatio);
    jsonObject["centroidNodes"]=nodeTags;
    jsonObject["responseNodes"]=responseNodeTags;
    jsonObject["ndm"]=ndm->text().toInt();
    jsonObject["ndf"]=ndf->text().toInt();
    //jsonObject["stories"]=numStories;

    QJsonArray rvArray;
    for (int i=0; i<varNamesAndValues.size()-1; i+=2) {
        QJsonObject rvObject;
        QString name = varNamesAndValues.at(i);
        rvObject["name"]=name;
        rvObject["value"]=QString("RV.")+name;
        rvArray.append(rvObject);
    }

    jsonObject["randomVar"]=rvArray;

    return true;
}


bool
CustomPy::inputFromJSON(QJsonObject &jsonObject)
{
    varNamesAndValues.clear();

    this->clear();
    QString stringNodes;
    if (jsonObject.contains("centroidNodes")) {
        QJsonArray nodeTags = jsonObject["centroidNodes"].toArray();
        int numCentroid = 0;
        foreach (const QJsonValue & value, nodeTags) {
            int tag = value.toInt();
            stringNodes = stringNodes + " " +  QString::number(tag);
            numCentroid++;
        }
        if (numCentroid != 0) {
            if (centroidNodes == NULL) {
                QLabel *label2a = new QLabel();
                label2a->setText("Centroid Nodes:");
                centroidNodes = new QLineEdit;
                layout->addWidget(label2a,5,0);
                layout->addWidget(centroidNodes,5,1);
            }
            centroidNodes->setText(stringNodes);
        }
    }

    if (jsonObject.contains("responseNodes")) {
        QString stringResponseNodes;
        QJsonArray nodeResponseTags = jsonObject["responseNodes"].toArray();
        foreach (const QJsonValue & value, nodeResponseTags) {
            int tag = value.toInt();
            stringResponseNodes = stringResponseNodes + " " +  QString::number(tag);
        }
        responseNodes->setText(stringResponseNodes);
    }

    // backward compatability .. response nodes used to be nodes
    if (jsonObject.contains("nodes")) {
        QString stringResponseNodes;
        QJsonArray nodeResponseTags = jsonObject["nodes"].toArray();
        foreach (const QJsonValue & value, nodeResponseTags) {
            int tag = value.toInt();
            stringResponseNodes = stringResponseNodes + " " +  QString::number(tag);
        }
        responseNodes->setText(stringResponseNodes);
    }

    if (jsonObject.contains("randomVar")) {
        QJsonArray randomVars = jsonObject["randomVar"].toArray();
        foreach (const QJsonValue & value, randomVars) {
            QJsonObject theRV = value.toObject();
            QString name = theRV["name"].toString();
            QString zero = "0";
            varNamesAndValues.append(name);
            varNamesAndValues.append(zero);
        }
    }
    readLineEditRV(jsonObject,"dampingRatio", dampingRatio);
    int theNDM = jsonObject["ndm"].toInt();
    int theNDF = 1;
    if (theNDM == 2)
      theNDF = 3;
    else if (theNDM == 3)
      theNDF = 6;
      
    if (jsonObject.contains("ndf")) {    
      theNDF = jsonObject["ndf"].toInt();
    }

    ndm->setText(QString::number(theNDM));
    ndf->setText(QString::number(theNDF));

    return true;
}


bool
CustomPy::outputAppDataToJSON(QJsonObject &jsonObject) {

    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    jsonObject["Application"] = "CustomPyInput";
    QJsonObject dataObj;
    QString fileName = file1->text();
    QFileInfo fileInfo(fileName);

    dataObj["mainScript"]= fileInfo.fileName();
    dataObj["modelPath"]=fileInfo.path();

    jsonObject["ApplicationData"] = dataObj;

    return true;
}
bool
CustomPy::inputAppDataFromJSON(QJsonObject &jsonObject) {

    //
    // from ApplicationData
    //

    if (jsonObject.contains("ApplicationData")) {
        QJsonObject dataObject = jsonObject["ApplicationData"].toObject();

        //
        // retrieve filename and path, set the QLIne Edit
        //

        QString fileName;
        QString filePath;

        if (dataObject.contains("mainScript")) {
            QJsonValue theName = dataObject["mainScript"];
            fileName = theName.toString();
        } else
            return false;

        if (dataObject.contains("modelPath")) {
            QJsonValue theName = dataObject["modelPath"];
            filePath = theName.toString();
        } else
            return false;

        file1->setText(QDir(filePath).filePath(fileName));

    } else {
        return false;
    }
    return true;
}



void
CustomPy::setFilename1(QString name1){

    // remove old random variables
    QStringList names;
    for (int i=0; i<varNamesAndValues.size()-1; i+=2) {
        names.append(varNamesAndValues.at(i));
    }
    RandomVariablesContainer *theRandomVariablesContainer=RandomVariablesContainer::getInstance();
    theRandomVariablesContainer->removeRandomVariables(names);

    // set file name & ebtry in qLine edit

    fileName1 = name1;
    file1->setText(name1);

    //
    // parse file for random variables and add them
    //

    OpenSeesParser theParser;
    varNamesAndValues = theParser.getVariables(fileName1);

    theRandomVariablesContainer->addConstantRVs(varNamesAndValues);

    return;
}

void
CustomPy::chooseFileName1(void) {
    fileName1=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
    this->setFilename1(fileName1);
}

void
CustomPy::specialCopyMainInput(QString fileName, QStringList varNames) {
    // if OpenSees or FEAP parse the file for the variables
    if (varNames.size() > 0) {
        OpenSeesParser theParser;
        theParser.writeFile(file1->text(), fileName, varNames);
    }
}


QString CustomPy::getMainInput() {
    return fileName1;
}

 bool
 CustomPy::copyFiles(QString &dirName) {

     QString fileName = file1->text();

     if (fileName.isEmpty()) {
         this->errorMessage("CustomPy - no file set");
         return false;
     }
     QFileInfo fileInfo(fileName);

     QString theFile = fileInfo.fileName();
     QString thePath = fileInfo.path();

     SimCenterAppWidget::copyPath(thePath, dirName, false);
     RandomVariablesContainer *theRandomVariablesContainer=RandomVariablesContainer::getInstance();
     QStringList varNames = theRandomVariablesContainer->getRandomVariableNames();

     // now create special copy of original main script that handles the RV
     OpenSeesParser theParser;
     QString copiedFile = dirName + QDir::separator() + theFile;
     theParser.writeFile(fileName, copiedFile, varNames);

     return true;
 }

