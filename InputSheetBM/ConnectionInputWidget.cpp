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

// Written: mmanning

#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QList>

#include "ConnectionInputWidget.h"

ConnectionInputWidget::ConnectionInputWidget(SimCenterWidget *parent) : SimCenterTableWidget(parent)
{

    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);

    QStringList headings;
    QList<int> dataTypes;
    headings << tr("Name");
    headings << tr("Type");
    headings << tr("Material");
    headings << tr("Thickness");

    headings << tr("Weld length baseplate");
    headings << tr("Weld length beam");
    headings << tr("Weld length brace");
    headings << tr("Weld length column");

    headings << tr("Thickness baseplate");
    headings << tr("Workpoint depth");
    headings << tr("Tab depth");
    headings << tr("Tab width");

    // Bolt Group
    headings << tr("Edge distance depth");
    headings << tr("Edge distance width");
    headings << tr("Number of bolts depth");
    headings << tr("Number of bolts width");
    headings << tr("Pattern");

    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QDouble;

    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;

    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;

    // Bolt Group
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QString;

    theSpreadsheet = new SpreadsheetWidget(16, 1000, headings, dataTypes, this);

    theLayout->addWidget(theSpreadsheet);

    this->setMinimumWidth(500);
}



ConnectionInputWidget::~ConnectionInputWidget()
{

}

void
ConnectionInputWidget::outputToJSON(QJsonObject &jsonObj){


    QJsonArray  jsonArray;
    int numRows = theSpreadsheet->getNumRows();
    for (int i=0; i<numRows; i++) {

        QJsonObject obj;
        QString name, type, material;
        double thickness, weld_len_brace, weld_len_beam, weld_len_column, weld_len_baseplate, thickness_baseplate, workpoint_depth, tab_depth, tab_width;

        // bolt group
        QString pattern;
        double edge_dist_depth, edge_dist_width, num_bolts_depth, num_bolts_width;

        // obtain info from spreadsheet
        if (theSpreadsheet->getString(i,0,name) == false || name.isEmpty())
            break;
        if (theSpreadsheet->getString(i,1,type) == false)
            break;
        if (theSpreadsheet->getString(i,2, material) == false)
            break;
        if (theSpreadsheet->getDouble(i,3,thickness) == false)
            break;

        if (theSpreadsheet->getDouble(i,4,weld_len_baseplate) == false)
            break;
        if (theSpreadsheet->getDouble(i,5,weld_len_beam) == false)
            break;
        if (theSpreadsheet->getDouble(i,6,weld_len_brace) == false)
            break;
        if (theSpreadsheet->getDouble(i,7,weld_len_column) == false)
            break;

        if (theSpreadsheet->getDouble(i,8,thickness_baseplate) == false)
            break;
        if (theSpreadsheet->getDouble(i,9,workpoint_depth) == false)
            break;
        if (theSpreadsheet->getDouble(i,10,tab_depth) == false)
            break;
        if (theSpreadsheet->getDouble(i,11,tab_width) == false)
            break;

        // bolt group
        if (theSpreadsheet->getDouble(i,12,edge_dist_depth) == false)
            break;
        if (theSpreadsheet->getDouble(i,13,edge_dist_width) == false)
            break;
        if (theSpreadsheet->getDouble(i,14,num_bolts_depth) == false)
            break;
        if (theSpreadsheet->getDouble(i,15,num_bolts_width) == false)
            break;
        if (theSpreadsheet->getString(i,16,pattern) == false || name.isEmpty())
            break;

        // now add the items to object
        obj["name"]=name;
        obj["type"]=type;
        obj["material"]=material;
        obj["thickness"]=thickness;

        obj["weld length baseplate"]=weld_len_baseplate;
        obj["weld length beam"]=weld_len_beam;
        obj["weld length brace"]=weld_len_brace;
        obj["weld length column"]=weld_len_column;

        //bolt group
        QJsonObject boltgroup;

        boltgroup["edge distance depth"]=edge_dist_depth;
        boltgroup["edge distance width"]=edge_dist_width;
        boltgroup["number of bolts depth"]=num_bolts_depth;
        boltgroup["number of bolts width"]=num_bolts_width;
        boltgroup["bar area corner"]=pattern;

        obj["bolt group"] = boltgroup;


        // add the object to the array
       jsonArray.append(obj);

    }

    // add the object
    jsonObj["connections"] = jsonArray;

}

void
ConnectionInputWidget::inputFromJSON(QJsonObject &jsonObject){

    int currentRow = 0;

    QString name, type, material;
    double thickness, weld_len_brace, weld_len_beam, weld_len_column, weld_len_baseplate, thickness_baseplate, workpoint_depth, tab_depth, tab_width;

    // bolt group
    QString pattern;
    double edge_dist_depth, edge_dist_width, num_bolts_depth, num_bolts_width;

    //
    //

    QJsonArray theArray = jsonObject["connections"].toArray();
    foreach (const QJsonValue &theValue, theArray) {
        // get values
        QJsonObject theObject = theValue.toObject();

        name = theObject["name"].toString();
        type = theObject["type"].toString();
        material = theObject["material"].toString();
        thickness = theObject["thickness"].toDouble();

        weld_len_baseplate = theObject["weld length baseplate"].toDouble();
        weld_len_beam = theObject["weld length beam"].toDouble();
        weld_len_brace = theObject["weld length brace"].toDouble();
        weld_len_column = theObject["weld length column"].toDouble();

        thickness_baseplate = theObject["thickness baseplate"].toDouble();
        workpoint_depth = theObject["workpoint depth"].toDouble();
        tab_depth = theObject["tab depth"].toDouble();
        tab_width = theObject["tab width"].toDouble();

        // bolt group
        QJsonValue theBGValue = theObject["bolt group"];
        QJsonObject bgObject = theBGValue.toObject();

        edge_dist_depth = theObject["edge distance depth"].toDouble();
        edge_dist_width = theObject["edge distance width"].toDouble();
        num_bolts_depth = theObject["number of bolts depth"].toDouble();
        num_bolts_width = theObject["number of bolts width"].toDouble();
        pattern = theObject["pattern"].toString();


        // add to the spreadsheet
        theSpreadsheet->setString(currentRow, 0, name);
        theSpreadsheet->setString(currentRow, 1, type);
        theSpreadsheet->setString(currentRow, 2, material);
        theSpreadsheet->setDouble(currentRow, 3, thickness);

        theSpreadsheet->setDouble(currentRow, 4, weld_len_baseplate);
        theSpreadsheet->setDouble(currentRow, 5, weld_len_beam);
        theSpreadsheet->setDouble(currentRow, 6, weld_len_brace);
        theSpreadsheet->setDouble(currentRow, 7, weld_len_column);

        theSpreadsheet->setDouble(currentRow, 8, thickness_baseplate);
        theSpreadsheet->setDouble(currentRow, 9, workpoint_depth);
        theSpreadsheet->setDouble(currentRow, 10, tab_depth);
        theSpreadsheet->setDouble(currentRow, 11, tab_width);

        // bolt group
        theSpreadsheet->setDouble(currentRow, 12, edge_dist_depth);
        theSpreadsheet->setDouble(currentRow, 13, edge_dist_width);
        theSpreadsheet->setDouble(currentRow, 14, num_bolts_depth);
        theSpreadsheet->setDouble(currentRow, 15, num_bolts_width);
        theSpreadsheet->setString(currentRow, 16, pattern);

        currentRow++;
    }


}


void
ConnectionInputWidget::clear(void)
{
    theSpreadsheet->clear();
}
