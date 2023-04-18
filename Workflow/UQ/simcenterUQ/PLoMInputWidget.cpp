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

// Written: fmckenna, kuanshi

#include <PLoMInputWidget.h>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>
#include <QPushButton>
#include <QFileDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>

#include <iostream>
#include <fstream>
#include <regex>
#include <iterator>
#include <string>
#include <sstream>
//#include <InputWidgetParameters.h>
//#include <InputWidgetEDP.h>
//#include <InputWidgetFEM.h>
#include <QButtonGroup>
#include <QRadioButton>
#include <QStackedWidget>
#include <QJsonDocument>
#include <QJsonArray>
#include <QComboBox>
#include <QDebug>

PLoMInputWidget::PLoMInputWidget(QWidget *parent)
    : UQ_Method(parent)
{
    auto layout = new QVBoxLayout();
    int wid = 0; // widget id

    //First we need to add type radio buttons
    m_typeButtonsGroup = new QButtonGroup(this);
    QRadioButton* rawDataRadioButton = new QRadioButton(tr("Raw Data"));
    QRadioButton* preTrainRadioButton = new QRadioButton(tr("Pre-trained Model"));
    m_typeButtonsGroup->addButton(rawDataRadioButton, 0);
    m_typeButtonsGroup->addButton(preTrainRadioButton, 1);
    QWidget* typeGroupBox = new QWidget(this);
    typeGroupBox->setContentsMargins(0,0,0,0);
    typeGroupBox->setStyleSheet("QGroupBox { font-weight: normal;}");
    QHBoxLayout* typeLayout = new QHBoxLayout(typeGroupBox);
    typeGroupBox->setLayout(typeLayout);
    typeLayout->addWidget(rawDataRadioButton);
    typeLayout->addWidget(preTrainRadioButton);
    typeLayout->addStretch();
    layout->addWidget(typeGroupBox,wid++);

    // input data widget
    rawDataGroup = new QWidget(this);
    rawDataGroup->setMaximumWidth(800);
    QGridLayout* rawDataLayout = new QGridLayout(rawDataGroup);
    rawDataGroup->setLayout(rawDataLayout);
    preTrainGroup = new QWidget(this);
    preTrainGroup->setMaximumWidth(800);
    QGridLayout* preTrainLayout = new QGridLayout(preTrainGroup);
    preTrainGroup->setLayout(preTrainLayout);
    // Create Input LineEdit
    inpFileDir = new QLineEdit();
    QPushButton *chooseInpFile = new QPushButton("Choose");
    connect(chooseInpFile, &QPushButton::clicked, this, [=](){
        inpFileDir->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)"));
        this->parseInputDataForRV(inpFileDir->text());
    });
    inpFileDir->setReadOnly(true);
    inpFileDir->setMaximumWidth(600);
    rawDataLayout->addWidget(new QLabel("Training Data File: Input"),0,0);
    rawDataLayout->addWidget(inpFileDir,0,1,1,3);
    rawDataLayout->addWidget(chooseInpFile,0,4);
    // Create Output LineEdit
    outFileDir = new QLineEdit();
    outFileDir->setMaximumWidth(600);
    chooseOutFile = new QPushButton("Choose");
    connect(chooseOutFile, &QPushButton::clicked, this, [=](){
        outFileDir->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)"));
        this->parseOutputDataForQoI(outFileDir->text());
    });
    outFileDir->setReadOnly(true);
    rawDataLayout->addWidget(new QLabel("Training Data File: Output"),1,0);
    rawDataLayout->addWidget(outFileDir,1,1,1,3);
    rawDataLayout->addWidget(chooseOutFile,1,4);
    errMSG=new QLabel("Unrecognized file format");
    errMSG->setStyleSheet({"color: red"});
    rawDataLayout->addWidget(errMSG,2,1);
    errMSG->hide();

    inpFileDir2 = new QLineEdit();
    QPushButton *chooseInpFile2 = new QPushButton("Choose");
    connect(chooseInpFile2, &QPushButton::clicked, this, [=](){
        inpFileDir2->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"", "h5 files (*.h5)"));
        this->parsePretrainedModelForRVQoI(inpFileDir2->text());
    });
    inpFileDir2->setReadOnly(true);
    preTrainLayout->addWidget(new QLabel("Training Data File: Pretrained Model"),0,0);
    preTrainLayout->addWidget(inpFileDir2,0,1,1,3);
    preTrainLayout->addWidget(chooseInpFile2,0,4);

    //We will add stacked widget to switch between raw data and trained model
    m_stackedWidgets = new QStackedWidget(this);
    m_stackedWidgets->addWidget(rawDataGroup);
    m_stackedWidgets->addWidget(preTrainGroup);
    m_typeButtonsGroup->button(0)->setChecked(true);
    m_stackedWidgets->setCurrentIndex(0);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(m_typeButtonsGroup, &QButtonGroup::idReleased, [this](int id)
    {
        if(id == 0) {
            m_typeButtonsGroup->button(0)->setChecked(true);
            m_stackedWidgets->setCurrentIndex(0);
            preTrained = false;
        }
        else if (id == 1) {
            m_typeButtonsGroup->button(1)->setChecked(true);
            m_stackedWidgets->setCurrentIndex(1);
            preTrained = true;
        }
    });
#else
    connect(m_typeButtonsGroup, QOverload<int>::of(&QButtonGroup::buttonReleased),this, [this](int id)
    {
        if(id == 0) {
            m_typeButtonsGroup->button(0)->setChecked(true);
            m_stackedWidgets->setCurrentIndex(0);
            preTrained = false;
        }
        else if (id == 1) {
            m_typeButtonsGroup->button(1)->setChecked(true);
            m_stackedWidgets->setCurrentIndex(1);
            preTrained = true;
        }
    });
#endif

    layout->addWidget(m_stackedWidgets,wid++);

    // create widget for new sample ratio
    newSampleRatioWidget = new QWidget();
    newSampleRatioWidget->setMaximumWidth(400);
    QGridLayout* nsrLayout = new QGridLayout(newSampleRatioWidget);
    newSampleRatioWidget->setLayout(nsrLayout);
    ratioNewSamples = new QLineEdit();
    ratioNewSamples->setText(tr("5"));
    ratioNewSamples->setValidator(new QIntValidator);
    ratioNewSamples->setToolTip("The ratio between the number of new realizations and the size of original sample. \nIf \"0\" is given, the PLoM model is trained without new predictions");
    ratioNewSamples->setMaximumWidth(150);
    QLabel *newSNR = new QLabel("New Sample Number Ratio");
    nsrLayout->addWidget(newSNR, 0, 0);
    nsrLayout->addWidget(ratioNewSamples, 0, 1);
    layout->addWidget(newSampleRatioWidget, wid++);

    // Create Advanced options
    // advanced option widget
    QWidget *advOptGroup = new QWidget(this);
    QVBoxLayout* advOptLayout = new QVBoxLayout(advOptGroup);
    advOptGroup->setLayout(advOptLayout);
    int widd = 0;

    QHBoxLayout *advComboLayout = new QHBoxLayout();
    theAdvancedCheckBox = new QCheckBox();
    theAdvancedTitle=new QLabel("Advanced Options");
    theAdvancedTitle->setStyleSheet("font-weight: bold; color: gray");
    //advOptLayout->addWidget(theAdvancedTitle, widd, 1, Qt::AlignBottom);
    //advOptLayout->addWidget(theAdvancedCheckBox, widd, 0, Qt::AlignBottom);
    advComboLayout->addWidget(theAdvancedTitle);
    advComboLayout->addWidget(theAdvancedCheckBox);
    /***
    // create advanced combobox
    theAdvancedComboBox = new QComboBox();
    theAdvancedComboBox->addItem(tr("General"));
    theAdvancedComboBox->addItem(tr("KDE"));
    theAdvancedComboBox->addItem(tr("Constraints"));
    theAdvancedComboBox->setCurrentIndex(0);
    theAdvancedComboBox->setVisible(false);
    advComboLayout->addWidget(theAdvancedComboBox);
    ***/
    advComboLayout->addStretch();
    advOptLayout->addLayout(advComboLayout, widd++);

    // division line
    lineA = new QFrame;
    lineA->setFrameShape(QFrame::HLine);
    lineA->setFrameShadow(QFrame::Sunken);
    lineA->setMaximumWidth(300);
    advOptLayout->addWidget(lineA);
    lineA->setVisible(false);    

    // tab widget for adv. options
    advComboWidget = new QTabWidget(this);
    advOptLayout->addWidget(advComboWidget);
    advComboWidget->setVisible(false);
    advComboWidget->setStyleSheet("QTabBar {font-size: 10pt}");
    advComboWidget->setContentsMargins(0, 0, 0, 0);
    // adv. opt. general widget
    advGeneralWidget = new QWidget();
    advGeneralWidget->setMaximumWidth(800);
    QGridLayout* advGeneralLayout = new QGridLayout(advGeneralWidget);
    advGeneralWidget->setLayout(advGeneralLayout);
    // Log transform
    theLogtLabel=new QLabel("Log-space Transform");
    theLogtLabel2=new QLabel("     (only if data always positive)");
    theLogtCheckBox = new QCheckBox();
    advGeneralLayout->addWidget(theLogtLabel, 0, 0);
    advGeneralLayout->addWidget(theLogtLabel2, 0, 1,1,-1,Qt::AlignLeft);
    advGeneralLayout->addWidget(theLogtCheckBox, 0, 1);
    theLogtLabel->setVisible(false);
    theLogtLabel2->setVisible(false);
    theLogtCheckBox->setVisible(false);
    // random seed
    randomSeed = new QLineEdit();
    randomSeed->setText(tr("10"));
    randomSeed->setValidator(new QIntValidator);
    randomSeed->setToolTip("Random Seed Number");
    randomSeed->setMaximumWidth(150);
    newRandomSeed = new QLabel("Random Seed");
    advGeneralLayout->addWidget(newRandomSeed, 1, 0);
    advGeneralLayout->addWidget(randomSeed, 1, 1);
    randomSeed->setVisible(false);
    newRandomSeed->setVisible(false);
    // pca tolerance
    epsilonPCA = new QLineEdit();
    epsilonPCA->setText(tr("0.0001"));
    epsilonPCA->setValidator(new QDoubleValidator);
    epsilonPCA->setToolTip("PCA Tolerance");
    epsilonPCA->setMaximumWidth(150);
    newEpsilonPCA = new QLabel("PCA Tolerance");
    advGeneralLayout->addWidget(newEpsilonPCA, 2, 0);
    advGeneralLayout->addWidget(epsilonPCA, 2, 1);
    epsilonPCA->setVisible(false);
    newEpsilonPCA->setVisible(false);
    //
    advComboWidget->addTab(advGeneralWidget, "General");

    // adv. opt. kde widget
    advKDEWidget = new QWidget();
    advKDEWidget->setMaximumWidth(800);
    QGridLayout* advKDELayout = new QGridLayout(advKDEWidget);
    advKDEWidget->setLayout(advKDELayout);
    // kde smooth factor
    smootherKDE = new QLineEdit();
    smootherKDE->setText(tr("25"));
    smootherKDE->setValidator(new QDoubleValidator);
    smootherKDE->setToolTip("KDE Smooth Factor");
    smootherKDE->setMaximumWidth(150);
    newSmootherKDE = new QLabel("KDE Smooth Factor");
    advKDELayout->addWidget(newSmootherKDE, 0, 0);
    advKDELayout->addWidget(smootherKDE, 0, 1);
    smootherKDE->setVisible(false);
    newSmootherKDE->setVisible(false);
    // diff. maps
    theDMLabel=new QLabel("Diffusion Maps");
    theDMCheckBox = new QCheckBox();
    advKDELayout->addWidget(theDMLabel, 1, 0);
    advKDELayout->addWidget(theDMCheckBox, 1, 1);
    theDMLabel->setVisible(false);
    theDMCheckBox->setVisible(false);
    theDMCheckBox->setChecked(true);
    // diff. maps tolerance
    tolKDE = new QLineEdit();
    tolKDE->setText(tr("0.1"));
    tolKDE->setValidator(new QDoubleValidator);
    tolKDE->setToolTip("Diffusion Maps Tolerance: ratio between the cut-off eigenvalue and the first eigenvalue.");
    tolKDE->setMaximumWidth(150);
    newTolKDE = new QLabel("Diff. Maps Tolerance");
    advKDELayout->addWidget(newTolKDE, 2, 0);
    advKDELayout->addWidget(tolKDE, 2, 1);
    tolKDE->setVisible(false);
    newTolKDE->setVisible(false);
    tolKDE->setDisabled(false);
    connect(theDMCheckBox,SIGNAL(toggled(bool)),this,SLOT(setDiffMaps(bool)));
    //
    advComboWidget->addTab(advKDEWidget, "Kernel Density Estimation");

    // adv. opt. constraints widget
    advConstraintsWidget = new QWidget();
    advConstraintsWidget->setMaximumWidth(800);
    QGridLayout* advConstraintsLayout = new QGridLayout(advConstraintsWidget);
    advConstraintsWidget->setLayout(advConstraintsLayout);
    //
    theConstraintsButton = new QCheckBox();
    theConstraintsLabel2 = new QLabel();
    theConstraintsLabel2->setText("Add constratins");
    constraintsPath = new QLineEdit();
    chooseConstraints = new QPushButton(tr("Choose"));
    chooseConstraints->setMaximumWidth(150);
    connect(chooseConstraints, &QPushButton::clicked, this, [=](){
        constraintsPath->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)"));
    });
    constraintsPath->setReadOnly(true);
    theConstraintsLabel1 = new QLabel();
    theConstraintsLabel1->setText("Constraints file (.py)");
    advConstraintsLayout->addWidget(theConstraintsButton,0,1,Qt::AlignTop);
    advConstraintsLayout->addWidget(theConstraintsLabel2,0,0,Qt::AlignTop);
    advConstraintsLayout->addWidget(theConstraintsLabel1,1,0,Qt::AlignTop);
    advConstraintsLayout->addWidget(constraintsPath,1,1,1,2,Qt::AlignTop);
    advConstraintsLayout->addWidget(chooseConstraints,1,3,Qt::AlignTop);
    constraintsPath->setVisible(false);
    theConstraintsLabel1->setVisible(false);
    theConstraintsLabel2->setVisible(false);
    chooseConstraints->setVisible(false);
    theConstraintsButton->setVisible(false);
    constraintsPath->setDisabled(1);
    chooseConstraints->setDisabled(1);
    constraintsPath->setStyleSheet("background-color: lightgrey;border-color:grey");
    connect(theConstraintsButton,SIGNAL(toggled(bool)),this,SLOT(setConstraints(bool)));
    // iterations when applying constraints
    numIter = new QLineEdit();
    numIter->setText(tr("50"));
    numIter->setValidator(new QIntValidator);
    numIter->setToolTip("Iteration Number");
    numIter->setMaximumWidth(150);
    numIterLabel = new QLabel("Iteration Number");
    advConstraintsLayout->addWidget(numIterLabel, 2, 0);
    advConstraintsLayout->addWidget(numIter, 2, 1);
    numIter->setVisible(false);
    numIterLabel->setVisible(false);
    numIter->setDisabled(1);
    numIter->setStyleSheet("background-color: lightgrey;border-color:grey");
    // iteration tol
    tolIter = new QLineEdit();
    tolIter->setText(tr("0.02"));
    tolIter->setValidator(new QDoubleValidator);
    tolIter->setToolTip("Iteration Tolerance");
    tolIter->setMaximumWidth(150);
    tolIterLabel = new QLabel("Iteration Tolerance");
    advConstraintsLayout->addWidget(tolIterLabel, 3, 0);
    advConstraintsLayout->addWidget(tolIter, 3, 1);
    tolIter->setVisible(false);
    tolIterLabel->setVisible(false);
    tolIter->setDisabled(1);
    tolIter->setStyleSheet("background-color: lightgrey;border-color:grey");
    //
    advComboWidget->addTab(advConstraintsWidget, "Constraints");

    // create the stacked widgets
    /***
    adv_stackedWidgets = new QStackedWidget(this);
    adv_stackedWidgets->addWidget(advGeneralWidget);
    adv_stackedWidgets->addWidget(advKDEWidget);
    adv_stackedWidgets->addWidget(advConstraintsWidget);
    adv_stackedWidgets->setCurrentIndex(0);
    connect(theAdvancedComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int id)
    {
        adv_stackedWidgets->setCurrentIndex(id);
    });
    advOptLayout->addWidget(adv_stackedWidgets);
    ***/
    advOptLayout->addStretch();

    layout->addWidget(advOptGroup, wid++);
    layout->addStretch();

    //
    // Finish
    //

    //layout->setRowStretch(wid, 1);
    //layout->setColumnStretch(6, 1);
    this->setLayout(layout);

    outFileDir->setDisabled(0);
    chooseOutFile->setDisabled(0);
    //chooseOutFile->setStyleSheet("background-color: lightgrey;border-color:grey");

    connect(theAdvancedCheckBox,SIGNAL(toggled(bool)),this,SLOT(doAdvancedSetup(bool)));
}


PLoMInputWidget::~PLoMInputWidget()
{

}


// SLOT function
void PLoMInputWidget::doAdvancedSetup(bool tog)
{
    if (tog) {
        theAdvancedTitle->setStyleSheet("font-weight: bold; color: black");
    } else {
        theAdvancedTitle->setStyleSheet("font-weight: bold; color: gray");
        theLogtCheckBox->setChecked(false);
    }

    //theAdvancedComboBox->setVisible(tog);
    advComboWidget->setVisible(tog);
    lineA->setVisible(tog);
    theLogtCheckBox->setVisible(tog);
    theLogtLabel->setVisible(tog);
    theLogtLabel2->setVisible(tog);
    theDMCheckBox->setVisible(tog);
    theDMLabel->setVisible(tog);
    epsilonPCA->setVisible(tog);
    newEpsilonPCA->setVisible(tog);
    smootherKDE->setVisible(tog);
    newSmootherKDE->setVisible(tog);
    tolKDE->setVisible(tog);
    newTolKDE->setVisible(tog);
    randomSeed->setVisible(tog);
    newRandomSeed->setVisible(tog);
    constraintsPath->setVisible(tog);
    theConstraintsLabel1->setVisible(tog);
    theConstraintsLabel2->setVisible(tog);
    chooseConstraints->setVisible(tog);
    theConstraintsButton->setVisible(tog);
    numIter->setVisible(tog);
    numIterLabel->setVisible(tog);
    tolIter->setVisible(tog);
    tolIterLabel->setVisible(tog);
}


void PLoMInputWidget::setOutputDir(bool tog)
{
    if (tog) {
        outFileDir->setDisabled(0);
        chooseOutFile->setDisabled(0);
        chooseOutFile->setStyleSheet("color: white");
        //theFemWidget->setFEMforGP("GPdata");
        parseInputDataForRV(inpFileDir->text());
        parseOutputDataForQoI(outFileDir->text());
    } else {
        outFileDir->setDisabled(1);
        chooseOutFile->setDisabled(1);
        chooseOutFile->setStyleSheet("background-color: lightgrey;border-color:grey");
        //theEdpWidget->setGPQoINames(QStringList("") );
        outFileDir->setText(QString("") );
        //theFemWidget->setFEMforGP("GPmodel");
        //theFemWidget->femProgramChanged("OpenSees");
        //theEdpWidget->setGPQoINames(QStringList({}) );// remove GP RVs
        //theParameters->setGPVarNamesAndValues(QStringList({}));// remove GP RVs
    }
}

void PLoMInputWidget::setConstraints(bool tog)
{
    if (tog) {
        constraintsPath->setDisabled(0);
        chooseConstraints->setDisabled(0);
        constraintsPath->setStyleSheet("color: white");
        numIter->setStyleSheet("background-color: white");
        tolIter->setStyleSheet("background-color: white");
        numIter->setDisabled(0);
        tolIter->setDisabled(0);
    } else {
        constraintsPath->setDisabled(1);
        chooseConstraints->setDisabled(1);
        constraintsPath->setStyleSheet("background-color: lightgrey;border-color:grey");
        numIter->setStyleSheet("background-color: lightgrey;border-color:grey");
        tolIter->setStyleSheet("background-color: lightgrey;border-color:grey");
        numIter->setDisabled(1);
        tolIter->setDisabled(1);
    }
}

void PLoMInputWidget::setDiffMaps(bool tog)
{
    if (tog) {
        tolKDE->setDisabled(0);
        tolKDE->setStyleSheet("background-color: white");
    } else {
        tolKDE->setDisabled(1);
        tolKDE->setStyleSheet("background-color: lightgrey;border-color:grey");
    }
}

bool
PLoMInputWidget::outputToJSON(QJsonObject &jsonObj){

    bool result = true;

    if (m_typeButtonsGroup->button(0)->isChecked()) {
        jsonObj["preTrained"] = false;
        jsonObj["inpFile"]=inpFileDir->text();
        jsonObj["outFile"]=outFileDir->text();
    } else {
        jsonObj["preTrained"] = true;
        jsonObj["inpFile2"] = inpFileDir2->text();
    }


    jsonObj["outputData"]=true;

    jsonObj["newSampleRatio"]=ratioNewSamples->text().toInt();

    jsonObj["advancedOpt"]=theAdvancedCheckBox->isChecked();
    if (theAdvancedCheckBox->isChecked())
    {
        jsonObj["logTransform"]=theLogtCheckBox->isChecked();
        jsonObj["diffusionMaps"] = theDMCheckBox->isChecked();
        jsonObj["randomSeed"] = randomSeed->text().toInt();
        jsonObj["epsilonPCA"] = epsilonPCA->text().toDouble();
        jsonObj["smootherKDE"] = smootherKDE->text().toDouble();
        jsonObj["kdeTolerance"] = tolKDE->text().toDouble();
        jsonObj["constraints"]= theConstraintsButton->isChecked();
        if (theConstraintsButton->isChecked()) {
            jsonObj["constraintsFile"] = constraintsPath->text();
            jsonObj["numIter"] = numIter->text().toInt();
            jsonObj["tolIter"] = tolIter->text().toDouble();
        }
    }
    jsonObj["parallelExecution"]=false;

    return result;    
}


int PLoMInputWidget::parseInputDataForRV(QString name1){

    double numberOfColumns=countColumn(name1);

    QStringList varNamesAndValues;
    for (int i=0;i<numberOfColumns;i++) {
        varNamesAndValues.append(QString("RV_column%1").arg(i+1));
        varNamesAndValues.append("nan");
    }
    //theParameters->setGPVarNamesAndValues(varNamesAndValues);
    numSamples=0;
    return 0;
}

int PLoMInputWidget::parseOutputDataForQoI(QString name1){
    // get number of columns
    double numberOfColumns=countColumn(name1);
    QStringList qoiNames;
    for (int i=0;i<numberOfColumns;i++) {
        qoiNames.append(QString("QoI_column%1").arg(i+1));
    }
    //theEdpWidget->setGPQoINames(qoiNames);
    return 0;
}

int PLoMInputWidget::parsePretrainedModelForRVQoI(QString name1){

    // five tasks here:
    // 1. parse the JSON file of the pretrained model
    // 2. create RV
    // 3. create QoI
    // 4. check inpData file, inpFile.in
    // 5. check outFile file, outFile.in

    // look for the JSON file in the model directory
    QString fileName = name1;
    fileName.replace(".h5",".json");
    QFile jsonFile(fileName);
    if (!jsonFile.open(QFile::ReadOnly | QFile::Text)) {
        QString message = QString("Error: could not open file") + fileName;
        this->errorMessage(message);
        return 1;
    }
    // place contents of file into json object
    QString val;
    val=jsonFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObject = doc.object();
    // close file
    jsonFile.close();
    // check file contains valid object
    if (jsonObject.isEmpty()) {
        this->errorMessage("ERROR: file either empty or malformed JSON "+fileName);
        return 1;
    }
    this->statusMessage("Pretrained model JSON file loaded.");

    // create RV
    QJsonArray xLabels;
    if (jsonObject.contains("xlabels")) {
        xLabels = jsonObject["xlabels"].toArray();
    } else {
        this->errorMessage("ERROR: xlables are missing in "+fileName);
        return 1;
    }
    int numberOfColumns = xLabels.size();
    QStringList varNamesAndValues;
    for (int i=0;i<numberOfColumns;i++) {
        varNamesAndValues.append(xLabels[i].toString());
        varNamesAndValues.append("nan");
    }
    //theParameters->setGPVarNamesAndValues(varNamesAndValues);
    numSamples=0;
    this->statusMessage("RV created.");

    // create QoI
    QJsonArray yLabels;
    if (jsonObject.contains("ylabels")) {
        yLabels = jsonObject["ylabels"].toArray();
    } else {
        this->errorMessage("ERROR: ylables are missing in "+fileName);
        return 1;
    }
    numberOfColumns = yLabels.size();
    QStringList qoiNames;
    for (int i=0;i<numberOfColumns;i++) {
        qoiNames.append(yLabels[i].toString());
    }
    //theEdpWidget->setGPQoINames(qoiNames);
    this->statusMessage("QoI created.");

    // check inpFile
    QFileInfo fileInfo(fileName);
    QString path = fileInfo.absolutePath();
    QFile inpFile(path+QDir::separator()+"inpFile.in");
    if (!inpFile.open(QFile::ReadOnly | QFile::Text)) {
        QString message = QString("Error: could not open file") + inpFile.fileName();
        this->errorMessage(message);
        return 1;
    }
    inpFile.close();
    inpFileDir->setText(inpFile.fileName());

    // check outFile
    QFile outFile(path+QDir::separator()+"outFile.in");
    if (!outFile.open(QFile::ReadOnly | QFile::Text)) {
        QString message = QString("Error: could not open file") + outFile.fileName();
        this->errorMessage(message);
        return 1;
    }
    outFile.close();
    outFileDir->setText(outFile.fileName());
    this->statusMessage("Input data loaded.");

    // return
    return 0;
}

int PLoMInputWidget::countColumn(QString name1){
    // get number of columns
    std::ifstream inFile(name1.toStdString());
    // read lines of input searching for pset using regular expression
    std::string line;
    errMSG->hide();

    int numberOfColumns_pre = -100;
    while (getline(inFile, line)) {
        int  numberOfColumns=1;
        bool previousWasSpace=false;
        //for(int i=0; i<line.size(); i++){
        for(size_t i=0; i<line.size(); i++){
            if(line[i] == '%' || line[i] == '#'){ // ignore header
                numberOfColumns = numberOfColumns_pre;
                break;
            }
            if(line[i] == ' ' || line[i] == '\t' || line[i] == ','){
                if(!previousWasSpace)
                    numberOfColumns++;
                previousWasSpace = true;
            } else {
                previousWasSpace = false;
            }
        }
        if(previousWasSpace)// when there is a blank space at the end of each row
            numberOfColumns--;

        if (numberOfColumns_pre==-100)  // to pass header
        {
            numberOfColumns_pre=numberOfColumns;
            continue;
        }
        if (numberOfColumns != numberOfColumns_pre)// Send an error
        {
            errMSG->show();
            numberOfColumns_pre=0;
            break;
        }
    }
    // close file
    inFile.close();
    return numberOfColumns_pre;
}

bool
PLoMInputWidget::inputFromJSON(QJsonObject &jsonObject){

    bool result = false;
    preTrained = false;
    if (jsonObject.contains("preTrained")) {
        preTrained = jsonObject["preTrained"].toBool();
        result = true;
    } else {
        return false;
    }
    if (preTrained) {
        if (jsonObject.contains("inpFile2")) {
            QString fileDir=jsonObject["inpFile2"].toString();
            inpFileDir2->setText(fileDir);
            result = true;
        } else {
            return false;
        }

    } else {
        if (jsonObject.contains("inpFile")) {
            QString fileDir=jsonObject["inpFile"].toString();
            inpFileDir->setText(fileDir);
            result = true;
        } else {
            return false;
        }

        if (jsonObject.contains("outputData")) {
          if (jsonObject["outputData"].toBool()) {
              QString fileDir=jsonObject["outFile"].toString();
              outFileDir->setText(fileDir);
              //theFemWidget->setFEMforGP("GPdata");
          }
          result = true;
        } else {
          return false;
        }
    }


    if (jsonObject.contains("newSampleRatio")) {
        int samples=jsonObject["newSampleRatio"].toInt();
        ratioNewSamples->setText(QString::number(samples));
    } else {
        result = false;
    }

  if (jsonObject.contains("advancedOpt")) {
      theAdvancedCheckBox->setChecked(jsonObject["advancedOpt"].toBool());
      if (jsonObject["advancedOpt"].toBool()) {
        theAdvancedCheckBox->setChecked(true);
        theLogtCheckBox->setChecked(jsonObject["logTransform"].toBool());
        theDMCheckBox->setChecked(jsonObject["diffusionMaps"].toBool());
        randomSeed->setText(QString::number(jsonObject["randomSeed"].toInt()));
        smootherKDE->setText(QString::number(jsonObject["smootherKDE"].toDouble()));
        tolKDE->setText(QString::number(jsonObject["kdeTolerance"].toDouble()));
        epsilonPCA->setText(QString::number(jsonObject["epsilonPCA"].toDouble()));
        theConstraintsButton->setChecked(jsonObject["constraints"].toBool());
        if (jsonObject["constraints"].toBool()) {
            constraintsPath->setText(jsonObject["constraintsFile"].toString());
            numIter->setText(QString::number(jsonObject["numIter"].toInt()));
            tolIter->setText(QString::number(jsonObject["tolIter"].toDouble()));
        }
      }
     result = true;
  } else {
     return false;
  }

  return result;
}

bool
PLoMInputWidget::copyFiles(QString &fileDir) {
    if (preTrained) {
        qDebug() << inpFileDir2->text();
        qDebug() << fileDir + QDir::separator() + "surrogatePLoM.h5";
        QFile::copy(inpFileDir2->text(), fileDir + QDir::separator() + "surrogatePLoM.h5");
        qDebug() << inpFileDir2->text().replace(".h5",".json");
        qDebug() << fileDir + QDir::separator() + "surrogatePLoM.json";
        QFile::copy(inpFileDir2->text().replace(".h5",".json"), fileDir + QDir::separator() + "surrogatePLoM.json");
        QFile::copy(inpFileDir->text(), fileDir + QDir::separator() + "inpFile.in");
        QFile::copy(outFileDir->text(), fileDir + QDir::separator() + "outFile.in");
    } else {
        QFile::copy(inpFileDir->text(), fileDir + QDir::separator() + "inpFile.in");
        QFile::copy(outFileDir->text(), fileDir + QDir::separator() + "outFile.in");
    }
    if (theConstraintsButton->isChecked()) {
        QFile::copy(constraintsPath->text(), fileDir + QDir::separator() + "plomConstraints.py");
    }
    return true;
}

void
PLoMInputWidget::clear(void)
{

}

int
PLoMInputWidget::getNumberTasks()
{
  return numSamples;
}
