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
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QList>
#include <sstream>
#include <string>

#include "PointInputWidget.h"




PointInputWidget::PointInputWidget(SimCenterWidget *parent) : SimCenterTableWidget(parent)
{

    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);

    QStringList headings;
    QList<int> dataTypes;
    headings << tr("Name");
    headings << tr("Type");
    headings << tr("Mass");

    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QString;


    theSpreadsheet = new SpreadsheetWidget(3, 1000, headings, dataTypes, this);

    theLayout->addWidget(theSpreadsheet);

    this->setMinimumWidth(500);

}





PointInputWidget::~PointInputWidget()
{

}

void
PointInputWidget::outputToJSON(QJsonObject &jsonObj){
    using namespace std;

    QJsonArray  jsonArray;
    int numRows = theSpreadsheet->getNumRows();
    for (int i=0; i<numRows; i++) {

        QJsonObject obj;
        QString name, type, mass;

        // obtain info from spreadsheet
        if (theSpreadsheet->getString(i,0,name) == false || name.isEmpty())
            break;
        if (theSpreadsheet->getString(i,1,type) == false)
            break;

        // treat mass, an array of reals, as a string for now

        if (theSpreadsheet->getString(i,2, mass) == false)
            break;

        // convert string of numbers to a json array
        mass.replace("[", "");
        mass.replace("]", "");
        mass.replace(",", " ");

        QJsonArray massArray;
        istringstream iss(mass.toStdString());
        do
        {
            std::string subs;
            iss >> subs;
            massArray.append(std::atof(subs.c_str()));
        } while (iss);


        // now add the items to object
        obj["name"]=name;
        obj["type"]=type;
        obj["mass"]=massArray;

        // add the object to the array
       jsonArray.append(obj);

    }

    // add the object
    jsonObj["points"] = jsonArray;

}

void
PointInputWidget::inputFromJSON(QJsonObject &jsonObject){

    int currentRow = 0;

    // treat mass, an array of reals, as a string for now
    QString name, type, mass;
    //QArrayData foo;

    mass.append("[ ");
    QJsonArray theArray = jsonObject["points"].toArray();
    foreach (const QJsonValue &theValue, theArray) {
        // get values
        QJsonObject theObject = theValue.toObject();

        name = theObject["name"].toString();
        type = theObject["type"].toString();

        QJsonValue theMassValue = theObject["mass"];
        QJsonArray massArray = theMassValue.toArray();

        int i = 0;
        double massdouble;
        QString massstring;
        foreach (const QJsonValue &tmpval, massArray) {
            massdouble = massArray.at(i).toDouble();
            massstring = QString::number(massdouble);
            mass.append(massstring);
            if (  massArray.at(i) != massArray.last() ) { mass.append(", ");  }
            i++;
        }
        mass.append(" ]");

        // add to the spreadsheet
        theSpreadsheet->setString(currentRow, 0, name);
        theSpreadsheet->setString(currentRow, 1, type);
        theSpreadsheet->setString(currentRow, 2, mass);

        currentRow++;
    }


}


void
PointInputWidget::clear(void)
{
    theSpreadsheet->clear();
}


