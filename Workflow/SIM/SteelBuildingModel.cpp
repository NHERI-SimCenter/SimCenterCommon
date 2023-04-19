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

// Written: Stevan Gavrilovic

#include "SteelBuildingModel.h"
#include "ReadWriteRVJSON.h"
#include "RandomVariable.h"
#include "GeneralInformationWidget.h"
#include "OpenSeesParser.h"
#include "RandomVariablesContainer.h"
#include "InputWidgetOpenSeesAnalysis.h"
#include <LineEditRV.h>
#include <ReadWriteRVJSON.h>

#include <QHeaderView>
#include <QMessageBox>
#include <QTableWidget>
#include <QGridLayout>
#include <QFileInfo>
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <QValidator>
#include <QFileInfo>

#include <math.h>
#include <string>
#include <sstream>
#include <iostream>

//using namespace std;

SteelBuildingModel::SteelBuildingModel(RandomVariablesContainer *theRandomVariableIW, QWidget *parent): SimCenterAppWidget(parent), theRandomVariablesContainer(theRandomVariableIW)
{

    pathToDataFiles = "NULL";

    buildingGeomFileName = "Geometry.csv";
    ELFFileName = "ELFParameters.csv";
    memberDepthFileName = "MemberDepth.csv";
    loadingFileName = "Loads.csv";

    // The main layout box
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QHBoxLayout *inputPropLayout = new QHBoxLayout();
    QVBoxLayout *buildingPropertiesLayout = new QVBoxLayout();
    QVBoxLayout *ELFParametersLayout = new QVBoxLayout();
    QHBoxLayout *fileInputLayout = new QHBoxLayout();

    // Building information
    QGroupBox* buildingPropertiesBox = new QGroupBox("Building Information");
    QGridLayout *buildingPropertiesGrid = new QGridLayout();
    
    RandomVariablesContainer *randomVariables = RandomVariablesContainer::getInstance();

    dampingRatio = new LineEditRV(randomVariables);
    dampingRatio->setText("0.02");
    dampingRatio->setMaximumWidth(100);
    dampingRatio->setMinimumWidth(100);    
    dampingRatio->setToolTip(tr("damping ratio, .02 = 2% damping"));

    numFloorsLE = createTextEntry(tr("Number Stories"), tr("number of stories in the building"),buildingPropertiesGrid, 0, 0, 100, 100);    
    buildingPropertiesGrid->addWidget(new QLabel("Damping Ratio"), 0, 3);
    buildingPropertiesGrid->addWidget(dampingRatio, 0, 4);    

    
    firstStoryHeightLE = createTextEntry(tr("First Story Height"), tr("This is the height of the first story, which may be different than the rest"),buildingPropertiesGrid, 1, 3, 100, 100);
    typStoryHeightLE = createTextEntry(tr("Typical Story Height"),tr("The typical height of the stories in the building"), buildingPropertiesGrid, 1, 0, 100, 100);
    numBayXLE = createTextEntry(tr("Number of bays in X direction"), tr("Number of bays in the X direction"),buildingPropertiesGrid, 2, 0, 100, 100);
    numBayZLE = createTextEntry(tr("Number of bays in Z direction"), tr("Number of bays in the Z direction"),buildingPropertiesGrid, 2, 3, 100, 100);
    xBayWidthLE = createTextEntry(tr("Width of bays in X direction"),tr("Width of bays in X direction in feet"), buildingPropertiesGrid,3,0, 100,100);
    zBayWidthLE = createTextEntry(tr("Width of bays in Z direction"),tr("Width of bays in Z direction in feet"), buildingPropertiesGrid,3,3, 100,100);
    numLFRSXLE = createTextEntry(tr("Number of LFRS X direction"), tr("The number of LFRS members in the X direction"), buildingPropertiesGrid,4, 0);
    numLFRSZLE = createTextEntry(tr("Number of LFRS Z direction"), tr("The number of LFRS members in the Z direction"), buildingPropertiesGrid,4, 3);
    buildingPropertiesBox->setLayout(buildingPropertiesGrid);

    // Validate the inputs
    numFloorsLE->setValidator(new QIntValidator);
    // firstStoryHeightLE->setValidator(new QDoubleValidator);
    numBayXLE->setValidator(new QIntValidator);
    numBayZLE->setValidator(new QIntValidator);
    // typStoryHeightLE->setValidator(new QDoubleValidator);
    // xBayWidthLE->setValidator(new QDoubleValidator);
    // zBayWidthLE->setValidator(new QDoubleValidator);
    numLFRSXLE->setValidator(new QIntValidator);
    numLFRSZLE->setValidator(new QIntValidator);

    connect(numFloorsLE,SIGNAL(editingFinished()),this,SLOT(onStoryLEChange()));

    // ELF Parameters
    QGroupBox* ELFPropertiesBox = new QGroupBox("ELF Parameters - Reference: ASCE 7-10 Section 12.8");
    QGridLayout *ELFPropertiesGrid = new QGridLayout();
    SsLE = createTextEntry(tr("Ss [g]"), tr("Units of g"),ELFPropertiesGrid, 0, 0, 100, 100);
    S1LE = createTextEntry(tr("S1 [g]"), tr("Units of g"),ELFPropertiesGrid, 0, 3, 100, 100);
    TLLE = createTextEntry(tr("TL"),tr("Unitless"), ELFPropertiesGrid, 1, 0, 100, 100);
    CdLE = createTextEntry(tr("Cd"), tr("Unitless"),ELFPropertiesGrid, 2, 0, 100, 100);
    RLE = createTextEntry(tr("R"), tr("Unitless"),ELFPropertiesGrid, 1, 3, 100, 100);
    leLE = createTextEntry(tr("le"),tr("Unitless"), ELFPropertiesGrid,3,0, 100,100);
    rhoLE = createTextEntry(tr("rho"),tr("Unitless"), ELFPropertiesGrid,2,3, 100,100);
    siteClassLE = createTextEntry(tr("Site Class"), tr("Unitless"), ELFPropertiesGrid,4, 0);
    CtLE = createTextEntry(tr("Ct"), tr("Unitless"), ELFPropertiesGrid,3, 3);
    xLE = createTextEntry(tr("x"), tr("Unitless"), ELFPropertiesGrid,4, 3);
    ELFPropertiesBox->setLayout(ELFPropertiesGrid);

    // Set the validators
    //    SsLE
    //    S1LE
    //    TLLE
    //    CdLE
    //    RLE,
    //    leLE
    //    rhoLE
    //    siteClassLE
    //    CtLE
    //    xLE

    // Connect all of the line edits to a slot that can process any RVs that come in
    // Comment out or delete the lines where RVs are not expected to come into the inputs
    connect(numFloorsLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(firstStoryHeightLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(typStoryHeightLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(numBayXLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(numBayZLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(xBayWidthLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(zBayWidthLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(numLFRSXLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(numLFRSZLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(SsLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(S1LE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(TLLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(CdLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(RLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(leLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(rhoLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    // connect(siteClassCB,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(CtLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(xLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));

    QGroupBox* inputFileBox = new QGroupBox("Building Data Files");
    QGridLayout *fileInputGrid = new QGridLayout();
    inputFileBox->setLayout(fileInputGrid);

    QLabel *label1 = new QLabel();
    label1->setText("Building Data Folder");

    QLabel *label2 = new QLabel();
    label2->setText("Users should cite this tool as follows:\nGuan, X., Burton, H., & Sabol, T. (2020). Python-based computational platform to automate seismic design, nonlinear structural model construction and analysis of steel moment resisting frames. Engineering Structures, 224, 111199.");
    label2->setWordWrap(true);

    fileLE = new QLineEdit;
    QPushButton *chooseFileButton = new QPushButton();
    chooseFileButton->setText(tr("Choose"));

    QPushButton *saveFileButton = new QPushButton();
    saveFileButton->setText(tr("Save File"));

    QPushButton *loadFileButton = new QPushButton();
    loadFileButton->setText(tr("Load File"));

    chooseFileButton->setFixedWidth(100);
    saveFileButton->setFixedWidth(200);
    loadFileButton->setFixedWidth(200);

    connect(chooseFileButton,SIGNAL(clicked()),this,SLOT(chooseFileFolderDialog()));
    connect(loadFileButton,SIGNAL(clicked()),this,SLOT(loadDataFromFolder()));
    connect(saveFileButton,SIGNAL(clicked()),this,SLOT(saveDataToFolder()));

    fileInputGrid->addWidget(label1,0,0);
    fileInputGrid->addWidget(fileLE,0,1);
    fileInputGrid->addWidget(chooseFileButton,0,2);
    fileInputGrid->addWidget(saveFileButton,1,0);
    fileInputGrid->addWidget(loadFileButton,1,1);

    buildingPropertiesLayout->addWidget(buildingPropertiesBox);
    ELFParametersLayout->addWidget(ELFPropertiesBox);
    fileInputLayout->addWidget(inputFileBox);
    fileInputLayout->addWidget(label2);

    // Add the member depth and loading spread sheets
    memberDepthSpreadsheet = new QTableWidget();
    loadingSpreadsheet = new QTableWidget();

    // Connect the slots to the spreadsheets
    connect(memberDepthSpreadsheet, SIGNAL(cellClicked(int,int)), this, SLOT(onSpreadsheetCellClicked(int,int)));
    connect(memberDepthSpreadsheet, SIGNAL(cellEntered(int,int)), this,SLOT(onSpreadsheetCellClicked(int,int)));
    connect(memberDepthSpreadsheet, SIGNAL(cellChanged(int,int)), this,SLOT(onSpreadsheetCellChanged(int,int)));

    connect(loadingSpreadsheet, SIGNAL(cellClicked(int,int)), this, SLOT(onSpreadsheetCellClicked(int,int)));
    connect(loadingSpreadsheet, SIGNAL(cellEntered(int,int)), this,SLOT(onSpreadsheetCellClicked(int,int)));
    connect(loadingSpreadsheet, SIGNAL(cellChanged(int,int)), this,SLOT(onSpreadsheetCellChanged(int,int)));

    inputPropLayout->addLayout(buildingPropertiesLayout);
    inputPropLayout->addLayout(ELFParametersLayout);

    inputPropLayout->addStretch();
    fileInputLayout->addStretch();

    mainLayout->addLayout(inputPropLayout);
    mainLayout->addLayout(fileInputLayout);

    QGroupBox* memberSizesBox = new QGroupBox("Member Depth");
    QHBoxLayout *memberSizesLayout = new QHBoxLayout();
    memberSizesLayout->addWidget(memberDepthSpreadsheet);
    memberSizesBox->setLayout(memberSizesLayout);

    QGroupBox* loadingBox = new QGroupBox("Loading");
    QHBoxLayout *loadingLayout = new QHBoxLayout();
    loadingLayout->addWidget(loadingSpreadsheet);
    loadingBox->setLayout(loadingLayout);

    mainLayout->addWidget(memberSizesBox);
    mainLayout->addWidget(loadingBox);

    this->setLayout(mainLayout);

    // add signal and slot connections with GI
    GeneralInformationWidget *theGI = GeneralInformationWidget::getInstance();

    this->setDefaultValues();
    
    connect(this,SIGNAL(numStoriesOrHeightChanged(int, double)), theGI, SLOT(setNumStoriesAndHeight(int, double)));
    connect(this,SIGNAL(buildingDimensionsChanged(double,double,double)),theGI,SLOT(setBuildingDimensions(double,double,double)));
}


SteelBuildingModel::~SteelBuildingModel()
{

}


void SteelBuildingModel::onSpreadsheetCellClicked(int row, int column)
{

    // Check to make sure that a table widget is sending this
    QTableWidget* tableSender = qobject_cast<QTableWidget*>(sender());
    if(tableSender == nullptr)
        return;

    QTableWidgetItem *item = tableSender->item(row,column);
    cellText = item->text();

}


void SteelBuildingModel::onSpreadsheetCellChanged(int row, int column)
{

    // Check to make sure that a table widget is sending this
    QTableWidget* tableSender = qobject_cast<QTableWidget*>(sender());
    if(tableSender == nullptr || tableSender == memberDepthSpreadsheet)
        return;

    if (updatingPropertiesTable == false)
    {
        QTableWidgetItem *item = tableSender->item(row,column);
        QString newText = item->text();

        if (cellText != newText)
        {
            // If existing text is already a random variable, remove it
            auto isRV = this->checkRV(cellText);
            if (isRV)
            {
                this->removeRandomVariable(cellText);
            }

            // Check if new text is a random variable, and add it if it is
            this->addRandomVariable(newText);
        }
    }
}


bool SteelBuildingModel::checkRV(const QString& value)
{
    bool ok;
    double textToDouble = value.toDouble(&ok);
    Q_UNUSED(textToDouble);

    if(ok)
        return false;

    return true;
}


bool SteelBuildingModel::outputToJSON(QJsonObject &jsonObject)
{
    jsonObject["type"]="SteelBuildingModel";

    jsonObject["numStories"]= numFloorsLE->text().toInt();
    writeLineEditRV(jsonObject,"dampingRatio", dampingRatio);
    
    // writeLineEditRV(jsonObject,"Kx", numBayXLE);

    //    QJsonArray theArray;
    //    for (int i = 0; i <loadingSpreadsheet->rowCount(); ++i)
    //    {
    //        QJsonObject loadingData;
    //        writeCellRV(loadingData, "FloorWeight", loadingSpreadsheet->item(i,0));
    //        writeCellRV(loadingData, "FloorDeadLoad", loadingSpreadsheet->item(i,1));
    //        writeCellRV(loadingData, "FloorLiveLoad", loadingSpreadsheet->item(i,2));
    //        writeCellRV(loadingData, "BeamDeadLoad", loadingSpreadsheet->item(i,3));
    //        writeCellRV(loadingData, "BeamLiveLoad", loadingSpreadsheet->item(i,4));
    //        writeCellRV(loadingData, "LeaningColDeadLoad", loadingSpreadsheet->item(i,5));
    //        writeCellRV(loadingData, "LeaningColLiveLoad", loadingSpreadsheet->item(i,6));

    //        theArray.append(loadingData);
    //    }

    //    jsonObject["LoadingData"]=theArray;

    QJsonArray rvArray;
    for (int i=0; i<varNamesAndValues.size()-1; i+=2)
    {
        QJsonObject rvObject;
        QString name = varNamesAndValues.at(i);
        rvObject["name"]=name;
        rvObject["value"]=QString("RV.")+name;

        rvArray.append(rvObject);
    }

    jsonObject["randomVar"]=rvArray;

    // Output the path to the data folder
    QFileInfo fullPath(pathToDataFiles);
    if (fullPath.exists()){
        QString dirName = fullPath.fileName();
        jsonObject["pathDataFolder"]=pathToDataFiles ;
        jsonObject["folderName"]=dirName;
    } else {
        jsonObject["pathDataFolder"]="NULL";
        jsonObject["folderName"]="NO-FILE";
    }

    return true;
}


bool SteelBuildingModel::inputFromJSON(QJsonObject &jsonObject)
{
    QJsonValue pathDataFolderObj = jsonObject["pathDataFolder"];
   readLineEditRV(jsonObject,"dampingRatio", dampingRatio);
   
    if (!pathDataFolderObj.isNull())
    {
        pathToDataFiles = pathDataFolderObj.toString();

        // Load the data files
        this->loadDataFromFolder();

        fileLE->setText(pathToDataFiles);
    }
    else
    {
        qDebug() << "No pathDataFolder key in json object in "<<__FUNCSIG__;
        return false;
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

    return true;
}


bool SteelBuildingModel::outputAppDataToJSON(QJsonObject &jsonObject) {

    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    jsonObject["Application"] = "SteelBuildingModel";
    QJsonObject dataObj;
    jsonObject["ApplicationData"]=dataObj;

    return true;
}


bool SteelBuildingModel::inputAppDataFromJSON(QJsonObject &jsonObject) {

    Q_UNUSED(jsonObject);
    //
    // from ApplicationData
    //

    return true;
}


bool SteelBuildingModel::copyFiles(QString &dirName) {

    //
    // copy files to directory in dirName so that they are copied when this runs remotely
    //

    QDir dir(dirName);
    QFileInfo fullPath(pathToDataFiles);
    QString name;

    if (!fullPath.exists()){
        name = "autosda_files";
	pathToDataFiles = dirName + QDir::separator() + name;	
	dir.mkdir(name);    	
    } else {
        //mkdir and copy
        name = fullPath.fileName();
        dir.mkdir(name);
	pathToDataFiles = dirName + QDir::separator() + name;
    }

    this->saveDataToFolder();

    return true;
}


void SteelBuildingModel::addRandomVariable(QString &text, int numReferences)
{

    // Quick return if not a random variable
    auto isRV = this->checkRV(text);
    if (!isRV)
        return;

    if (randomVariables.contains(text))
    {
        randomVariables[text] = randomVariables[text]+numReferences;
    }
    else
    {
        randomVariables[text] = numReferences;
        //RandomVariable *theRV = new RandomVariable(QString("Uncertain"), text, "Dakota");
        //theRandomVariablesContainer->addRandomVariable(theRV);
        theRandomVariablesContainer->addRandomVariable(text);
    }
}


void SteelBuildingModel::removeRandomVariable(QString &text, int numReferences)
{
    if (randomVariables.contains(text))
    {
        randomVariables[text] = randomVariables[text]-numReferences;

        if (randomVariables[text] < 1)
        {
            QStringList rvsToRemove; rvsToRemove << text;
            theRandomVariablesContainer->removeRandomVariables(rvsToRemove);

            randomVariables.remove(text);
        }
    }
    else
    {
        qDebug() << "No random variable with name " << text;
    }
}


void SteelBuildingModel::showEvent(QShowEvent *event) {
    this->QWidget::showEvent(event);
}


void SteelBuildingModel::onStoryLEChange(void)
{

    bool OK;
    auto numFloors = numFloorsLE->text().toInt(&OK);

    // Append or trim rows as required
    auto numRows = memberDepthSpreadsheet->rowCount();

    // Check for unrealistic inputs
    if(!OK || numFloors < 1 || numFloors == numRows)
        return;

    if(numFloors > numRows)
    {
        for(int i = numRows; i<numFloors; ++i)
        {
            memberDepthSpreadsheet->insertRow(i);
            loadingSpreadsheet->insertRow(i);

            // For the new rows copy the items down from the cells above
            if(i > 0)
            {
                QTableWidgetItem *item = nullptr;

                item = new QTableWidgetItem(*loadingSpreadsheet->item(i-1,0));
                item->setToolTip(QString("Weight [kips] of floor " + QString::number(i+1)));
                loadingSpreadsheet->setItem(i,0, item);

                item = new QTableWidgetItem(*loadingSpreadsheet->item(i-1,1));
                item->setToolTip(QString("Floor dead load [psf] of story " + QString::number(i+1)));
                loadingSpreadsheet->setItem(i,1,item);

                item = new QTableWidgetItem(*loadingSpreadsheet->item(i-1,2));
                item->setToolTip(QString("Floor live load [psf] of story " + QString::number(i+1)));
                loadingSpreadsheet->setItem(i,2,item);

                item = new QTableWidgetItem(*loadingSpreadsheet->item(i-1,3));
                item->setToolTip(QString("Beam dead load [lb/ft] of story " + QString::number(i+1)));
                loadingSpreadsheet->setItem(i,3,item);

                item = new QTableWidgetItem(*loadingSpreadsheet->item(i-1,4));
                item->setToolTip(QString("Beam live load [lb/ft] of story " + QString::number(i+1)));
                loadingSpreadsheet->setItem(i,4,item);

                item = new QTableWidgetItem(*loadingSpreadsheet->item(i-1,5));
                item->setToolTip(QString("Leaning column dead load [kips] of story " + QString::number(i+1)));
                loadingSpreadsheet->setItem(i,5,item);

                item = new QTableWidgetItem(*loadingSpreadsheet->item(i-1,6));
                item->setToolTip(QString("Leaning column live load [kips] of story " + QString::number(i+1)));
                loadingSpreadsheet->setItem(i,6,item);

                item = new QTableWidgetItem(*memberDepthSpreadsheet->item(i-1,0));
                item->setToolTip(QString("Depth of exterior column of story " + QString::number(i+1)));
                memberDepthSpreadsheet->setItem(i,0, item);

                item = new QTableWidgetItem(*memberDepthSpreadsheet->item(i-1,1));
                item->setToolTip(QString("Depth of interior column of story " + QString::number(i+1)));
                memberDepthSpreadsheet->setItem(i,1,item);

                item = new QTableWidgetItem(*memberDepthSpreadsheet->item(i-1,2));
                item->setToolTip(QString("Depth of beam of story " + QString::number(i+1)));
                memberDepthSpreadsheet->setItem(i,2,item);
            }
        }
    }
    else
    {
        for(int i = numRows -1; i>=numFloors; --i)
        {
            memberDepthSpreadsheet->removeRow(i);
            loadingSpreadsheet->removeRow(i);
        }
    }

    // Calculate the building height for the general information widget
    auto firstFloorHeight = firstStoryHeightLE->text().toDouble(&OK);

    if(!OK)
        return;

    auto typStoryHeight = typStoryHeightLE->text().toDouble(&OK);

    if(!OK)
        return;
    auto buildingHeight = firstFloorHeight + static_cast<double>(numFloors-1)*typStoryHeight;

    // Set the floors and building height to the general information widget
    emit numStoriesOrHeightChanged(numFloors, buildingHeight);

    return;
}

void SteelBuildingModel::onBuildingDimLEChange(void)
{
    bool OK;

    // Width of the bays in the x-direction
    auto xBayWidth = xBayWidthLE->text().toDouble(&OK);

    if(!OK)
        return;

    // Number of bays in x-direction
    auto numBayX = numBayXLE->text().toInt(&OK);

    if(!OK)
        return;

    // Width of the bays in the z-direction
    auto zBayWidth = zBayWidthLE->text().toDouble(&OK);

    if(!OK)
        return;

    // Number of bays in z-direction
    auto numBayZ = numBayZLE->text().toInt(&OK);

    if(!OK)
        return;


    // Dimensions of the building
    auto buildingWidth = xBayWidth*static_cast<double>(numBayX);
    auto buildingDepth = zBayWidth*static_cast<double>(numBayZ);
    auto planArea = buildingWidth*buildingDepth;

    emit buildingDimensionsChanged(buildingWidth,buildingDepth,planArea);

    return;
}


void SteelBuildingModel::onLineEditTextChanged()
{
    // Check to make sure that a line edit is sending this
    QLineEdit* lineEditSender = qobject_cast<QLineEdit*>(sender());
    if(lineEditSender == nullptr)
        return;

    // Save the new text
    auto newText = lineEditSender->text();

    // Check if an "undo" operation is available
    if(lineEditSender->isUndoAvailable())
    {
        // Get the old text by performing an "undo" operation, then redo to return to the new value
        lineEditSender->undo();
        auto oldText = lineEditSender->text();
        lineEditSender->redo();

        // Do nothing if the old text is equal to the new text
        if(oldText == newText)
            return;

        // Check if the old text is a random variable, and try to remove it
        if(this->checkRV(oldText))
            this->removeRandomVariable(oldText);
    }

    // If the new text is a RV, add it
    this->addRandomVariable(newText);

    return;
}


QStringList SteelBuildingModel::getLoadingTableHeadings(void)
{
    QStringList headings;
    headings << tr("Floor Weight [kips]") << tr("Floor Dead Load [psf]") << tr("Floor Live Load [psf]")
             << tr("Beam Dead Load [lb/ft]") << tr("Beam Live Load [lb/ft]")
             << tr("Leaning Column Dead Load [kips]")<< tr("Leaning Column Live Load [kips]");

    return headings;
}


QStringList SteelBuildingModel::getMemberTableHeadings(void)
{
    QStringList headings;
    headings << tr("Exterior Column") << tr("Interior Column") << tr("Beam");

    return headings;
}


void SteelBuildingModel::chooseFileFolderDialog(void)
{

    pathToDataFiles = QFileDialog::getExistingDirectory(this,tr("Building Data Folder"));

    // Return if the user cancels
    if(pathToDataFiles.isEmpty())
    {
        pathToDataFiles = "NULL";
        return;
    }

    // Set file name & entry in qLine edit
    fileLE->setText(pathToDataFiles);

    // Parse file for random variables and add them
    OpenSeesParser theParser;
    varNamesAndValues = theParser.getVariables(pathToDataFiles);

    theRandomVariablesContainer->addConstantRVs(varNamesAndValues);

    return;
}


void SteelBuildingModel::loadDataFromFolder(void)
{

    // Ask for the file path if the file path has not yet been set, and return if it is still null
    if(pathToDataFiles.compare("NULL") == 0)
        this->chooseFileFolderDialog();

    if(pathToDataFiles.compare("NULL") == 0)
        return;

    // Check if the directory exists
    QDir directory(pathToDataFiles);
    if (!directory.exists())
    {
        QString errMsg = "Cannot find the directory: "+ pathToDataFiles + "\n" +"Check your directory and try again.";
        this->errorMessageDialog(errMsg);
        return;
    }


    // Remove old random variables
    QStringList names;
    for (int i=0; i<varNamesAndValues.size()-1; i+=2)
    {
        names.append(varNamesAndValues.at(i));
    }

    theRandomVariablesContainer->removeRandomVariables(names);

    // First try to load the file with the ELF parameters
    auto pathToELF = directory.filePath(ELFFileName);

    auto res = this->parseELFDataFile(pathToELF);
    if(res != 0)
    {
        QString errMsg = "Error parsing the " + ELFFileName + " file in the directory" + pathToDataFiles + "\n" +"Check the file and try again.";
        this->errorMessageDialog(errMsg);
        return;
    }


    // Then try to load the file with the building geometry parameters
    auto pathGeomData = directory.filePath(buildingGeomFileName);
    auto res2 = this->parseGeometryDataFile(pathGeomData);
    if(res2 != 0)
    {
        QString errMsg = "Error parsing the " + buildingGeomFileName +" file in the directory" + pathToDataFiles + "\n" +"Check the file and try again.";
        this->errorMessageDialog(errMsg);
        return;
    }


    // Then try to load the file with the loading information
    auto pathLoadData = directory.filePath(loadingFileName);
    auto res3 = this->parseLoadsDataFile(pathLoadData);
    if(res3 != 0)
    {
        QString errMsg = "Error parsing the "+ loadingFileName + " file in the directory" + pathToDataFiles + "\n" +"Check the file and try again.";
        this->errorMessageDialog(errMsg);
        return;
    }


    // Then try to load the file with the member depth information
    auto pathMemberDepth = directory.filePath(memberDepthFileName);
    auto res4 = this->parseMemberDepthDataFile(pathMemberDepth);
    if(res4 != 0)
    {
        QString errMsg = "Error parsing the "+ memberDepthFileName +" file in the directory" + pathToDataFiles + "\n" +"Check the file and try again.";
        this->errorMessageDialog(errMsg);
        return;
    }


    return;
}


void SteelBuildingModel::saveDataToFolder(void)
{

    // Ask for the file path if the file path has not yet been set
    if(pathToDataFiles.compare("NULL") == 0)
        this->chooseFileFolderDialog();

    if(pathToDataFiles.compare("NULL") == 0)
        return;

    // Check if the directory exists
    QDir directory(pathToDataFiles);
    if (!directory.exists())
    {
        QString errMsg = "Cannot find the directory: "+ pathToDataFiles + "\n" +"Check your directory and try again.";
        this->errorMessageDialog(errMsg);
        return;
    }

    // Save the file with the ELF parameters
    auto dirToELF = directory.filePath(ELFFileName);

    auto res = this->saveELFDataFile(dirToELF);
    if(res != 0)
    {
        QString errMsg = "Error saving the " + ELFFileName + " file in the directory" + pathToDataFiles + "\n" +"Check the file and try again.";
        this->errorMessageDialog(errMsg);
        return;
    }

    // Save the file with the building geometry parameters
    auto dirToGeom = directory.filePath(buildingGeomFileName);

    auto res2 = this->saveBuildingGeomFile(dirToGeom);
    if(res2 != 0)
    {
        QString errMsg = "Error saving the " + buildingGeomFileName + " file in the directory" + pathToDataFiles + "\n" +"Check the file and try again.";
        this->errorMessageDialog(errMsg);
        return;
    }


    // Save the file with the loading parameters
    auto dirToLoading = directory.filePath(loadingFileName);

    auto res3 = this->saveLoadsFile(dirToLoading);
    if(res3 != 0)
    {
        QString errMsg = "Error saving the " + loadingFileName + " file in the directory" + pathToDataFiles + "\n" +"Check the file and try again.";
        this->errorMessageDialog(errMsg);
        return;
    }


    // Save the file with the loading parameters
    auto dirToMemberDepth = directory.filePath(memberDepthFileName);

    auto res4 = this->saveMemberDepthFile(dirToMemberDepth);
    if(res4 != 0)
    {
        QString errMsg = "Error saving the " + memberDepthFileName + " file in the directory" + pathToDataFiles + "\n" +"Check the file and try again.";
        this->errorMessageDialog(errMsg);
        return;
    }

    return;
}


int SteelBuildingModel::parseELFDataFile(const QString& pathToFile)
{

    auto fileName = ELFFileName;

    auto csvData = parseCSVFile(pathToFile);

    if(csvData.empty())
        return -1;

    if(csvData.size() != 2)
    {
        qDebug()<<"Error in parsing the file "<<fileName<<" in "<<__FUNCSIG__<<". The number of rows should be 2";
        return -1;
    }

    auto parameterStr = csvData[1];

    if(parameterStr.size()!= 10)
    {
        qDebug()<<"Error in parsing the file "<<fileName<<" in "<<__FUNCSIG__<<". The number of parameters should be 10";
        return -1;
    }

    // Set the parameter strings to the line edits
    SsLE->setText(parameterStr[0]);
    S1LE->setText(parameterStr[1]);
    TLLE->setText(parameterStr[2]);
    CdLE->setText(parameterStr[3]);
    RLE->setText(parameterStr[4]);
    leLE->setText(parameterStr[5]);
    rhoLE->setText(parameterStr[6]);
    siteClassLE->setText(parameterStr[7]);
    CtLE->setText(parameterStr[8]);
    xLE->setText(parameterStr[9]);

    SsLE->editingFinished();
    S1LE->editingFinished();
    TLLE->editingFinished();
    CdLE->editingFinished();
    RLE->editingFinished();
    leLE->editingFinished();
    rhoLE->editingFinished();
    siteClassLE->editingFinished();
    CtLE->editingFinished();
    xLE->editingFinished();

    return 0;
}


int SteelBuildingModel::parseGeometryDataFile(const QString& pathToFile)
{

    auto fileName = buildingGeomFileName;

    auto csvData = parseCSVFile(pathToFile);

    if(csvData.empty())
        return -1;

    if(csvData.size() != 2)
    {
        qDebug()<<"Error in parsing the file "<<fileName<<" in "<<__FUNCSIG__<<". The number of rows should be 2";
        return -1;
    }

    auto parameterStr = csvData[1];

    if(parameterStr.size()!= 9)
    {
        qDebug()<<"Error in parsing the file "<<fileName<<" in "<<__FUNCSIG__<<". The number of parameters should be 10";
        return -1;
    }

    // Set the parameter strings to the line edits
    numFloorsLE->setText(parameterStr[0]);
    numBayXLE->setText(parameterStr[1]);
    numBayZLE->setText(parameterStr[2]);
    firstStoryHeightLE->setText(parameterStr[3]);
    typStoryHeightLE->setText(parameterStr[4]);
    xBayWidthLE->setText(parameterStr[5]);
    zBayWidthLE->setText(parameterStr[6]);
    numLFRSXLE->setText(parameterStr[7]);
    numLFRSZLE->setText(parameterStr[8]);

    // Let the program now the edits are finished so that it may find any RVs
    numFloorsLE->editingFinished();
    numBayXLE->editingFinished();
    numBayZLE->editingFinished();
    firstStoryHeightLE->editingFinished();
    typStoryHeightLE->editingFinished();
    xBayWidthLE->editingFinished();
    zBayWidthLE->editingFinished();
    numLFRSXLE->editingFinished();
    numLFRSZLE->editingFinished();

    return 0;
}


int SteelBuildingModel::parseLoadsDataFile(const QString& pathToFile)
{
    auto fileName = loadingFileName;

    auto csvData = parseCSVFile(pathToFile);

    if(csvData.empty())
        return -1;

    if(csvData.size() <= 2)
    {
        qDebug()<<"Error in parsing the file "<<fileName<<" in "<<__FUNCSIG__<<". The number of rows should be 2";
        return -1;
    }

    // Pop off the first row
    csvData.pop_front();

    this->updateLoadingSpreadSheet(csvData);

    return 0;
}


int SteelBuildingModel::parseMemberDepthDataFile(const QString& pathToFile)
{
    auto fileName = memberDepthFileName;

    auto csvData = parseCSVFile(pathToFile);

    if(csvData.empty())
        return -1;

    if(csvData.size() <= 2)
    {
        qDebug()<<"Error in parsing the file "<<fileName<<" in "<<__FUNCSIG__<<". The number of rows should be 2";
        return -1;
    }

    // Pop off the first row
    csvData.pop_front();

    this->updateMemberDepthSpreadSheet(csvData);

    return 0;
}


int SteelBuildingModel::saveELFDataFile(const QString& pathToFile)
{
    QFile file(pathToFile);

    if (!file.open(QIODevice::WriteOnly))
    {
        QString errMsg = "Cannot find the required file: " + ELFFileName + " in the directory" + pathToDataFiles + "\n" +"Check your directory and try again.";
        this->errorMessageDialog(errMsg);
        return -1;
    }

    QTextStream csvFileOut(&file);

    // This has to match the header in the  ELFParameters.csv file verbatim, otherwise the import code in the python script will recognize it
    QString headerLine = "Ss,S1,TL,Cd,R,Ie,rho,site class,Ct,x\n";

    csvFileOut<<headerLine.toUtf8();

    csvFileOut<<SsLE->text().toUtf8()<<",";
    csvFileOut<<S1LE->text().toUtf8()<<",";
    csvFileOut<<TLLE->text().toUtf8()<<",";
    csvFileOut<<CdLE->text().toUtf8()<<",";
    csvFileOut<<RLE->text().toUtf8()<<",";
    csvFileOut<<leLE->text().toUtf8()<<",";
    csvFileOut<<rhoLE->text().toUtf8()<<",";
    csvFileOut<<siteClassLE->text().toUtf8()<<",";
    csvFileOut<<CtLE->text().toUtf8()<<",";
    csvFileOut<<xLE->text().toUtf8();

    return 0;
}


int SteelBuildingModel::saveBuildingGeomFile(const QString& pathToFile)
{
    QFile file(pathToFile);

    if (!file.open(QIODevice::WriteOnly))
    {
        QString errMsg = "Cannot find the required file: " + buildingGeomFileName + " in the directory" + pathToDataFiles + "\n" +"Check your directory and try again.";
        this->errorMessageDialog(errMsg);
        return -1;
    }

    QTextStream csvFileOut(&file);

    // This has to match the header in the Geometry.csv file verbatim, otherwise the import code in the python script will recognize it
    QString headerLine = "number of story,number of X bay,number of Z bay,first story height,typical story height,X bay width,Z bay width,number of X LFRS,number of Z LFRS\n";

    csvFileOut<<headerLine.toUtf8();

    csvFileOut<<numFloorsLE->text().toUtf8()<<",";
    csvFileOut<<numBayXLE->text().toUtf8()<<",";
    csvFileOut<<numBayZLE->text().toUtf8()<<",";
    csvFileOut<<firstStoryHeightLE->text().toUtf8()<<",";
    csvFileOut<<typStoryHeightLE->text().toUtf8()<<",";
    csvFileOut<<xBayWidthLE->text().toUtf8()<<",";
    csvFileOut<<zBayWidthLE->text().toUtf8()<<",";
    csvFileOut<<numLFRSXLE->text().toUtf8()<<",";
    csvFileOut<<numLFRSZLE->text().toUtf8();

    return 0;
}


int SteelBuildingModel::saveLoadsFile(const QString& pathToFile)
{
    QFile file(pathToFile);

    if (!file.open(QIODevice::WriteOnly))
    {
        QString errMsg = "Cannot find the required file: " + loadingFileName + " in the directory" + pathToDataFiles + "\n" +"Check your directory and try again.";
        this->errorMessageDialog(errMsg);
        return -1;
    }

    QTextStream csvFileOut(&file);

    // This has to match the header in the  loads.csv file verbatim, otherwise the import code in the python script will recognize it
    QString headerLine = "floor weight,floor dead load,floor live load,beam dead load,beam live load,leaning column dead load,leaning column live load\n";

    csvFileOut<<headerLine.toUtf8();

    auto numRows = loadingSpreadsheet->rowCount();
    auto numCols = loadingSpreadsheet->columnCount();

    for (int i = 0; i< numRows; ++i)
    {
        for(int j = 0; j<numCols; ++j)
        {
            csvFileOut<<loadingSpreadsheet->item(i,j)->text();

            // Add the terminating character
            if(j != numCols-1)
                csvFileOut<<",";
            else
                csvFileOut<<"\n";
        }
    }

    return 0;
}


int SteelBuildingModel::saveMemberDepthFile(const QString& pathToFile)
{
    QFile file(pathToFile);

    if (!file.open(QIODevice::WriteOnly))
    {
        QString errMsg = "Cannot find the required file: " + memberDepthFileName + " in the directory" + pathToDataFiles + "\n" +"Check your directory and try again.";
        this->errorMessageDialog(errMsg);
        return -1;
    }

    QTextStream csvFileOut(&file);

    // This has to match the header in the  MemberDepth.csv file verbatim, otherwise the import code in the python script will recognize it
    QString headerLine = "exterior column,interior column,beam\n";

    csvFileOut<<headerLine.toUtf8();

    auto numRows = memberDepthSpreadsheet->rowCount();
    auto numCols = memberDepthSpreadsheet->columnCount();

    auto strOutput = [](const QString& in)
    {
        if(in.contains(','))
            return "\"" + in.toUtf8() + "\"";
        else
            return in.toUtf8();
    };

    for (int i = 0; i<numRows; ++i)
    {
        for(int j = 0; j<numCols; ++j)
        {
            csvFileOut<<strOutput(memberDepthSpreadsheet->item(i,j)->text());

            // Add the terminating character
            if(j != numCols-1)
                csvFileOut<<",";
            else
                csvFileOut<<"\n";
        }
    }

    return 0;
}


void SteelBuildingModel::errorMessageDialog(const QString& errorString)
{
    QMessageBox msgBox;
    msgBox.setText(errorString);
    msgBox.setStandardButtons(QMessageBox::Close);
    msgBox.exec();
}


QVector<QStringList> SteelBuildingModel::parseCSVFile(const QString &pathToFile)
{
    QVector<QStringList> returnVec;

    QFile geomFile(pathToFile);

    if (!geomFile.open(QIODevice::ReadOnly))
    {
        QString errMsg = "Cannot find the file: " + pathToFile + "\nCheck your directory and try again.";
        this->errorMessageDialog(errMsg);
        return returnVec;
    }

    QStringList rowLines;
    while (!geomFile.atEnd())
    {
        QString line = geomFile.readLine();

        rowLines << line;
    }

    auto numRows = rowLines.size();
    if(numRows == 0)
    {
        qDebug()<<"Error in parsing the .csv file "<<pathToFile<<" in "<<__FUNCSIG__;
        return returnVec;
    }

    returnVec.reserve(numRows);

    for(auto&& it: rowLines)
    {
        auto lineStr = this->parseLineCSV(it);

        returnVec.push_back(lineStr);
    }

    return returnVec;
}


QStringList SteelBuildingModel::parseLineCSV(const QString &csvString)
{
    QStringList fields;
    QString value;

    bool hasQuote = false;

    for (int i = 0; i < csvString.size(); ++i)
    {
        const QChar current = csvString.at(i);

        // Normal state
        if (hasQuote == false)
        {
            // Comma
            if (current == ',')
            {
                // Save field
                fields.append(value.trimmed());
                value.clear();
            }

            // Double-quote
            else if (current == '"')
            {
                hasQuote = true;
                value += current;
            }

            // Other character
            else
                value += current;
        }
        else if (hasQuote)
        {
            // Check for another double-quote
            if (current == '"')
            {
                if (i < csvString.size())
                {
                    // A double double-quote?
                    if (i+1 < csvString.size() && csvString.at(i+1) == '"')
                    {
                        value += '"';

                        // Skip a second quote character in a row
                        i++;
                    }
                    else
                    {
                        hasQuote = false;
                        value += '"';
                    }
                }
            }

            // Other character
            else
                value += current;
        }
    }

    if (!value.isEmpty())
        fields.append(value.trimmed());


    // Remove quotes and whitespace around quotes
    for (int i=0; i<fields.size(); ++i)
        if (fields[i].length()>=1 && fields[i].left(1)=='"')
        {
            fields[i]=fields[i].mid(1);
            if (fields[i].length()>=1 && fields[i].right(1)=='"')
                fields[i]=fields[i].left(fields[i].length()-1);
        }

    return fields;
}


void SteelBuildingModel::updateLoadingSpreadSheet(const QVector<QStringList>& data)
{
    if(data.empty())
    {
        qDebug()<<"Error in "<<__FUNCSIG__<<". Empty data vector";
        return;
    }

    loadingSpreadsheet->clear();
    loadingSpreadsheet->setColumnCount(7);
    loadingSpreadsheet->setRowCount(data.size());
    QStringList loadingHeadings = this->getLoadingTableHeadings();
    loadingSpreadsheet->setHorizontalHeaderLabels(loadingHeadings);
    loadingSpreadsheet->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    loadingSpreadsheet->verticalHeader()->setVisible(false);

    updatingPropertiesTable = true;
    for (int i = 0; i< data.size(); ++i)
    {
        auto tableRow = data[i];

        if(tableRow.size() != 7)
        {
            qDebug()<<"Error in "<<__FUNCSIG__<<". The number of parameters per row should be 7";
            return;
        }

        // Check if any of the table inputs are random variables, and add them if they are
        for(auto&& it : tableRow)
            this->addRandomVariable(it);

        QTableWidgetItem *item;
        item = new QTableWidgetItem(tableRow[0]);
        item->setToolTip(QString("Weight [kips] of floor " + QString::number(i+1)));
        loadingSpreadsheet->setItem(i,0, item);

        item = new QTableWidgetItem(tableRow[1]);
        item->setToolTip(QString("Floor dead load [psf] of story " + QString::number(i+1)));
        loadingSpreadsheet->setItem(i,1,item);

        item = new QTableWidgetItem(tableRow[2]);
        item->setToolTip(QString("Floor live load [psf] of story " + QString::number(i+1)));
        loadingSpreadsheet->setItem(i,2,item);

        item = new QTableWidgetItem(tableRow[3]);
        item->setToolTip(QString("Beam dead load [lb/ft] of story " + QString::number(i+1)));
        loadingSpreadsheet->setItem(i,3,item);

        item = new QTableWidgetItem(tableRow[4]);
        item->setToolTip(QString("Beam live load [lb/ft] of story " + QString::number(i+1)));
        loadingSpreadsheet->setItem(i,4,item);

        item = new QTableWidgetItem(tableRow[5]);
        item->setToolTip(QString("Leaning column dead load [kips] of story " + QString::number(i+1)));
        loadingSpreadsheet->setItem(i,5,item);

        item = new QTableWidgetItem(tableRow[6]);
        item->setToolTip(QString("Leaning column live load [kips] of story " + QString::number(i+1)));
        loadingSpreadsheet->setItem(i,6,item);
    }

    updatingPropertiesTable = false;

    return;
}


void SteelBuildingModel::updateMemberDepthSpreadSheet(const QVector<QStringList>& data)
{

    if(data.empty())
    {
        qDebug()<<"Error in "<<__FUNCSIG__<<". Empty data vector";
        return;
    }

    memberDepthSpreadsheet->clear();
    memberDepthSpreadsheet->setColumnCount(3);
    memberDepthSpreadsheet->setRowCount(data.size());
    QStringList memberHeadings = this->getMemberTableHeadings();
    memberDepthSpreadsheet->setHorizontalHeaderLabels(memberHeadings);
    memberDepthSpreadsheet->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memberDepthSpreadsheet->verticalHeader()->setVisible(false);

    updatingPropertiesTable = true;
    for (int i = 0; i< data.size(); ++i)
    {
        auto tableRow = data[i];

        if(tableRow.size() != 3)
        {
            qDebug()<<"Error in "<<__FUNCSIG__<<". The number of parameters per row should be 7";
            return;
        }

        QTableWidgetItem *item;
        item = new QTableWidgetItem(tableRow[0]);
        item->setToolTip(QString("Depth of exterior column of story " + QString::number(i+1)));
        memberDepthSpreadsheet->setItem(i,0, item);

        item = new QTableWidgetItem(tableRow[1]);
        item->setToolTip(QString("Depth of interior column of story " + QString::number(i+1)));
        memberDepthSpreadsheet->setItem(i,1,item);

        item = new QTableWidgetItem(tableRow[2]);
        item->setToolTip(QString("Depth of beam of story " + QString::number(i+1)));
        memberDepthSpreadsheet->setItem(i,2,item);
    }

    updatingPropertiesTable = false;

    return;
}


void SteelBuildingModel::setDefaultValues(void)
{
    auto numFloors = 3;

    // Building geometry default values
    numFloorsLE->setText(QString::number(numFloors));
    numBayXLE->setText("4");
    numBayZLE->setText("4");
    firstStoryHeightLE->setText("13.0");
    typStoryHeightLE->setText("13.0");
    xBayWidthLE->setText("30.0");
    zBayWidthLE->setText("30.0");
    numLFRSXLE->setText("2");
    numLFRSZLE->setText("2");

    // ELF parameters default values
    SsLE->setText("2.25");
    S1LE->setText("0.6");
    TLLE->setText("8");
    CdLE->setText("5.5");
    RLE->setText( "8");
    leLE->setText("1");
    rhoLE->setText("1");
    siteClassLE->setText("D");
    CtLE->setText("0.028");
    xLE->setText("0.8");

    QVector<QStringList> defaultMemberSizes(numFloors);

    QVector<QStringList> defaultLoadingSizes(numFloors);

    for(int i = 0; i<numFloors; ++i)
    {
        QStringList loading;
        loading<<"2289"<<"106"<<"50"<<"1590"<<"750"<<"954"<<"450";

        QStringList memberDepths;
        memberDepths<<"W14"<<"W14"<<"W27, W30, W33";

        defaultLoadingSizes[i]=loading;
        defaultMemberSizes[i]=memberDepths;
    }

    this->updateMemberDepthSpreadSheet(defaultMemberSizes);
    this->updateLoadingSpreadSheet(defaultLoadingSizes);

    GeneralInformationWidget *theGI = GeneralInformationWidget::getInstance();

    auto buildingHeight = 13.0 + static_cast<double>(numFloors-1)*13.0;
    auto buildingWidth = 30.0*4.0;
    auto buildingDepth = 30.0*4.0;

    theGI->setNumStoriesAndHeight(numFloors, buildingHeight);
    theGI->setBuildingDimensions(buildingWidth, buildingDepth, buildingWidth*buildingDepth);

    // InputWidgetOpenSeesAnalysis *theOSWidget = InputWidgetOpenSeesAnalysis::getInstance();

    return;
}
