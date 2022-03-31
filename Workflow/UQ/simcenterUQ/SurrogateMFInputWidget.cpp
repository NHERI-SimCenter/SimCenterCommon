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

// Written: fmckenna
#include <RandomVariablesContainer.h>
#include <SurrogateMFInputWidget.h>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>
#include <QPushButton>
#include <QFileDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <iostream>
#include <fstream>
#include <regex>
#include <iterator>
#include <string>
#include <sstream>
//#include <InputWidgetParameters.h>
//#include <InputWidgetEDP.h>
//#include <InputWidgetFEM.h>

SurrogateMFInputWidget::SurrogateMFInputWidget(QWidget *parent)
: UQ_Method(parent)
{
    auto layout = new QGridLayout();

    theHighSimButton = new QCheckBox("Simulation model");
    theHighSimButton -> setStyleSheet("font-weight: bold; color: grey");

    theLowSimButton = new QCheckBox("Simulation model");
    theLowSimButton -> setStyleSheet("font-weight: bold; color: grey");


    // /////////////////////////////////////
    // HIGH FIDELITY
    // /////////////////////////////////////

    // create a high fidelity box
    QGroupBox *theHFbox = new QGroupBox("High-fidelity dataset");
    QGridLayout *theHFlayout = new QGridLayout();
    theHFbox->setLayout(theHFlayout);
    layout->addWidget(theHFbox,0,0,1,-1);

    //
    // DATA
    //

    // Input

    int id_hf = 0;
    inpFileDir_HF = new QLineEdit();
    chooseInpFile_HF = new QPushButton("Choose");
    connect(chooseInpFile_HF, &QPushButton::clicked, this, [=](){
        QString filename = QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)");
        if(!filename.isEmpty()) {
            inpFileDir_HF->setText(filename);
            if ((!theHighSimButton->isChecked()) && (!theLowSimButton->isChecked()) )
                this->parseInputDataForRV(inpFileDir_HF->text());
            else
                this->countColumn(inpFileDir_HF->text()); // to give error
        }

    });
    inpFileDir_HF->setMinimumWidth(600);
    inpFileDir_HF->setReadOnly(true);
    theHFlayout->addWidget(new QLabel("Training Points (Input) File"),id_hf,0);
    theHFlayout->addWidget(inpFileDir_HF,id_hf,1,1,3);
    theHFlayout->addWidget(chooseInpFile_HF,id_hf++,4);

    // Output

    outFileDir_HF = new QLineEdit();
    chooseOutFile_HF = new QPushButton("Choose");
    connect(chooseOutFile_HF, &QPushButton::clicked, this, [=](){
        QString filename = QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)");
        if(!filename.isEmpty()) {
            outFileDir_HF->setText(filename);
            if ((!theHighSimButton->isChecked()) && (!theLowSimButton->isChecked()) )
                this->parseOutputDataForQoI(outFileDir_HF->text());
            else
                this->countColumn(outFileDir_HF->text(),true); // to give error
        }
    });
    outFileDir_HF->setMinimumWidth(600);
    outFileDir_HF->setReadOnly(true);
    theHFlayout->addWidget(new QLabel("System Results (Output) File     "),id_hf,0,Qt::AlignTop);
    theHFlayout->addWidget(outFileDir_HF,id_hf,1,1,3,Qt::AlignTop);
    theHFlayout->addWidget(chooseOutFile_HF,id_hf++,4,Qt::AlignTop);

    //
    // Model
    //

    line0= new QFrame;
    line0->setFrameShape(QFrame::HLine);
    line0->setFrameShadow(QFrame::Sunken);

    theHighSimOptions = new QWidget();
    QGridLayout *theHighSimGrid = new QGridLayout(theHighSimOptions);
    theHighSimGrid->setMargin(0);

    theHFlayout->addWidget(theHighSimButton,id_hf,1);
    theHFlayout->addWidget(theHighSimOptions,id_hf++,2,3,-1);
    theHFlayout->addWidget(line0, id_hf++, 1);


    theHFlayout->setRowStretch(4, 1);
    theHFlayout->setColumnStretch(5, 1);
    line0->hide();

    // FEM model

    int id_fe=0;
    QLabel * theFEMLabelHF = new QLabel("   Please specify the model at the FEM Tab");
    theHighSimGrid->addWidget(theFEMLabelHF, id_fe++, 0,1,2);
    theFEMLabelHF->setStyleSheet("font-style: italic");

    // Create layout label and entry for # samples

    numSamples_HF = new QLineEdit();
    numSamples_HF->setText(tr("30"));
    numSamples_HF->setValidator(new QIntValidator);
    numSamples_HF->setToolTip("Specify the number of samples");
    numSamples_HF->setMaximumWidth(150);

    theHighSimGrid->addWidget(new QLabel("   Max Number of Model Runs"), id_fe, 0);
    theHighSimGrid->addWidget(numSamples_HF, id_fe++, 1);


    // Existing Dataset

    theExistingCheckBox_HF = new QCheckBox();
    theHighSimGrid->addWidget(new QLabel("   Start with Existing Dataset"), id_fe, 0);
    theHighSimGrid->addWidget(theExistingCheckBox_HF, id_fe++, 1);


    theHighSimGrid->setRowStretch(id_fe, 1);
    theHighSimGrid->setColumnStretch(2, 1);
    theHighSimOptions->hide();

    // /////////////////////////////////////
    // LOW FIDELITY - DATA
    // /////////////////////////////////////

    QGroupBox *theLFbox = new QGroupBox("Low-fidelity dataset");
    QGridLayout *theLFlayout = new QGridLayout();
    theLFbox->setLayout(theLFlayout);
    layout->addWidget(theLFbox,1,0,1,-1);

    // Input

    int id = 0;

    inpFileDir_LF = new QLineEdit();
    chooseInpFile_LF = new QPushButton("Choose");
    connect(chooseInpFile_LF, &QPushButton::clicked, this, [=](){
        QString filename = QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)");
        if(!filename.isEmpty()) {
            inpFileDir_LF->setText(filename);
            if ((!theHighSimButton->isChecked()) && (!theLowSimButton->isChecked()) )
                this->parseInputDataForRV(inpFileDir_LF->text());
            else
                this->countColumn(inpFileDir_LF->text()); // to give error
            }
    });
    inpFileDir_LF->setMinimumWidth(600);
    inpFileDir_LF->setReadOnly(true);
    theLFlayout->addWidget(new QLabel("Training Points (Input) File"),id,0);
    theLFlayout->addWidget(inpFileDir_LF,id,1,1,3);
    theLFlayout->addWidget(chooseInpFile_LF,id++,4);

    outFileDir_LF = new QLineEdit();
    chooseOutFile_LF = new QPushButton("Choose");
    connect(chooseOutFile_LF, &QPushButton::clicked, this, [=](){
        QString filename = QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)");
        if(!filename.isEmpty()) {
            outFileDir_LF->setText(filename);
            if ((!theHighSimButton->isChecked()) && (!theLowSimButton->isChecked()) )
                this->parseOutputDataForQoI(outFileDir_LF->text());
            else
                this->countColumn(outFileDir_LF->text(),true); // to give error
     }
    });
    outFileDir_LF->setMinimumWidth(600);
    outFileDir_LF->setReadOnly(true);
    theLFlayout->addWidget(new QLabel("System Results (Output) File     "),id,0,Qt::AlignTop);
    theLFlayout->addWidget(outFileDir_LF,id,1,1,3,Qt::AlignTop);
    theLFlayout->addWidget(chooseOutFile_LF,id++,4,Qt::AlignTop);

    // /////////////////////////////////////
    // LOW FIDELITY - Model
    // /////////////////////////////////////

    line1= new QFrame;
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);

    theLowSimOptions = new QWidget();
    QGridLayout *theLowSimGrid = new QGridLayout(theLowSimOptions);
    theLowSimGrid->setMargin(0);

    theLFlayout->addWidget(theLowSimButton,id,1);
    theLFlayout->addWidget(theLowSimOptions,id++,2,3,-1);
    theLFlayout->addWidget(line1, id++, 1);

    line1->hide();

    theLFlayout->setRowStretch(4, 1);
    theLFlayout->setColumnStretch(5, 1);

    // FEM model

    id_fe=0;
    QLabel * theFEMLabel_LF = new QLabel("   Please specify the model at the FEM Tab");
    theLowSimGrid->addWidget(theFEMLabel_LF, id_fe++, 0,1,2);
    theFEMLabel_LF->setStyleSheet("font-style: italic");


    numSamples_LF = new QLineEdit();
    numSamples_LF->setText(tr("150"));
    numSamples_LF->setValidator(new QIntValidator);
    numSamples_LF->setToolTip("Specify the number of samples");
    numSamples_LF->setMaximumWidth(150);

    theLowSimGrid->addWidget(new QLabel("   Max Number of Model Runs"), id_fe, 0);
    theLowSimGrid->addWidget(numSamples_LF, id_fe++, 1);


    // Existing Dataset

    theExistingCheckBox_LF = new QCheckBox();
    theLowSimGrid->addWidget(new QLabel("   Start with Existing Dataset"), id_fe, 0);
    theLowSimGrid->addWidget(theExistingCheckBox_LF, id_fe++, 1);


    theLowSimGrid->setRowStretch(id_fe, 1);
    theLowSimGrid->setColumnStretch(2, 1);
    theLowSimOptions->hide();

    //
    // Errors
    //

    ignore_fem_message=new QLabel("Any information entered on the FEM tab will be ignored");
    ignore_fem_message->setStyleSheet({"color: blue"});
    layout->addWidget(ignore_fem_message,2,0,Qt::AlignLeft);


    errMSG=new QLabel("");
    errMSG->setStyleSheet({"color: red"});
    layout->addWidget(errMSG,3,0,Qt::AlignLeft);



    // /////////////////////////////////////
    // SIMULATION OPTIONS
    // /////////////////////////////////////

    theSimBox = new QGroupBox("Simulation Options");
    QGridLayout *theSimGrid = new QGridLayout();
    theSimBox->setLayout(theSimGrid);
    layout->addWidget(theSimBox,4,0,1,-1);
    theSimBox->setVisible(false);

    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;
    randomSeed = new QLineEdit();
    randomSeed->setText(QString::number(randomNumber));
    randomSeed->setValidator(new QIntValidator);
    randomSeed->setToolTip("Set the seed");
    randomSeed->setMaximumWidth(150);

    theSimGrid->addWidget(new QLabel("   Random Seed"), id_fe, 0);
    theSimGrid->addWidget(randomSeed, id_fe++, 1);

//    create convergence criteria

    accuracyMeasure = new QLineEdit();
    accuracyMeasure->setText(tr("0.02"));
    accuracyMeasure->setValidator(new QDoubleValidator);
    accuracyMeasure->setToolTip("NRMSE: normalized root mean square error");
    accuracyMeasure->setMaximumWidth(150);

    theSimGrid->addWidget(new QLabel("   Target Accuracy (normalized error)   "), id_fe, 0);
    theSimGrid->addWidget(accuracyMeasure, id_fe++, 1);

//    Max computation time (approximate)

    timeMeasure = new QLineEdit();
    timeMeasure->setText(tr("60"));
    timeMeasure->setValidator(new QIntValidator);
    timeMeasure->setToolTip("Max Computation Time (minutes)");
    timeMeasure->setMaximumWidth(150);

    theSimGrid->addWidget(new QLabel("   Max Computation Time (minutes)    "), id_fe, 0);
    theSimGrid->addWidget(timeMeasure, id_fe++, 1);

    // Do DoE

    //theDoELabel=new QLabel("DoE Options");
    theDoESelection = new QComboBox();

//    theDoESelection->addItem(tr("Pareto"),0);
//    theDoESelection->addItem(tr("IMSEw"),1);
//    theDoESelection->addItem(tr("MMSEw"),2);
    theDoESelection->addItem(tr("None"),0);
    theDoESelection->setMaximumWidth(150);
    theDoESelection->setCurrentIndex(0);

    //theDoECheckBox = new QCheckBox();
    //theSimGrid->addWidget(new QLabel("   Do Adaptive Design of Experiments"), id_fe, 0);

    theSimGrid->addWidget(new QLabel("   DoE Options"), id_fe, 0);
    theSimGrid->addWidget(theDoESelection, id_fe++, 1);

    theSimGrid->setColumnStretch(2,1);
    theSimGrid->setRowStretch(4,1);

    //
    // Parallel Execution
    //

    parallelCheckBox = new QCheckBox();
    parallelCheckBox -> setChecked(true);
    theSimGrid->addWidget(new QLabel("   Parallel Execution"), id_fe, 0);
    theSimGrid->addWidget(parallelCheckBox, id_fe++, 1);

    // /////////////////////////////////////
    // ADVANCED
    // /////////////////////////////////////

    theAdvancedCheckBox = new QCheckBox("Advanced Options for Gaussian Process Model");
    theAdvancedCheckBox->setStyleSheet("font-weight: bold; color: gray");
    layout->addWidget(theAdvancedCheckBox, 5, 0,Qt::AlignBottom);

    lineA = new QFrame;
    lineA->setFrameShape(QFrame::HLine);
    lineA->setFrameShadow(QFrame::Sunken);
    lineA->setMaximumWidth(420);
    layout->addWidget(lineA, 6, 0, 1, 3);
    lineA->setVisible(false);

    // Selection of GP kernel

    theKernelLabel=new QLabel("Kernel Function");

    gpKernel = new QComboBox();
    gpKernel->addItem(tr("Matern 5/2"));
    gpKernel->addItem(tr("Matern 3/2"));
    gpKernel->addItem(tr("Radial Basis"));
    gpKernel->addItem(tr("Exponential"));
    gpKernel->setMaximumWidth(150);
    layout->addWidget(theKernelLabel, 7, 0);
    layout->addWidget(gpKernel, 7, 1);
    gpKernel->setCurrentIndex(0);
    theKernelLabel->setVisible(false);
    gpKernel->setVisible(false);

    // Use Linear trending function

    theLinearLabel=new QLabel("Add Linear Trend Function");

    theLinearCheckBox = new QCheckBox();
    layout->addWidget(theLinearLabel, 8, 0);
    layout->addWidget(theLinearCheckBox, 8, 1);
    //theLinearLabel->setStyleSheet("color: gray");
    //theLinearCheckBox->setDisabled(1);
    theLinearLabel->setVisible(false);
    theLinearCheckBox->setVisible(false);

    // Use Log transform

    theLogtLabel=new QLabel("Log-space Transform of QoI");
    theLogtLabel2=new QLabel("     (check this box only when all responses are always positive)");

    theLogtCheckBox = new QCheckBox();
    layout->addWidget(theLogtLabel, 9, 0);
    layout->addWidget(theLogtLabel2, 9, 1);
    layout->addWidget(theLogtCheckBox, 9, 1);
    theLogtLabel->setVisible(false);
    theLogtLabel2->setVisible(false);
    theLogtCheckBox->setVisible(false);


    //
    // Nugget function
    //

    theNuggetLabel=new QLabel("Nugget Values for each QoI");
    theNuggetSelection = new QComboBox();

    theNuggetSelection->addItem(tr("Optimize"),0);
    theNuggetSelection->addItem(tr("Fixed Values"),1);
    theNuggetSelection->addItem(tr("Fixed Bounds"),2);
    theNuggetSelection->setMaximumWidth(150);
    theNuggetSelection->setCurrentIndex(0);

    theNuggetVals = new QLineEdit();
    theNuggetVals->setToolTip("Provide nugget values");
    //theNuggetVals->setMaximumWidth(150);

    layout->addWidget(theNuggetLabel, 10, 0);
    layout->addWidget(theNuggetSelection, 10, 1);
    layout->addWidget(theNuggetVals, 11, 1,1,2);

    theNuggetLabel->setVisible(false);
    theNuggetSelection->setVisible(false);
    theNuggetVals->setVisible(false);

    connect(theNuggetSelection,SIGNAL(currentIndexChanged(int)),this,SLOT(showNuggetBox(int)));

    //
    // Finish
    //

    layout->setRowStretch(11, 1);
    layout->setColumnStretch(8, 1);
    this->setLayout(layout);

    connect(theLowSimButton,SIGNAL(toggled(bool)),this,SLOT(setLowSim(bool)));
    connect(theHighSimButton,SIGNAL(toggled(bool)),this,SLOT(setHighSim(bool)));
    //connect(theLowDataButton,SIGNAL(toggled(bool)),this,SLOT(setHighSim(bool)));
    //connect(theHighDataButton,SIGNAL(toggled(bool)),this,SLOT(setHighData(bool)));

    connect(theAdvancedCheckBox,SIGNAL(toggled(bool)),this,SLOT(doAdvancedGP(bool)));
    connect(theExistingCheckBox_LF,SIGNAL(toggled(bool)),this,SLOT(doExistingLF(bool)));
    connect(theExistingCheckBox_HF,SIGNAL(toggled(bool)),this,SLOT(doExistingHF(bool)));
    theExistingCheckBox_LF->setChecked(true);
    theExistingCheckBox_HF->setChecked(true);

}


SurrogateMFInputWidget::~SurrogateMFInputWidget()
{

}


void
SurrogateMFInputWidget::showNuggetBox(int idx)
{

    if (idx == 0) {
        theNuggetVals->hide();
    } else if (idx==1){
        theNuggetVals->show();
        theNuggetVals->setPlaceholderText("QoI1, QoI2,..");
    } else if (idx==2) {
        theNuggetVals->show();
        theNuggetVals->setPlaceholderText("[QoI1_LB,QoI1_UB], [QoI2_LB,QoI2_UB],..");
    }

};


void SurrogateMFInputWidget::doExistingLF(bool tog)
{
    if (tog) {
        inpFileDir_LF->setDisabled(0);
        outFileDir_LF->setDisabled(0);
        chooseInpFile_LF->setDisabled(0);
        chooseInpFile_LF->setStyleSheet("color: white");
        chooseOutFile_LF->setDisabled(0);
        chooseOutFile_LF->setStyleSheet("color: white");
    } else {
        inpFileDir_LF->setDisabled(1);
        outFileDir_LF->setDisabled(1);
        chooseInpFile_LF->setDisabled(1);
        chooseInpFile_LF->setStyleSheet("background-color: lightgrey;border-color:grey");
        chooseOutFile_LF->setDisabled(1);
        chooseOutFile_LF->setStyleSheet("background-color: lightgrey;border-color:grey");
    }
}

void SurrogateMFInputWidget::doExistingHF(bool tog)
{
    if (tog) {
        inpFileDir_HF->setDisabled(0);
        outFileDir_HF->setDisabled(0);
        chooseInpFile_HF->setDisabled(0);
        chooseInpFile_HF->setStyleSheet("color: white");
        chooseOutFile_HF->setDisabled(0);
        chooseOutFile_HF->setStyleSheet("color: white");
    } else {
        inpFileDir_HF->setDisabled(1);
        outFileDir_HF->setDisabled(1);
        chooseInpFile_HF->setDisabled(1);
        chooseInpFile_HF->setStyleSheet("background-color: lightgrey;border-color:grey");
        chooseOutFile_HF->setDisabled(1);
        chooseOutFile_HF->setStyleSheet("background-color: lightgrey;border-color:grey");
    }
}

// SLOT function
void SurrogateMFInputWidget::doAdvancedGP(bool tog)
{
    if (tog) {
        theAdvancedCheckBox->setStyleSheet("font-weight: bold; color: black");
        theNuggetSelection->setCurrentIndex(0);
        theNuggetVals->hide();
    } else {
        theAdvancedCheckBox->setStyleSheet("font-weight: bold; color: gray");
        gpKernel->setCurrentIndex(0);
        theLinearCheckBox->setChecked(false);
        theLogtCheckBox->setChecked(false);
    }

    lineA->setVisible(tog);
    gpKernel->setVisible(tog);
    theLinearCheckBox->setVisible(tog);
    theLogtCheckBox->setVisible(tog);
    theLinearLabel->setVisible(tog);
    theLogtLabel->setVisible(tog);
    theLogtLabel2->setVisible(tog);
    theKernelLabel->setVisible(tog);
    theNuggetLabel->setVisible(tog);
    theNuggetSelection->setVisible(tog);
    //theNuggetVals->setVisible(tog);
}


void SurrogateMFInputWidget::setLowSim(bool tog)
{
    theExistingCheckBox_LF->setChecked(!tog);

    // Temporary
    if (theLowSimButton->isChecked() && theHighSimButton->isChecked()) {
        errorMessage("Currently, the option to use both high- and low-fidelity models is not supported");
        theLowSimButton->setChecked(false);
        return;
    }

    if (theLowSimButton->isChecked() && theHighSimButton->isChecked()) {
        errMSG->setText("");
        theSimBox->setVisible(true);
        ignore_fem_message->setText("");
    } else if (!theLowSimButton->isChecked() && !theHighSimButton->isChecked()) {
        theSimBox->setVisible(false);
        ignore_fem_message->setText("Any information entered on the FEM tab will be ignored");
    } else {
        errMSG->setText("");
        theSimBox->setVisible(true);
        ignore_fem_message->setText("");

    }

    if (theLowSimButton->isChecked()) {
        theLowSimOptions->show();
        line1->show();
        theLowSimButton -> setStyleSheet("font-weight: bold; color: black");
    } else {
        theExistingCheckBox_LF->setChecked(true);
        parseInputDataForRV(inpFileDir_LF->text());
        parseOutputDataForQoI(outFileDir_LF->text());
        theLowSimOptions->hide();
        line1->hide();
        theLowSimButton -> setStyleSheet("font-weight: bold; color: grey");
    }
    setRV_Defaults();
}
void SurrogateMFInputWidget::setHighSim(bool tog) {

    // Temporary
    if (theHighSimButton->isChecked() && theLowSimButton->isChecked()) {
        errorMessage("Currently, the option to use both high- and low-fidelity models is not supported");
        theHighSimButton->setChecked(false);
        return;
    }

    theExistingCheckBox_HF->setChecked(!tog);

    if (theLowSimButton->isChecked() && theHighSimButton->isChecked()) {
        errMSG->setText("");
        theSimBox->setVisible(true);
        ignore_fem_message->setText("");
    } else if (!theLowSimButton->isChecked() && !theHighSimButton->isChecked()) {
        //FMK theFemWidget->setFEMforGP("GPdata");
        theSimBox->setVisible(false);
        ignore_fem_message->setText("Any information entered on the FEM tab will be ignored");

    } else {
        errMSG->setText("");
        theSimBox->setVisible(true);
        ignore_fem_message->setText("");
    }

    if (theHighSimButton->isChecked()) {
        theHighSimOptions->show();
        line0->show();
        theHighSimButton -> setStyleSheet("font-weight: bold; color: black");
    } else {
        theExistingCheckBox_HF->setChecked(true);
        parseInputDataForRV(inpFileDir_HF->text());
        parseOutputDataForQoI(outFileDir_HF->text());
        theHighSimOptions->hide();
        line0->hide();
        theHighSimButton -> setStyleSheet("font-weight: bold; color: grey");
    }
    setRV_Defaults();
}

bool
SurrogateMFInputWidget::outputToJSON(QJsonObject &jsonObj){
    bool result = true;

    QJsonObject jsonHF;
    QJsonObject jsonLF;

    jsonHF["fromModel"]=theHighSimButton->isChecked();
    jsonLF["fromModel"]=theLowSimButton->isChecked();

    if (theHighSimButton->isChecked())
    {
        jsonHF["samples"]=numSamples_HF->text().toInt();
        jsonHF["existingDoE"]=theExistingCheckBox_HF->isChecked();
        if (theExistingCheckBox_HF->isChecked())
        {
            jsonHF["inpFile"]=inpFileDir_HF->text();
            jsonHF["outFile"]=outFileDir_HF->text();
        } else {
            jsonHF["inpFile"]="NA";
            jsonHF["outFile"]="NA";
        }
        jsonHF["seed"]=randomSeed->text().toInt();
        jsonHF["timeLimit"]=timeMeasure->text().toDouble();
        jsonHF["accuracyLimit"]=accuracyMeasure->text().toDouble();
        jsonHF["DoEmethod"]=theDoESelection->currentText();
        jsonHF["parallelExecution"]=parallelCheckBox->isChecked();
    } else {
        jsonHF["inpFile"]=inpFileDir_HF->text();
        jsonHF["outFile"]=outFileDir_HF->text();
        // dummy
        jsonHF["samples"]="NA";
        jsonHF["existingDoE"]=true;
        jsonHF["seed"]="NA";
        jsonHF["timeLimit"]="NA";
        jsonHF["accuracyLimit"]="NA";
        jsonHF["DoEmethod"]="NA";
        jsonHF["parallelExecution"]=true;
    }

    if (theLowSimButton->isChecked())
    {
        jsonLF["samples"]=numSamples_LF->text().toInt();
        jsonLF["existingDoE"]=theExistingCheckBox_LF->isChecked();
        if (theExistingCheckBox_LF->isChecked())
        {
            jsonLF["inpFile"]=inpFileDir_LF->text();
            jsonLF["outFile"]=outFileDir_LF->text();
        } else {
            jsonLF["inpFile"]="NA";
            jsonLF["outFile"]="NA";
        }
        jsonLF["seed"]=randomSeed->text().toInt();
        jsonLF["timeLimit"]=timeMeasure->text().toDouble();
        jsonLF["accuracyLimit"]=accuracyMeasure->text().toDouble();
        jsonLF["DoEmethod"]=theDoESelection->currentText();
        jsonLF["parallelExecution"]=parallelCheckBox->isChecked();
    } else {
        jsonLF["inpFile"]=inpFileDir_LF->text();
        jsonLF["outFile"]=outFileDir_LF->text();
        // dummy
        jsonLF["samples"]="NA";
        jsonLF["existingDoE"]=true;
        jsonLF["seed"]="NA";
        jsonLF["timeLimit"]="NA";
        jsonLF["accuracyLimit"]="NA";
        jsonLF["DoEmethod"]="NA";
        jsonLF["parallelExecution"]=true;
    }

//    if (theHighSimButton->isChecked() || theLowSimButton->isChecked()) {
//        jsonObj["seed"]=randomSeed->text().toInt();
//        jsonObj["timeLimit"]=timeMeasure->text().toDouble();
//        jsonObj["accuracyLimit"]=accuracyMeasure->text().toDouble();
//        jsonObj["DoEmethod"]=theDoESelection->currentText();
//        jsonObj["parallelExecution"]=parallelCheckBox->isChecked();
//    }

    jsonObj["advancedOpt"]=theAdvancedCheckBox->isChecked();
    if (theAdvancedCheckBox->isChecked())
    {
        jsonObj["kernel"]=gpKernel->currentText();
        jsonObj["linear"]=theLinearCheckBox->isChecked();
        jsonObj["logTransform"]=theLogtCheckBox->isChecked();
        jsonObj["nuggetOpt"]=theNuggetSelection->currentText();
        jsonObj["nuggetString"]=theNuggetVals->text();
    } else {
        jsonObj["kernel"]="Radial Basis";
        jsonObj["linear"]=false;
        jsonObj["logTransform"]=false;
        jsonObj["nuggetOpt"]="Optimize";
        jsonObj["nuggetString"]="NA";
    }

    jsonObj["highFidelity"]=jsonHF;
    jsonObj["lowFidelity"]=jsonLF;

    return result;
}

int SurrogateMFInputWidget::parseInputDataForRV(QString name1){


    double numberOfColumns=countColumn(name1);

    QStringList varNamesAndValues;
    for (int i=0;i<numberOfColumns;i++) {
        varNamesAndValues.append(QString("RV_column%1").arg(i+1));
        varNamesAndValues.append("nan");
    }
    setRV_Defaults();
    RandomVariablesContainer *theRVs =  RandomVariablesContainer::getInstance();
    theRVs->addRVsWithValues(varNamesAndValues);

    return 0;
}

int SurrogateMFInputWidget::parseOutputDataForQoI(QString name1){
    // get number of columns
    double numberOfColumns=countColumn(name1, true);
    QStringList qoiNames;
    for (int i=0;i<numberOfColumns;i++) {
        qoiNames.append(QString("QoI_column%1").arg(i+1));
    }
    //FMK theEdpWidget->setGPQoINames(qoiNames);
    return 0;
}

int SurrogateMFInputWidget::countColumn(QString name1, bool is_qoi){
    // get number of columns
    std::ifstream inFile(name1.toStdString());
    // read lines of input searching for pset using regular expression
    std::string line;

    int numberOfColumns_pre = -100;
    int  numberOfColumns=1;

    while (getline(inFile, line)) {
        numberOfColumns=1;
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
            errMSG->setText("Unrecognized file format");
            errMSG->setStyleSheet({"color: red"});

            numberOfColumns_pre=0;

            inFile.close();
            return 0;
        }
    }

    if ((numberOfColumns_pre!=-100)) {
        if (is_qoi) {
            if (numberOfColumns==1) {
                errMSG->setText("Total number of QoI variable is 1. Create 1 entry on the QoI Tab.");
            } else {
                errMSG->setText("Total number of QoI variables is " + QString::number(numberOfColumns) + ". Create " + QString::number(numberOfColumns) + " entries on the QoI Tab.");
            }
            errMSG->setStyleSheet({"color: blue"});
        } else {
                errMSG->setText({""});

         }
    }

    // close file
    inFile.close();
    return numberOfColumns_pre;
}


bool
SurrogateMFInputWidget::inputFromJSON(QJsonObject &jsonObject){

    bool result = true;

    QJsonObject jsonHF = jsonObject["highFidelity"].toObject();
    QJsonObject jsonLF = jsonObject["lowFidelity"].toObject();


    if (jsonHF.contains("fromModel")) {
      if (jsonHF["fromModel"].toBool()) {
          theHighSimButton->setChecked(true);

          if (jsonHF.contains("samples")) {
              int samples=jsonHF["samples"].toInt();
              numSamples_HF->setText(QString::number(samples));
          } else {
              result = false;
          }

          if (jsonHF.contains("existingDoE")) {
              if (jsonHF["existingDoE"].toBool()) {
                  theExistingCheckBox_HF->setChecked(true);
                  inpFileDir_HF -> setText(jsonHF["inpFile"].toString());
                  outFileDir_HF -> setText(jsonHF["outFile"].toString());
              } else {
                  theExistingCheckBox_HF->setChecked(false);
              }
          } else {
              result = false;
          }

      } else {
          theLowSimButton->setChecked(false);
          outFileDir_HF->setText(jsonHF["outFile"].toString());
          inpFileDir_HF->setText(jsonHF["inpFile"].toString());
      }
    } else {
      return false;
    }

    if (jsonLF.contains("fromModel")) {
      if (jsonLF["fromModel"].toBool()) {
          theLowSimButton->setChecked(true);

          if (jsonLF.contains("samples")) {
              int samples=jsonLF["samples"].toInt();
              numSamples_LF->setText(QString::number(samples));
          } else {
              result = false;
          }

          if (jsonLF.contains("existingDoE")) {
              if (jsonLF["existingDoE"].toBool()) {
                  theExistingCheckBox_LF->setChecked(true);
                  inpFileDir_LF -> setText(jsonLF["inpFile"].toString());
                  outFileDir_LF -> setText(jsonLF["outFile"].toString());
              } else {
                  theExistingCheckBox_LF->setChecked(false);
              }
          } else {
              result = false;
          }

      } else {
          theLowSimButton->setChecked(false);
          outFileDir_LF->setText(jsonLF["outFile"].toString());
          inpFileDir_LF->setText(jsonLF["inpFile"].toString());
      }
    } else {
      return false;
    }


    if (jsonLF["fromModel"].toBool() || jsonHF["fromModel"].toBool())
    {
        QJsonObject jsonTmp;
        if (jsonLF["fromModel"].toBool()) {
            jsonTmp = jsonLF;
         } else if (jsonHF["fromModel"].toBool()) {
            jsonTmp = jsonHF;
        }

        if (jsonTmp.contains("seed") && jsonTmp.contains("DoEmethod")) {
            double seed=jsonTmp["seed"].toDouble();
            randomSeed->setText(QString::number(seed));

            theDoESelection->setCurrentText(jsonTmp["DoEmethod"].toString());
        } else {
            result = false;
        }

        if (jsonTmp.contains("timeLimit") && jsonTmp.contains("accuracyLimit")) {
            int time=jsonTmp["timeLimit"].toInt();
            double accuracy=jsonTmp["accuracyLimit"].toDouble();
            timeMeasure->setText(QString::number(time));
            accuracyMeasure->setText(QString::number(accuracy));
        } else {
            result = false;
        }

        if (jsonObject.contains("parallelExecution")) {
            parallelCheckBox->setChecked(jsonObject["parallelExecution"].toBool());
        } else {
            parallelCheckBox->setChecked(true); // for compatibility. later change it to error. (sy - june 2021)
        }
    }

  if (jsonObject.contains("advancedOpt")) {
      theAdvancedCheckBox->setChecked(jsonObject["advancedOpt"].toBool());
      if (jsonObject["advancedOpt"].toBool()) {
        theAdvancedCheckBox->setChecked(true);
        QString method =jsonObject["kernel"].toString();
        int index = gpKernel->findText(method);
        if (index == -1) {
            return false;
        }
        gpKernel->setCurrentIndex(index);
        theLinearCheckBox->setChecked(jsonObject["linear"].toBool());
        theLogtCheckBox->setChecked(jsonObject["logTransform"].toBool());

        if (jsonObject.contains("nuggetOpt")) {
            QString nuggetOpt =jsonObject["nuggetOpt"].toString();
            index = theNuggetSelection->findText(nuggetOpt);
            if (index == -1) {
                return false;
            }
            theNuggetSelection->setCurrentIndex(index);
            if (index!=0){
                theNuggetVals->setText(jsonObject["nuggetString"].toString());
            }
        } else {
            theNuggetSelection->setCurrentIndex(0);
        }
      }
  } else {
     return false;
  }

  return result;
}

bool
SurrogateMFInputWidget::copyFiles(QString &fileDir) {

    if ((!theHighSimButton->isChecked()) || (theExistingCheckBox_HF->isChecked())) {
        QFile::copy(inpFileDir_HF->text(), fileDir + QDir::separator() + "inpFile_HF.in");
        QFile::copy(outFileDir_HF->text(), fileDir + QDir::separator() + "outFile_HF.in");

    }
    if ((!theLowSimButton->isChecked()) || (theExistingCheckBox_LF->isChecked())) {
        QFile::copy(inpFileDir_LF->text(), fileDir + QDir::separator() + "inpFile_LF.in");
        QFile::copy(outFileDir_LF->text(), fileDir + QDir::separator() + "outFile_LF.in");
    }
    return true;
}

void
SurrogateMFInputWidget::clear(void)
{

}

int
SurrogateMFInputWidget::getNumberTasks()
{
  return numSamples_LF->text().toInt();
}

void
SurrogateMFInputWidget::setRV_Defaults(void) {

  RandomVariablesContainer *theRVs =  RandomVariablesContainer::getInstance();
  //QString classType("Uniform");
  QString engineType("SimCenterUQ");
  QString classType;
  if ((!theHighSimButton->isChecked()) && (!theLowSimButton->isChecked()) ){
      classType=QString("NA");
  } else {
      classType=QString("Uniform");
    }
  theRVs->setDefaults(engineType, classType, Uniform);
}
