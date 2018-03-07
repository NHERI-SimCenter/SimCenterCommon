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

FramesectionInputWidget::FramesectionInputWidget(SimCenterWidget *parent) : SimCenterTableWidget(parent)
{
    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);

    QStringList headings;
    QList<int> dataTypes;
    headings << tr("Name");
    dataTypes << SIMPLESPREADSHEET_QString;
    headings << tr("Type");
    dataTypes << SIMPLESPREADSHEET_QString;
    headings << tr("Material");
    dataTypes << SIMPLESPREADSHEET_QString;

    headings << tr("Depth");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("Width");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("Shape");
    dataTypes << SIMPLESPREADSHEET_QString;

    headings << tr("thickness");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("weld length brace");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("weld length column");
    dataTypes << SIMPLESPREADSHEET_QString;
    headings << tr("weld length baseplate");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("thickness baseplate");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("workpoint depth");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("top flange width");
    dataTypes << SIMPLESPREADSHEET_QString;
    headings << tr("top flange thickness");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("web thickness");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("bottom flange width");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("bottom flange thickness");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("fillet radius");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("corner radius");
    dataTypes << SIMPLESPREADSHEET_QDouble;

    headings << tr("longitudinal rebar material");
    dataTypes << SIMPLESPREADSHEET_QString;
    headings << tr("longitudinal rebar material corner");
    dataTypes << SIMPLESPREADSHEET_QString;
    headings << tr("longitudinal rebar num bars depth");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("longitudinal rebar num bars width");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("longitudinal rebar bar area");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("longitudinal rebar bar area corner");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("longitudinal rebar cover");
    dataTypes << SIMPLESPREADSHEET_QDouble;

    headings << tr("transverse rebar material");
    dataTypes << SIMPLESPREADSHEET_QString;
    headings << tr("transverse rebar num bars depth");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("transverse rebar num bars width");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("transverse rebar num bars thickness");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("transverse rebar bar area");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("transverse rebar spacing");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    headings << tr("transverse rebar cover");
    dataTypes << SIMPLESPREADSHEET_QDouble;

    theSpreadsheet = new SpreadsheetWidget(headings.size(), 1000, headings, dataTypes, this);

    theLayout->addWidget(theSpreadsheet);

    this->setMinimumWidth(500);
}


FramesectionInputWidget::~FramesectionInputWidget()
{

}

void
FramesectionInputWidget::outputToJSON(QJsonObject &jsonObj){

        QJsonArray  jsonArray;
        int numRows = theSpreadsheet->getNumRows();
        for (int i=0; i<numRows; i++) {

            QJsonObject obj;
            QString name, fs_type, fs_material, shape;
            double depth, width, thickness, wlb, wlc, wlbp, tbp, wpd, tfw, tft, wt, bfw, bft, fr, cr;

            // longitudinal rebar
            QString lr_material, lr_materialCorner;
            double lr_numBarsDepth, lr_numBarsWidth, lr_barArea, lr_barAreaCorner, lr_cover;

            // transverse rebar
            QString tr_material;
            double tr_numBarsDepth, tr_numBarsWidth, tr_numBarsThickness, tr_barArea, tr_barAreaCorner, tr_spacing, tr_cover;



            // obtain info from spreadsheet
            if (theSpreadsheet->getString(i,0,name) == false || name.isEmpty())
                break;
            if (theSpreadsheet->getString(i,1,fs_type) == false)
                break;
            if (theSpreadsheet->getString(i,2, fs_material) == false)
                break;
            if (theSpreadsheet->getDouble(i,3,depth) == false)
                break;
            if (theSpreadsheet->getDouble(i,4,width) == false)
                break;
            if (theSpreadsheet->getString(i,5,shape) == false)
                break;
            if (theSpreadsheet->getDouble(i,6,thickness) == false)
                break;
            if (theSpreadsheet->getDouble(i,7,wlb) == false)
                break;
            if (theSpreadsheet->getDouble(i,8,wlc) == false)
                break;
            if (theSpreadsheet->getDouble(i,9,wlbp) == false)
                break;
            if (theSpreadsheet->getDouble(i,10,tbp) == false)
                break;
            if (theSpreadsheet->getDouble(i,11,wpd) == false)
                break;

            if (theSpreadsheet->getDouble(i,12,tfw) == false)
                break;
            if (theSpreadsheet->getDouble(i,13,tft) == false)
                break;
            if (theSpreadsheet->getDouble(i,14,wt) == false)
                break;
            if (theSpreadsheet->getDouble(i,15,bfw) == false)
                break;
            if (theSpreadsheet->getDouble(i,16,bft) == false)
                break;
            if (theSpreadsheet->getDouble(i,17,fr) == false)
                break;
            if (theSpreadsheet->getDouble(i,18,cr) == false)
                break;

            // longitudinal rebar  ********* FIX THIS ************* then renumber!!!
            if (theSpreadsheet->getString(i,19,lr_material) == false)
                break;
            if (theSpreadsheet->getString(i,20,lr_materialCorner) == false)
                break;
            if (theSpreadsheet->getDouble(i,21,lr_numBarsDepth) == false)
                break;
            if (theSpreadsheet->getDouble(i,22,lr_numBarsWidth) == false)
                break;
            if (theSpreadsheet->getDouble(i,23,lr_barArea) == false)
                break;
            if (theSpreadsheet->getDouble(i,24,lr_barAreaCorner) == false)
                break;
            if (theSpreadsheet->getDouble(i,25,lr_cover) == false)
                break;

            // transverse rebar
            if (theSpreadsheet->getString(i,26,tr_material) == false)
                break;
            if (theSpreadsheet->getDouble(i,27,tr_numBarsDepth) == false)
                break;
            if (theSpreadsheet->getDouble(i,28,tr_numBarsWidth) == false)
                break;
            if (theSpreadsheet->getDouble(i,29,tr_numBarsThickness) == false)
                break;
            if (theSpreadsheet->getDouble(i,30,tr_barArea) == false)
                break;
//            if (theSpreadsheet->getDouble(i,31,tr_barAreaCorner) == false)
//                break;
            if (theSpreadsheet->getDouble(i,31,tr_spacing) == false)
                break;
            if (theSpreadsheet->getDouble(i,32,tr_cover) == false)
                break;




            // now add the items to object
            obj["name"]=name;
            obj["type"]=fs_type;
            obj["material"]=fs_material;
            obj["depth"]=depth;
            obj["width"]=width;
            obj["shape"]=shape;
            obj["thickness"]=thickness;
            obj["weld length brace"]=wlb;
            obj["weld length column"]=wlc;
            obj["weld length baseplate"]=wlbp;
            obj["thickness baseplate"]=tbp;
            obj["workpoint depth"]=wpd;
            obj["top flange width"]=tfw;
            obj["top flange thickness"]=tft;
            obj["web thickness"]=wt;
            obj["bottom flange width"]=bfw;
            obj["bottom flange thickness"]=bft;
            obj["fillet radius"]=fr;
            obj["corner radius"]=cr;


            // longitudinal rebar   -FIX THIS!!!!!!! todo
            QJsonObject lrebar;

            lrebar["material"]=lr_material;
            lrebar["material corner"]=lr_materialCorner;
            lrebar["num bars depth"]=lr_numBarsDepth;
            lrebar["num bars width"]=lr_numBarsWidth;
            lrebar["bar area"]=lr_barArea;
            lrebar["bar area corner"]=lr_barAreaCorner;
            lrebar["cover"]=lr_cover;

            obj["longitudinal rebar"] = lrebar;

            // transverse rebar
            QJsonObject trebar;

            trebar["material"]=tr_material;
            trebar["bar area"]=tr_barArea;
//            trebar["bar area corner"]=tr_barAreaCorner;
            trebar["spacing"]=tr_spacing;
            trebar["cover"]=tr_cover;
            trebar["num bars depth"]=tr_numBarsDepth;
            trebar["num bars width"]=tr_numBarsWidth;
            trebar["num bars thickness"]=tr_numBarsThickness;

            obj["transverse rebar"] = trebar;


            // add the object to the array
            jsonArray.append(obj);
        }

        // add the object
        jsonObj["framesections"] = jsonArray;

}

void
FramesectionInputWidget::inputFromJSON(QJsonObject &jsonObject){

    int currentRow = 0;

    QString name, fs_type, fs_material, shape;
    double depth, width, thickness, wlb, wlc, wlbp, tbp, wpd, tfw, tft, wt, bfw, bft, fr, cr;

    // longitudinal rebar
    QString lr_material, lr_materialCorner;
    double lr_numBarsDepth, lr_numBarsWidth, lr_barArea, lr_barAreaCorner, lr_cover, lr_spacing;

    // transverse rebar
    QString tr_material;
    double tr_numBarsDepth, tr_numBarsWidth, tr_numBarsThickness, tr_barArea, tr_barAreaCorner, tr_spacing, tr_cover;

    //
    // get the cline data (a json array) from the object, and for every
    // object in the array, get the values and add to the spreadsheet
    //

    QJsonArray theArray = jsonObject["framesections"].toArray();
    foreach (const QJsonValue &theValue, theArray) {
        // get values
        QJsonObject theObject = theValue.toObject();

        name = theObject["name"].toString();
        fs_type = theObject["type"].toString();
        fs_material = theObject["material"].toString();
        depth = theObject["depth"].toDouble();
        width = theObject["width"].toDouble();
        shape = theObject["shape"].toString();

        thickness = theObject["thickness"].toDouble();
        wlb = theObject["weld length brace"].toDouble();
        wlc = theObject["weld length column"].toDouble();
        wlbp = theObject["weld length baseplate"].toDouble();
        tbp = theObject["thickness baseplate"].toDouble();
        wpd = theObject["workpoint depth"].toDouble();
        tfw = theObject["top flange width"].toDouble();
        tft = theObject["top flange thickness"].toDouble();
        wt = theObject["web thickness"].toDouble();
        bfw = theObject["bottom flange width"].toDouble();
        bft = theObject["bottom flange thickness"].toDouble();
        fr = theObject["fillet radius"].toDouble();
        cr = theObject["corner radius"].toDouble();

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


        // add to the spreadsheet
        theSpreadsheet->setString(currentRow, 0, name);
        theSpreadsheet->setString(currentRow, 1, fs_type);
        theSpreadsheet->setString(currentRow, 2, fs_material);

        theSpreadsheet->setDouble(currentRow, 3, depth);
        theSpreadsheet->setDouble(currentRow, 4, width);
        theSpreadsheet->setString(currentRow, 5, shape);

        theSpreadsheet->setDouble(currentRow, 6, thickness);
        theSpreadsheet->setDouble(currentRow, 7, wlb);
        theSpreadsheet->setDouble(currentRow, 8, wlc);
        theSpreadsheet->setDouble(currentRow, 9, wlbp);
        theSpreadsheet->setDouble(currentRow, 10, tbp);
        theSpreadsheet->setDouble(currentRow, 11, wpd);
        theSpreadsheet->setDouble(currentRow, 12, tfw);
        theSpreadsheet->setDouble(currentRow, 13, tft);
        theSpreadsheet->setDouble(currentRow, 14, wt);
        theSpreadsheet->setDouble(currentRow, 15, bfw);
        theSpreadsheet->setDouble(currentRow, 16, bft);
        theSpreadsheet->setDouble(currentRow, 17, fr);
        theSpreadsheet->setDouble(currentRow, 18, cr);


        // add longitudinal rebar to the spreadsheet
        theSpreadsheet->setString(currentRow, 19, lr_material);
        theSpreadsheet->setString(currentRow, 20, lr_materialCorner);
        theSpreadsheet->setDouble(currentRow, 21, lr_numBarsDepth);
        theSpreadsheet->setDouble(currentRow, 21, lr_numBarsWidth);
        theSpreadsheet->setDouble(currentRow, 23, lr_numBarsDepth);
        theSpreadsheet->setDouble(currentRow, 24, lr_barArea);
        theSpreadsheet->setDouble(currentRow, 25, lr_spacing);
        theSpreadsheet->setDouble(currentRow, 26, lr_cover);

        // add transverse rebar to the spreadsheet

        theSpreadsheet->setString(currentRow, 27, tr_material);
        theSpreadsheet->setDouble(currentRow, 28, tr_numBarsDepth);
        theSpreadsheet->setDouble(currentRow, 29, tr_numBarsWidth);
        theSpreadsheet->setDouble(currentRow, 30, tr_numBarsThickness);
        theSpreadsheet->setDouble(currentRow, 31, tr_barArea);
        theSpreadsheet->setDouble(currentRow, 32, tr_barAreaCorner);
        theSpreadsheet->setDouble(currentRow, 33, tr_spacing);
        theSpreadsheet->setDouble(currentRow, 34, tr_cover);


        currentRow++;
    }

}


void
FramesectionInputWidget::clear(void)
{
    theSpreadsheet->clear();
}

