
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


WallsectionInputWidget::WallsectionInputWidget(SimCenterWidget *parent) : SimCenterTableWidget(parent)
{
    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);

    QStringList headings;
    QList<int> dataTypes;
    headings << tr("Name");
    headings << tr("Type");

    headings << tr("Length");
    headings << tr("Thickness");
    headings << tr("Boundary element length");
    headings << tr("Mass per area");

    headings << tr("longitudinal rebar material");
    headings << tr("longitudinal rebar material corner");
    headings << tr("longitudinal rebar num bars depth");
    headings << tr("longitudinal rebar num bars width");
    headings << tr("longitudinal rebar bar area");
    headings << tr("longitudinal rebar bar area corner");
    headings << tr("longitudinal rebar cover");

    headings << tr("transverse rebar material");
    headings << tr("transverse rebar num bars depth");
    headings << tr("transverse rebar num bars width");
    headings << tr("transverse rebar num bars thickness");
    headings << tr("transverse rebar bar area");
    headings << tr("transverse rebar spacing");
    headings << tr("transverse rebar cover");

    //longitudinal boundary element rebar (LBER)
    headings << tr("LBER material");
    headings << tr("LBER num bars length");
    headings << tr("LBER num bars thickness");
    headings << tr("LBER bar area");
    headings << tr("LBER spacing");

    //transverse boundary element rebar (TBER)
    headings << tr("TBER material");
    headings << tr("TBER num bars length");
    headings << tr("TBER num bars thickness");
    headings << tr("TBER bar area");
    headings << tr("TBER spacing");

    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QString;

    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;

    // longitudinal rebar
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;

    // transverse rebar
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;

    //longitudinal boundary element rebar
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;

    //transverse boundary element rebar
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;

    theSpreadsheet = new SpreadsheetWidget(28, 1000, headings, dataTypes, this);

    theLayout->addWidget(theSpreadsheet);

    this->setMinimumWidth(500);


}



WallsectionInputWidget::~WallsectionInputWidget()
{

}

void
WallsectionInputWidget::outputToJSON(QJsonObject &jsonObj){

    QJsonArray  jsonArray;
    int numRows = theSpreadsheet->getNumRows();
    for (int i=0; i<numRows; i++) {

        QJsonObject obj;
        QString name, ws_type;
        double ws_thickness, bel, massperarea;

        // longitudinal rebar
        QString lr_material, lr_materialCorner;
        double lr_numBarsDepth, lr_numBarsWidth, lr_barArea, lr_barAreaCorner, lr_cover;

        // transverse rebar
        QString tr_material;
        double tr_numBarsDepth, tr_numBarsWidth, tr_numBarsThickness, tr_barArea, tr_barAreaCorner, tr_spacing, tr_cover;

        //longitudinal boundary element rebar (LBER)
        QString lber_material;
        double  lber_numBarsLength, lber_numBarsThickness, lber_barArea, lber_spacing;

        //transverse boundary element rebar (TBER)
        QString tber_material;
        double  tber_numBarsLength, tber_numBarsThickness, tber_barArea, tber_spacing;


        // obtain info from spreadsheet
        if (theSpreadsheet->getString(i,0,name) == false || name.isEmpty())
            break;
        if (theSpreadsheet->getString(i,1,ws_type) == false)
            break;
        if (theSpreadsheet->getDouble(i,2, ws_thickness) == false)
            break;
        if (theSpreadsheet->getDouble(i,3,bel) == false)
            break;
        if (theSpreadsheet->getDouble(i,4,massperarea) == false)
            break;

        // longitudinal rebar  ********* FIX THIS ************* then renumber!!!
        if (theSpreadsheet->getString(i,19,lr_material) == false)
            break;
        if (theSpreadsheet->getString(i,20,lr_materialCorner) == false)
            break;
        if (theSpreadsheet->getDouble(i,12,lr_numBarsDepth) == false)
            break;
        if (theSpreadsheet->getDouble(i,13,lr_numBarsWidth) == false)
            break;
        if (theSpreadsheet->getDouble(i,14,lr_barArea) == false)
            break;
        if (theSpreadsheet->getDouble(i,15,lr_barAreaCorner) == false)
            break;
        if (theSpreadsheet->getDouble(i,16,lr_cover) == false)
            break;

        // transverse rebar
        if (theSpreadsheet->getString(i,00,tr_material) == false)
            break;
        if (theSpreadsheet->getDouble(i,00,tr_numBarsDepth) == false)
            break;
        if (theSpreadsheet->getDouble(i,00,tr_numBarsWidth) == false)
            break;
        if (theSpreadsheet->getDouble(i,00,tr_numBarsThickness) == false)
            break;
        if (theSpreadsheet->getDouble(i,00,tr_barArea) == false)
            break;
        if (theSpreadsheet->getDouble(i,00,tr_barAreaCorner) == false)
            break;
        if (theSpreadsheet->getDouble(i,00,tr_spacing) == false)
            break;
        if (theSpreadsheet->getDouble(i,00,tr_cover) == false)
            break;

        //longitudinal boundary element rebar
        if (theSpreadsheet->getString(i,00,lber_material) == false)
            break;
        if (theSpreadsheet->getDouble(i,00,lber_numBarsLength) == false)
            break;
        if (theSpreadsheet->getDouble(i,00,lber_numBarsThickness) == false)
            break;
        if (theSpreadsheet->getDouble(i,00,lber_barArea) == false)
            break;
        if (theSpreadsheet->getDouble(i,00,lber_spacing) == false)
            break;

        //transverse boundary element rebar
        if (theSpreadsheet->getString(i,00,tber_material) == false)
            break;
        if (theSpreadsheet->getDouble(i,00,tber_numBarsLength) == false)
            break;
        if (theSpreadsheet->getDouble(i,00,tber_numBarsThickness) == false)
            break;
        if (theSpreadsheet->getDouble(i,00,tber_barArea) == false)
            break;
        if (theSpreadsheet->getDouble(i,00,tber_spacing) == false)
            break;


        // now add the items to object
        obj["name"]=name;
        obj["type"]=ws_type;
        obj["material"]=ws_thickness;
        obj["depth"]=bel;
        obj["width"]=massperarea;

        // longitudinal rebar   -FIX THIS!!!!!!! todo
        QJsonObject lrebar;

        lrebar["material"]=tr_material;
        lrebar["material corner"]=lr_materialCorner;
        lrebar["num bars depth"]=lr_numBarsDepth;
        lrebar["num bars width"]=lr_numBarsWidth;
        lrebar["bar area"]=lr_barArea;
        lrebar["bar area corner"]=lr_barAreaCorner;

        lrebar["longitudinal rebar"] = lrebar;

        // transverse rebar
        QJsonObject trebar;

        trebar["material"]=tr_material;
        trebar["bar area"]=tr_barArea;
        trebar["bar area corner"]=tr_barAreaCorner;
        trebar["spacing"]=tr_spacing;
        trebar["cover"]=tr_cover;
        trebar["num bars depth"]=tr_numBarsDepth;
        trebar["num bars width"]=tr_numBarsWidth;
        trebar["num bars thickness"]=tr_numBarsThickness;

        obj["transverse rebar"] = trebar;


        // longitudinal boundary element rebar
        QJsonObject lberrebar;

        lberrebar["material"]=lber_material;
        lberrebar["num bars length"]=lber_numBarsLength;
        lberrebar["num bars thickness"]=lber_numBarsThickness;
        lberrebar["bar area"]=lber_barArea;
        lberrebar["lber_spacing"]=lber_spacing;


        obj["longitudinal boundary element rebar"] = lberrebar;

        // transverse boundary element rebar
        QJsonObject tberrebar;

        lberrebar["material"]=tber_material;
        lberrebar["num bars length"]=tber_numBarsLength;
        lberrebar["num bars thickness"]=tber_numBarsThickness;
        lberrebar["bar area"]=tber_barArea;
        lberrebar["lber_spacing"]=tber_spacing;


        obj["longitudinal boundary element rebar"] = lberrebar;

        // add the object to the array
        jsonArray.append(obj);
    }

    // add the object
    jsonObj["wallsections"] = jsonArray;

}

void
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

    QJsonArray theArray = jsonObject["framesections"].toArray();
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


}


void
WallsectionInputWidget::clear(void)
{
    theSpreadsheet->clear();
}
