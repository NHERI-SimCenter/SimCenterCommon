
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


#include "SlabsectionInputWidget.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QList>



SlabsectionInputWidget::SlabsectionInputWidget(SimCenterWidget *parent) : SimCenterTableWidget(parent)
{

    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);

    QStringList headings;
    QList<int> dataTypes;
    headings << tr("Name");
    headings << tr("Type");
    headings << tr("Material");
    headings << tr("Thickness");
    headings << tr("Mass per area");

    // longitudinal rebar
    headings << tr("longitudinal rebar material");
    headings << tr("longitudinal rebar material corner");
    headings << tr("longitudinal rebar num bars depth");
    headings << tr("longitudinal rebar num bars width");
    headings << tr("longitudinal rebar bar area");
    headings << tr("longitudinal rebar bar area corner");
    headings << tr("longitudinal rebar cover");

    // transverse rebar
    headings << tr("transverse rebar material");
    headings << tr("transverse rebar num bars depth");
    headings << tr("transverse rebar num bars width");
    headings << tr("transverse rebar num bars thickness");
    headings << tr("transverse rebar bar area");
    headings << tr("transverse rebar spacing");
    headings << tr("transverse rebar cover");

    // shear rebar
    headings << tr("shear rebar material");
    headings << tr("shear rebar bar area");
    headings << tr("shear rebar spacing");


    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QString;

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

    // shear rebar
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;


    theSpreadsheet = new SpreadsheetWidget(28, 1000, headings, dataTypes, this);

    theLayout->addWidget(theSpreadsheet);

    this->setMinimumWidth(500);

}


SlabsectionInputWidget::~SlabsectionInputWidget()
{

}

void
SlabsectionInputWidget::outputToJSON(QJsonObject &jsonObj){


    QJsonArray  jsonArray;
    int numRows = theSpreadsheet->getNumRows();
    for (int i=0; i<numRows; i++) {

        QJsonObject obj;
        QString name, slab_type, slab_material;
        double slab_thickness, massperarea;

        // longitudinal rebar
        QString lr_material, lr_materialCorner;
        double lr_numBarsDepth, lr_numBarsWidth, lr_barArea, lr_barAreaCorner, lr_cover;

        // transverse rebar
        QString tr_material;
        double tr_numBarsDepth, tr_numBarsWidth, tr_numBarsThickness, tr_barArea, tr_barAreaCorner, tr_spacing, tr_cover;

        // shear rebar
        QString shr_material;
        double shr_barArea, shr_spacing;

        // obtain info from spreadsheet
        if (theSpreadsheet->getString(i,0,name) == false || name.isEmpty())
            break;
        if (theSpreadsheet->getString(i,1,slab_type) == false)
            break;
        if (theSpreadsheet->getString(i,2, slab_material) == false)
            break;
        if (theSpreadsheet->getDouble(i,3,slab_thickness) == false)
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


        // shear rebar
        if (theSpreadsheet->getString(i,00,shr_material) == false)
            break;
        if (theSpreadsheet->getDouble(i,00,shr_barArea) == false)
            break;
        if (theSpreadsheet->getDouble(i,00,shr_spacing) == false)
            break;

        // now add the items to object
        obj["name"]=name;
        obj["type"]=slab_type;
        obj["material"]=slab_material;
        obj["thickness"]=slab_thickness;
        obj["massperarea"]=massperarea;

        // longitudinal rebar   -FIX THIS!!!!!!! todo
        QJsonObject lrebar;

        lrebar["material"]=tr_material;
        lrebar["material corner"]=lr_materialCorner;
        lrebar["num bars depth"]=lr_numBarsDepth;
        lrebar["num bars width"]=lr_numBarsWidth;
        lrebar["bar area"]=lr_barArea;
        lrebar["bar area corner"]=lr_barAreaCorner;


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

        // shear rebar
        QJsonObject shrrebar;

        shrrebar["material"]=shr_material;
        shrrebar["bar area"]=shr_barArea;
        shrrebar["spacing"]=shr_spacing;

        obj["shear rebar"] = shrrebar;


        // add the object to the array
       jsonArray.append(obj);

    }

    // add the object
    jsonObj["slabsections"] = jsonArray;

}

void
SlabsectionInputWidget::inputFromJSON(QJsonObject &jsonObject){

    int currentRow = 0;

    QString name, slab_type, slab_material;
    double slab_thickness, massperarea;

    // longitudinal rebar
    QString lr_material, lr_materialCorner;
    double lr_numBarsDepth, lr_numBarsWidth, lr_barArea, lr_barAreaCorner, lr_cover, lr_spacing;

    // transverse rebar
    QString tr_material;
    double tr_numBarsDepth, tr_numBarsWidth, tr_numBarsThickness, tr_barArea, tr_barAreaCorner, tr_spacing, tr_cover;

    // shear rebar
    QString shr_material;
    double shr_barArea, shr_spacing;

    //
    // get the cline data (a json array) from the object, and for every
    // object in the array, get the values and add to the spreadsheet
    //

    QJsonArray theArray = jsonObject["framesections"].toArray();
    foreach (const QJsonValue &theValue, theArray) {
        // get values
        QJsonObject theObject = theValue.toObject();

        name = theObject["name"].toString();
        slab_type = theObject["type"].toString();
        slab_material = theObject["material"].toString();
        slab_thickness = theObject["thickness"].toDouble();
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

        // shear rebar
        QJsonValue theSHRValue = theObject["shear rebar"];
        QJsonObject shrObject = theSHRValue.toObject();

        shr_material = shrObject["material"].toString();
        shr_barArea = shrObject["bar area"].toDouble();
        shr_spacing = shrObject["spacing"].toDouble();


        // add to the spreadsheet
        theSpreadsheet->setString(currentRow, 0, name);
        theSpreadsheet->setString(currentRow, 1, slab_type);
        theSpreadsheet->setString(currentRow, 2, slab_material);

        theSpreadsheet->setDouble(currentRow, 3, slab_thickness);
        theSpreadsheet->setDouble(currentRow, 4, massperarea);

        // add longitudinal rebar to the spreadsheet
        theSpreadsheet->setString(currentRow, 5, lr_material);
        theSpreadsheet->setString(currentRow, 6, lr_materialCorner);
        theSpreadsheet->setDouble(currentRow, 7, lr_numBarsDepth);
        theSpreadsheet->setDouble(currentRow, 8, lr_numBarsWidth);
        theSpreadsheet->setDouble(currentRow, 9, lr_numBarsDepth);
        theSpreadsheet->setDouble(currentRow, 10, lr_barArea);
        theSpreadsheet->setDouble(currentRow, 11, lr_spacing);
        theSpreadsheet->setDouble(currentRow, 12, lr_cover);

        // add transverse rebar to the spreadsheet
        theSpreadsheet->setString(currentRow, 13, tr_material);
        theSpreadsheet->setDouble(currentRow, 14, tr_numBarsDepth);
        theSpreadsheet->setDouble(currentRow, 15, tr_numBarsWidth);
        theSpreadsheet->setDouble(currentRow, 16, tr_numBarsThickness);
        theSpreadsheet->setDouble(currentRow, 17, tr_barArea);
        theSpreadsheet->setDouble(currentRow, 18, tr_barAreaCorner);
        theSpreadsheet->setDouble(currentRow, 19, tr_spacing);
        theSpreadsheet->setDouble(currentRow, 20, tr_cover);

        // add shear rebar to the spreadsheet
        theSpreadsheet->setString(currentRow, 21, shr_material);
        theSpreadsheet->setDouble(currentRow, 22, shr_barArea);
        theSpreadsheet->setDouble(currentRow, 23, shr_spacing);


        currentRow++;
    }


}


void
SlabsectionInputWidget::clear(void)
{
    theSpreadsheet->clear();
}
