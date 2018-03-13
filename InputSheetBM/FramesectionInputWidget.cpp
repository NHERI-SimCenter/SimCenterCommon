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

#include "FramesectionInputWidget.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QList>

FramesectionInputWidget::FramesectionInputWidget(QString framesectionType, SimCenterWidget *parent) : SimCenterTableWidget(parent)
{
    if (framesectionType == "concrete rectangular column") { this->concreteRectColFS(); }
    if (framesectionType == "concrete box column") { this->concreteBoxColFS(); }
    if (framesectionType == "concrete circular column") { this->concreteCircColFS(); }
    if (framesectionType == "concrete pipe column") { this->concretePipeColFS(); }

    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);

    theSpreadsheet = new SpreadsheetWidget(this->tableHeader.size(), 1000, this->tableHeader, this->dataTypes, this);
    theLayout->addWidget(theSpreadsheet);
    this->setMinimumWidth(500);
}

FramesectionInputWidget::FramesectionInputWidget(QStringList headings, QList<int> dataTypes, QString framesectionType, SimCenterWidget *parent) : SimCenterTableWidget(parent)
{
    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);

    theSpreadsheet = new SpreadsheetWidget(headings.size(), 1000, headings, dataTypes, this);
    theLayout->addWidget(theSpreadsheet);

    this->setMinimumWidth(500);
    this->tableHeader = headings;
    this->dataTypes = dataTypes;
    this->framesectionType = framesectionType;
}


FramesectionInputWidget::~FramesectionInputWidget()
{

}

void FramesectionInputWidget::concreteRectColFS() {
    QStringList concreteRectColFSHeadings;
    QList<int> concreteRectColFSDataTypes;
    this->framesectionType = "concrete rectangular column";

    concreteRectColFSHeadings << tr("Name");
    concreteRectColFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteRectColFSHeadings << tr("Material");
    concreteRectColFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteRectColFSHeadings << tr("depth");
    concreteRectColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectColFSHeadings << tr("width");
    concreteRectColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectColFSHeadings << tr("longitudinal rebar material");
    concreteRectColFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteRectColFSHeadings << tr("longitudinal rebar material corner");
    concreteRectColFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteRectColFSHeadings << tr("longitudinal rebar num bars depth");
    concreteRectColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectColFSHeadings << tr("longitudinal rebar num bars width");
    concreteRectColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectColFSHeadings << tr("longitudinal rebar bar area");
    concreteRectColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectColFSHeadings << tr("longitudinal rebar bar area corner");
    concreteRectColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectColFSHeadings << tr("longitudinal rebar cover");
    concreteRectColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectColFSHeadings << tr("transverse rebar material");
    concreteRectColFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteRectColFSHeadings << tr("transverse rebar num bars depth");
    concreteRectColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectColFSHeadings << tr("transverse rebar num bars width");
    concreteRectColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectColFSHeadings << tr("transverse rebar bar area");
    concreteRectColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectColFSHeadings << tr("transverse rebar spacing");
    concreteRectColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectColFSHeadings << tr("mass per length");
    concreteRectColFSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = concreteRectColFSHeadings;
    this->dataTypes = concreteRectColFSDataTypes;
}

// same as concrete rectangular column framesection with
// two field additions: flange thickness (double), web thickness (double)
void FramesectionInputWidget::concreteBoxColFS() {
    QStringList concreteBoxColFSHeadings;
    QList<int> concreteBoxColFSDataTypes;
    this->framesectionType = "concrete box column";

    concreteBoxColFSHeadings << tr("Name");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteBoxColFSHeadings << tr("Material");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteBoxColFSHeadings << tr("depth");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBoxColFSHeadings << tr("width");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBoxColFSHeadings << tr("flange thickness");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBoxColFSHeadings << tr("web thickness");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBoxColFSHeadings << tr("longitudinal rebar material");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteBoxColFSHeadings << tr("longitudinal rebar material corner");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteBoxColFSHeadings << tr("longitudinal rebar num bars depth");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBoxColFSHeadings << tr("longitudinal rebar num bars width");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBoxColFSHeadings << tr("longitudinal rebar bar area");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBoxColFSHeadings << tr("longitudinal rebar bar area corner");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBoxColFSHeadings << tr("longitudinal rebar cover");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBoxColFSHeadings << tr("transverse rebar material");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteBoxColFSHeadings << tr("transverse rebar num bars depth");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBoxColFSHeadings << tr("transverse rebar num bars width");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBoxColFSHeadings << tr("transverse rebar bar area");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBoxColFSHeadings << tr("transverse rebar spacing");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBoxColFSHeadings << tr("mass per length");
    concreteBoxColFSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = concreteBoxColFSHeadings;
    this->dataTypes = concreteBoxColFSDataTypes;
}

void FramesectionInputWidget::concreteCircColFS() {
    QStringList concreteCircColFSHeadings;
    QList<int> concreteCircColFSDataTypes;
    this->framesectionType = "concrete circular column";

    concreteCircColFSHeadings << tr("Name");
    concreteCircColFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteCircColFSHeadings << tr("Material");
    concreteCircColFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteCircColFSHeadings << tr("diameter");
    concreteCircColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteCircColFSHeadings << tr("longitudinal rebar material");
    concreteCircColFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteCircColFSHeadings << tr("longitudinal rebar num bars");
    concreteCircColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteCircColFSHeadings << tr("longitudinal rebar bar area");
    concreteCircColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteCircColFSHeadings << tr("longitudinal rebar cover");
    concreteCircColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteCircColFSHeadings << tr("transverse rebar material");
    concreteCircColFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteCircColFSHeadings << tr("transverse rebar bar area");
    concreteCircColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteCircColFSHeadings << tr("transverse rebar spacing");
    concreteCircColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteCircColFSHeadings << tr("mass per length");
    concreteCircColFSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = concreteCircColFSHeadings;
    this->dataTypes = concreteCircColFSDataTypes;
}

//same as concrete circular column + 1 field: wall thickness
void FramesectionInputWidget::concretePipeColFS() {
    QStringList concretePipeColFSHeadings;
    QList<int> concretePipeColFSDataTypes;
    this->framesectionType = "concrete pipe column";

    concretePipeColFSHeadings << tr("Name");
    concretePipeColFSDataTypes << SIMPLESPREADSHEET_QString;
    concretePipeColFSHeadings << tr("Material");
    concretePipeColFSDataTypes << SIMPLESPREADSHEET_QString;
    concretePipeColFSHeadings << tr("diameter");
    concretePipeColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concretePipeColFSHeadings << tr("wall thickness");
    concretePipeColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concretePipeColFSHeadings << tr("longitudinal rebar material");
    concretePipeColFSDataTypes << SIMPLESPREADSHEET_QString;
    concretePipeColFSHeadings << tr("longitudinal rebar num bars");
    concretePipeColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concretePipeColFSHeadings << tr("longitudinal rebar bar area");
    concretePipeColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concretePipeColFSHeadings << tr("longitudinal rebar cover");
    concretePipeColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concretePipeColFSHeadings << tr("transverse rebar material");
    concretePipeColFSDataTypes << SIMPLESPREADSHEET_QString;
    concretePipeColFSHeadings << tr("transverse rebar bar area");
    concretePipeColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concretePipeColFSHeadings << tr("transverse rebar spacing");
    concretePipeColFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concretePipeColFSHeadings << tr("mass per length");
    concretePipeColFSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = concretePipeColFSHeadings;
    this->dataTypes = concretePipeColFSDataTypes;
}

void
FramesectionInputWidget::outputToJSON(QJsonObject &jsonObj){return;}


void
FramesectionInputWidget::outputToJSON(QJsonArray &jsonArray){
        int numRows = theSpreadsheet->getNumRows();

        // for each row of the spreadsheet
        for (int i=0; i<numRows; i++) {
            QJsonObject obj, lrebar, trebar;
            // for each field defined in private member var tableHeader
            // and the respective data type defined in private member var dataTypes
            // (tableHeader and dataTypes defined in constructor function)
            for (int j=0; j<(this->tableHeader.size()); j++) {
                QString fieldName = this->tableHeader[j].toLower();

                if(this->dataTypes[j] == 0) {
                    //string
                    QString tmpString;
                    if (theSpreadsheet->getString(i,j,tmpString) == false || tmpString.isEmpty()) {
                        qDebug() << "no value for " << fieldName << " in row " << i;
                        return;
                    }
                    if (fieldName.startsWith("longitudinal rebar ") == true) {
                        lrebar[fieldName.remove(0,19)] = tmpString;
                    } else if (fieldName.startsWith("transverse rebar ") == true) {
                        trebar[fieldName.remove(0,17)] = tmpString;
                    } else {
                        obj[fieldName] = tmpString;
                    }
                } else if (this->dataTypes[j] == 1) {
                    //double
                    double tmpDouble;
                    if (theSpreadsheet->getDouble(i,j,tmpDouble) == false) {
                        qDebug() << "no value for " << fieldName << " in row " << i;
                        return;
                    }
                    if (fieldName.startsWith("longitudinal rebar ") == true) {
                        lrebar[fieldName.remove(0,19)] = tmpDouble;
                    } else if (fieldName.startsWith("transverse rebar ") == true) {
                        trebar[fieldName.remove(0,17)] = tmpDouble;
                    } else {
                        obj[fieldName] = tmpDouble;
                    }
                }
            }

            obj["longitudinal rebar"] = lrebar;
            obj["transverse rebar"] = trebar;
            obj["type"] = framesectionType;
            jsonArray.append(obj);
        }

}

void
FramesectionInputWidget::inputFromJSON(QJsonObject &jsonObject) { return; }

void
FramesectionInputWidget::inputFromJSON(QJsonArray &jsonArray){
    int currentRow = 0;

    foreach (const QJsonValue &theValue, jsonArray) {
        QJsonObject theObject = theValue.toObject();
        if (theObject["type"] == this->framesectionType) {
            QJsonObject lrebar = theObject["longitudinal rebar"].toObject();
            QJsonObject trebar = theObject["transverse rebar"].toObject();

            for (int j=0; j<(this->tableHeader.size()); j++) {
                QString fieldName = this->tableHeader[j].toLower();

                if(this->dataTypes[j] == 0) {
                    QString tmpString;
                    if(fieldName.startsWith("longitudinal rebar ") == true) {
                        tmpString = lrebar[fieldName.remove(0,19)].toString();
                    } else if (fieldName.startsWith("transverse rebar ") == true) {
                        tmpString = trebar[fieldName.remove(0,17)].toString();
                    } else {
                        tmpString = theObject[fieldName].toString();
                    }
                    theSpreadsheet->setString(currentRow, j, tmpString);
                } else if (this->dataTypes[j] == 1) {
                    double tmpDouble;
                    if(fieldName.startsWith("longitudinal rebar ") == true) {
                        tmpDouble = lrebar[fieldName.remove(0,19)].toDouble();
                    } else if (fieldName.startsWith("transverse rebar ") == true) {
                        tmpDouble = trebar[fieldName.remove(0,17)].toDouble();
                    } else {
                        tmpDouble = theObject[fieldName].toDouble();
                    }
                    theSpreadsheet->setDouble(currentRow, j, tmpDouble);
                }
            }
            currentRow++;
        }
    }
}


void
FramesectionInputWidget::clear(void)
{
    theSpreadsheet->clear();
}

