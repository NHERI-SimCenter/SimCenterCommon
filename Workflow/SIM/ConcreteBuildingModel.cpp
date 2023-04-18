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

// Written: Dr. Stevan Gavrilovic

#include "ConcreteBuildingModel.h"
#include "ReadWriteRVJSON.h"
#include "RandomVariable.h"
#include "GeneralInformationWidget.h"
#include "RandomVariablesContainer.h"
#include <LineEditRV.h>
#include <ReadWriteRVJSON.h>

#include <QMessageBox>
#include <QTableWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>
#include <QValidator>


ConcreteBuildingModel::ConcreteBuildingModel(RandomVariablesContainer *theRandomVariableIW, QWidget *parent): SimCenterAppWidget(parent), theRandomVariablesContainer(theRandomVariableIW)
{

    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);

    // The main layout box
    QGridLayout *mainLayout = new QGridLayout(this);

    RandomVariablesContainer *randomVariables = RandomVariablesContainer::getInstance();

    // Columm sections
    QGroupBox* columnSectionsBox = new QGroupBox("Column Sections");
    QGridLayout *columnSectionsGrid = new QGridLayout(columnSectionsBox);

    QLabel* extLabel = new QLabel("Exterior");
    extLabel->setStyleSheet("font-weight: bold");

    QLabel* extWidthLabel = new QLabel("Width (m)");
    extWidthLE = new QLineEdit();
    extWidthLE->setValidator(new QDoubleValidator);

    QLabel* extDepthLabel = new QLabel("Depth (m)");
    extDepthLE = new QLineEdit();
    extDepthLE->setValidator(new QDoubleValidator);

    columnSectionsGrid->addWidget(extLabel,0,0,1,2,Qt::AlignCenter);
    columnSectionsGrid->addWidget(extWidthLabel,1,0,1,1);
    columnSectionsGrid->addWidget(extWidthLE,1,1,1,1);

    columnSectionsGrid->addWidget(extDepthLabel,2,0,1,1);
    columnSectionsGrid->addWidget(extDepthLE,2,1,1,1);

    QLabel* intLabel = new QLabel("Interior");
    intLabel->setStyleSheet("font-weight: bold");

    QLabel* intWidthLabel = new QLabel("Width (m)");
    intWidthLE = new QLineEdit();
    intWidthLE->setValidator(new QDoubleValidator);

    QLabel* intDepthLabel = new QLabel("Depth (m)");
    intDepthLE = new QLineEdit();
    intDepthLE->setValidator(new QDoubleValidator);

    columnSectionsGrid->addWidget(intLabel,0,2,1,2,Qt::AlignCenter);
    columnSectionsGrid->addWidget(intWidthLabel,1,2,1,1);
    columnSectionsGrid->addWidget(intWidthLE,1,3,1,1);

    columnSectionsGrid->addWidget(intDepthLabel,2,2,1,1);
    columnSectionsGrid->addWidget(intDepthLE,2,3,1,1);

    // Beam sections
    QGroupBox* beamSectionsBox = new QGroupBox("Beam Sections");
    QGridLayout *beamSectionsGrid = new QGridLayout(beamSectionsBox);

    QLabel* beamWidthhLabel = new QLabel("Width (m)");
    beamWidthLE = new QLineEdit();
    beamWidthLE->setValidator(new QDoubleValidator);

    QLabel* beamDepthLabel = new QLabel("Depth (m)");
    beamDepthLE = new QLineEdit();
    beamDepthLE->setValidator(new QDoubleValidator);

    beamSectionsGrid->addWidget(beamWidthhLabel,0,0);
    beamSectionsGrid->addWidget(beamWidthLE,0,1);
    beamSectionsGrid->addWidget(beamDepthLabel,1,0);
    beamSectionsGrid->addWidget(beamDepthLE,1,1);

    // Frame geometry
    QGroupBox* frameGeometryBox = new QGroupBox("Building Information");
    QGridLayout *frameGeometryGrid = new QGridLayout(frameGeometryBox);

    dampingRatio = new LineEditRV(randomVariables);
    dampingRatio->setText("0.04");
    dampingRatio->setMaximumWidth(100);
    dampingRatio->setMinimumWidth(100);    
    dampingRatio->setToolTip(tr("damping ratio, .04 = 4% damping"));
    frameGeometryGrid->addWidget(new QLabel("Damping Ratio"), 0, 0);
    frameGeometryGrid->addWidget(dampingRatio, 0, 1);        
    
    QLabel* vecStoryLabel = new QLabel("Vector of story heights (m) = h1,h2,h3,...");
    vecStoryLE = new QLineEdit();
    connect(vecStoryLE,&QLineEdit::editingFinished,this,&ConcreteBuildingModel::onStoryLEChange);

    QLabel* vecSpansLabel = new QLabel("Vector of spans (m) = s1,s2,s3,...");
    vecSpansLE = new QLineEdit();

    connect(vecSpansLE,&QLineEdit::editingFinished,this,&ConcreteBuildingModel::onSpanLEChange);

    frameGeometryGrid->addWidget(vecStoryLabel,1,0,1,1);
    frameGeometryGrid->addWidget(vecStoryLE,1,1,1,1);

    frameGeometryGrid->addWidget(vecSpansLabel,2,0,1,1);
    frameGeometryGrid->addWidget(vecSpansLE,2,1,1,1);

    // Design params
    QGroupBox* designParamBox = new QGroupBox("Seismic ASCE 7-16 Design Parameters");
    QGridLayout* designParamGrid = new QGridLayout(designParamBox);

    QLabel* RLabel = new QLabel("R");
    RLE = new QLineEdit();
    RLE->setValidator(new QDoubleValidator);

    QLabel* CdLabel = new QLabel("Cd");
    CdLE = new QLineEdit();
    CdLE->setValidator(new QDoubleValidator);

    QLabel* OmegaLabel = new QLabel("Ω");
    OmegaLE = new QLineEdit();
    OmegaLE->setValidator(new QDoubleValidator);

    QLabel* SDSLabel = new QLabel("<html><head/><body><p>S<span style=\" vertical-align:sub;\">DS  </span>(g) =</p></body></html>");
    SDSLE = new QLineEdit();
    SDSLE->setValidator(new QDoubleValidator);

    QLabel* SD1Label = new QLabel("<html><head/><body><p>S<span style=\" vertical-align:sub;\">D1  </span>(g) =</p></body></html>");
    SD1LE = new QLineEdit();
    SD1LE->setValidator(new QDoubleValidator);

    QLabel* TLLabel = new QLabel("<html><head/><body><p>T<span style=\" vertical-align:sub;\">L  </span>(sec) =</p></body></html>");
    TLLE = new QLineEdit();
    TLLE->setValidator(new QDoubleValidator);

    designParamGrid->addWidget(RLabel,0,0);
    designParamGrid->addWidget(RLE,0,1);
    designParamGrid->addWidget(CdLabel,1,0);
    designParamGrid->addWidget(CdLE,1,1);
    designParamGrid->addWidget(OmegaLabel,2,0);
    designParamGrid->addWidget(OmegaLE,2,1);

    designParamGrid->addWidget(SDSLabel,0,2);
    designParamGrid->addWidget(SDSLE,0,3);
    designParamGrid->addWidget(SD1Label,1,2);
    designParamGrid->addWidget(SD1LE,1,3);
    designParamGrid->addWidget(TLLabel,2,2);
    designParamGrid->addWidget(TLLE,2,3);

    QFrame* line = new QFrame();
    line->setGeometry(QRect());
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);

    // Add a line to break up the box
    designParamGrid->addWidget(line,0,4,3,1);

    // Bottom design param layout
    QGridLayout* designParamBotGrid = new QGridLayout();

    QLabel* memberLabel = new QLabel("Member");
    memberLabel->setStyleSheet("font-weight: bold");

    QLabel* momInertiaLabel = new QLabel("Moment of inertia for elastic analysis");
    momInertiaLabel->setStyleSheet("font-weight: bold");

    QLabel* colLabel = new QLabel("Columns");
    colIgLE = new QLineEdit();
    colIgLE->setValidator(new QDoubleValidator);
    QLabel* IgLabel1 = new QLabel("<html><head/><body><p><span style=\" font-size:12pt;\">I</span><span style=\" font-size:12pt; vertical-align:sub;\">g</span></p></body></html>");

    QLabel* beamsLabel = new QLabel("Beams");
    beamIgLE = new QLineEdit();
    beamIgLE->setValidator(new QDoubleValidator);
    QLabel* IgLabel2 = new QLabel("<html><head/><body><p><span style=\" font-size:12pt;\">I</span><span style=\" font-size:12pt; vertical-align:sub;\">g</span></p></body></html>");

    designParamBotGrid->addWidget(memberLabel,0,0,1,1,Qt::AlignCenter);
    designParamBotGrid->addWidget(momInertiaLabel,0,1,1,2,Qt::AlignCenter);

    designParamBotGrid->addWidget(colLabel,1,0,1,1);
    designParamBotGrid->addWidget(colIgLE,1,1,1,1);
    designParamBotGrid->addWidget(IgLabel1,1,2,1,1);

    designParamBotGrid->addWidget(beamsLabel,2,0,1,1);
    designParamBotGrid->addWidget(beamIgLE,2,1,1,1);
    designParamBotGrid->addWidget(IgLabel2,2,2,1,1);

    designParamGrid->addLayout(designParamBotGrid,0,5,3,1);

    // Materials
    QGroupBox* materialsBox = new QGroupBox("Materials");
    QGridLayout* materialsGrid = new QGridLayout(materialsBox);

    QLabel* fyLabel = new QLabel("<html><head/><body><p>f<span style=\" vertical-align:sub;\">y</span> (MPa)</p></body></html>");
    fyLE = new QLineEdit();

    QLabel* beamsfpcLabel = new QLabel("<html><head/><body><p>Beams f´<span style=\" vertical-align:sub;\">c</span> (MPa)</p></body></html>");
    beamfpcLE = new QLineEdit();

    QLabel* colsfpcLabel = new QLabel("<html><head/><body><p>Columns f´<span style=\" vertical-align:sub;\">c</span> (MPa)</p></body></html>");
    colsfpcLE = new QLineEdit();

    materialsGrid->addWidget(fyLabel,0,0);
    materialsGrid->addWidget(fyLE,0,1);
    materialsGrid->addWidget(beamsfpcLabel,1,0);
    materialsGrid->addWidget(beamfpcLE,1,1);
    materialsGrid->addWidget(colsfpcLabel,2,0);
    materialsGrid->addWidget(colsfpcLE,2,1);

    // Frame loading
    QGroupBox* loadingBox = new QGroupBox("Frame Loading");
    QGridLayout* loadingGrid = new QGridLayout(loadingBox);

    QLabel* deadLoadLabel = new QLabel("<html><head/><body><p>Dead Load (kN/m<span style=\" vertical-align:super;\">2</span>)</p></body></html>");
    deadLaodLE = new QLineEdit();

    QLabel* liveLoadLabel = new QLabel("<html><head/><body><p>Live Load (kN/m<span style=\" vertical-align:super;\">2</span>)</p></body></html>");
    liveLoadLE = new QLineEdit();

    QLabel* tribLengthGravityLabel = new QLabel("Tributary Length for gravity (m)");
    tribLengthGravityLE = new QLineEdit();

    QLabel* tribLengthSeismicLabel = new QLabel("Tributary Length for seismic (m)");
    tribLengthSeismicLE = new QLineEdit();

    loadingGrid->addWidget(deadLoadLabel,0,0);
    loadingGrid->addWidget(deadLaodLE,0,1);
    loadingGrid->addWidget(liveLoadLabel,1,0);
    loadingGrid->addWidget(liveLoadLE,1,1);
    loadingGrid->addWidget(tribLengthGravityLabel,2,0);
    loadingGrid->addWidget(tribLengthGravityLE,2,1);
    loadingGrid->addWidget(tribLengthSeismicLabel,3,0);
    loadingGrid->addWidget(tribLengthSeismicLE,3,1);

    // Plastic Hinge Length
    QGroupBox* plasticHingeBox = new QGroupBox("Plastic hinge length lp");
    QVBoxLayout* plasticHingeVBoxLayout = new QVBoxLayout(plasticHingeBox);

    hingOpt1Radio = new QRadioButton();
    hingOpt2Radio = new QRadioButton();
    hingOpt3Radio = new QRadioButton();

    QLabel* optButton1Label = new QLabel("<html><head/><body><p><span style=\" font-style:italic;\">l</span><span style=\" font-style:italic; vertical-align:sub;\">p</span><span style=\" font-style:italic;\"> = 0.5H</span></p></body></html>");
    QLabel* optButton2Label = new QLabel("<html><head/><body><p><span style=\" font-style:italic;\">l</span><span style=\" font-style:italic; vertical-align:sub;\">p</span><span style=\" font-style:italic;\"> = 0.08l + 0.022d</span><span style=\" font-style:italic; vertical-align:sub;\">b</span><span style=\" font-style:italic;\">f</span><span style=\" font-style:italic; vertical-align:sub;\">y </span><span style=\" font-style:italic;\">(Priestley and Park)</span></p></body></html>");
    QLabel* optButton3Label = new QLabel("<html><head/><body><p><span style=\" font-style:italic;\">l</span><span style=\" font-style:italic; vertical-align:sub;\">p</span><span style=\" font-style:italic;\"> = 0.05l + 0.1d</span><span style=\" font-style:italic; vertical-align:sub;\">b</span><span style=\" font-style:italic;\">f</span><span style=\" font-style:italic; vertical-align:sub;\">y </span><span style=\" font-style:italic;\">/ √f\'</span><span style=\" font-style:italic; vertical-align:sub;\">c </span><span style=\" font-style:italic;\">(Berry)</span></p></body></html>");

    hingeButtonGroup = new QButtonGroup(this);
    hingeButtonGroup->addButton(hingOpt1Radio,1);
    hingeButtonGroup->addButton(hingOpt2Radio,2);
    hingeButtonGroup->addButton(hingOpt3Radio,3);

    QHBoxLayout *row1 = new QHBoxLayout();
    row1->addWidget(hingOpt1Radio);
    row1->addWidget(optButton1Label);
    row1->addStretch();

    QHBoxLayout *row2 = new QHBoxLayout();
    row2->addWidget(hingOpt2Radio);
    row2->addWidget(optButton2Label);
    row2->addStretch();

    QHBoxLayout *row3 = new QHBoxLayout();
    row3->addWidget(hingOpt3Radio);
    row3->addWidget(optButton3Label);
    row3->addStretch();

    plasticHingeVBoxLayout->addLayout(row1);
    plasticHingeVBoxLayout->addLayout(row2);
    plasticHingeVBoxLayout->addLayout(row3);

    // Plastic Hinge Length
    QGroupBox* regularizationBox = new QGroupBox("Regularization of Material Stress-strain Curves");
    QVBoxLayout* regularizationLayout = new QVBoxLayout(regularizationBox);

    regOpt1Radio = new QRadioButton("Yes");
    regOpt2Radio = new QRadioButton("No");

    regButtonGroup = new QButtonGroup(this);
    regButtonGroup->addButton(regOpt1Radio,0); // yes
    regButtonGroup->addButton(regOpt2Radio,1); // no

    regularizationLayout->addWidget(regOpt1Radio);
    regularizationLayout->addWidget(regOpt2Radio);

    // Add everything to the main layout
    mainLayout->addWidget(frameGeometryBox,0,0,1,2);
    mainLayout->addWidget(columnSectionsBox,1,0);
    mainLayout->addWidget(beamSectionsBox,1,1);
    mainLayout->addWidget(designParamBox,2,0,1,2);
    mainLayout->addWidget(materialsBox,3,0);
    mainLayout->addWidget(loadingBox,3,1);
    mainLayout->addWidget(plasticHingeBox,4,0);
    mainLayout->addWidget(regularizationBox,4,1);
    mainLayout->setColumnStretch(0,0);
    mainLayout->setColumnStretch(1,0);


    // Connect the line edits that can be RVs to a slot that can process any RVs that come in
    connect(fyLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(beamfpcLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(colsfpcLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(deadLaodLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(liveLoadLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(tribLengthGravityLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));
    connect(tribLengthSeismicLE,SIGNAL(editingFinished()),this,SLOT(onLineEditTextChanged()));

    // add signal and slot connections with GI
    GeneralInformationWidget *theGI = GeneralInformationWidget::getInstance();

    this->setDefaultValues();
    
    
    connect(this,SIGNAL(numStoriesOrHeightChanged(int, double)), theGI, SLOT(setNumStoriesAndHeight(int, double)));
    connect(this,SIGNAL(buildingDimensionsChanged(double,double,double)),theGI,SLOT(setBuildingDimensions(double,double,double)));
}


ConcreteBuildingModel::~ConcreteBuildingModel()
{

}


bool ConcreteBuildingModel::checkRV(const QString& value)
{
    bool ok;
    double textToDouble = value.toDouble(&ok);
    Q_UNUSED(textToDouble);

    if(ok)
        return false;

    return true;
}


bool ConcreteBuildingModel::outputToJSON(QJsonObject &jsonObject)
{

    // The application type
    jsonObject["type"]="ConcreteBuildingModel";

    writeLineEditRV(jsonObject,"dampingRatio", dampingRatio);
    
    jsonObject["ExtColWidth"] = extWidthLE->text();
    jsonObject["ExtColDepth"] = extDepthLE->text();
    jsonObject["IntColWidth"] = intWidthLE->text();
    jsonObject["IntColDepth"] = intDepthLE->text();
    jsonObject["BeamWidth"] = beamWidthLE->text();
    jsonObject["BeamDepth"] = beamDepthLE->text();
    jsonObject["VecStoryHeights"] = vecStoryLE->text();
    jsonObject["VecSpans"] = vecSpansLE->text();

    jsonObject["RParam"] = RLE->text();
    jsonObject["CdParam"] = CdLE->text();
    jsonObject["OmegaParam"] = OmegaLE->text();
    jsonObject["SDSParam"] = SDSLE->text();
    jsonObject["SD1Param"] = SD1LE->text();
    jsonObject["TLParam"] = TLLE->text();

    jsonObject["ColIg"] = colIgLE->text();
    jsonObject["BeamIg"] = beamIgLE->text();

    // Treat the random variables as a special case
    writeLineEditRV(jsonObject,"FySteel", fyLE);
    writeLineEditRV(jsonObject,"BeamFpc", beamfpcLE);
    writeLineEditRV(jsonObject,"ColFpc", colsfpcLE);

    writeLineEditRV(jsonObject,"DeadLoad", deadLaodLE);
    writeLineEditRV(jsonObject,"LiveLoad", liveLoadLE);
    writeLineEditRV(jsonObject,"TribLengthGravity", tribLengthGravityLE);
    writeLineEditRV(jsonObject,"TribLengthSeismic", tribLengthSeismicLE);

    auto plasticHingeOpt = hingeButtonGroup->checkedId();

    jsonObject["PlasticHingeOpt"] = plasticHingeOpt;

    auto regChecked = regButtonGroup->checkedId();

    jsonObject["IncludeRegularization"] = regChecked == 0 ? true : false;

    // Output the random variables
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


    return true;
}


bool ConcreteBuildingModel::inputFromJSON(QJsonObject &jsonObject)
{
    // The application type
    readLineEditRV(jsonObject,"dampingRatio", dampingRatio);
    extWidthLE->setText(jsonObject["ExtColWidth"].toString());
    extDepthLE->setText(jsonObject["ExtColDepth"].toString());
    intWidthLE->setText(jsonObject["IntColWidth"].toString());
    intDepthLE->setText(jsonObject["IntColDepth"].toString());
    beamWidthLE->setText(jsonObject["BeamWidth"].toString());
    beamDepthLE->setText(jsonObject["BeamDepth"].toString());
    vecStoryLE->setText(jsonObject["VecStoryHeights"].toString());
    vecSpansLE->setText(jsonObject["VecSpans"].toString());
    RLE->setText(jsonObject["RParam"].toString());
    CdLE->setText(jsonObject["CdParam"].toString());
    OmegaLE->setText(jsonObject["OmegaParam"].toString());
    SDSLE->setText(jsonObject["SDSParam"].toString());
    SD1LE->setText(jsonObject["SD1Param"].toString());
    TLLE->setText(jsonObject["TLParam"].toString());
    colIgLE->setText( jsonObject["ColIg"].toString());
    beamIgLE->setText(jsonObject["BeamIg"].toString());

    // Treat the random variables as a special case
    readLineEditRV(jsonObject,"FySteel", fyLE);
    readLineEditRV(jsonObject,"BeamFpc", beamfpcLE);
    readLineEditRV(jsonObject,"ColFpc", colsfpcLE);

    readLineEditRV(jsonObject,"DeadLoad", deadLaodLE);
    readLineEditRV(jsonObject,"LiveLoad", liveLoadLE);
    readLineEditRV(jsonObject,"TribLengthGravity", tribLengthGravityLE);
    readLineEditRV(jsonObject,"TribLengthSeismic", tribLengthSeismicLE);

    int plasticHingeOpt = jsonObject["PlasticHingeOpt"].toInt();

    auto button = hingeButtonGroup->button(plasticHingeOpt);
    if(button)
    {
        button->setChecked(true);
    }
    else
    {
        this->errorMessage("Could not recognize the plastic hinge length option provided " + jsonObject["PlasticHingeOpt"].toString() + " (only options 1,2,3 are available). Setting default to option 1");
        hingOpt1Radio->setChecked(true);
    }

    auto regOpt = jsonObject["IncludeRegularization"].toBool();

   if(regOpt == true)
       regOpt1Radio->setChecked(true);
   else
       regOpt2Radio->setChecked(true);

    return true;
}


bool ConcreteBuildingModel::outputAppDataToJSON(QJsonObject &jsonObject) {

    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    jsonObject["Application"] = "ConcreteBuildingModel";
    QJsonObject dataObj;
    jsonObject["ApplicationData"]=dataObj;

    return true;
}


bool ConcreteBuildingModel::inputAppDataFromJSON(QJsonObject &jsonObject) {

    Q_UNUSED(jsonObject);
    //
    // from ApplicationData
    //

    return true;
}


bool ConcreteBuildingModel::copyFiles(QString &dirName) {

    Q_UNUSED(dirName);
    return true;
}


void ConcreteBuildingModel::addRandomVariable(QString &text, int numReferences)
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


void ConcreteBuildingModel::removeRandomVariable(QString &text, int numReferences)
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


void ConcreteBuildingModel::onLineEditTextChanged()
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


void ConcreteBuildingModel::errorMessageDialog(const QString& errorString)
{
    QMessageBox msgBox;
    msgBox.setText(errorString);
    msgBox.setStandardButtons(QMessageBox::Close);
    msgBox.exec();
}


void ConcreteBuildingModel::setDefaultValues(void)
{
    extWidthLE->setText("0.4");
    extDepthLE->setText("0.5");
    intWidthLE->setText("0.4");
    intDepthLE->setText("0.5");
    beamWidthLE->setText("0.4");
    beamDepthLE->setText("0.5");
    vecStoryLE->setText("3.5,3,3");
    vecSpansLE->setText("6,6");
    RLE->setText("4.0");
    CdLE->setText("3.5");
    OmegaLE->setText("3.0");
    SDSLE->setText("0.5");
    SD1LE->setText("0.2");
    TLLE->setText("4.0");
    colIgLE->setText("0.7");
    beamIgLE->setText("0.35");
    fyLE->setText("420.0");
    beamfpcLE->setText("28.0");
    colsfpcLE->setText("28.0");
    deadLaodLE->setText("6.0");
    liveLoadLE->setText("2.0");
    tribLengthGravityLE->setText("5.0");
    tribLengthSeismicLE->setText("5.0");

    hingOpt1Radio->setChecked(true);
    regOpt1Radio->setChecked(true);

    this->onStoryLEChange();
    this->onSpanLEChange();
}


void ConcreteBuildingModel::onStoryLEChange(void)
{
    auto text = vecStoryLE->text();

    if(text.isEmpty())
        return;

    auto res = text.split(",");

    // Case where there is only one story
    if(res.empty())
        res.append(text);

    double buildingHeight = 0.0;
    int numFloors = 0;
    for(auto&& it : res)
    {
        // Remove any whitespace from start and end
        auto heightStr = it.simplified();
        bool OK;
        auto res = heightStr.toDouble(&OK);

        if(!OK)
        {
            this->infoMessage("Warning: only numeric values separated by ',' are allowed to be a story height");
            return;
        }

        ++numFloors;
        buildingHeight += res;
    }

    // Set the floors and building height to the general information widget
    emit numStoriesOrHeightChanged(numFloors, buildingHeight);
}


void ConcreteBuildingModel::onSpanLEChange(void)
{
    auto text = vecSpansLE->text();

    if(text.isEmpty())
        return;

    auto res = text.split(",");

    // Case where there is only one story
    if(res.empty())
        res.append(text);

    double buildingWidth = 0.0;
    int numSpans = 0;
    for(auto&& it : res)
    {
        // Remove any whitespace from start and end
        auto spanStr = it.simplified();
        bool OK;
        auto res = spanStr.toDouble(&OK);

        if(!OK)
        {
            this->infoMessage("Warning: only numeric values separated by ',' are allowed to be a story height");
            return;
        }

        ++numSpans;
        buildingWidth += res;
    }

    auto buildingDepth = 0.0;
    auto planArea = 0.0;

    // Set the building dims to the general information widget
    emit buildingDimensionsChanged(buildingWidth,buildingDepth,planArea);

}
