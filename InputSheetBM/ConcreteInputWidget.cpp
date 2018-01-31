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

#include "ConcreteInputWidget.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QList>

ConcreteInputWidget::ConcreteInputWidget(QWidget *parent) : SimCenterTableWidget(parent)
{
    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);

    QStringList headings;
    QList<int> dataTypes;
    headings << tr("Name");
    headings << tr("fc");
    headings << tr("fcu");
    headings << tr("ft");
    headings << tr("Ec");
    headings << tr("Et");
    headings << tr("epsc");
    headings << tr("epscu");
    headings << tr("nu");
    headings << tr("rho");
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
    theSpreadsheet = new SpreadsheetWidget(10, 1000, headings, dataTypes, this);

    theLayout->addWidget(theSpreadsheet);
    this->setMinimumWidth(200);
}

ConcreteInputWidget::~ConcreteInputWidget()
{

}

void
ConcreteInputWidget::outputToJSON(QJsonObject &jsonObj){
    return;
}

void
ConcreteInputWidget::outputToJSON(QJsonArray &jsonArray){
\
    int numRows = theSpreadsheet->getNumRows();
    for (int i=0; i<numRows; i++) {

        QJsonObject obj;
        QString name;
        double Ec, Et, fc, fcu, epsc, epscu, ft, rho, nu;

        // obtain info from spreadsheet
        if (theSpreadsheet->getString(i,0,name) == false || name.isEmpty())
            break;
        if (theSpreadsheet->getDouble(i,1,fc) == false)
            break;
        if (theSpreadsheet->getDouble(i,2,fcu) == false)
            break;
        if (theSpreadsheet->getDouble(i,3,ft) == false)
            break;
        if (theSpreadsheet->getDouble(i,4,Ec) == false)
            break;
        if (theSpreadsheet->getDouble(i,5,Et) == false)
            break;
        if (theSpreadsheet->getDouble(i,6,epsc) == false)
            break;
        if (theSpreadsheet->getDouble(i,7,epscu) == false)
            break;
        if (theSpreadsheet->getDouble(i,8,nu) == false)
            break;
        if (theSpreadsheet->getDouble(i,9,rho) == false)
            break;

        // now add the items to object
        obj["name"]=name;
        obj["type"]=QString(tr("concrete"));
        obj["fc"]=fc;
        obj["fcu"]=fcu;
        obj["ft"]=ft;
        obj["Ec"]=Ec;
        obj["Et"]=Et;
        obj["epsc"]=epsc;
        obj["epscu"]=epscu;
        obj["nu"]=nu;
        obj["rho"]=rho;

        // add the object to the array
        jsonArray.append(obj);
    }
}

void
ConcreteInputWidget::inputFromJSON(QJsonObject &theObject)
{
    // this has to be called one object at a time for efficiency
    // could use the rVarray above. This array will contain multiple
    // object types and could parse each to to see if corect type.
    //  BUT too slow if multiple material types,
    //  int currentRow = 0;

    QString name, type;
    double Ec, Et, fc, fcu, epsc, epscu, ft, rho, nu;
    type = theObject["type"].toString();
    if (type == QString(tr("concrete"))) {
        name = theObject["name"].toString();
        fc = theObject["fc"].toDouble();
        fcu = theObject["fcu"].toDouble();
        ft = theObject["ft"].toDouble();
        Ec = theObject["Ec"].toDouble();
        Et = theObject["Et"].toDouble();
        epsc = theObject["epsc"].toDouble();
        epscu = theObject["epscu"].toDouble();
        nu = theObject["nu"].toDouble();
        rho = theObject["rho"].toDouble();

        // add to the spreadsheet
        theSpreadsheet->setString(currentRow, 0, name);
        theSpreadsheet->setDouble(currentRow, 1, fc);
        theSpreadsheet->setDouble(currentRow, 2, fcu);
        theSpreadsheet->setDouble(currentRow, 3, ft);
        theSpreadsheet->setDouble(currentRow, 4, Ec);
        theSpreadsheet->setDouble(currentRow, 5, Et);
        theSpreadsheet->setDouble(currentRow, 6, epsc);
        theSpreadsheet->setDouble(currentRow, 7, epscu);
        theSpreadsheet->setDouble(currentRow, 8, nu);
        theSpreadsheet->setDouble(currentRow, 9, rho);

        currentRow++;
    }
}

void
ConcreteInputWidget::clear(void)
{
    currentRow = 0;
    theSpreadsheet->clear();
}
