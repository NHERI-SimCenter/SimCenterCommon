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
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

//
// function to read from a jsn object the value associated with a key.
// value may be a RV.name or a double value, if RV.name RV. to be stripped
//


// Written: fmckenna

#include "MDOF_BuildingModel.h"
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <sectiontitle.h>
#include <QFileInfo>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;
#include <QGridLayout>
#include <SpreadsheetWidget.h>
#ifdef _GRAPHICS_Qt3D
   #include <GraphicView2D.h>
#else
#include <GlWidget2D.h>
#endif



#include <QHeaderView>
#include <RandomVariable.h>

#include <math.h>

#include <GeneralInformationWidget.h>
#include <OpenSeesParser.h>
#include <RandomVariablesContainer.h>
#include <QTableWidget>


//#include <InputWidgetParameters.h>
#include <QValidator>


bool
readLineEditRV(QJsonObject &jsonObject, const char *key, QLineEdit *value)
{
    if (jsonObject.contains(key)) {
        QJsonValue theValue = jsonObject[key];
        if (theValue.isString()) {
            QString text = theValue.toString();
            text.remove(0,3); // remove RV.
            value->setText(text);

        } else if (theValue.isDouble())
            value->setText(QString::number(theValue.toDouble()));
    }
    return false;
}

// function to write to a json object a key-value pair. if value is not
// a double it is treated as a RV

bool
writeLineEditRV(QJsonObject &jsonObject, const char *key, QLineEdit *value)
{
    QString valueText = value->text();
    bool ok;

    double valueDouble = valueText.QString::toDouble(&ok);
    if (ok == true)
        jsonObject[key]=valueDouble;
    else
        jsonObject[key]= QString("RV.") + valueText;

    return true;
}

bool
readCellRV(QJsonObject &jsonObject, const char *key, QTableWidgetItem *value)
{
    if (jsonObject.contains(key)) {
        QJsonValue theValue = jsonObject[key];
        if (theValue.isString()) {
            QString text = theValue.toString();
            text.remove(0,3); // remove RV.
            value->setText(text);

        } else if (theValue.isDouble())
            value->setText(QString::number(theValue.toDouble()));
        return true;
    }
    return false;
}


bool
writeCellRV(QJsonObject &jsonObject, const char *key, QTableWidgetItem *value)
{
    QString valueText = value->text();
    bool ok;

    double valueDouble = valueText.QString::toDouble(&ok);
    if (ok == true)
        jsonObject[key]=valueDouble;
    else
        jsonObject[key]= QString("RV.") + valueText;

    return true;
}






static QLineEdit *
createTextEntry(QString text,
                QString toolTip,
                QGridLayout *theLayout,
                int row,
                int col =0,
                int minL=100,
                int maxL=100,
                QString *unitText = NULL,
                bool itemRight = false)
{
    Q_UNUSED(itemRight);
    //QHBoxLayout *entryLayout = new QHBoxLayout();
    QLabel *entryLabel = new QLabel();

    entryLabel->setText(text);

    QLineEdit *res = new QLineEdit();
    res->setText("0.0");
    res->setMinimumWidth(minL);
    res->setMaximumWidth(maxL);
    //res->setValidator(new QDoubleValidator);

    theLayout->addWidget(entryLabel,row,col);
    theLayout->addWidget(res,row,col+1);

    if (unitText != NULL) {
        QLabel *unitLabel = new QLabel();
        unitLabel->setText(*unitText);
        unitLabel->setMinimumWidth(40);
        unitLabel->setMaximumWidth(50);
        //entryLayout->addWidget(unitLabel);
        theLayout->addWidget(unitLabel, row,col+2);
    }

    if (!toolTip.isEmpty()) {
        res->setToolTip(toolTip);
    }

    return res;
}

MDOF_BuildingModel::MDOF_BuildingModel(RandomVariablesContainer *theRandomVariableIW, QWidget *parent)
  : SimCenterAppWidget(parent), theRandomVariablesContainer(theRandomVariableIW), 
    numStories(0),
    floorHeights(0), storyHeights(0),
    fMinSelected(-1),fMaxSelected(-1), 
    sMinSelected(-1),sMaxSelected(-1),
    floorSelected(-1),storySelected(-1)
{
    numStories = 0; // originally set to 0, so that when setnumStories text later no seg fault
    floorW = "144";
    storyH = "144.0";
    Kx = "100.0";
    Fyx = "1.0e6";
    bx = "0.1";
    Ky ="100.0";
    Fyy = "1.0e6";
    by = "0.1";
    K_theta = "1e10";

    QHBoxLayout *layout = new QHBoxLayout();

    QVBoxLayout *inputLayout = new QVBoxLayout();
    QVBoxLayout *graphicLayout = new QVBoxLayout();

    //
    // create to hold major model inputs
    //

    //Building information
    QGroupBox* mainProperties = new QGroupBox("Building Information");
    QGridLayout *mainPropertiesLayout = new QGridLayout();
    inFloors = createTextEntry(tr("Number Stories"), tr("number of stories in building"),mainPropertiesLayout, 0, 0, 100, 100);
    inWeight = createTextEntry(tr("Floor Weights"), tr("total building weight, each floor will have a weight given by weight/ number of floors"), mainPropertiesLayout, 1, 0, 100, 100);
    storyHeight = createTextEntry(tr("Story Heights"), tr("story heights, building height equals number of fstories * story heighyt"),mainPropertiesLayout, 1, 3, 100, 100);
    inKx = createTextEntry(tr("Story Stiffness X dirn"), tr("story stiffnesses, that force required to push the floor above unit distance, assuming all other stories have infinite stiffness"),mainPropertiesLayout, 2, 0, 100, 100);
    inKy = createTextEntry(tr("Story Stiffness Y dirn"), tr("story stiffnesses, that force required to push the floor above unit distance, assuming all other stories have infinite stiffness"),mainPropertiesLayout, 2, 3, 100, 100);
    inK_theta = createTextEntry(tr("Rotational Story Stiffness"),tr("rotational story stiffness"), mainPropertiesLayout, 5, 0, 100, 100);
    inFyx = createTextEntry(tr("Yield Strength X dirn"),tr("The force in X direction at which the story yields"), mainPropertiesLayout,3,0, 100,100);
    inBx = createTextEntry(tr("Hardening Ratio X dirn"), tr("Hardening ratio defines ratio between original stiffness and current stiffness in X direction"), mainPropertiesLayout,4, 0);
    inFyy = createTextEntry(tr("Yield Strength Y dirn"),tr("The force in Y direction at which the story yields"), mainPropertiesLayout,3,3, 100,100);
    inBy = createTextEntry(tr("Hardening Ratio Y dirn"), tr("Hardening ratio defines ratio between original stiffness and current stiffness in Y direction"), mainPropertiesLayout,4, 3);

    massX = createTextEntry(tr("Mass Ecentricity X dirn"), tr("Mass eccentricty x dirn"), mainPropertiesLayout,6, 0);
    massY = createTextEntry(tr("Mass Ecentricity Y dirn"), tr("Mass eccentricty y dirn"), mainPropertiesLayout,6, 3);

    responseX = createTextEntry(tr("Response Ecentricity X dirn"), tr("Response eccentricty x dirn"), mainPropertiesLayout,7, 0);
    responseY = createTextEntry(tr("Response Ecentricity Y dirn"), tr("Response eccentricty y dirn"), mainPropertiesLayout,7, 3);

    responseX->setValidator(new QDoubleValidator);
    responseY->setValidator(new QDoubleValidator);

    inFloors->setValidator(new QIntValidator);
    storyHeight->setValidator(new QDoubleValidator);

    //inWeight->setValidator(new QDoubleValidator);
    //inKx->setValidator(new QDoubleValidator);
    //inKy->setValidator(new QDoubleValidator);
    //inK_theta->setValidator(new QDoubleValidator);

    inFloors->setText(QString::number(1));
    inWeight->setText(floorW);
    storyHeight->setText(storyH);
    inKx->setText(Kx);
    inKy->setText(Ky);
    inK_theta->setText(K_theta);
    inFyx->setText(Fyx);
    inFyy->setText(Fyy);
    inBx->setText(bx);
    inBy->setText(by);   

    connect(inFloors,SIGNAL(editingFinished()), this, SLOT(on_inFloors_editingFinished()));
    connect(inWeight,SIGNAL(editingFinished()), this, SLOT(on_inWeight_editingFinished()));
    connect(storyHeight,SIGNAL(editingFinished()), this, SLOT(on_storyHeight_editingFinished()));
    connect(inKx,SIGNAL(editingFinished()), this, SLOT(on_inKx_editingFinished()));
    connect(inKy,SIGNAL(editingFinished()), this, SLOT(on_inKy_editingFinished()));
    connect(inK_theta,SIGNAL(editingFinished()), this, SLOT(on_inK_theta_editingFinished()));
    connect(inFyx,SIGNAL(editingFinished()), this, SLOT(on_inFyx_editingFinished()));
    connect(inFyy,SIGNAL(editingFinished()), this, SLOT(on_inFyy_editingFinished()));
    connect(inBx,SIGNAL(editingFinished()), this, SLOT(on_inBx_editingFinished()));
    connect(inBy,SIGNAL(editingFinished()), this, SLOT(on_inBy_editingFinished()));

    mainProperties->setLayout(mainPropertiesLayout);
    inputLayout->addWidget(mainProperties);

    //
    // widget to hold floor weight properties for when a number of floors selected
    //

    floorMassFrame = new QGroupBox("Selected Floor Weights");
    QGridLayout *floorMassFrameLayout = new QGridLayout();
    inFloorWeight = createTextEntry(tr("Floor Weight"), tr("individual floor weight, will change total weight if edit"),floorMassFrameLayout,0,0, 100, 100);
    floorMassFrame->setLayout(floorMassFrameLayout);
    inputLayout->addWidget(floorMassFrame);
    floorMassFrame->setVisible(false);

    connect(inFloorWeight,SIGNAL(editingFinished()), this, SLOT(on_inFloorWeight_editingFinished()));

    //
    // widget to hold story properties for when a number of floors selected
    //

    storyPropertiesFrame = new QGroupBox("Selected Story Properties");
    storyPropertiesFrame->setObjectName(QString::fromUtf8("storyPropertiesFrame"));
    QGridLayout *storyPropertiesFrameLayout = new QGridLayout();
    inStoryHeight = createTextEntry(tr("Story Height"), tr("for stories selected sets story height, effects overall height if edited"), storyPropertiesFrameLayout,0,0,100,100);
    inStoryHeight->setValidator(new QDoubleValidator);

    inStoryKx = createTextEntry(tr("Stiffness X dirn"), tr("for stories selected force required to push each floor 1 inch assuming all other floors infinite stiffness"), storyPropertiesFrameLayout,1,0, 100,100);
    inStoryFyx = createTextEntry(tr("Yield Strength X dirn"),tr("for stories selected force at which story yields, i.e. if aply more force floor will not return to original position assuming all other rigid"), storyPropertiesFrameLayout,2,0, 100,100);
    inStoryBx = createTextEntry(tr("Hardening Ratio X dirn"), tr("for stories selected the hardening ratio defines ratio between original stiffness and current stiffness"), storyPropertiesFrameLayout,3, 0);
    inStoryKy = createTextEntry(tr("Stiffness Y dirn"), tr("for stories selected force required to push each floor 1 inch assuming all other floors infinite stiffness"), storyPropertiesFrameLayout,1,3, 100,100);
    inStoryFyy = createTextEntry(tr("Yield Strength Y dirn"),tr("for stories selected force at which story yields, i.e. if aply more force floor will not return to original position assuming all other rigid"), storyPropertiesFrameLayout,2,3, 100,100);
    inStoryBy = createTextEntry(tr("Hardening Ratio Y dirn"), tr("for stories selected the hardening ratio defines ratio between original stiffness and current stiffness"), storyPropertiesFrameLayout,3, 3);

    storyPropertiesFrame->setLayout(storyPropertiesFrameLayout);
    storyPropertiesFrame->setLayout(storyPropertiesFrameLayout);

    inputLayout->addWidget(storyPropertiesFrame);
    storyPropertiesFrame->setVisible(false);

    connect(inStoryHeight, SIGNAL(editingFinished()), this, SLOT(on_inStoryHeight_editingFinished()));
    connect(inStoryKx, SIGNAL(editingFinished()), this, SLOT(on_inStoryKx_editingFinished()));
    connect(inStoryFyx,SIGNAL(editingFinished()),this, SLOT(on_inStoryFyx_editingFinished()));
    connect(inStoryBx, SIGNAL(editingFinished()),this,SLOT(on_inStoryBx_editingFinished()));
    connect(inStoryKy, SIGNAL(editingFinished()), this, SLOT(on_inStoryKy_editingFinished()));
    connect(inStoryFyy,SIGNAL(editingFinished()),this, SLOT(on_inStoryFyy_editingFinished()));
    connect(inStoryBy, SIGNAL(editingFinished()),this,SLOT(on_inStoryBy_editingFinished()));

    //
    // add spreadsheet to hold individual properties
    //

    QStringList headings;
    headings << tr("Weight") << tr("Height") << tr("K_x") << tr("Fy_x") << tr("b_x") << tr("K_y") << tr("Fy_y") << tr("b_y") << tr("K_theta");
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;

    //theSpreadsheet = new SpreadsheetWidget(9, 4, headings, dataTypes,0);
    theSpreadsheet = new QTableWidget();
    inputLayout->addWidget(theSpreadsheet);

    connect(theSpreadsheet, SIGNAL(cellClicked(int,int)), this, SLOT(on_theSpreadsheet_cellClicked(int,int)));
    connect(theSpreadsheet, SIGNAL(cellEntered(int,int)), this,SLOT(on_theSpreadsheet_cellClicked(int,int)));
    connect(theSpreadsheet, SIGNAL(cellChanged(int,int)), this,SLOT(on_theSpreadsheet_cellChanged(int,int)));

    inputLayout->addStretch();
    theView = 0;

#ifdef _GRAPHICS_Qt3D
   theView = new GraphicView2D();
#else
    theView = new GlWidget2D();
    theView->setController(this);
#endif


    //theView = new GlWidget2D();
    //theView->setController(this);


   //  theView->setMinimumHeight(250);
   //  theView->setMinimumWidth(250);

    theView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    graphicLayout->addWidget(theView);


    layout->addLayout(inputLayout);
    layout->addLayout(graphicLayout);

    layout->addStretch();
    this->setLayout(layout);

    this->on_inFloors_editingFinished();


    // add signal and slot connections with GI
    GeneralInformationWidget *theGI = GeneralInformationWidget::getInstance();
    double plan, w, d;
    theGI->getBuildingDimensions(w, d, plan);
    buildingW = w;
    buildingD = d;

    /*    connect(this,SIGNAL(numFloorsChanged(int)), theGI, SLOT(setNumFloors(int)));
    connect(this,SIGNAL(heightChanged(double)), theGI, SLOT(setHeight(double)));
    connect(theGI,SIGNAL(numFloorsChanged(int)), this, SLOT(setNumFloors(int)));
    connect(theGI,SIGNAL(buildingHeightChanged(double)), this, SLOT(setHeight(double)));
    */
    connect(this,SIGNAL(numStoriesOrHeightChanged(int, double)), theGI, SLOT(setNumStoriesAndHeight(int, double)));
    connect(theGI,SIGNAL(numStoriesOrHeightChanged(int, double)), this, SLOT(setNumStoriesAndHeight(int, double)));
    connect(theGI,SIGNAL(buildingDimensionsChanged(double,double,double)),this,SLOT(setBuildingDimensions(double,double,double)));
}

MDOF_BuildingModel::~MDOF_BuildingModel()
{
    if (storyHeights != 0)
        delete [] storyHeights;
    if (floorHeights != 0)
        delete [] floorHeights;
}


void
MDOF_BuildingModel::on_inFloors_editingFinished()
{

    QString textFloors =  inFloors->text();
    int numStoriesText = textFloors.toInt();

    if (numStoriesText <= 0) {
        inFloors->setText(QString(numStories));
        return;
    }

    if (numStoriesText != numStories) {

        numStories = numStoriesText;

        floorSelected = -1;
        storySelected = -1;
        fMinSelected = -1;
        fMaxSelected = -1;
        sMinSelected = -1;
        sMaxSelected = -1;

        if (floorHeights != 0)
            delete [] floorHeights;
        if (storyHeights != 0)
            delete [] storyHeights;

        floorHeights = new double[numStories+1];
        storyHeights = new double[numStories];

        // remove random variables
        QMap<QString, int>::iterator i;
        QStringList rvs;
        for (i = randomVariables.begin(); i != randomVariables.end(); ++i)
            rvs << i.key();
        theRandomVariablesContainer->removeRandomVariables(rvs);

        randomVariables.clear();

        //this->updateSpreadsheet();
        theSpreadsheet->clear();
        theSpreadsheet->setColumnCount(9);
        theSpreadsheet->setRowCount(numStories);

        // theSpreadsheet->horizontalHeader()->setStretchLastSection(true);// horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
        //theSpreadsheet->setFixedWidth(344);

        QStringList headings;
        headings << tr("Weight") << tr("Height") << tr("K_x") << tr("Fy_x") << tr("b_x") << tr("K_y") << tr("Fy_y") << tr("b_y") << tr("K_theta");

        // determine floor weights (only if building weight not RV);
        floorW = inWeight->text();
        bool ok;
        double storyHeightToDouble = storyH.toDouble(&ok);
        if (ok == false)
            storyHeightToDouble = 1.0;
        buildingH = storyHeightToDouble * numStories;

        // determine floor and story heights
        for (int i=0; i<numStories; i++) {
            floorHeights[i] = i*buildingH/(1.*numStories);
            storyHeights[i] = buildingH/(1.*numStories);
        }
        floorHeights[numStories] = buildingH;

        //
        // create spreadsheet entries
        //

        updatingPropertiesTable = true;
        for (int i=0; i<numStories; i++) {

            QTableWidgetItem *item;
            item = new QTableWidgetItem(floorW);
            item->setToolTip(QString("weight of floor " + QString::number(i+1)));
            theSpreadsheet->setItem(i,0, item);

            item = new QTableWidgetItem(QString().setNum(storyHeights[i]));
            item->setToolTip(QString("height of story " + QString::number(i+1)));
            theSpreadsheet->setItem(i,1,item);

            item = new QTableWidgetItem(Kx);
            item->setToolTip(QString("initial stiffness of story " + QString::number(i+1) + " in x dirn"));
            theSpreadsheet->setItem(i,2,item);

            item = new QTableWidgetItem(Fyx);
            item->setToolTip(QString("yield strength of story " + QString::number(i+1) + " in x dirn"));
            theSpreadsheet->setItem(i,3,item);

            item = new QTableWidgetItem(bx);
            item->setToolTip(QString("hardening ratio of story " + QString::number(i+1) + " in x dirn"));
            theSpreadsheet->setItem(i,4, item);

            item = new QTableWidgetItem(Ky);
            item->setToolTip(QString("initial stiffness of story " + QString::number(i+1)) + " in y dirn");
            theSpreadsheet->setItem(i,5,item);

            item = new QTableWidgetItem(Fyy);
            item->setToolTip(QString("yield strength of story " + QString::number(i+1) + " in y dirn"));
            theSpreadsheet->setItem(i,6,item);

            item = new QTableWidgetItem(by);
            item->setToolTip(QString("hardening ratio of story " + QString::number(i+1) + " in y dirn"));
            theSpreadsheet->setItem(i,7, item);

            item = new QTableWidgetItem(K_theta);
            item->setToolTip(QString("rotational stiffness of floor  " + QString::number(i+1)));
            theSpreadsheet->setItem(i,8, item);
        }


        double value = Kx.toDouble(&ok);
        Q_UNUSED(value);
        if (!ok) {
             this->addRandomVariable(Kx,numStories);
        }

        // theSpreadsheet->resizeRowsToContents();
        // theSpreadsheet->resizeColumnsToContents();
        theSpreadsheet->setHorizontalHeaderLabels(headings);
        theSpreadsheet->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        theSpreadsheet->verticalHeader()->setVisible(false);

        updatingPropertiesTable = false;

        floorSelected = -1;
        storySelected = -1;

        emit numStoriesOrHeightChanged(numStories, buildingH);
	//        emit heightChanged(buildingH);
    }

    if (theView != 0) {
        this->draw();
#ifndef _GRAPHICS_Qt3D
        theView->update();
#endif
    }

}



void
MDOF_BuildingModel::on_inWeight_editingFinished()
{
    QString text =  inWeight->text();

    // if null, use old
    if (text.isNull()) {
        inWeight->setText(floorW);
        return;
    }

    // set new Kx value
    floorW = text;

    //
    // update spreadsheet column with new value
    //

    bool ok;
    updatingPropertiesTable = true;
    for (int i=0; i<numStories; i++) {
        QTableWidgetItem *item = theSpreadsheet->item(i,0);

        QString oldText = item->text();
        if (oldText != text) {
             // if old text not double, it was a random variable need to remove it
            double value = oldText.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->removeRandmVariable(oldText);
            }
            // if new text not double, add as RV
            value = text.toDouble(&ok);
            if (!ok) {
                this->addRandomVariable(text);
            }
            // set new text
            item->setText(text);
        }
    }
    updatingPropertiesTable = false;
}

void
MDOF_BuildingModel::on_storyHeight_editingFinished()
{
    QString text =  storyHeight->text();

    qDebug() << "MDOF::on_storyHeight" << text;

    if (text.isNull()) {
        storyHeight->setText(storyH);
        return;
    }

    storyH = text;

    bool ok;
    double storyHeight = text.toDouble(&ok);

    if (ok != true)
        storyHeight = 100.0;

    updatingPropertiesTable = true;
    buildingH = numStories * storyHeight;

    qDebug() << "MDOF::on_storyHeight" << text << " " << buildingH << " " << storyHeight;

    for (int i=0; i<numStories; i++) {
        QTableWidgetItem *item = theSpreadsheet->item(i,1);
        QString oldText=item->text();
        bool okOld;
        double value = oldText.toDouble(&okOld);
        Q_UNUSED(value);
        if (okOld != true) {
            this->removeRandmVariable(oldText);
        }
        if (ok != true) {
            this->addRandomVariable(text);
        }
        item->setText(text);
        floorHeights[i] = i*storyHeight;
        storyHeights[i] = storyHeight;
    }

    floorHeights[numStories] = buildingH;
    emit numStoriesOrHeightChanged(numStories, buildingH);

    if (theView != 0) {
        this->draw();
        theView->update();
    }

    updatingPropertiesTable = false;
}


void
MDOF_BuildingModel::on_inKx_editingFinished()
{
    QString text =  inKx->text();

    // if null, use old
    if (text.isNull()) {
        inKx->setText(Kx);
        return;
    }

    // set new Kx value
    Kx = text;

    //
    // update spreadsheet column with new value
    //

    bool ok;
    updatingPropertiesTable = true;
    for (int i=0; i<numStories; i++) {
        QTableWidgetItem *item = theSpreadsheet->item(i,2);

        QString oldText = item->text();
        if (oldText != text) {
             // if old text not double, it was a random variable need to remove it
            double value = oldText.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->removeRandmVariable(oldText);
            }
            // if new text not double, add as RV
            value = text.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->addRandomVariable(text);
            }
            // set new text
            item->setText(text);
        }
    }

    updatingPropertiesTable = false;
}

void
MDOF_BuildingModel::on_inKy_editingFinished()
{
    QString text =  inKy->text();

    // if null, use old
    if (text.isNull()) {
        inKy->setText(Ky);
        return;
    }

    // set new Kx value
    Ky = text;

    //
    // update spreadsheet column with new value
    //

    bool ok;
    updatingPropertiesTable = true;
    for (int i=0; i<numStories; i++) {
        QTableWidgetItem *item = theSpreadsheet->item(i,5);

        QString oldText = item->text();
        if (oldText != text) {
             // if old text not double, it was a random variable need to remove it
            double value = oldText.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->removeRandmVariable(oldText);
            }
            // if new text not double, add as RV
            value = text.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->addRandomVariable(text);
            }
            // set new text
            item->setText(text);
        }
    }
    updatingPropertiesTable = false;
}

void
MDOF_BuildingModel::on_inK_theta_editingFinished()
{
    QString text =  inK_theta->text();

    // if null, use old
    if (text.isNull()) {
        inK_theta->setText(K_theta);
        return;
    }

    // set new Kx value
    K_theta = text;

    //
    // update spreadsheet column with new value
    //

    bool ok;
    updatingPropertiesTable = true;
    for (int i=0; i<numStories; i++) {
        QTableWidgetItem *item = theSpreadsheet->item(i,8);

        QString oldText = item->text();
        if (oldText != text) {
             // if old text not double, it was a random variable need to remove it
            double value = oldText.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->removeRandmVariable(oldText);
            }
            // if new text not double, add as RV
            value = text.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->addRandomVariable(text);
            }
            // set new text
            item->setText(text);
        }
    }
    updatingPropertiesTable = false;
}

void MDOF_BuildingModel::on_inFyx_editingFinished()
{
    QString text =  inFyx->text();

    // if null, use old
    if (text.isNull()) {
        inFyx->setText(Fyx);
        return;
    }

    // set new Fyx value
    Fyx = text;

    //
    // update spreadsheet column with new value
    //

    bool ok;
    updatingPropertiesTable = true;
    for (int i=0; i<numStories; i++) {
        QTableWidgetItem *item = theSpreadsheet->item(i,3);

        QString oldText = item->text();
        if (oldText != text) {
             // if old text not double, it was a random variable need to remove it
            double value = oldText.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->removeRandmVariable(oldText);
            }
            // if new text not double, add as RV
            value = text.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->addRandomVariable(text);
            }
            // set new text
            item->setText(text);
        }
    }
    updatingPropertiesTable = false;

}

//TODO: This code is not maintainable, we need to switch to Model-View Architecture
void MDOF_BuildingModel::on_inFyy_editingFinished()
{
    QString text =  inFyy->text();

    // if null, use old
    if (text.isNull()) {
        inFyy->setText(Fyy);
        return;
    }

    // set new Fyy value
    Fyy = text;

    //
    // update spreadsheet column with new value
    //

    bool ok;
    updatingPropertiesTable = true;
    for (int i=0; i<numStories; i++) {
        QTableWidgetItem *item = theSpreadsheet->item(i,6);

        QString oldText = item->text();
        if (oldText != text) {
             // if old text not double, it was a random variable need to remove it
            double value = oldText.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->removeRandmVariable(oldText);
            }
            // if new text not double, add as RV
            value = text.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->addRandomVariable(text);
            }
            // set new text
            item->setText(text);
        }
    }
    updatingPropertiesTable = false;

}

//TODO: This code is not maintainable, we need to switch to Model-View Architecture
void MDOF_BuildingModel::on_inBx_editingFinished()
{
    QString text =  inBx->text();

    // if null, use old
    if (text.isNull()) {
        inBx->setText(bx);
        return;
    }

    // set new Bx value
    bx = text;

    //
    // update spreadsheet column with new value
    //

    bool ok;
    updatingPropertiesTable = true;
    for (int i=0; i<numStories; i++) {
        QTableWidgetItem *item = theSpreadsheet->item(i,4);

        QString oldText = item->text();
        if (oldText != text) {
             // if old text not double, it was a random variable need to remove it
            double value = oldText.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->removeRandmVariable(oldText);
            }
            // if new text not double, add as RV
            value = text.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->addRandomVariable(text);
            }
            // set new text
            item->setText(text);
        }
    }
    updatingPropertiesTable = false;

}

//TODO: This code is not maintainable, we need to switch to Model-View Architecture
void MDOF_BuildingModel::on_inBy_editingFinished()
{
    QString text =  inBy->text();

    // if null, use old
    if (text.isNull()) {
        inBy->setText(by);
        return;
    }

    // set new By value
    by = text;

    //
    // update spreadsheet column with new value
    //

    bool ok;
    updatingPropertiesTable = true;
    for (int i=0; i<numStories; i++) {
        QTableWidgetItem *item = theSpreadsheet->item(i,7);

        QString oldText = item->text();
        if (oldText != text) {
             // if old text not double, it was a random variable need to remove it
            double value = oldText.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->removeRandmVariable(oldText);
            }
            // if new text not double, add as RV
            value = text.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->addRandomVariable(text);
            }
            // set new text
            item->setText(text);
        }
    }
    updatingPropertiesTable = false;

}

void MDOF_BuildingModel::on_inFloorWeight_editingFinished()
{
    /*
    if (updatingPropertiesTable == true)
        return;

    QString text =  inFloorWeight->text();
    if (text.isNull())
        return;

    double textToDouble = text.toDouble();
    for (int i=fMinSelected-1; i<fMaxSelected; i++)
        if (i != -1)
            weights[i] = textToDouble;

    buildingW = 0;
    for (int i=0; i<numStories; i++)
        buildingW = buildingW+weights[i];

    inWeight->setText(QString::number(buildingW));
 */
}


void MDOF_BuildingModel::on_inStoryHeight_editingFinished()
{

    if (updatingPropertiesTable == true)
        return;

    qDebug() << "onInStoryHEightChanged";

    QString text =  inStoryHeight->text();
    if (text.isNull())
        return;

    double newStoryHeight = text.toDouble();
    double *newFloorHeights = new double[numStories+1];

    // determine new floor heights, cludgy can rewrite now store storyHeights
    newFloorHeights[0] = 0;
    for (int i=0; i<sMinSelected; i++)
        newFloorHeights[i+1] = floorHeights[i+1];
    updatingPropertiesTable = true;
    for (int i=sMinSelected; i<=sMaxSelected; i++) {
        newFloorHeights[i+1] = newFloorHeights[i]+newStoryHeight;
        storyHeights[i] = newStoryHeight;
    }
    updatingPropertiesTable = false;
    for (int i=sMaxSelected+1; i<numStories; i++)
        newFloorHeights[i+1] = newFloorHeights[i]+floorHeights[i+1]-floorHeights[i];


    bool needReset = false;
    for (int i=0; i<=numStories; i++) {
        if (floorHeights[i] != newFloorHeights[i]){
            needReset = true;
            i=numStories+1;
        }
    }
    Q_UNUSED(needReset);

    delete [] floorHeights;
    floorHeights = newFloorHeights;

    buildingH = newFloorHeights[numStories];
    qDebug() << "MDOF: newHeight" << floorHeights[numStories];
    emit numStoriesOrHeightChanged(numStories, buildingH);
}

void MDOF_BuildingModel::on_inStoryKx_editingFinished()
{
    if (sMinSelected == -1 || sMaxSelected == -1)
        return;

    QString text =  inStoryKx->text();

    if (text.isNull())
        return;

    //
    // loop over selected entries, changing values in spreadsheet and adding/removing random variables as needed
    //

    bool ok;
    updatingPropertiesTable = true;

    for (int i=sMinSelected; i<=sMaxSelected; i++) {
        QTableWidgetItem *item = theSpreadsheet->item(i,2);
        QString oldText = item->text();

        if (oldText != text) {
            // if old text not double, remove random Variable
            double value;
            value = oldText.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->removeRandmVariable(oldText);
            }
            // if new text not double, add random variable
            value = text.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->addRandomVariable(text);
            }
            item->setText(text);
        }
    }

    updatingPropertiesTable = false;
}

void MDOF_BuildingModel::on_inStoryFyx_editingFinished()
{
    if (sMinSelected == -1 || sMaxSelected == -1)
        return;

    QString text =  inStoryFyx->text();

    if (text.isNull())
        return;

    //
    // loop over selected entries, changing values in spreadsheet and adding/removing random variables as needed
    //

    bool ok;
    double value;
    updatingPropertiesTable = true;

    for (int i=sMinSelected; i<=sMaxSelected; i++) {
        QTableWidgetItem *item = theSpreadsheet->item(i,3);
        QString oldText = item->text();

        if (oldText != text) {
            // if old text not double, remove random Variable
            value = oldText.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->removeRandmVariable(oldText);
            }
            // if new text not double, add random variable
            value = text.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->addRandomVariable(text);
            }
            item->setText(text);
        }
    }

    updatingPropertiesTable = false;
}

void MDOF_BuildingModel::on_inStoryBx_editingFinished()
{
    if (sMinSelected == -1 || sMaxSelected == -1)
        return;

    QString text =  inStoryBx->text();

    if (text.isNull())
        return;

    //
    // loop over selected entries, changing values in spreadsheet and adding/removing random variables as needed
    //

    bool ok;
    double value;
    updatingPropertiesTable = true;

    for (int i=sMinSelected; i<=sMaxSelected; i++) {
        QTableWidgetItem *item = theSpreadsheet->item(i,4);
        QString oldText = item->text();

        if (oldText != text) {
            // if old text not double, remove random Variable
            value = oldText.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->removeRandmVariable(oldText);
            }
            // if new text not double, add random variable
            value = text.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->addRandomVariable(text);
            }
            item->setText(text);
        }
    }

    updatingPropertiesTable = false;
}

void MDOF_BuildingModel::on_inStoryKy_editingFinished()
{
    if (sMinSelected == -1 || sMaxSelected == -1)
        return;

    QString text =  inStoryKy->text();

    if (text.isNull())
        return;

    //
    // loop over selected entries, changing values in spreadsheet and adding/removing random variables as needed
    //

    bool ok;
    double value;
    updatingPropertiesTable = true;

    for (int i=sMinSelected; i<=sMaxSelected; i++) {
        QTableWidgetItem *item = theSpreadsheet->item(i,5);
        QString oldText = item->text();

        if (oldText != text) {
            // if old text not double, remove random Variable
            value = oldText.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->removeRandmVariable(oldText);
            }
            // if new text not double, add random variable
            value = text.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->addRandomVariable(text);
            }
            item->setText(text);
        }
    }

    updatingPropertiesTable = false;
}

void MDOF_BuildingModel::on_inStoryFyy_editingFinished()
{
    if (sMinSelected == -1 || sMaxSelected == -1)
        return;

    QString text =  inStoryFyy->text();

    if (text.isNull())
        return;

    //
    // loop over selected entries, changing values in spreadsheet and adding/removing random variables as needed
    //

    bool ok;
    double value;
    updatingPropertiesTable = true;

    for (int i=sMinSelected; i<=sMaxSelected; i++) {
        QTableWidgetItem *item = theSpreadsheet->item(i,6);
        QString oldText = item->text();

        if (oldText != text) {
            // if old text not double, remove random Variable
            value = oldText.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->removeRandmVariable(oldText);
            }
            // if new text not double, add random variable
            value = text.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->addRandomVariable(text);
            }
            item->setText(text);
        }
    }
}

void MDOF_BuildingModel::on_inStoryBy_editingFinished()
{
    if (sMinSelected == -1 || sMaxSelected == -1)
        return;

    QString text =  inStoryBy->text();

    if (text.isNull())
        return;

    //
    // loop over selected entries, changing values in spreadsheet and adding/removing random variables as needed
    //

    bool ok;
    double value;
    updatingPropertiesTable = true;

    for (int i=sMinSelected; i<=sMaxSelected; i++) {
        QTableWidgetItem *item = theSpreadsheet->item(i,7);
        QString oldText = item->text();

        if (oldText != text) {
            // if old text not double, remove random Variable
            value = oldText.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->removeRandmVariable(oldText);
            }
            // if new text not double, add random variable
            value = text.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->addRandomVariable(text);
            }
            item->setText(text);
        }
    }
}



void MDOF_BuildingModel::on_theSpreadsheet_cellClicked(int row, int column)
{
    QTableWidgetItem *item = theSpreadsheet->item(row,column);
    cellText = item->text();

    if (column == 0) {
        floorSelected = row+1;
        storySelected = -1;
    }

    else if (column > 0 && column < 8) {
        storySelected = row;
        floorSelected = -1;
    } else {
        storySelected = -1;
        floorSelected = -1;
    }

    fMinSelected = -1;
    fMaxSelected = -1;
    sMinSelected = -1;
    sMaxSelected = -1;

    this->draw();
    if (theView != 0)
    theView->update();
}


void MDOF_BuildingModel::on_theSpreadsheet_cellChanged(int row, int column)
{
    if (updatingPropertiesTable == false) {

        QTableWidgetItem *item = theSpreadsheet->item(row,column);
        QString text = item->text();

        bool ok;
        double textToDouble = text.toDouble(&ok);
        Q_UNUSED(textToDouble);

        if (cellText != text) {
            // if old text not double, remove random Variable
            double value = cellText.toDouble(&ok);
            Q_UNUSED(value);
            if (!ok) {
                this->removeRandmVariable(cellText);
            }
            // if new text not double, add random variable
            value = text.toDouble(&ok);
            Q_UNUSED(value);

            if (!ok) {
                this->addRandomVariable(text);
            } else {
                if (column == 1) {
                    double newHeight = 0.;
                    storyHeights[row] = 0.;
                    floorHeights[0] = 0;
                    for (int row=0; row<numStories; row++) {
                        QTableWidgetItem *item = theSpreadsheet->item(row,column);
                        QString text = item->text();

                        bool ok;
                        double textToDouble = text.toDouble(&ok);
                        storyHeights[row]=textToDouble;
                        newHeight += textToDouble;
                        floorHeights[row+1] = newHeight;
                    }
                    buildingH = newHeight;

                    if (theView != 0) {
                        this->draw();
                        theView->update();
                    }


                    emit numStoriesOrHeightChanged(numStories, buildingH);
                }
            }
        }
    }
}


void MDOF_BuildingModel::clear(void)
{

}



bool
MDOF_BuildingModel::outputToJSON(QJsonObject &jsonObject)
{
     jsonObject["type"]="MDOF_BuildingModel";

    int numStories = inFloors->text().toInt();
    jsonObject["numStories"]= numStories;

    writeLineEditRV(jsonObject,"weight", inWeight);
    writeLineEditRV(jsonObject,"height", storyHeight);
    writeLineEditRV(jsonObject,"Kx", inKx);
    writeLineEditRV(jsonObject,"Ky", inKy);
    writeLineEditRV(jsonObject,"Krz", inK_theta);
    writeLineEditRV(jsonObject,"Fyx", inFyx);
    writeLineEditRV(jsonObject,"Fyy",inFyy);
    writeLineEditRV(jsonObject,"Bx",inBx);
    writeLineEditRV(jsonObject,"By",inBy);
    writeLineEditRV(jsonObject,"massX",massX);
    writeLineEditRV(jsonObject,"massY",massY);
    writeLineEditRV(jsonObject,"responseX",responseX);
    writeLineEditRV(jsonObject,"responseY",responseY);

    QJsonArray theArray;
    for (int i = 0; i <numStories; ++i) {
        QJsonObject floorData;
        writeCellRV(floorData, "weight", theSpreadsheet->item(i,0));
        writeCellRV(floorData, "height", theSpreadsheet->item(i,1));
        writeCellRV(floorData, "kx", theSpreadsheet->item(i,2));
        writeCellRV(floorData, "Fyx", theSpreadsheet->item(i,3));
        writeCellRV(floorData, "bx", theSpreadsheet->item(i,4));
        writeCellRV(floorData, "ky", theSpreadsheet->item(i,5));
        writeCellRV(floorData, "Fyy", theSpreadsheet->item(i,6));
        writeCellRV(floorData, "by", theSpreadsheet->item(i,7));
        writeCellRV(floorData, "Ktheta", theSpreadsheet->item(i,8));

        theArray.append(floorData);
    }
    jsonObject["ModelData"]=theArray;

    QJsonArray rvArray;
    for (int i=0; i<varNamesAndValues.size()-1; i+=2) {
        QJsonObject rvObject;
        QString name = varNamesAndValues.at(i);
        rvObject["name"]=name;
        rvObject["value"]=QString("RV.")+name;

        rvArray.append(rvObject);
    }

    jsonObject["randomVar"]=rvArray;

    return true;
}


bool
MDOF_BuildingModel::inputFromJSON(QJsonObject &jsonObject)
{
    varNamesAndValues.clear();
    int numStoriesOld = numStories;

    if (jsonObject.contains("numStories")) {
        QJsonValue theValue = jsonObject["numStories"];
        numStories = theValue.toInt();
        inFloors->setText(QString::number(numStories));
    } else {
        qDebug() << "No numStories key in json object";
        return false;
    }

    if (numStories <= 0) {
        inFloors->setText(QString(numStoriesOld));
        return true;
    }

    if (numStoriesOld != numStories) {

        floorSelected = -1;
        storySelected = -1;
        fMinSelected = -1;
        fMaxSelected = -1;
        sMinSelected = -1;
        sMaxSelected = -1;

        if (floorHeights != 0)
            delete [] floorHeights;
        if (storyHeights != 0)
            delete [] storyHeights;

        floorHeights = new double[numStories+1];
        storyHeights = new double[numStories];
    }

   readLineEditRV(jsonObject,"weight", inWeight);
   readLineEditRV(jsonObject,"height", inStoryHeight);
   readLineEditRV(jsonObject,"Kx", inKx);
   readLineEditRV(jsonObject,"Ky", inKy);

   readLineEditRV(jsonObject,"Krz", inK_theta);
   readLineEditRV(jsonObject,"Fyx", inFyx);
   readLineEditRV(jsonObject,"Fyy",inFyy);
   readLineEditRV(jsonObject,"Bx",inBx);
   readLineEditRV(jsonObject,"By",inBy);
   readLineEditRV(jsonObject,"massX",massX);
   readLineEditRV(jsonObject,"massY",massY);
   readLineEditRV(jsonObject,"responseX",responseX);
   readLineEditRV(jsonObject,"responseY",responseY);

   QString typHeight =  inStoryHeight->text();
   bool ok;
   double height = typHeight.toDouble(&ok);
   if (ok == false) height = 1.0;

   theSpreadsheet->clear();
   theSpreadsheet->setColumnCount(9);
   theSpreadsheet->setRowCount(numStories);

   // theSpreadsheet->horizontalHeader()->setStretchLastSection(true);// horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
   //theSpreadsheet->setFixedWidth(344);

   QStringList headings;
   headings << tr("Weight") << tr("Height") << tr("K_x") << tr("Fy_x") << tr("b_x") << tr("K_y") << tr("Fy_y") << tr("b_y") << tr("K_theta");
   theSpreadsheet->setHorizontalHeaderLabels(headings);


   if (jsonObject.contains("ModelData")) {

       QJsonValue theValue = jsonObject["ModelData"];
       if (!theValue.isArray()) {
           return false;
       }

       QJsonArray theArray = theValue.toArray();
       if (numStories != theArray.size()) {
           return false;
       }

       updatingPropertiesTable = true;
       floorHeights[0] = 0.;

       for (int i=0; i<numStories; i++) {

           QJsonValue theFloor = theArray[i];
           QJsonObject floorData = theFloor.toObject();

           QTableWidgetItem *item;
           item = new QTableWidgetItem();
           readCellRV(floorData, "weight", item);
           item->setToolTip(QString("weight of floor " + QString::number(i+1)));
           theSpreadsheet->setItem(i, 0, item);

           item = new QTableWidgetItem();
           readCellRV(floorData, "height", item);
           item->setToolTip(QString("height of story " + QString::number(i+1)));
           theSpreadsheet->setItem(i, 1, item);

           double heightStory = item->text().toDouble(&ok);
           if (ok == false) heightStory = height;
           Q_UNUSED(heightStory);

           storyHeights[i]= height;
           floorHeights[i+1] = floorHeights[i]+height;

           item = new QTableWidgetItem();
           readCellRV(floorData, "kx", item);
           item->setToolTip(QString("initial stiffness of story " + QString::number(i+1) + " in x dirn"));
           theSpreadsheet->setItem(i, 2, item);

           item = new QTableWidgetItem();
           readCellRV(floorData, "Fyx", item);
           item->setToolTip(QString("yield strength of story " + QString::number(i+1) + "in x dirn"));
           theSpreadsheet->setItem(i, 3, item);

           item = new QTableWidgetItem();
           readCellRV(floorData, "bx", item);
           item->setToolTip(QString("hardening ratio of story " + QString::number(i+1) + "in x dirn"));
           theSpreadsheet->setItem(i, 4, item);

           item = new QTableWidgetItem();
           readCellRV(floorData, "ky", item);
           item->setToolTip(QString("initial stiffness of story " + QString::number(i+1) + "in y dirn"));
           theSpreadsheet->setItem(i, 5, item);

           item = new QTableWidgetItem();
           readCellRV(floorData, "Fyy", item);
           item->setToolTip(QString("yield strength of story " + QString::number(i+1) + "in y dirn"));
           theSpreadsheet->setItem(i, 6, item);


           item = new QTableWidgetItem();
           readCellRV(floorData, "by", item);
           item->setToolTip(QString("hardening ratio of story " + QString::number(i+1) + "in y dirn"));
           theSpreadsheet->setItem(i, 7, item);

           item = new QTableWidgetItem();
           readCellRV(floorData, "Ktheta", item);
           item->setToolTip(QString("rotational stiffness of story " + QString::number(i+1) + " about z axis"));
           theSpreadsheet->setItem(i, 8, item);
       }

   }

   QJsonArray rvArray;
   for (int i=0; i<varNamesAndValues.size()-1; i+=2) {
       QJsonObject rvObject;
       QString name = varNamesAndValues.at(i);
       rvObject["name"]=name;
       rvObject["value"]=QString("RV.")+name;

       rvArray.append(rvObject);
   }

    jsonObject["randomVar"]=rvArray;

    updatingPropertiesTable = false;

    buildingH = floorHeights[numStories];

    if (theView != 0) {
        this->draw();
        theView->update();
    }

    return true;
}


bool
MDOF_BuildingModel::outputAppDataToJSON(QJsonObject &jsonObject) {

    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    jsonObject["Application"] = "MDOF_BuildingModel";
    QJsonObject dataObj;
    jsonObject["ApplicationData"]=dataObj;

    return true;
}
bool
MDOF_BuildingModel::inputAppDataFromJSON(QJsonObject &jsonObject) {

    Q_UNUSED(jsonObject);
    //
    // from ApplicationData
    //

    return true;
}



 bool
 MDOF_BuildingModel::copyFiles(QString &dirName) {
     Q_UNUSED(dirName);
    return true;
 }


 //void
 //MDOF_BuildingModel::draw(GlWidget2D *theView)
 void
 MDOF_BuildingModel::draw()
 {
     if (numStories == 0)
            return;

    if (theView == 0)
        return;

    theView->reset();
    int viewW = theView->width();
    int viewH = theView->height();
    float pointW = 0.1*buildingH/(3.*numStories);
    float pointH = 0.1*buildingH/(1.0*numStories);
     pointW = buildingW/buildingH*pointH*viewW/viewH;

     for (int i=0; i<numStories; i++) {
         if (i == storySelected)
             theView->drawLine(i+1+numStories, 0.,floorHeights[i], 0. ,floorHeights[i+1], 2, 1, 0, 0);
         else if (i >= sMinSelected && i <= sMaxSelected)
             theView->drawLine(i+1+numStories, 0.,floorHeights[i], 0.,floorHeights[i+1], 2, 1, 0, 0);
         else
             theView->drawLine(i+1+numStories, 0.,floorHeights[i],0.,floorHeights[i+1], 2, 0, 0, 0);    }

     for (int i=0; i<=numStories; i++) {
         if (i == floorSelected)
             theView->drawPoint(i, 0.,floorHeights[i], 10, 1, 0, 0, pointW, pointH);
         else if (i >= fMinSelected && i <= fMaxSelected)
             theView->drawPoint(i, 0.,floorHeights[i], 10, 1, 0, 0, pointW, pointH);
         else {
             theView->drawPoint(i, 0.,floorHeights[i], 10, 0, 0, 1, pointW, pointH);
         }
     }
     theView->drawLine(0, -10., 0.0, 10., 0.0, 1.0, 0., 0., 0.);
#ifndef _GRAPHICS_Qt3D
        theView->drawBuffers();
#endif
   //
 }

 void
 MDOF_BuildingModel::getBoundary(float &height, float &width)
 {
  height  = buildingH;
  width = buildingW;
 }

 void
 MDOF_BuildingModel::setSelectionBoundary(float y1, float y2)
 {
     // determine min and max of y1,y2
     float yMin = 0;
     float yMax = 0;
     if (y1 < y2) {
         yMin = y1;
         yMax = y2;
     } else {
         yMin = y2;
         yMax = y1;
     }

     // determine min and max of nodes in [y1,y2] range
     fMinSelected = -1;
     fMaxSelected = -1;

     for (int i=0; i<=numStories; i++) {
         if (floorHeights[i] >= yMin && floorHeights[i] <= yMax) {
             if (fMinSelected == -1)
                 fMinSelected = i;
             fMaxSelected = i;
         }
     }

     // determine min and max of stories in [y1, y2] range;
     sMinSelected = -1;
     sMaxSelected = -1;
     for (int i=0; i<numStories; i++) {
         double midStoryHeight = (floorHeights[i]+floorHeights[i+1])/2.;
         if (midStoryHeight >= yMin && midStoryHeight <= yMax) {
             if (sMinSelected == -1)
                 sMinSelected = i;
             sMaxSelected = i;
         }
     }

     if (fMinSelected == 0 && fMaxSelected == numStories) {

         floorMassFrame->setVisible(false);
         storyPropertiesFrame->setVisible(false);
         //spreadSheetFrame->setVisible(true);
         fMinSelected = -1;
         fMaxSelected = -1;
         sMinSelected = -1;
         sMaxSelected = -1;

     } else if (fMinSelected == fMaxSelected && fMinSelected != -1) {

         floorMassFrame->setVisible(true);
         if (sMinSelected == -1 && sMaxSelected == -1)
             storyPropertiesFrame->setVisible(false);
         else
             storyPropertiesFrame->setVisible(true);
         //spreadSheetFrame->setVisible(false);
         floorSelected=-1;
         storySelected=-1;

     } else if (fMinSelected != -1 && fMaxSelected != -1) {
         floorMassFrame->setVisible(true);
         storyPropertiesFrame->setVisible(true);
         //spreadSheetFrame->setVisible(false);
         floorSelected=-1;
         storySelected=-1;

     } else if (sMinSelected != -1 && sMaxSelected != -1) {
         floorMassFrame->setVisible(false);
         storyPropertiesFrame->setVisible(true);
         //spreadSheetFrame->setVisible(false);
         floorSelected=-1;
         storySelected=-1;

     } else {

         floorMassFrame->setVisible(false);
         storyPropertiesFrame->setVisible(false);
        // spreadSheetFrame->setVisible(true);
         floorSelected=-1;
         storySelected=-1;
     }

     this->draw();
     theView->update();
     return;
 }

 void
 MDOF_BuildingModel::addRandomVariable(QString &text, int numReferences) {
     if (randomVariables.contains(text)) {
         randomVariables[text] = randomVariables[text]+numReferences;
     } else {
         randomVariables[text] = numReferences;
         RandomVariable *theRV = new RandomVariable(QString("Uncertain"), text);
         theRandomVariablesContainer->addRandomVariable(theRV);
     }
 }

 void
 MDOF_BuildingModel::removeRandmVariable(QString &text, int numReferences) {
     if (randomVariables.contains(text)) {
         randomVariables[text] = randomVariables[text]-numReferences;

         if (randomVariables[text] < 1) {
             QStringList rvsToRemove; rvsToRemove << text;
             theRandomVariablesContainer->removeRandomVariables(rvsToRemove);

             randomVariables.remove(text);
         }
     } else {
         qDebug() << "MDOF_BuildingModel - reomveRandomVariable:: no random variable with name " << text;
     }

 }


 void
 MDOF_BuildingModel::setNumStoriesAndHeight(int newFloors, double newHeight) {

     if ((newFloors > 0 && newFloors != numStories) ||
             (newHeight > 0 && newHeight != buildingH)) {
         inFloors->setText(QString::number(newFloors));
         storyHeight->setText(QString::number(newHeight/(newFloors*1.0)));
         if (newFloors > 0 && newFloors != numStories)
             this->on_inFloors_editingFinished();
         if (newHeight > 0 && newHeight != buildingH)
             this->on_storyHeight_editingFinished();
     }
 }

 void
 MDOF_BuildingModel::setBuildingDimensions(double newW, double newD, double planArea) {
    Q_UNUSED(planArea);
    buildingW = newW;
    buildingD = newD;

    this->draw();
 }


 void MDOF_BuildingModel::showEvent(QShowEvent *event) {
     this->QWidget::showEvent(event);
     this->draw();
 }

