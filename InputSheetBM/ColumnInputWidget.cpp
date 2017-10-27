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

#include "ColumnInputWidget.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QList>

ColumnInputWidget::ColumnInputWidget(QWidget *parent) : SimCenterWidget(parent)
{
    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);

    QStringList headings;
    QList<int> dataTypes;
    headings << tr("Name");
    headings << tr("CLine");
    headings << tr("Floor1");
    headings << tr("Floor2");
    headings << tr("section");
    headings << tr("ratio_start");
    headings << tr("ratio_end");
    headings << tr("angle");
    headings << tr("section");
    headings << tr("ratio_start");
    headings << tr("ratio_end");
    headings << tr("angle");
    headings << tr("section");
    headings << tr("ratio_start");
    headings << tr("ratio_end");
    headings << tr("angle");
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    theSpreadsheet = new SimpleSpreadsheetWidget(16, 1000, headings, dataTypes, this);

    theLayout->addWidget(theSpreadsheet);

    this->setMinimumWidth(500);
}

ColumnInputWidget::~ColumnInputWidget()
{

}

void
ColumnInputWidget::outputToJSON(QJsonObject &jsonObj){

     // create a json array and for each row add a json object to it
    QJsonArray  jsonArray;

    int numRows = theSpreadsheet->getNumRows();
    for (int i=0; i<numRows; i++) {

        QJsonObject obj;
        QString name;
        QString floor1, floor2;
        QString cline;
        QString section1, section2, section3;
        double ang, ratS, ratE;
        if (theSpreadsheet->getString(i,0,name) == false || name.isEmpty())
            break;
        if (theSpreadsheet->getString(i,1,cline) == false)
            break;
        if (theSpreadsheet->getString(i,2,floor1) == false)
            break;
        if (theSpreadsheet->getString(i,3,floor2) == false)
            break;
        if (theSpreadsheet->getString(i,4,section1) == false)
            break;
        if (theSpreadsheet->getDouble(i,5,ratS) == false)
            break;
        if (theSpreadsheet->getDouble(i,6,ratE) == false)
            break;
        if (theSpreadsheet->getDouble(i,7,ang) == false)
            break;

        obj["name"]=name;
        obj["cline"]=cline;
        QJsonArray floors;
        floors.append(floor1);
        floors.append(floor2);
        obj["floor"] =floors;

        QJsonArray segments;
        QJsonObject segment1;
        QJsonArray ratios1;

        segment1["section"]=section1;
        segment1["angle"]=ang;
        ratios1.append(ratS);
        ratios1.append(ratE);
        segment1["ratio"] = ratios1;

        segments.append(segment1);

        if ((theSpreadsheet->getString(i,8,section1) == true) &&
                (theSpreadsheet->getDouble(i,9,ratS) == true) &&
                (theSpreadsheet->getDouble(i,10,ratE) == true) &&
                (theSpreadsheet->getDouble(i,11,ang) == true))  {

            QJsonObject segment2;
            QJsonArray ratios2;

            segment2["section"]=section1;
            segment2["angle"]=ang;
            ratios2.append(ratS);
            ratios2.append(ratE);
            segment2["ratio"] = ratios2;

            segments.append(segment2);
        }

        if ((theSpreadsheet->getString(i,12,section1) == true) &&
                (theSpreadsheet->getDouble(i,13,ratS) == true) &&
                (theSpreadsheet->getDouble(i,14,ratE) == true) &&
                (theSpreadsheet->getDouble(i,15,ang) == true))  {

            QJsonObject segment3;
            QJsonArray ratios3;

            segment3["section"]=section1;
            segment3["angle"]=ang;
            ratios3.append(ratS);
            ratios3.append(ratE);
            segment3["ratio"] = ratios3;

            segments.append(segment3);
        }


        obj["segment"]=segments;

        // add the object to the array
        jsonArray.append(obj);

    }

    // finally add the array to the input arg
    jsonObj["columns"]=jsonArray;
}

void
ColumnInputWidget::inputFromJSON(QJsonObject &jsonObject){

    QString name;
    QJsonArray clineArray;
    QJsonArray floorArray;
    QString clineValue, floor1Value, floor2Value;
    int currentRow = 0;

    //
    // get the cline data (a json array) from the object, and for every
    // object in the array, get the values and add to the spreadsheet
    //

    QJsonArray theArray = jsonObject["columns"].toArray();
    foreach (const QJsonValue &theValue, theArray) {
        // get values
        QJsonObject theObject = theValue.toObject();
        QJsonValue theColumnValue = theObject["name"];
        name = theColumnValue.toString();

        QJsonValue theClineValue = theObject["cline"];
        clineArray = theClineValue.toArray();
        QJsonValue cValue = clineArray.at(0);
        clineValue = cValue.toString();

        QJsonValue theFloorValue = theObject["floor"];
        floorArray = theFloorValue.toArray();
        QJsonValue f1Value = floorArray.at(0);
        QJsonValue f2Value = floorArray.at(1);
        floor1Value = f1Value.toString();
        floor2Value = f2Value.toString();

        QJsonArray theSegmentArray = theObject["segment"].toArray();

        // add to the spreadsheet
        theSpreadsheet->setString(currentRow, 0, name);
        theSpreadsheet->setString(currentRow, 1, clineValue);
        theSpreadsheet->setString(currentRow, 2, floor1Value);
        theSpreadsheet->setString(currentRow, 3, floor2Value);

        QString section;
        double angle;
        double ratio1Value, ratio2Value;
        int currentSection = 0;
        int offset = 0;

        foreach (const QJsonValue &theValue, theSegmentArray) {

            // get values
            QJsonObject theObject = theValue.toObject();

            section = theObject["section"].toString();
            angle = theObject["angle"].toDouble();

            // ratio can be a two member array or single value
            if ( theObject["ratio"].isArray() ) {
                QJsonArray theRatioArray = theObject["ratio"].toArray();
                QJsonValue r1Value = theRatioArray.at(0);
                QJsonValue r2Value = theRatioArray.at(1);
                ratio1Value = r1Value.toDouble();
                ratio2Value = r2Value.toDouble();
            }
            else {
                QJsonValue r1Value = theObject["ratio"];
                ratio1Value = r1Value.toDouble();
                // TODO: correect default for ratio2?????
                ratio2Value = 0.0;
            }

            // all segment sections on the same row so must shift for each new set
            offset = (currentSection * 4) + 4;
            theSpreadsheet->setString(currentRow, offset, section);
            theSpreadsheet->setDouble(currentRow, (offset + 1), ratio1Value);
            theSpreadsheet->setDouble(currentRow, (offset + 2), ratio2Value);
            theSpreadsheet->setDouble(currentRow, (offset + 3), angle);

            currentSection++;
        }
        currentRow++;
    }

}

void
ColumnInputWidget::clear(void)
{
    theSpreadsheet->clear();
}
