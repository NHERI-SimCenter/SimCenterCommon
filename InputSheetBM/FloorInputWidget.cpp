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
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

#include "FloorInputWidget.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QList>
#include "BimClasses.h"
#include <jansson.h>

FloorInputWidget::FloorInputWidget(QWidget *parent) : SimCenterTableWidget(parent)
{
    fillingTableFromMap = false;

    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);

    QStringList headings;
    QList<int> dataTypes;
    headings << tr("Name");
    headings << tr("elevation");
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    theSpreadsheet = new SpreadsheetWidget(2, 1000, headings, dataTypes, this);

    theLayout->addWidget(theSpreadsheet);
    this->setMinimumWidth(200);
    theSpreadsheet->setTabKeyNavigation(true);

    // connect signals and slots
    connect(theSpreadsheet,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(somethingEntered(int,int,int,int)));
    connect(theSpreadsheet,SIGNAL(cellChanged(int,int)),this,SLOT(somethingChanged(int,int)));
}

FloorInputWidget::~FloorInputWidget()
{

}

bool
FloorInputWidget::outputToJSON(QJsonObject &jsonObj){

    // use jansson to set up a jansson JSON object
    json_t *objFloors = json_array();
    Floor::writeObjects(objFloors);
    json_t *objJan = json_object();

    json_object_set(objJan, "floors", objFloors);

    // dump that jansson object to a QJsonDoc and convertinto QJson object
    char *jsonText = json_dumps(objJan, JSON_COMPACT);


    QJsonDocument doc = QJsonDocument::fromJson(jsonText);

    if (doc.isNull()) {
       qDebug() << "Floor invalid JSON";
    }

    QJsonObject objQt = doc.object();
    QJsonArray theArray = objQt["floors"].toArray();

    /*
    QJsonDocument doc1(theArray);
    QString strJson(doc1.toJson(QJsonDocument::Compact));
    qDebug() << strJson;
    */

    // finally add the array to the input arg
    jsonObj["floors"]=theArray;

    // free memory json_dumps allocaed
    free(jsonText);

    return(true);
}

bool
FloorInputWidget::inputFromJSON(QJsonObject &jsonObject)
{
    fillingTableFromMap = true;
    Floor::removeAllFloor();

    // get QJson object & Convert to Jansson for Floors to read
    QJsonArray theArray = jsonObject["floors"].toArray();
    QJsonDocument doc(theArray);
    QString strJson(doc.toJson(QJsonDocument::Compact));
qDebug() << strJson;

    json_t *janssonObj = json_object();
    json_error_t error;
    janssonObj = json_loads(strJson.toStdString().c_str(), 0, &error);
    qDebug() << json_dumps(janssonObj, JSON_COMPACT);
    Floor::readObjects(janssonObj);

    QString name;
    double height;
    string rvHeight;

    int currentRow = 0;
    std::map<string, Floor *>::iterator it = Floor::theFloors.begin();
    while (it != Floor::theFloors.end()) {
        Floor *theFloor = it->second;
        QString name(QString::fromStdString((theFloor->name)));
        theSpreadsheet->setString(currentRow, 0, name);
        if (theFloor->rvHeight != 0) {
            QString name(QString::fromStdString(*(theFloor->rvHeight)));
            theSpreadsheet->setString(currentRow, 1, name);
        } else
            theSpreadsheet->setDouble(currentRow, 1, theFloor->height);
        it++;
        currentRow++;

    }
    /*
    int currentRow = 0;
    QString name;
    double zLoc;

    //
    // get the cline data (a json array) from the object, and for every
    // object in the array, get the values and add to the spreadsheet
    //

    QJsonArray theArray = jsonObject["floors"].toArray();
    foreach (const QJsonValue &theValue, theArray) {
        // get values
        QJsonObject theObject = theValue.toObject();
        name = theObject["name"].toString();
        zLoc = theObject["elevation"].toDouble();

        // add to the spreadsheet
        theSpreadsheet->setString(currentRow, 0, name);
        theSpreadsheet->setDouble(currentRow, 1, zLoc);

        currentRow++;
    }
    */
    fillingTableFromMap = false;
    return(true);
}

void
FloorInputWidget::clear(void)
{
    theSpreadsheet->clear();
}

void
FloorInputWidget::somethingChanged(int row, int column) {

    if (fillingTableFromMap == true) {
        return;
    }

    QString name;
    double height = 0.;
    QString rvHeight;
    string *rvHeightString = NULL;

    QTableWidgetItem *theName = theSpreadsheet->item(row, 0);
    QTableWidgetItem *theHeight = theSpreadsheet->item(row,1);


    //
    // make sure name exists and is unique
    //   if not unique reset to last value and return w/o doing anything
    //

    if (theName == NULL) {
        qDebug() << "NO CELL";
        return; // do not add Floor until all data exists
    }
    if (theSpreadsheet->getString(row,0,name) == false || name == QString("")) {
        qDebug() << "NO NAME";
        return; // problem with name
    }

    // check name is unique, if not set string to what it was before entry
    if (column == 0) {
        Floor *existingFloor = Floor::getFloor(name.toStdString());
        if (existingFloor != NULL) {
            //theSpreadsheet->takeItem(row,0);
            //QString blankString("");
            theSpreadsheet->setString(row, 0, currentName);
            return;
        }
    }

    //
    // if height exists, update map in Floors with new entry
    //

    if (theHeight == NULL)
        return;

    if (theSpreadsheet->getDouble(row,1,height) == false) {
        if (theSpreadsheet->getString(row,1,rvHeight) == false) {
            qDebug() << "NO HEIGHT";
            return;
        } else {
            rvHeightString = new string(rvHeight.toStdString());
        }
    }


    if (column == 0) { // add new floor, remove old if renamed
        if (currentName != name)
            Floor::removeFloor(currentName.toStdString());
        Floor::addFloor(name.toStdString(), height, rvHeightString);
    } else // reset properties on existing floor
        Floor::newFloorProperties(name.toStdString(), height, rvHeightString);
}

void
FloorInputWidget::somethingEntered(int row, int column, int row2, int col2) {

    if (column == 0) {
        if (theSpreadsheet->getString(row, column, currentName) == false)
            currentName.clear();
   } else
        currentName.clear();

}
