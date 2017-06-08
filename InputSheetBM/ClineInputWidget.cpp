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

#include "ClineInputWidget.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QList>

ClineInputWidget::ClineInputWidget(QWidget *parent) : QWidget(parent)
{
    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);

    QStringList headings;
    QList<int> dataTypes;
    headings << tr("Name");
    headings << tr("xLoc");
    headings << tr("yLoc");
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    theSpreadsheet = new SimpleSpreadsheetWidget(3, 1000, headings, dataTypes, this);

    theLayout->addWidget(theSpreadsheet);
    this->setMinimumWidth(400);
}

ClineInputWidget::~ClineInputWidget()
{

}

void
ClineInputWidget::outputToJSON(QJsonObject &jsonObj){

    QJsonArray  jsonArrayCline;
   // theSpreadsheet->outputToJSON(jsonArray);
    int numRows = theSpreadsheet->getNumRows();
    for (int i=0; i<numRows; i++) {

            QJsonObject obj;
            QString name;
            double xLoc, yLoc;
            if (theSpreadsheet->getString(i,0,name) == false)
                break;
            if (theSpreadsheet->getDouble(i,1,xLoc) == false)
                break;
            if (theSpreadsheet->getDouble(i,1,yLoc) == false)
                break;
            obj["name"]=name;
            QJsonArray coords;
            coords.append(xLoc);
            coords.append(yLoc);
            obj["location"]=coords;

             jsonArrayCline.append(obj);
        }

    jsonObj["clines"]=jsonArrayCline;

}

void
ClineInputWidget::inputFromJSON(QJsonObject &jsonObject){

}

void
ClineInputWidget::clear(void)
{


}
