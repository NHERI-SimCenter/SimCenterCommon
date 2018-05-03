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

#include "WallsectionInputWidget.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QList>

//Constructor takes the name for this type of wallsection and a SimCenterWidget
//WallsectionInputWidget class implementation contains details about what fields
//are included for each wallsection type
WallsectionInputWidget::WallsectionInputWidget(QString wallsectionType, SimCenterWidget *parent) : SimCenterTableWidget(parent) {
    if (wallsectionType == "concrete rectangular wall") { this->concreteRectWallWS(); }
    if (wallsectionType == "concrete flanged wall") { this->concreteFlangedWallWS(); }
    if (wallsectionType == "concrete barbell wall") { this->concreteBarbellWallWS(); }

    this->setupSpreadsheet();
}

//Constructor takes a string list of column headings, an integer list of data types,
//the name for this type of wallsection, and a SimCenterWidget
WallsectionInputWidget::WallsectionInputWidget(QStringList headings, QList<int> dataTypes, QString wallsectionType, SimCenterWidget *parent) : SimCenterTableWidget(parent) {
    this->tableHeader = headings;
    this->dataTypes = dataTypes;
    this->wallsectionType = wallsectionType;

    this->setupSpreadsheet();
}

WallsectionInputWidget::~WallsectionInputWidget()
{

}

void
WallsectionInputWidget::setupSpreadsheet() {
    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);
    theSpreadsheet = new SpreadsheetWidget(tableHeader.size(), 1000, tableHeader, dataTypes, this);
    theLayout->addWidget(theSpreadsheet);
    this->setMinimumWidth(500);
}

bool
WallsectionInputWidget::outputToJSON(QJsonObject &jsonObj){return(true);}

bool
WallsectionInputWidget::outputToJSON(QJsonArray &jsonArray) {
    int numRows = theSpreadsheet->getNumRows();

    // for each row of the spreadsheet
    for (int i=0; i<numRows; i++) {
        QJsonObject obj, lrebar, trebar, lber, tber;
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
                } else if (fieldName.startsWith("lber ") == true) {
                    lber[fieldName.remove(0,5)] = tmpString;
                } else if (fieldName.startsWith("tber ") == true) {
                    tber[fieldName.remove(0,5)] = tmpString;
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
                } else if (fieldName.startsWith("lber ") == true) {
                    lber[fieldName.remove(0,5)] = tmpDouble;
                } else if (fieldName.startsWith("tber ") == true) {
                    tber[fieldName.remove(0,5)] = tmpDouble;
                } else {
                    obj[fieldName] = tmpDouble;
                }
            }
        }

        if (!lrebar.isEmpty()) { obj["longitudinal rebar"] = lrebar; }
        if (!trebar.isEmpty()) { obj["transverse rebar"] = trebar; }
        if (!lber.isEmpty()) { obj["longitudinal boundary element rebar"] = lber; }
        if (!tber.isEmpty()) { obj["transverse boundary element rebar"] = tber; }
        obj["type"] = wallsectionType;
        jsonArray.append(obj);
    }

    return(true);

}

bool
WallsectionInputWidget::inputFromJSON(QJsonObject &jsonObject){


    int currentRow = 0;

    QString name, ws_type;
    double ws_length, ws_thickness, bel, massperarea;

    // longitudinal rebar
    QString lr_material, lr_materialCorner;
    double lr_numBarsDepth, lr_numBarsWidth, lr_barArea, lr_barAreaCorner, lr_cover;

    // transverse rebar
    QString tr_material;
    double tr_numBarsDepth, tr_numBarsWidth, tr_numBarsThickness, tr_barArea, tr_barAreaCorner, tr_spacing, tr_cover;

    //longitudinal boundary element rebar (LBER)
    QString lber_material;
    double  lber_numBarsLength, lber_numBarsThickness, lber_barArea, lber_cover, lr_spacing;

    //transverse boundary element rebar (TBER)
    QString tber_material;
    double  tber_numBarsLength, tber_numBarsThickness, tber_barArea, tber_spacing;

    //
    // get the cline data (a json array) from the object, and for every
    // object in the array, get the values and add to the spreadsheet
    //

    QJsonArray theArray = jsonObject["wallsections"].toArray();
    foreach (const QJsonValue &theValue, theArray) {
        // get values
        QJsonObject theObject = theValue.toObject();

        name = theObject["name"].toString();
        ws_type = theObject["type"].toString();
        ws_length = theObject["length"].toDouble();
        ws_thickness = theObject["thickness"].toDouble();
        bel = theObject["boundary element length"].toDouble();
        massperarea = theObject["massperarea"].toDouble();


        // longitudinal rebar
        QJsonValue theLRValue = theObject["longitudinal rebar"];
        QJsonObject lrObject = theLRValue.toObject();

        lr_material = lrObject["material"].toString();
        lr_materialCorner = lrObject["material corner"].toString();
        lr_numBarsDepth = lrObject["num bars depth"].toDouble();
        lr_numBarsWidth = lrObject["num bars width"].toDouble();
        lr_barArea = lrObject["bar area"].toDouble();
        lr_barAreaCorner = lrObject["bar area corner"].toDouble();
        lr_cover = lrObject["lr_cover"].toDouble();
        lr_spacing = lrObject["lr_spacing"].toDouble();

        //********* FINISH THIS

        // transverse rebar
        QJsonValue theTRValue = theObject["transverse rebar"];
        QJsonObject trObject = theTRValue.toObject();

        tr_material = trObject["material"].toString();
        tr_numBarsDepth = trObject["num bars depth"].toDouble();
        tr_numBarsWidth = trObject["num bars width"].toDouble();
        tr_numBarsThickness  = trObject["num bars thickness"].toDouble();
        tr_barArea = trObject["bar area"].toDouble();
        tr_barAreaCorner = trObject["bar area corner"].toDouble();
        tr_spacing = trObject["spacing"].toDouble();
        tr_cover = trObject["cover"].toDouble();


        // longitudinal boundary element rebar
        QJsonValue theLBERValue = theObject["longitudinal boundary element rebar"];
        QJsonObject lberObject = theLBERValue.toObject();

        lber_material = lberObject["material"].toString();
        lber_numBarsLength = lberObject["num bars length"].toDouble();
        lber_numBarsThickness = lberObject["num bars thickness"].toDouble();
        lber_barArea  = lberObject["bar area"].toDouble();
        lber_cover = lberObject["cover"].toDouble();


        // transverse boundary element rebar
        QJsonValue theTBERValue = theObject["transverse boundary element rebar"];
        QJsonObject tberObject = theTBERValue.toObject();

        tber_material = tberObject["material"].toString();
        tber_numBarsLength = tberObject["num bars length"].toDouble();
        tber_numBarsThickness = tberObject["num bars thickness"].toDouble();
        tber_barArea  = tberObject["bar area"].toDouble();
        tber_spacing = tberObject["spacing"].toDouble();



        // add to the spreadsheet
        theSpreadsheet->setString(currentRow, 0, name);
        theSpreadsheet->setString(currentRow, 1, ws_type);

        theSpreadsheet->setDouble(currentRow, 2, ws_length);
        theSpreadsheet->setDouble(currentRow, 3, ws_thickness);
        theSpreadsheet->setDouble(currentRow, 4, bel);
        theSpreadsheet->setDouble(currentRow, 5, massperarea);

        // add longitudinal rebar to the spreadsheet
        theSpreadsheet->setString(currentRow, 6, lr_material);
        theSpreadsheet->setString(currentRow, 7, lr_materialCorner);
        theSpreadsheet->setDouble(currentRow, 7, lr_numBarsDepth);
        theSpreadsheet->setDouble(currentRow, 8, lr_numBarsWidth);
        theSpreadsheet->setDouble(currentRow, 9, lr_numBarsDepth);
        theSpreadsheet->setDouble(currentRow, 10, lr_barArea);
        theSpreadsheet->setDouble(currentRow, 11, lr_spacing);
        theSpreadsheet->setDouble(currentRow, 12, lr_cover);

        // add transverse rebar to the spreadsheet
        theSpreadsheet->setString(currentRow, 00, tr_material);
        theSpreadsheet->setDouble(currentRow, 00, tr_numBarsDepth);
        theSpreadsheet->setDouble(currentRow, 00, tr_numBarsWidth);
        theSpreadsheet->setDouble(currentRow, 00, tr_numBarsThickness);
        theSpreadsheet->setDouble(currentRow, 00, tr_barArea);
        theSpreadsheet->setDouble(currentRow, 00, tr_barAreaCorner);
        theSpreadsheet->setDouble(currentRow, 00, tr_spacing);
        theSpreadsheet->setDouble(currentRow, 00, tr_cover);

        // add longitudinal boundary element rebar to the spreadsheet
        theSpreadsheet->setString(currentRow, 00, lber_material);
        theSpreadsheet->setDouble(currentRow, 00, lber_numBarsLength);
        theSpreadsheet->setDouble(currentRow, 00, lber_numBarsThickness);
        theSpreadsheet->setDouble(currentRow, 00, lber_barArea);
        theSpreadsheet->setDouble(currentRow, 00, lber_cover);

        // add transverse boundary element rebar to the spreadsheet
        theSpreadsheet->setString(currentRow, 00, tber_material);
        theSpreadsheet->setDouble(currentRow, 00, tber_numBarsLength);
        theSpreadsheet->setDouble(currentRow, 00, tber_numBarsThickness);
        theSpreadsheet->setDouble(currentRow, 00, tber_barArea);
        theSpreadsheet->setDouble(currentRow, 00, tber_spacing);

        currentRow++;
    }

    return(true);
}

bool
WallsectionInputWidget::inputFromJSON(QJsonArray &jsonArray) {
    int currentRow = 0;

    foreach (const QJsonValue &theValue, jsonArray) {
        QJsonObject theObject = theValue.toObject();
        if (theObject["type"] == wallsectionType) {
            QJsonObject lrebar = theObject["longitudinal rebar"].toObject();
            QJsonObject trebar = theObject["transverse rebar"].toObject();
            QJsonObject lber = theObject["longitudinal boundary element rebar"].toObject();
            QJsonObject tber = theObject["transverse boundary element rebar"].toObject();

            for (int j=0; j<tableHeader.size(); j++) {
                QString fieldName = tableHeader[j].toLower();

                if(dataTypes[j] == 0) {
                    QString tmpString;
                    if(fieldName.startsWith("longitudinal rebar ") == true) {
                        tmpString = lrebar[fieldName.remove(0,19)].toString();
                    } else if (fieldName.startsWith("transverse rebar ") == true) {
                        tmpString = trebar[fieldName.remove(0,17)].toString();
                    } else if (fieldName.startsWith("lber ") == true) {
                        tmpString = lber[fieldName.remove(0,5)].toString();
                    } else if (fieldName.startsWith("tber ") == true) {
                        tmpString = tber[fieldName.remove(0,5)].toString();
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
                    } else if (fieldName.startsWith("lber ") == true) {
                        tmpDouble = lber[fieldName.remove(0,5)].toDouble();
                    } else if (fieldName.startsWith("tber") == true) {
                        tmpDouble = tber[fieldName.remove(0,5)].toDouble();
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
WallsectionInputWidget::clear(void)
{
    theSpreadsheet->clear();
}

void
WallsectionInputWidget::concreteRectWallWS() {
    QStringList concreteRectWallWSHeadings;
    QList<int> concreteRectWallWSDataTypes;
    this->wallsectionType = "concrete rectangular wall";

    concreteRectWallWSHeadings << tr("Name");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QString;
    concreteRectWallWSHeadings << tr("length");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("thickness");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("boundary element length");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("longitudinal rebar material");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QString;
    concreteRectWallWSHeadings << tr("longitudinal rebar num bars thickness");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("longitudinal rebar bar area");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("longitudinal rebar spacing");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("longitudinal rebar cover");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("transverse rebar material");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QString;
    concreteRectWallWSHeadings << tr("transverse rebar num bars thickness");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("transverse rebar bar area");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("transverse rebar spacing");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("transverse rebar cover");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("LBER material");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QString;
    concreteRectWallWSHeadings << tr("LBER num bars length");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("LBER num bars thickness");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("LBER bar area");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("LBER cover");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("TBER material");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QString;
    concreteRectWallWSHeadings << tr("TBER num bars length");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("TBER num bars thickness");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("TBER bar area");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("TBER spacing");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteRectWallWSHeadings << tr("mass per area");
    concreteRectWallWSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = concreteRectWallWSHeadings;
    this->dataTypes = concreteRectWallWSDataTypes;
}

void
WallsectionInputWidget::concreteFlangedWallWS() {
    QStringList concreteFlangedWallWSHeadings;
    QList<int> concreteFlangedWallWSDataTypes;
    this->wallsectionType = "concrete flanged wall";

    concreteFlangedWallWSHeadings << tr("Name");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QString;
    concreteFlangedWallWSHeadings << tr("length");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("flange width");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("flange thickness");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("web thickness");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("boundary element length");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("longitudinal rebar material");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QString;
    concreteFlangedWallWSHeadings << tr("longitudinal rebar num bars thickness");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("longitudinal rebar bar area");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("longitudinal rebar spacing");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("longitudinal rebar cover");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("transverse rebar material");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QString;
    concreteFlangedWallWSHeadings << tr("transverse rebar num bars thickness");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("transverse rebar bar area");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("transverse rebar spacing");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("transverse rebar cover");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("LBER material");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QString;
    concreteFlangedWallWSHeadings << tr("LBER num bars length");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("LBER num bars thickness");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("LBER bar area");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("LBER cover");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("TBER material");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QString;
    concreteFlangedWallWSHeadings << tr("TBER num bars length");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("TBER num bars thickness");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("TBER bar area");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("TBER spacing");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteFlangedWallWSHeadings << tr("mass per area");
    concreteFlangedWallWSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = concreteFlangedWallWSHeadings;
    this->dataTypes = concreteFlangedWallWSDataTypes;
}

void
WallsectionInputWidget::concreteBarbellWallWS() {
    QStringList concreteBarbellWallWSHeadings;
    QList<int> concreteBarbellWallWSDataTypes;
    this->wallsectionType = "concrete barbell wall";

    concreteBarbellWallWSHeadings << tr("Name");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QString;
    concreteBarbellWallWSHeadings << tr("length");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("top flange width");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("top flange thickness");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("web thickness");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("bottom flange width");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("bottom flange thickness");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("boundary element length");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("longitudinal rebar material");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QString;
    concreteBarbellWallWSHeadings << tr("longitudinal rebar num bars thickness");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("longitudinal rebar bar area");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("longitudinal rebar spacing");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("longitudinal rebar cover");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("transverse rebar material");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QString;
    concreteBarbellWallWSHeadings << tr("transverse rebar num bars thickness");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("transverse rebar bar area");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("transverse rebar spacing");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("transverse rebar cover");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("LBER material");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QString;
    concreteBarbellWallWSHeadings << tr("LBER num bars length");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("LBER num bars thickness");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("LBER bar area");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("LBER cover");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("TBER material");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QString;
    concreteBarbellWallWSHeadings << tr("TBER num bars length");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("TBER num bars thickness");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("TBER bar area");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("TBER spacing");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;
    concreteBarbellWallWSHeadings << tr("mass per area");
    concreteBarbellWallWSDataTypes << SIMPLESPREADSHEET_QDouble;

    this->tableHeader = concreteBarbellWallWSHeadings;
    this->dataTypes = concreteBarbellWallWSDataTypes;
}
