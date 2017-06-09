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

#include "SteelInputWidget.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QList>

SteelInputWidget::SteelInputWidget(QWidget *parent) : QWidget(parent)
{
    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);

    QStringList headings;
    QList<int> dataTypes;
    headings << tr("Name");
    headings << tr("E");
    headings << tr("Fy");
    headings << tr("Fu");
    headings << tr("nu");
    headings << tr("rho");
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    theSpreadsheet = new SimpleSpreadsheetWidget(6, 1000, headings, dataTypes, this);

    theLayout->addWidget(theSpreadsheet);
    this->setMinimumWidth(200);
}

SteelInputWidget::~SteelInputWidget()
{

}

void
SteelInputWidget::outputToJSON(QJsonArray &jsonArray){

    // create a json array and for each row add a json object to it

    int numRows = theSpreadsheet->getNumRows();
    for (int i=0; i<numRows; i++) {

        QJsonObject obj;
        QString name;
        double E, Fy, Fu, rho, nu;

        // obtain info from spreadsheet
        if (theSpreadsheet->getString(i,0,name) == false)
            break;
        if (theSpreadsheet->getDouble(i,1,E) == false)
            break;
        if (theSpreadsheet->getDouble(i,2,Fy) == false)
            break;
        if (theSpreadsheet->getDouble(i,3,Fu) == false)
            break;
        if (theSpreadsheet->getDouble(i,4,nu) == false)
            break;
        if (theSpreadsheet->getDouble(i,5,rho) == false)
            break;

        // now add the items to object
        obj["name"]=name;
        obj["type"]=QString(tr("steel"));
        obj["fy"]=Fy;
        obj["fu"]=Fu;
        obj["E"]=E;
        obj["nu"]=nu;
        obj["rho"]=rho;

        // add the object to the array
        jsonArray.append(obj);
    }
}

void
SteelInputWidget::inputFromJSON(QJsonObject &theObject)
{
    // this has to be called one object at a time for efficiency
    // could use the rVarray above. This array will contain multiple
    // object types and could parse each to to see if corect type.
    //  BUT too slow if multiple material types,
    //  int currentRow = 0;

    QString name, type;
    double E, Fy, Fu, rho, nu;
    type = theObject["type"].toString();
    if (type == QString(tr("steel"))) {
        name = theObject["name"].toString();
        E = theObject["E"].toDouble();
        Fy = theObject["fy"].toDouble();
        Fu = theObject["fu"].toDouble();
        nu = theObject["nu"].toDouble();
        rho = theObject["rho"].toDouble();

        // add to the spreadsheet
        theSpreadsheet->setString(currentRow, 0, name);
        theSpreadsheet->setDouble(currentRow, 1, E);
        theSpreadsheet->setDouble(currentRow, 2, Fy);
        theSpreadsheet->setDouble(currentRow, 3, Fu);
        theSpreadsheet->setDouble(currentRow, 4, nu);
        theSpreadsheet->setDouble(currentRow, 5, rho);

        currentRow++;
    }
}

void
SteelInputWidget::clear(void)
{
    currentRow = 0;
    theSpreadsheet->clear();
}
