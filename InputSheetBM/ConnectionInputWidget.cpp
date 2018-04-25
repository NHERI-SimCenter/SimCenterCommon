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

ConnectionInputWidget::ConnectionInputWidget(QString connectionType, SimCenterWidget *parent) : SimCenterTableWidget(parent)
{
    if (connectionType == "gusset with foldline") { this->gussetConnection(); }
    if (connectionType == "gusset without foldline") { this->gussetConnection(); }
    if (connectionType == "baseplate gusset with foldline") { this->baseplateConnection(); }
    if (connectionType == "baseplate gusset without foldline") { this->baseplateConnection(); }
    if (connectionType == "welded shear tab") { this->weldedShearTab(); }
    if (connectionType == "bolted shear tab") { this->boltedShearTab(); }

    this->connectionType = connectionType;
    this->setupSpreadsheet();
}

ConnectionInputWidget::ConnectionInputWidget(QStringList headings, QList<int> dataTypes, QString connectionType, SimCenterWidget *parent) : SimCenterTableWidget(parent)
{
    this->tableHeader = headings;
    this->dataTypes = dataTypes;
    this->connectionType = connectionType;

    this->setupSpreadsheet();
}

ConnectionInputWidget::~ConnectionInputWidget()
{

}

bool
ConnectionInputWidget::outputToJSON(QJsonObject &jsonObj){return(true);}

bool
ConnectionInputWidget::outputToJSON(QJsonArray &jsonArray){
    int numRows = theSpreadsheet->getNumRows();

    // for each row of the spreadsheet
    for (int i=0; i<numRows; i++) {
        QJsonObject obj, boltGroup;
        // for each field defined in private member var tableHeader
        // and the respective data type defined in private member var dataTypes
        // (tableHeader and dataTypes defined in constructor function)
        for (int j=0; j<tableHeader.size(); j++) {
            QString fieldName = tableHeader[j].toLower();

            if(dataTypes[j] == 0) {
                //string
                QString tmpString;
                if (theSpreadsheet->getString(i,j,tmpString) == false || tmpString.isEmpty()) {
                    qDebug() << "no value for " << fieldName << " in row " << i;
                    // TODO: need to actually break out of this loop
                    return(true);
                }
                if (fieldName.startsWith("bolt group ") == true) {
                    boltGroup[fieldName.remove(0,11)] = tmpString;
                } else {
                    obj[fieldName] = tmpString;
                }
            } else if (dataTypes[j] == 1) {
                //double
                double tmpDouble;
                if (theSpreadsheet->getDouble(i,j,tmpDouble) == false) {
                    qDebug() << "no value for " << fieldName << " in row " << i;
                    // TODO: need to actually break out of this loop
                    return(true);
                }
                if (fieldName.startsWith("bolt group ") == true) {
                    boltGroup[fieldName.remove(0,11)] = tmpDouble;
                } else {
                    obj[fieldName] = tmpDouble;
                }
            }
        }

        if(!boltGroup.isEmpty()) { obj["bolt group"] = boltGroup; }
        obj["type"] = connectionType;
        jsonArray.append(obj);
    }
    return(true);
}

bool
ConnectionInputWidget::inputFromJSON(QJsonObject &jsonObject){return(true);}

bool
ConnectionInputWidget::inputFromJSON(QJsonArray &jsonArray) {
    int currentRow = 0;

    foreach (const QJsonValue &theValue, jsonArray) {
        QJsonObject theObject = theValue.toObject();
        if (theObject["type"] == connectionType) {
            QJsonObject boltGroup = theObject["bolt group"].toObject();

            for (int j=0; j<tableHeader.size(); j++) {
                QString fieldName = tableHeader[j].toLower();

                if(dataTypes[j] == 0) {
                    QString tmpString;
                    if(fieldName.startsWith("bolt group ") == true) {
                        tmpString = boltGroup[fieldName.remove(0,11)].toString();
                    } else {
                        tmpString = theObject[fieldName].toString();
                    }
                    theSpreadsheet->setString(currentRow, j, tmpString);
                } else if (dataTypes[j] == 1) {
                    double tmpDouble;
                    if(fieldName.startsWith("bolt group ") == true) {
                        tmpDouble = boltGroup[fieldName.remove(0,11)].toDouble();
                    } else {
                        tmpDouble = theObject[fieldName].toDouble();
                    }
                    theSpreadsheet->setDouble(currentRow, j, tmpDouble);
                }
            }
            currentRow++;
        }
    }
    return(true);
}

void
ConnectionInputWidget::clear(void)
{
    theSpreadsheet->clear();
}

void
ConnectionInputWidget::setupSpreadsheet() {
    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);
    theSpreadsheet = new SpreadsheetWidget(tableHeader.size(), 1000, tableHeader, dataTypes, this);
    theLayout->addWidget(theSpreadsheet);
    this->setMinimumWidth(500);
}

void
ConnectionInputWidget::gussetConnection() {
    QStringList gussetHeadings;
    QList<int> gussetDataTypes;

    gussetHeadings << tr("Name");
    gussetDataTypes << SIMPLESPREADSHEET_QString;
    gussetHeadings << tr("material");
    gussetDataTypes << SIMPLESPREADSHEET_QDouble;
    gussetHeadings << tr("thickness");
    gussetDataTypes << SIMPLESPREADSHEET_QDouble;
    gussetHeadings << tr("weld length brace");
    gussetDataTypes << SIMPLESPREADSHEET_QDouble;
    gussetHeadings << tr("weld length column");
    gussetDataTypes << SIMPLESPREADSHEET_QDouble;
    gussetHeadings << tr("weld length beam");
    gussetDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = gussetHeadings;
    this->dataTypes = gussetDataTypes;
}

void
ConnectionInputWidget::baseplateConnection() {
    QStringList baseplateHeadings;
    QList<int> baseplateDataTypes;

    baseplateHeadings << tr("Name");
    baseplateDataTypes << SIMPLESPREADSHEET_QString;
    baseplateHeadings << tr("material");
    baseplateDataTypes << SIMPLESPREADSHEET_QDouble;
    baseplateHeadings << tr("thickness");
    baseplateDataTypes << SIMPLESPREADSHEET_QDouble;
    baseplateHeadings << tr("weld length brace");
    baseplateDataTypes << SIMPLESPREADSHEET_QDouble;
    baseplateHeadings << tr("weld length column");
    baseplateDataTypes << SIMPLESPREADSHEET_QDouble;
    baseplateHeadings << tr("weld length baseplate");
    baseplateDataTypes << SIMPLESPREADSHEET_QDouble;
    baseplateHeadings << tr("thickness baseplate");
    baseplateDataTypes << SIMPLESPREADSHEET_QDouble;
    baseplateHeadings << tr("workpoint depth");
    baseplateDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = baseplateHeadings;
    this->dataTypes = baseplateDataTypes;
}

void
ConnectionInputWidget::weldedShearTab() {
    QStringList weldedShearTabHeadings;
    QList<int> weldedShearTabDataTypes;

    weldedShearTabHeadings << tr("Name");
    weldedShearTabDataTypes << SIMPLESPREADSHEET_QString;
    weldedShearTabHeadings << tr("material");
    weldedShearTabDataTypes << SIMPLESPREADSHEET_QDouble;
    weldedShearTabHeadings << tr("thickness");
    weldedShearTabDataTypes << SIMPLESPREADSHEET_QDouble;
    weldedShearTabHeadings << tr("tab width");
    weldedShearTabDataTypes << SIMPLESPREADSHEET_QDouble;
    weldedShearTabHeadings << tr("tab depth");
    weldedShearTabDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = weldedShearTabHeadings;
    this->dataTypes = weldedShearTabDataTypes;
}

void
ConnectionInputWidget::boltedShearTab() {
    QStringList boltedShearTabHeadings;
    QList<int> boltedShearTabDataTypes;

    boltedShearTabHeadings << tr("Name");
    boltedShearTabDataTypes << SIMPLESPREADSHEET_QString;
    boltedShearTabHeadings << tr("material");
    boltedShearTabDataTypes << SIMPLESPREADSHEET_QDouble;
    boltedShearTabHeadings << tr("thickness");
    boltedShearTabDataTypes << SIMPLESPREADSHEET_QDouble;
    boltedShearTabHeadings << tr("tab width");
    boltedShearTabDataTypes << SIMPLESPREADSHEET_QDouble;
    boltedShearTabHeadings << tr("tab depth");
    boltedShearTabDataTypes << SIMPLESPREADSHEET_QDouble;
    boltedShearTabHeadings << tr("bolt group edge distance depth");
    boltedShearTabDataTypes << SIMPLESPREADSHEET_QDouble;
    boltedShearTabHeadings << tr("bolt group edge distance width");
    boltedShearTabDataTypes << SIMPLESPREADSHEET_QDouble;
    boltedShearTabHeadings << tr("bolt group number of bolts depth");
    boltedShearTabDataTypes << SIMPLESPREADSHEET_QDouble;
    boltedShearTabHeadings << tr("bolt group number of bolts width");
    boltedShearTabDataTypes << SIMPLESPREADSHEET_QDouble;
    boltedShearTabHeadings << tr("bolt group pattern");
    boltedShearTabDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = boltedShearTabHeadings;
    this->dataTypes = boltedShearTabDataTypes;
}
