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

#include <PLoMSimuWidget.h>
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
#include <MonteCarloInputWidget.h>
#include <LatinHypercubeInputWidget.h>
//#include <ImportanceSamplingInputWidget.h>
#include <GaussianProcessInputWidget.h>
#include <PCEInputWidget.h>
#include <MultiFidelityMonteCarlo.h>
#include <SimCenterIntensityMeasureWidget.h>
#include <QMessageBox>
#include<QDebug>

PLoMSimuWidget::PLoMSimuWidget(QWidget *parent)
    : UQ_Method(parent)
{
    auto layout = new QVBoxLayout();
    int wid = 0; // widget id

    //First we need to add type radio buttons
    m_typeButtonsGroup = new QButtonGroup(this);
    QRadioButton* rawDataRadioButton = new QRadioButton(tr("Raw Data"));
    rawDataRadioButton->hide();
    m_typeButtonsGroup->addButton(rawDataRadioButton, 0);
    QWidget* typeGroupBox = new QWidget(this);
    typeGroupBox->setContentsMargins(0,0,0,0);
    typeGroupBox->setStyleSheet("QGroupBox { font-weight: normal;}");
    QHBoxLayout* typeLayout = new QHBoxLayout(typeGroupBox);
    typeGroupBox->setLayout(typeLayout);
    typeLayout->addWidget(rawDataRadioButton);
    typeLayout->addStretch();
    layout->addWidget(typeGroupBox,wid++);
    layout->setSpacing(0);
    //    layout->setMargin(0);
    // sampling method
    samplingMethodGroup = new QWidget(this);
    QHBoxLayout *methodLayout= new QHBoxLayout;
    samplingMethodGroup->setLayout(methodLayout);
    QLabel *label1 = new QLabel();
    label1->setText(QString("Method"));
    samplingMethod = new QComboBox();
    samplingMethod->setMaximumWidth(200);
    samplingMethod->setMinimumWidth(200);
    samplingMethod->addItem(tr("LHS"));
    samplingMethod->addItem(tr("Monte Carlo"));
    //samplingMethod->addItem(tr("Importance Sampling"));
    //samplingMethod->addItem(tr("Gaussian Process Regression"));
    //samplingMethod->addItem(tr("Polynomial Chaos Expansion"));
    // samplingMethod->addItem(tr("Multi Fidelity Monte Carlo"));

    methodLayout->addWidget(label1);
    methodLayout->addWidget(samplingMethod);
    methodLayout->addStretch(1);
    layout->addWidget(samplingMethodGroup,wid++);

    samplingStackedWidget = new QStackedWidget();
    samplingStackedWidget->setMaximumWidth(800);
    theLHS = new LatinHypercubeInputWidget();
    samplingStackedWidget->addWidget(theLHS);
    theMC = new MonteCarloInputWidget();
    samplingStackedWidget->addWidget(theMC);
    //theIS = new ImportanceSamplingInputWidget();
    //theStackedWidget->addWidget(theIS);
    //theGP = new GaussianProcessInputWidget();
    //samplingStackedWidget->addWidget(theGP);
    //thePCE = new PCEInputWidget();
    //samplingStackedWidget->addWidget(thePCE);
    //theMFMC = new MultiFidelityMonteCarlo();
    //samplingStackedWidget->addWidget(theMFMC);
    // set current widget to index 0
    theCurrentMethod = theLHS;
    //    layout->addWidget(samplingStackedWidget,wid++,0);
    layout->addWidget(samplingStackedWidget, 1, Qt::AlignLeft);
    connect(samplingMethod, SIGNAL(currentTextChanged(QString)), this, SLOT(onTextChanged(QString)));

    // input data widget
    rawDataGroup = new QWidget(this);
    QGridLayout* rawDataLayout = new QGridLayout(rawDataGroup);
    rawDataGroup->setLayout(rawDataLayout);
    rawDataGroup->hide();
    // Create Input LineEdit
    inpFileDir = new QLineEdit();
    QPushButton *chooseInpFile = new QPushButton("Choose");
    connect(chooseInpFile, &QPushButton::clicked, this, [=](){
        inpFileDir->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)"));
        this->parseInputDataForRV(inpFileDir->text());
    });
    inpFileDir->setReadOnly(true);
    rawDataLayout->addWidget(new QLabel("Training Data File: Input"),0,0);
    rawDataLayout->addWidget(inpFileDir,0,1,1,3);
    rawDataLayout->addWidget(chooseInpFile,0,4);
    // Create Output LineEdit
    outFileDir = new QLineEdit();
    chooseOutFile = new QPushButton("Choose");
    connect(chooseOutFile, &QPushButton::clicked, this, [=](){
        outFileDir->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)"));
        this->parseOutputDataForQoI(outFileDir->text());
    });
    outFileDir->setReadOnly(true);
    rawDataLayout->addWidget(new QLabel("Training Data File: Output"),1,0,Qt::AlignTop);
    rawDataLayout->addWidget(outFileDir,1,1,1,3,Qt::AlignTop);
    rawDataLayout->addWidget(chooseOutFile,1,4,Qt::AlignTop);
    errMSG=new QLabel("Unrecognized file format");
    errMSG->setStyleSheet({"color: red"});
    rawDataLayout->addWidget(errMSG,2,1,Qt::AlignLeft);
    errMSG->hide();

    //We will add stacked widget to switch between grid and single location
    m_stackedWidgets = new QStackedWidget(this);
    m_stackedWidgets->hide();
    m_stackedWidgets->addWidget(rawDataGroup);
    m_typeButtonsGroup->button(0)->setChecked(true);
    m_stackedWidgets->setCurrentIndex(0);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(m_typeButtonsGroup, &QButtonGroup::idReleased, this, [this](int id)
    {
        if(id == 0) {
            m_typeButtonsGroup->button(0)->setChecked(true);
            m_stackedWidgets->setCurrentIndex(0);
            preTrained = false;
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
    });
#endif

    layout->addWidget(m_stackedWidgets,wid++);

    // create widget for new sample ratio
    newSampleRatioWidget = new QWidget();
    newSampleRatioWidget->setMaximumWidth(300);
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
    theAdvancedComboBox->addItem(tr("Affiliate Variables"));
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
    advComboWidget->setContentsMargins(0,0,0,0);

    // adv. opt. general widget
    advGeneralWidget = new QWidget();
    advGeneralWidget->setMaximumWidth(800);
    advGeneralWidget->setMaximumHeight(150);
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
    advKDEWidget->setMaximumHeight(150);
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
    advConstraintsWidget->setMaximumHeight(150);
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

    // Affiliate variable widget
    advAffiliateVariableWidget = new QWidget();
    QGridLayout* advAffiliateVariableLayout = new QGridLayout(advAffiliateVariableWidget);
    //advAffiliateVariableLayout->setMargin(0);
    advAffiliateVariableWidget->setLayout(advAffiliateVariableLayout);
    // combo box
    theAffiliateVariableComboBox = new QComboBox();
    theAffiliateVariableComboBox->addItem(tr("None"));
    theAffiliateVariableComboBox->addItem(tr("User Defined"));
    theAffiliateVariableComboBox->addItem(tr("Ground Motion Intensity"));
    theAffiliateVariableLabel = new QLabel(tr("Type"));
    advAffiliateVariableLayout->addWidget(theAffiliateVariableLabel,0,0);
    advAffiliateVariableLayout->addWidget(theAffiliateVariableComboBox,0,1);
    // empty widget
    emptyVariableWidget = new QWidget();
    // ground motion intensity measure widget
    theSCIMWidget = new SimCenterIntensityMeasureWidget();
    // user-defined script
    userVariableWidget = new QWidget();
    userVariableWidget->setMaximumWidth(800);
    userVariableWidget->setMaximumHeight(100);
    QGridLayout *userVariableLayout = new QGridLayout();
    userVariableWidget->setLayout(userVariableLayout);
    userVariableLabel = new QLabel(tr("User Defined Script"));
    userVariabelLine = new QLineEdit();
    chooseUserVar = new QPushButton(tr("Choose"));
    userVariableLabel->setVisible(false);
    userVariabelLine->setVisible(false);
    chooseUserVar->setVisible(false);
    connect(chooseUserVar, &QPushButton::clicked, this, [=](){
        userVariabelLine->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.py)"));
    });
    userVariableLayout->addWidget(userVariableLabel,0,0);
    userVariableLayout->addWidget(userVariabelLine,0,1);
    userVariableLayout->addWidget(chooseUserVar,0,2);
    //
    aff_stackedWidgets = new QStackedWidget(this);
    aff_stackedWidgets->addWidget(emptyVariableWidget);
    aff_stackedWidgets->addWidget(userVariableWidget);
    aff_stackedWidgets->addWidget(theSCIMWidget);
    aff_stackedWidgets->setCurrentIndex(0);
    connect(theAffiliateVariableComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int id)
    {
        aff_stackedWidgets->setCurrentIndex(id);
    });
    advAffiliateVariableLayout->addWidget(aff_stackedWidgets,1,0,1,9);
    //
    advComboWidget->addTab(advAffiliateVariableWidget, "User-Defined Variables");

    // create the stacked widgets
    /***
    adv_stackedWidgets = new QStackedWidget(this);
    adv_stackedWidgets->addWidget(advGeneralWidget);
    adv_stackedWidgets->addWidget(advKDEWidget);
    adv_stackedWidgets->addWidget(advConstraintsWidget);
    adv_stackedWidgets->addWidget(advAffiliateVariableWidget);
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
    connect(this, SIGNAL(eventTypeChanged(QString)), this, SLOT(onEventTypeChanged(QString)));
}


PLoMSimuWidget::~PLoMSimuWidget()
{

}


// SLOT function
void PLoMSimuWidget::doAdvancedSetup(bool tog)
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
    theAffiliateVariableComboBox->setVisible(tog);
    theAffiliateVariableLabel->setVisible(tog);
    userVariableLabel->setVisible(tog);
    userVariabelLine->setVisible(tog);
    chooseUserVar->setVisible(tog);

}


void PLoMSimuWidget::setOutputDir(bool tog)
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

void PLoMSimuWidget::setConstraints(bool tog)
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

void PLoMSimuWidget::setDiffMaps(bool tog)
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
PLoMSimuWidget::outputToJSON(QJsonObject &jsonObj){

    bool result = true;

    if (m_typeButtonsGroup->button(0)->isChecked()) {
        jsonObj["preTrained"] = false;
        //jsonObj["inpFile"]=inpFileDir->text();
        //jsonObj["outFile"]=outFileDir->text();
        jsonObj["inpFile"] = "PLoM_variables.csv";
        jsonObj["outFile"] = "PLoM_responses.csv";
    } else {
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

    // sampling methods
    QJsonObject samplingObj;
    theCurrentMethod->outputToJSON(samplingObj);
    samplingObj["method"] = samplingMethod->currentText();
    jsonObj["samplingMethod"] = samplingObj;

    if (aff_stackedWidgets->currentIndex()==2) {
        QJsonObject imJson;
        result = theSCIMWidget->outputToJSON(imJson);
        jsonObj["IntensityMeasure"] = imJson;
    }

    return result;    
}


int PLoMSimuWidget::parseInputDataForRV(QString name1){

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

int PLoMSimuWidget::parseOutputDataForQoI(QString name1){
    // get number of columns
    double numberOfColumns=countColumn(name1);
    QStringList qoiNames;
    for (int i=0;i<numberOfColumns;i++) {
        qoiNames.append(QString("QoI_column%1").arg(i+1));
    }
    //theEdpWidget->setGPQoINames(qoiNames);
    return 0;
}


int PLoMSimuWidget::countColumn(QString name1){
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
PLoMSimuWidget::inputFromJSON(QJsonObject &jsonObject){

    bool result = false;
    preTrained = false;
    if (jsonObject.contains("preTrained")) {
        preTrained = jsonObject["preTrained"].toBool();
        result = true;
    } else {
        return false;
    }
    if (preTrained) {
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
      // sampling method
      QJsonObject samplingObj = jsonObject["samplingMethod"].toObject();
      if (samplingObj.contains("method")) {
          QString method = samplingObj["method"].toString();
          int index = samplingMethod->findText(method);
          if (index == -1) {
              return false;
          }
          samplingMethod->setCurrentIndex(index);
          result = theCurrentMethod->inputFromJSON(samplingObj);
          if (result == false)
              return result;
      }
      // intensity measure
      qDebug() << "Start loading intensity measure";
      if (jsonObject.contains("IntensityMeasure")) {
          this->SetComboBoxItemEnabled(theAffiliateVariableComboBox, 2, true);
          aff_stackedWidgets->setCurrentIndex(2);
          theAffiliateVariableComboBox->setCurrentIndex(2);
          qDebug() << "Start loading intensity measure";
          result = theSCIMWidget->inputFromJSON(jsonObject);
      }
     result = true;
  } else {
     return false;
  }

  return result;
}

bool
PLoMSimuWidget::copyFiles(QString &fileDir) {
    if (preTrained) {
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
PLoMSimuWidget::clear(void)
{

}

int
PLoMSimuWidget::getNumberTasks()
{
  return numSamples;
}

void PLoMSimuWidget::onTextChanged(const QString &text)
{
  if (text=="LHS") {
    samplingStackedWidget->setCurrentIndex(0);
    theCurrentMethod = theLHS;
  }
  else if (text=="Monte Carlo") {
    samplingStackedWidget->setCurrentIndex(1);
    theCurrentMethod = theMC;
  }
  //else if (text=="Importance Sampling") {
  //  theStackedWidget->setCurrentIndex(2);
  //  theCurrentMethod = theIS;
  //}
  else if (text=="Gaussian Process Regression") {
    samplingStackedWidget->setCurrentIndex(2);
    theCurrentMethod = theGP;
  }
  else if (text=="Polynomial Chaos Expansion") {
    samplingStackedWidget->setCurrentIndex(3);
    theCurrentMethod = thePCE;
  }
  else if (text=="Multi Fidelity Monte Carlo") {
    samplingStackedWidget->setCurrentIndex(4);
    theCurrentMethod = theMFMC;
  }
}

void
PLoMSimuWidget::setEventType(QString type) {
    typeEVT = type;
    emit eventTypeChanged(typeEVT);
}

void
PLoMSimuWidget::onEventTypeChanged(QString typeEVT) {
    if (typeEVT.compare("EQ") ==0 ) {
        // an earthquake event type
        this->SetComboBoxItemEnabled(theAffiliateVariableComboBox, 2, true);
    } else {
        // not an earthquake event, inactivate ground motion intensity widget
        this->SetComboBoxItemEnabled(theAffiliateVariableComboBox, 2, false);
    }
}

void PLoMSimuWidget::SetComboBoxItemEnabled(QComboBox * comboBox, int index, bool enabled)
{
    auto * model = qobject_cast<QStandardItemModel*>(comboBox->model());
    assert(model);
    if(!model) return;

    auto * item = model->item(index);
    assert(item);
    if(!item) return;
    item->setEnabled(enabled);
}
