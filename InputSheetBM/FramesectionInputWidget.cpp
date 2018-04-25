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

//Constructor takes the name for this type of framesection and a SimCenterWidget
//FramesectionInputWidget class implementation contains details about what fields
//are included for each framesection type
FramesectionInputWidget::FramesectionInputWidget(QString framesectionType, SimCenterWidget *parent) : SimCenterTableWidget(parent)
{
    if (framesectionType == "concrete rectangular column") { this->concreteRectColFS(); }
    if (framesectionType == "concrete box column") { this->concreteBoxColFS(); }
    if (framesectionType == "concrete circular column") { this->concreteCircColFS(); }
    if (framesectionType == "concrete pipe column") { this->concretePipeColFS(); }
    if (framesectionType == "concrete rectangular beam") { this->concreteRectBeamFS(); }
    if (framesectionType == "concrete tee beam") { this->concreteTeeBeamFS(); }
    if (framesectionType == "concrete l beam") { this->concreteTeeBeamFS(); this->framesectionType = "concrete l beam"; }
    if (framesectionType == "concrete cross beam") { this->concreteCrossBeamFS(); }
    if (framesectionType == "steel wide flange") { this->steelWideFlangeFS(); }
    if (framesectionType == "steel channel") { this->steelChannelFS(); }
    if (framesectionType == "steel double channel") { this->steelDoubleChannelFS(); }
    if (framesectionType == "steel tee") { this->steelChannelFS(); this->framesectionType = "steel tee"; }
    if (framesectionType == "steel angle") { this->steelChannelFS(); this->framesectionType = "steel angle"; }
    if (framesectionType == "steel double angle") { this->steelDoubleAngleFS(); }
    if (framesectionType == "steel tube") { this->steelTubeFS(); }
    if (framesectionType == "filled steel tube") { this->filledSteelTubeFS(); }
    if (framesectionType == "steel pipe") { this->steelPipeFS(); }
    if (framesectionType == "filled steel pipe") { this->filledSteelPipeFS(); }
    if (framesectionType == "steel plate") { this->steelPlateFS(); }
    if (framesectionType == "steel rod") { this->steelRodFS(); }
    if (framesectionType == "buckling restrained brace") { this->bucklingRestrainedBraceFS(); }

    this->setupSpreadsheet();
}

//Constructor takes a string list of column headings, an integer list of data types,
//the name for this type of framesection, and a SimCenterWidget
FramesectionInputWidget::FramesectionInputWidget(QStringList headings, QList<int> dataTypes, QString framesectionType, SimCenterWidget *parent) : SimCenterTableWidget(parent)
{
    this->tableHeader = headings;
    this->dataTypes = dataTypes;
    this->framesectionType = framesectionType;

    this->setupSpreadsheet();
}


FramesectionInputWidget::~FramesectionInputWidget()
{

}

void
FramesectionInputWidget::setupSpreadsheet() {
    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);
    theSpreadsheet = new SpreadsheetWidget(tableHeader.size(), 1000, tableHeader, dataTypes, this);
    theLayout->addWidget(theSpreadsheet);
    this->setMinimumWidth(500);
}

bool
FramesectionInputWidget::outputToJSON(QJsonObject &jsonObj){return(true);}


bool
FramesectionInputWidget::outputToJSON(QJsonArray &jsonArray){
        int numRows = theSpreadsheet->getNumRows();

        // for each row of the spreadsheet
        for (int i=0; i<numRows; i++) {
            QJsonObject obj, lrebar, trebar;
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
                    if (fieldName.startsWith("longitudinal rebar ") == true) {
                        lrebar[fieldName.remove(0,19)] = tmpString;
                    } else if (fieldName.startsWith("transverse rebar ") == true) {
                        trebar[fieldName.remove(0,17)] = tmpString;
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
                    if (fieldName.startsWith("longitudinal rebar ") == true) {
                        lrebar[fieldName.remove(0,19)] = tmpDouble;
                    } else if (fieldName.startsWith("transverse rebar ") == true) {
                        trebar[fieldName.remove(0,17)] = tmpDouble;
                    } else {
                        obj[fieldName] = tmpDouble;
                    }
                }
            }

            if (!lrebar.isEmpty()) { obj["longitudinal rebar"] = lrebar; }
            if (!trebar.isEmpty()) { obj["transverse rebar"] = trebar; }
            obj["type"] = framesectionType;
            jsonArray.append(obj);
        }

        return(true);

}

bool
FramesectionInputWidget::inputFromJSON(QJsonObject &jsonObject) { return(true); }

bool
FramesectionInputWidget::inputFromJSON(QJsonArray &jsonArray){
    int currentRow = 0;
    foreach (const QJsonValue &theValue, jsonArray) {
        QJsonObject theObject = theValue.toObject();
        if (theObject["type"] == framesectionType) {
            QJsonObject lrebar = theObject["longitudinal rebar"].toObject();
            QJsonObject trebar = theObject["transverse rebar"].toObject();

            for (int j=0; j<tableHeader.size(); j++) {
                QString fieldName = tableHeader[j].toLower();

                if(dataTypes[j] == 0) {
                    QString tmpString;
                    if(fieldName.startsWith("longitudinal rebar ") == true) {
                        tmpString = lrebar[fieldName.remove(0,19)].toString();
                    } else if (fieldName.startsWith("transverse rebar ") == true) {
                        tmpString = trebar[fieldName.remove(0,17)].toString();
                    } else {
                        tmpString = theObject[fieldName].toString();
                    }
                    theSpreadsheet->setString(currentRow, j, tmpString);
                } else if (dataTypes[j] == 1) {
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
    return(true);
}

void
FramesectionInputWidget::clear(void)
{
    theSpreadsheet->clear();
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

void FramesectionInputWidget::concreteRectBeamFS() {
    QStringList concreteRectBeamFSHeadings;
    QList<int> concreteRectBeamFSDataTypes;
    this->framesectionType = "concrete rectangular beam";

    concreteRectBeamFSHeadings << tr("Name");
    concreteRectBeamFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteRectBeamFSHeadings << tr("Material");
    concreteRectBeamFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteRectBeamFSHeadings << tr("depth");
    concreteRectBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectBeamFSHeadings << tr("width");
    concreteRectBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectBeamFSHeadings << tr("longitudinal rebar material top");
    concreteRectBeamFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteRectBeamFSHeadings << tr("longitudinal rebar material bottom");
    concreteRectBeamFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteRectBeamFSHeadings << tr("longitudinal rebar num bars top");
    concreteRectBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectBeamFSHeadings << tr("longitudinal rebar num bars bottom");
    concreteRectBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectBeamFSHeadings << tr("longitudinal rebar bar area top");
    concreteRectBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectBeamFSHeadings << tr("longitudinal rebar bar area bottom");
    concreteRectBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectBeamFSHeadings << tr("longitudinal rebar cover top");
    concreteRectBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectBeamFSHeadings << tr("longitudinal rebar cover bottom");
    concreteRectBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectBeamFSHeadings << tr("transverse rebar material");
    concreteRectBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectBeamFSHeadings << tr("transverse rebar bar area");
    concreteRectBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectBeamFSHeadings << tr("transverse rebar spacing");
    concreteRectBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectBeamFSHeadings << tr("mass per length");
    concreteRectBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = concreteRectBeamFSHeadings;
    this->dataTypes = concreteRectBeamFSDataTypes;
}

void FramesectionInputWidget::concreteTeeBeamFS() {
    QStringList concreteTeeBeamFSHeadings;
    QList<int> concreteTeeBeamFSDataTypes;
    this->framesectionType = "concrete tee beam";

    concreteTeeBeamFSHeadings << tr("Name");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteTeeBeamFSHeadings << tr("Material");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteTeeBeamFSHeadings << tr("depth");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteTeeBeamFSHeadings << tr("width");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteTeeBeamFSHeadings << tr("flange thickness");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteTeeBeamFSHeadings << tr("web thickness at flange");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteTeeBeamFSHeadings << tr("web thickness at tip");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteTeeBeamFSHeadings << tr("longitudinal rebar material top");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteTeeBeamFSHeadings << tr("longitudinal rebar material bottom");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteTeeBeamFSHeadings << tr("longitudinal rebar num bars top");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteTeeBeamFSHeadings << tr("longitudinal rebar num bars bottom");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteTeeBeamFSHeadings << tr("longitudinal rebar bar area top");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteTeeBeamFSHeadings << tr("longitudinal rebar bar area bottom");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteTeeBeamFSHeadings << tr("longitudinal rebar cover top");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteTeeBeamFSHeadings << tr("longitudinal rebar cover bottom");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteTeeBeamFSHeadings << tr("transverse rebar material");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteTeeBeamFSHeadings << tr("transverse rebar bar area");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteTeeBeamFSHeadings << tr("transverse rebar spacing");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteTeeBeamFSHeadings << tr("mass per length");
    concreteTeeBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = concreteTeeBeamFSHeadings;
    this->dataTypes = concreteTeeBeamFSDataTypes;
}

void FramesectionInputWidget::concreteCrossBeamFS() {
    QStringList concreteCrossBeamFSHeadings;
    QList<int> concreteCrossBeamFSDataTypes;
    this->framesectionType = "concrete cross beam";

    concreteCrossBeamFSHeadings << tr("Name");
    concreteCrossBeamFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteCrossBeamFSHeadings << tr("Material");
    concreteCrossBeamFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteCrossBeamFSHeadings << tr("depth");
    concreteCrossBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteCrossBeamFSHeadings << tr("width");
    concreteCrossBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteCrossBeamFSHeadings << tr("flange thickness");
    concreteCrossBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteCrossBeamFSHeadings << tr("web thickness");
    concreteCrossBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteCrossBeamFSHeadings << tr("longitudinal rebar material center");
    concreteCrossBeamFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteCrossBeamFSHeadings << tr("longitudinal rebar material tip");
    concreteCrossBeamFSDataTypes << SIMPLESPREADSHEET_QString;
    concreteCrossBeamFSHeadings << tr("longitudinal rebar num bars center");
    concreteCrossBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteCrossBeamFSHeadings << tr("longitudinal rebar num bars tip");
    concreteCrossBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteCrossBeamFSHeadings << tr("longitudinal rebar bar area center");
    concreteCrossBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteCrossBeamFSHeadings << tr("longitudinal rebar bar area tip");
    concreteCrossBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteCrossBeamFSHeadings << tr("longitudinal rebar cover");
    concreteCrossBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteCrossBeamFSHeadings << tr("transverse rebar material");
    concreteCrossBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteCrossBeamFSHeadings << tr("transverse rebar bar area");
    concreteCrossBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteCrossBeamFSHeadings << tr("transverse rebar spacing");
    concreteCrossBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteCrossBeamFSHeadings << tr("mass per length");
    concreteCrossBeamFSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = concreteCrossBeamFSHeadings;
    this->dataTypes = concreteCrossBeamFSDataTypes;
}

void FramesectionInputWidget::steelWideFlangeFS() {
    QStringList steelWideFlangeFSHeadings;
    QList<int> steelWideFlangeFSDataTypes;
    this->framesectionType = "steel wide flange";

    steelWideFlangeFSHeadings << tr("Name");
    steelWideFlangeFSDataTypes << SIMPLESPREADSHEET_QString;
    steelWideFlangeFSHeadings << tr("database");
    steelWideFlangeFSDataTypes << SIMPLESPREADSHEET_QString;
    steelWideFlangeFSHeadings << tr("shape");
    steelWideFlangeFSDataTypes << SIMPLESPREADSHEET_QString;
    steelWideFlangeFSHeadings << tr("material");
    steelWideFlangeFSDataTypes << SIMPLESPREADSHEET_QString;
    steelWideFlangeFSHeadings << tr("depth");
    steelWideFlangeFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelWideFlangeFSHeadings << tr("top flange width");
    steelWideFlangeFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelWideFlangeFSHeadings << tr("top flange thickness");
    steelWideFlangeFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelWideFlangeFSHeadings << tr("web thickness");
    steelWideFlangeFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelWideFlangeFSHeadings << tr("bottom flange width");
    steelWideFlangeFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelWideFlangeFSHeadings << tr("bottom flange thickness");
    steelWideFlangeFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelWideFlangeFSHeadings << tr("fillet radius");
    steelWideFlangeFSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = steelWideFlangeFSHeadings;
    this->dataTypes = steelWideFlangeFSDataTypes;
}

void FramesectionInputWidget::steelChannelFS() {
    QStringList steelChannelFSHeadings;
    QList<int> steelChannelFSDataTypes;
    this->framesectionType = "steel channel";

    steelChannelFSHeadings << tr("Name");
    steelChannelFSDataTypes << SIMPLESPREADSHEET_QString;
    steelChannelFSHeadings << tr("database");
    steelChannelFSDataTypes << SIMPLESPREADSHEET_QString;
    steelChannelFSHeadings << tr("shape");
    steelChannelFSDataTypes << SIMPLESPREADSHEET_QString;
    steelChannelFSHeadings << tr("material");
    steelChannelFSDataTypes << SIMPLESPREADSHEET_QString;
    steelChannelFSHeadings << tr("depth");
    steelChannelFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelChannelFSHeadings << tr("width");
    steelChannelFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelChannelFSHeadings << tr("flange thickness");
    steelChannelFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelChannelFSHeadings << tr("web thickness");
    steelChannelFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelChannelFSHeadings << tr("fillet radius");
    steelChannelFSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = steelChannelFSHeadings;
    this->dataTypes = steelChannelFSDataTypes;
}

void FramesectionInputWidget::steelDoubleChannelFS() {
    QStringList steelDoubleChannelFSHeadings;
    QList<int> steelDoubleChannelFSDataTypes;
    this->framesectionType = "steel double channel";

    steelDoubleChannelFSHeadings << tr("Name");
    steelDoubleChannelFSDataTypes << SIMPLESPREADSHEET_QString;
    steelDoubleChannelFSHeadings << tr("database");
    steelDoubleChannelFSDataTypes << SIMPLESPREADSHEET_QString;
    steelDoubleChannelFSHeadings << tr("shape");
    steelDoubleChannelFSDataTypes << SIMPLESPREADSHEET_QString;
    steelDoubleChannelFSHeadings << tr("material");
    steelDoubleChannelFSDataTypes << SIMPLESPREADSHEET_QString;
    steelDoubleChannelFSHeadings << tr("depth");
    steelDoubleChannelFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelDoubleChannelFSHeadings << tr("width of single channel");
    steelDoubleChannelFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelDoubleChannelFSHeadings << tr("flange thickness");
    steelDoubleChannelFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelDoubleChannelFSHeadings << tr("web thickness");
    steelDoubleChannelFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelDoubleChannelFSHeadings << tr("back to back distance");
    steelDoubleChannelFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelDoubleChannelFSHeadings << tr("fillet radius");
    steelDoubleChannelFSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = steelDoubleChannelFSHeadings;
    this->dataTypes = steelDoubleChannelFSDataTypes;
}

void FramesectionInputWidget::steelDoubleAngleFS() {
    QStringList steelDoubleAngleFSHeadings;
    QList<int> steelDoubleAngleFSDataTypes;
    this->framesectionType = "steel double angle";

    steelDoubleAngleFSHeadings << tr("Name");
    steelDoubleAngleFSDataTypes << SIMPLESPREADSHEET_QString;
    steelDoubleAngleFSHeadings << tr("database");
    steelDoubleAngleFSDataTypes << SIMPLESPREADSHEET_QString;
    steelDoubleAngleFSHeadings << tr("shape");
    steelDoubleAngleFSDataTypes << SIMPLESPREADSHEET_QString;
    steelDoubleAngleFSHeadings << tr("material");
    steelDoubleAngleFSDataTypes << SIMPLESPREADSHEET_QString;
    steelDoubleAngleFSHeadings << tr("depth");
    steelDoubleAngleFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelDoubleAngleFSHeadings << tr("width of single angle");
    steelDoubleAngleFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelDoubleAngleFSHeadings << tr("flange thickness");
    steelDoubleAngleFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelDoubleAngleFSHeadings << tr("web thickness");
    steelDoubleAngleFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelDoubleAngleFSHeadings << tr("back to back distance");
    steelDoubleAngleFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelDoubleAngleFSHeadings << tr("fillet radius");
    steelDoubleAngleFSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = steelDoubleAngleFSHeadings;
    this->dataTypes = steelDoubleAngleFSDataTypes;
}

void FramesectionInputWidget::steelTubeFS() {
    QStringList steelTubeFSHeadings;
    QList<int> steelTubeFSDataTypes;
    this->framesectionType = "steel tube";

    steelTubeFSHeadings << tr("Name");
    steelTubeFSDataTypes << SIMPLESPREADSHEET_QString;
    steelTubeFSHeadings << tr("database");
    steelTubeFSDataTypes << SIMPLESPREADSHEET_QString;
    steelTubeFSHeadings << tr("shape");
    steelTubeFSDataTypes << SIMPLESPREADSHEET_QString;
    steelTubeFSHeadings << tr("material");
    steelTubeFSDataTypes << SIMPLESPREADSHEET_QString;
    steelTubeFSHeadings << tr("depth");
    steelTubeFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelTubeFSHeadings << tr("width");
    steelTubeFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelTubeFSHeadings << tr("flange thickness");
    steelTubeFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelTubeFSHeadings << tr("web thickness");
    steelTubeFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelTubeFSHeadings << tr("corner radius");
    steelTubeFSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = steelTubeFSHeadings;
    this->dataTypes = steelTubeFSDataTypes;
}

void FramesectionInputWidget::filledSteelTubeFS() {
    QStringList filledSteelTubeFSHeadings;
    QList<int> filledSteelTubeFSDataTypes;
    this->framesectionType = "filled steel tube";

    filledSteelTubeFSHeadings << tr("Name");
    filledSteelTubeFSDataTypes << SIMPLESPREADSHEET_QString;
    filledSteelTubeFSHeadings << tr("database");
    filledSteelTubeFSDataTypes << SIMPLESPREADSHEET_QString;
    filledSteelTubeFSHeadings << tr("shape");
    filledSteelTubeFSDataTypes << SIMPLESPREADSHEET_QString;
    filledSteelTubeFSHeadings << tr("material");
    filledSteelTubeFSDataTypes << SIMPLESPREADSHEET_QString;
    filledSteelTubeFSHeadings << tr("material fill");
    filledSteelTubeFSDataTypes << SIMPLESPREADSHEET_QString;
    filledSteelTubeFSHeadings << tr("depth");
    filledSteelTubeFSDataTypes << SIMPLESPREADSHEET_QDouble;
    filledSteelTubeFSHeadings << tr("width");
    filledSteelTubeFSDataTypes << SIMPLESPREADSHEET_QDouble;
    filledSteelTubeFSHeadings << tr("flange thickness");
    filledSteelTubeFSDataTypes << SIMPLESPREADSHEET_QDouble;
    filledSteelTubeFSHeadings << tr("web thickness");
    filledSteelTubeFSDataTypes << SIMPLESPREADSHEET_QDouble;
    filledSteelTubeFSHeadings << tr("corner radius");
    filledSteelTubeFSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = filledSteelTubeFSHeadings;
    this->dataTypes = filledSteelTubeFSDataTypes;
}

void FramesectionInputWidget::steelPipeFS() {
    QStringList steelPipeFSHeadings;
    QList<int> steelPipeFSDataTypes;
    this->framesectionType = "steel pipe";

    steelPipeFSHeadings << tr("Name");
    steelPipeFSDataTypes << SIMPLESPREADSHEET_QString;
    steelPipeFSHeadings << tr("database");
    steelPipeFSDataTypes << SIMPLESPREADSHEET_QString;
    steelPipeFSHeadings << tr("shape");
    steelPipeFSDataTypes << SIMPLESPREADSHEET_QString;
    steelPipeFSHeadings << tr("material");
    steelPipeFSDataTypes << SIMPLESPREADSHEET_QString;
    steelPipeFSHeadings << tr("diameter");
    steelPipeFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelPipeFSHeadings << tr("wall thickness");
    steelPipeFSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = steelPipeFSHeadings;
    this->dataTypes = steelPipeFSDataTypes;
}

void FramesectionInputWidget::filledSteelPipeFS() {
    QStringList filledSteelPipeFSHeadings;
    QList<int> filledSteelPipeFSDataTypes;
    this->framesectionType = "filled steel pipe";

    filledSteelPipeFSHeadings << tr("Name");
    filledSteelPipeFSDataTypes << SIMPLESPREADSHEET_QString;
    filledSteelPipeFSHeadings << tr("database");
    filledSteelPipeFSDataTypes << SIMPLESPREADSHEET_QString;
    filledSteelPipeFSHeadings << tr("shape");
    filledSteelPipeFSDataTypes << SIMPLESPREADSHEET_QString;
    filledSteelPipeFSHeadings << tr("material");
    filledSteelPipeFSDataTypes << SIMPLESPREADSHEET_QString;
    filledSteelPipeFSHeadings << tr("material fill");
    filledSteelPipeFSDataTypes << SIMPLESPREADSHEET_QString;
    filledSteelPipeFSHeadings << tr("diameter");
    filledSteelPipeFSDataTypes << SIMPLESPREADSHEET_QDouble;
    filledSteelPipeFSHeadings << tr("wall thickness");
    filledSteelPipeFSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = filledSteelPipeFSHeadings;
    this->dataTypes = filledSteelPipeFSDataTypes;
}

void FramesectionInputWidget::steelPlateFS() {
    QStringList steelPlateFSHeadings;
    QList<int> steelPlateFSDataTypes;
    this->framesectionType = "steel plate";

    steelPlateFSHeadings << tr("Name");
    steelPlateFSDataTypes << SIMPLESPREADSHEET_QString;
    steelPlateFSHeadings << tr("material");
    steelPlateFSDataTypes << SIMPLESPREADSHEET_QString;
    steelPlateFSHeadings << tr("depth");
    steelPlateFSDataTypes << SIMPLESPREADSHEET_QDouble;
    steelPlateFSHeadings << tr("width");
    steelPlateFSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = steelPlateFSHeadings;
    this->dataTypes = steelPlateFSDataTypes;
}

void FramesectionInputWidget::steelRodFS() {
    QStringList steelRodFSHeadings;
    QList<int> steelRodFSDataTypes;
    this->framesectionType = "steel rod";

    steelRodFSHeadings << tr("Name");
    steelRodFSDataTypes << SIMPLESPREADSHEET_QString;
    steelRodFSHeadings << tr("material");
    steelRodFSDataTypes << SIMPLESPREADSHEET_QString;
    steelRodFSHeadings << tr("diameter");
    steelRodFSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = steelRodFSHeadings;
    this->dataTypes = steelRodFSDataTypes;
}

void FramesectionInputWidget::bucklingRestrainedBraceFS() {
    QStringList bucklingBraceFSHeadings;
    QList<int> bucklingBraceFSDataTypes;
    this->framesectionType = "buckling restrained brace";

    bucklingBraceFSHeadings << tr("Name");
    bucklingBraceFSDataTypes << SIMPLESPREADSHEET_QString;
    bucklingBraceFSHeadings << tr("database");
    bucklingBraceFSDataTypes << SIMPLESPREADSHEET_QString;
    bucklingBraceFSHeadings << tr("shape");
    bucklingBraceFSDataTypes << SIMPLESPREADSHEET_QString;
    bucklingBraceFSHeadings << tr("material");
    bucklingBraceFSDataTypes << SIMPLESPREADSHEET_QString;
    bucklingBraceFSHeadings << tr("material core");
    bucklingBraceFSDataTypes << SIMPLESPREADSHEET_QString;
    bucklingBraceFSHeadings << tr("depth");
    bucklingBraceFSDataTypes << SIMPLESPREADSHEET_QDouble;
    bucklingBraceFSHeadings << tr("width");
    bucklingBraceFSDataTypes << SIMPLESPREADSHEET_QDouble;
    bucklingBraceFSHeadings << tr("area core");
    bucklingBraceFSDataTypes << SIMPLESPREADSHEET_QDouble;
    bucklingBraceFSHeadings << tr("yield length");
    bucklingBraceFSDataTypes << SIMPLESPREADSHEET_QDouble;
    bucklingBraceFSHeadings << tr("omega");
    bucklingBraceFSDataTypes << SIMPLESPREADSHEET_QDouble;
    bucklingBraceFSHeadings << tr("beta-omega");
    bucklingBraceFSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = bucklingBraceFSHeadings;
    this->dataTypes = bucklingBraceFSDataTypes;
}
