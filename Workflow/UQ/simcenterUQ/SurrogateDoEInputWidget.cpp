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

#include <SurrogateDoEInputWidget.h>
#include <RandomVariablesContainer.h>



#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include <QRadioButton>
#include <QSpinBox>
#include <QScrollArea>
#include <QStackedWidget>

SurrogateDoEInputWidget::SurrogateDoEInputWidget(QWidget *parent)
: UQ_Method(parent)
{

    auto layout2 = new QVBoxLayout();
    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);
    QFrame *widget = new QFrame(sa);
    sa->setWidget(widget);
    auto layout = new QGridLayout(widget);

    widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    layout2 -> addWidget(sa);
    widget -> setLayout(layout);

    int wid = 0; // widget id
    //
    // create layout label and entry for # samples
    //

    createLineEdits(numSamples, tr("150"), tr("Int"), tr("Specify the number of samples"), 150);
    QLabel *maxRun = new QLabel("Number of Samples");
    maxRun->setStyleSheet("font-weight: bold");
    layout->addWidget(maxRun, wid, 0);
    layout->addWidget(numSamples, wid++, 1);

    //
    // Max computation time (approximate)
    //

    createLineEdits(timeMeasure, tr("60"), tr("Int"), tr("Max Computation Time (minutes)"), 150);
    layout->addWidget(new QLabel("Max Computation Time (minutes)    "), wid, 0);
    layout->addWidget(timeMeasure, wid++, 1);

    //
    // create convergence criteria
    //

    createLineEdits(accuracyMeasure, tr("0.02"), tr("Double"), tr("NRMSE: normalized root mean square error"), 150);
    layout->addWidget(new QLabel("Target Accuracy (normalized err) "), wid, 0);
    layout->addWidget(accuracyMeasure, wid++, 1);

    //
    // Random Seed
    //

    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;

    createLineEdits(randomSeed, QString::number(randomNumber), tr("Int"), tr("Set the seed"), 150);
    layout->addWidget(new QLabel("Random Seed"), wid, 0);
    layout->addWidget(randomSeed, wid++, 1);

    //
    // Parallel Execution
    //

    parallelCheckBox = new QCheckBox();
    parallelCheckBox->setChecked(true);
    layout->addWidget(new QLabel("Parallel Execution"), wid, 0);
    layout->addWidget(parallelCheckBox, wid++, 1);

    //
    // Advanced options
    //

    theGpAdvancedCheckBox = new QCheckBox("Advanced Options for Gaussian Process Model ");
    theGpAdvancedCheckBox->setStyleSheet("font-weight: bold; color: grey");

    theGpAdvancedWidgetGroup = new QWidget();
    QGridLayout *theGpAdvancedWidgetLayout = new QGridLayout();
    theGpAdvancedWidgetGroup -> setLayout(theGpAdvancedWidgetLayout);
    theGpAdvancedWidgetGroup -> setVisible(false);
    //theGpAdvancedWidgetLayout ->setMargin(0);
    int aid =0;


    layout->addWidget(theGpAdvancedCheckBox, wid++, 0, 1, 3, Qt::AlignBottom);
    layout->addWidget(theGpAdvancedWidgetGroup, wid++, 0, 1, 3);

    QFrame *lineA = new QFrame;
    lineA->setFrameShape(QFrame::HLine);
    lineA->setFrameShadow(QFrame::Sunken);

   // layout->addWidget(theAdvancedTitle, wid, 0, 1, 3,Qt::AlignBottom);
    theGpAdvancedWidgetLayout->addWidget(lineA, aid++, 0, 1, 3);
    //lineA->setVisible(false);

    //
    // Selection of GP kernel
    //


    QStringList itemList = {tr("Matern 5/2"), tr("Matern 3/2"),tr("Radial Basis"), tr("Exponential")  };
    createComboBox(gpKernel, itemList, tr(""), 150,0);

    theGpAdvancedWidgetLayout->addWidget(new QLabel("Kernel Function"), aid, 0);
    theGpAdvancedWidgetLayout->addWidget(gpKernel, aid++, 1);

    //
    // Use Linear trending function
    //

    theLinearCheckBox = new QCheckBox();
    theLinearCheckBox->setToolTip("Default is no trending function");

    theGpAdvancedWidgetLayout->addWidget(new QLabel("Add Linear Trend Function"), aid, 0);
    theGpAdvancedWidgetLayout->addWidget(theLinearCheckBox, aid++, 1);

    //
    // Use Log transform
    //

    theLogtCheckBox = new QCheckBox("(check this box only when all responses are always positive)");
    theGpAdvancedWidgetLayout->addWidget(new QLabel("Log-space Transform of QoI"), aid, 0);
    theGpAdvancedWidgetLayout->addWidget(theLogtCheckBox, aid++, 1,1,-1);


    //
    // DoE Options
    //

    QStringList doeList = {tr("Pareto"), tr("IMSEw"),tr("MMSEw"), tr("None") };
    createComboBox(theDoESelection, doeList, tr(""), 150, 3);

    createLineEdits(initialDoE, tr(""), tr("Double"), tr("Provide the number of initial samples"), 150, tr("(Optional) Initial DoE #"));
    initialDoE->setVisible(false);
    theGpAdvancedWidgetLayout->addWidget(new QLabel("DoE Options"), aid, 0);
    theGpAdvancedWidgetLayout->addWidget(theDoESelection, aid, 1);
    //layout->addWidget(theDoEMsg, wid, 2);
    theGpAdvancedWidgetLayout->addWidget(initialDoE, aid++, 2);

    connect(theDoESelection,SIGNAL(currentIndexChanged(int)),this,SLOT(showDoEBox(int)));

    //
    // Nugget function
    //

    QStringList nuggetList = {tr("Optimize"), tr("Fixed Values"),tr("Fixed Bounds"), tr("Zero") , tr("Heteroscedastic")};

    createComboBox(theNuggetSelection, nuggetList, tr(""), 150, 0);
    createLineEdits(theNuggetVals, tr(""), tr("Double"), tr("Provide nugget values"), 300, tr(""));

    theNuggetMsg = new QLabel("in the log-transformed space");

    theGpAdvancedWidgetLayout->addWidget(new QLabel("Nugget Variances"), aid, 0);
    theGpAdvancedWidgetLayout->addWidget(theNuggetSelection, aid, 1);
    theGpAdvancedWidgetLayout->addWidget(theNuggetMsg, aid++, 2, Qt::AlignLeft);
    theGpAdvancedWidgetLayout->addWidget(theNuggetVals, aid++, 1,1,3);
    theNuggetVals->setVisible(false);
    theNuggetMsg->setVisible(false);

    connect(theNuggetSelection,SIGNAL(currentIndexChanged(int)),this,SLOT(showNuggetBox(int)));
    connect(theLogtCheckBox, &QCheckBox::toggled, this, [=](bool tog)  {
        if (tog && ((theNuggetSelection->currentIndex()!=0)||(theNuggetSelection->currentIndex()!=3)))
            theNuggetMsg -> setVisible(true);
        else
            theNuggetMsg -> setVisible(false);
    });

    createLineEdits(numSampToBeRepl, tr(""), tr("Int"), tr("Number of samples to be replicated"), 300, tr("(Optional) Default: 8×#RVs"));
    createLineEdits(numRepl, tr(""), tr("Int"), tr("Number of replications"), 300, tr("(Optional) Default: 10"));

    repMsg = new QLabel("");
    repLabelA = new QLabel("# samples to be replicated (A)");
    repLabelB = new QLabel("# replication per sample (B)");
    theGpAdvancedWidgetLayout->addWidget(repLabelA, aid, 0);
    theGpAdvancedWidgetLayout->addWidget(numSampToBeRepl, aid++, 1,1,-1);
    theGpAdvancedWidgetLayout->addWidget(repLabelB, aid, 0);
    theGpAdvancedWidgetLayout->addWidget(numRepl, aid++, 1,1,-1);
    theGpAdvancedWidgetLayout->addWidget(repMsg, aid++, 1,1,5);

    repLabelA->setVisible(false);
    repLabelB->setVisible(false);
    numSampToBeRepl->setVisible(false);
    numRepl->setVisible(false);
    repMsg->setVisible(false);

    connect(numSampToBeRepl,SIGNAL(textEdited(QString)),this,SLOT(updateSimNumber(QString)));
    connect(numRepl,SIGNAL(textEdited(QString)),this,SLOT(updateSimNumber(QString)));
    connect(numSamples,SIGNAL(textEdited(QString)),this,SLOT(updateSimNumber(QString)));



    //
    // Use Existing Initial DoE
    //
    theExistingCheckBox = new QCheckBox("Start with Existing Dataset");
    theExistingCheckBox ->setStyleSheet("font-weight: bold; color: grey");

    theExistingWidgetGroup = new QWidget();
    QGridLayout *theExistingWidgetLayout = new QGridLayout();
    theExistingWidgetGroup -> setLayout(theExistingWidgetLayout);
    theExistingWidgetGroup -> setVisible(false);
    //theExistingWidgetLayout ->setMargin(0);
    int eid =0;

    layout->addWidget(theExistingCheckBox, wid++, 0, 1, 3, Qt::AlignBottom);
    layout->addWidget(theExistingWidgetGroup, wid++, 0, 1, 4);

    QFrame *lineB = new QFrame;
    lineB->setFrameShape(QFrame::HLine);
    lineB->setFrameShadow(QFrame::Sunken);
    theExistingWidgetLayout->addWidget(lineB, eid++, 0, 1, 3);

    //
    // Input data
    //

    createLineEdits(inpFileDir, tr(""), tr("Str"), tr(""), 200, tr(""));

    chooseInpFile = new QPushButton("Choose");
    connect(chooseInpFile, &QPushButton::clicked, this, [=](){
        QString fileName = QFileDialog::getOpenFileName(this, "Open Simulation Model", "", "All files (*.*)");
        if(!fileName.isEmpty()) {
            inpFileDir->setText(fileName);
            this->checkValidityData(fileName);
            setWindowFilePath(fileName);
        }
    });
    theExistingWidgetLayout->addWidget(new QLabel("Training Points (Input RV)"),eid,0);
    theExistingWidgetLayout->addWidget(inpFileDir,eid,1);
    theExistingWidgetLayout->addWidget(chooseInpFile,eid++,2,Qt::AlignLeft);

    //
    // Output data
    //

    createLineEdits(outFileDir, tr(""), tr("Str"), tr(""), 200, tr(""));

    chooseOutFile = new QPushButton("Choose");
    connect(chooseOutFile, &QPushButton::clicked, this, [=](){
        QString outputName =QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)");

        if(!outputName.isEmpty()) {
            outFileDir->setText(outputName);
            this->checkValidityData(outputName);
        }
    });
    theExistingWidgetLayout->addWidget(new QLabel("System Responses (Output QoI)"),eid,0,Qt::AlignTop);
    theExistingWidgetLayout->addWidget(outFileDir,eid,1,Qt::AlignTop);
    theExistingWidgetLayout->addWidget(chooseOutFile,eid++,2,Qt::AlignLeft);
    theExistingWidgetLayout->setRowStretch(eid,3);

    errMSG=new QLabel("Unrecognized file format");
    errMSG->setStyleSheet({"color: red"});
    layout->addWidget(errMSG,wid++,1,1,2,Qt::AlignLeft);
    layout->setSpacing(10);
    errMSG->hide();

    //
    // Advanced options for EEUQ
    //

    theGpAdvancedCheckBoxEE = new QCheckBox("Advanced Options (Earthquake specific)");
    theGpAdvancedCheckBoxEE->setStyleSheet("font-weight: bold; color: grey");

    theGpAdvancedWidgetGroupEE = new QWidget();
    QGridLayout *theGpAdvancedWidgetLayoutEE= new QGridLayout();
    theGpAdvancedWidgetGroupEE -> setLayout(theGpAdvancedWidgetLayoutEE);
    theGpAdvancedWidgetGroupEE -> setVisible(false);
    int eeid =0;

    layout->addWidget(theGpAdvancedCheckBoxEE, wid++, 0, 1, 3, Qt::AlignBottom);
    layout->addWidget(theGpAdvancedWidgetGroupEE, wid++, 0, 1, -1);

    QFrame *lineC = new QFrame;
    lineC->setFrameShape(QFrame::HLine);
    lineC->setFrameShadow(QFrame::Sunken);
    theGpAdvancedWidgetLayoutEE->addWidget(lineC, eeid++, 0, 1, 3);

    //
    // Selection of im
    //

    QStringList imChoices = {tr("Ground Motion Intensity"), tr("None")};
    createComboBox(imChoicesComboBox, imChoices, tr(""), 300,0);

    theGpAdvancedWidgetLayoutEE->addWidget(new QLabel("Input postprocess"), eeid++, 0);
    theGpAdvancedWidgetLayoutEE->addWidget(imChoicesComboBox,eeid++,0,1,9);
    useGeoMeanIM = new QCheckBox("Use geometric mean when 2 or more ground motion components are given");
    theGpAdvancedWidgetLayoutEE->addWidget(useGeoMeanIM, eeid++, 0);
    useGeoMeanIM -> setChecked(true);
    useGeoMeanIM -> show();

    QWidget *emptyVariableWidget = new QWidget();
    theSCIMWidget = new SimCenterIntensityMeasureWidget();
    im_stackedWidgets = new QStackedWidget(this);
    im_stackedWidgets->addWidget(theSCIMWidget);
    im_stackedWidgets->addWidget(emptyVariableWidget);
    im_stackedWidgets->setCurrentIndex(0);

    connect(imChoicesComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int id)
    {
        im_stackedWidgets->setCurrentIndex(id);

        if (id==0){
            // "Ground Motion Intensity"
            useGeoMeanIM ->setVisible(true);
        } else {
            useGeoMeanIM ->setVisible(false);
        }
    });
    theGpAdvancedWidgetLayoutEE->addWidget(im_stackedWidgets,eeid++,0,1,9);
    // not an earthquake event, inactivate ground motion intensity widget
    theGpAdvancedCheckBoxEE->setVisible(false);
    theGpAdvancedCheckBoxEE->setChecked(false);
    theLogtCheckBox->setChecked(false);


    //
    // Finish
    //

    layout->setRowStretch(wid, 1);
    layout->setColumnStretch(5, 1);
    this->setLayout(layout2);
    connect(theGpAdvancedCheckBox,SIGNAL(toggled(bool)),this,SLOT(doAdvancedGP(bool)));
    connect(theGpAdvancedCheckBoxEE,SIGNAL(toggled(bool)),this,SLOT(doAdvancedEE(bool)));
    connect(theExistingCheckBox,SIGNAL(toggled(bool)),this,SLOT(doExistingGP(bool)));
    connect(this, SIGNAL(eventTypeChanged(QString)), this, SLOT(onEventTypeChanged(QString)));

}

SurrogateDoEInputWidget::~SurrogateDoEInputWidget()
{

}

void
SurrogateDoEInputWidget::showNuggetBox(int idx)
{
    numSims = numSamples->text().toInt();
    theNuggetVals->clear();
    numSampToBeRepl ->clear();


    repLabelA->setVisible(false);
    repLabelB->setVisible(false);
    numSampToBeRepl->setVisible(false);
    numRepl->setVisible(false);
    repMsg->setVisible(false);

    if (idx == 0) {
        theNuggetVals->hide();
    } else if (idx==1){
        theNuggetVals->show();
        theNuggetVals->setPlaceholderText("QoI₁, QoI₂,..");
    } else if (idx==2) {
        theNuggetVals->show();
        theNuggetVals->setPlaceholderText("[QoI₁ˡᵇ,QoI₁ᵘᵇ], [QoI₂ˡᵇ,QoI₂ᵘᵇ],..");
    } else if (idx==3) {
        theNuggetVals->hide();
    } else if (idx==4) {
        theNuggetVals->hide();
        if (typeEVT.compare("EQ") !=0 ) {
            repMsg ->setText("With the replications, the expected number of simulations is " + numSamples->text() + "+A*(B-1)");
            repMsg -> setStyleSheet({"color: black"});
            repLabelA->setVisible(true);
            repLabelB->setVisible(true);
            numSampToBeRepl->setVisible(true);
            numRepl->setVisible(true);
            repMsg->setVisible(true);
        }
    }
    if ((theLogtCheckBox->isChecked()) && (idx!=0))
        theNuggetMsg -> setVisible(true);
    else
        theNuggetMsg -> setVisible(false);
};


void
SurrogateDoEInputWidget::updateSimNumber(QString a)
{


    if (!(numSampToBeRepl->text()=="") && ((numSampToBeRepl->text().toInt()<2) || (numSampToBeRepl->text().toInt() > numSamples->text().toInt()))) {
        repMsg -> setText("The number of samples to be replicated (A) is recommended to be greater than 1 and smaller than  \n the number of the unique samples (" + numSamples->text() +"), a value greater than 4×#RV is highly recommended");
        repMsg -> setStyleSheet({"color: red"});
        return;
    }

    if (!(numRepl->text()=="") && numRepl->text().toInt()<2) {
        repMsg -> setText("The number of replications (B) should be greater than 1 and a value greater than 5 is recommended");
         repMsg -> setStyleSheet({"color: red"});
         return;
    }


    if ((numSampToBeRepl->text()=="") || (numRepl->text()=="")) {
        repMsg ->setText("With the replications, the expected number of simulations is " + numSamples->text() + "+A*(B-1)");
        repMsg -> setStyleSheet({"color: black"});
        return;

    }

    numSims = numSamples->text().toInt() + (numSampToBeRepl->text().toInt() * (numRepl->text().toInt() - 1) );
    repMsg ->setText("With the replications, the expected number of simulations is " + QString::number(numSims));
    repMsg -> setStyleSheet({"color: black"});

}



void
SurrogateDoEInputWidget::showDoEBox(int idx)
{
    initialDoE->clear();
    if (idx == 3) {
        initialDoE->hide();
    } else {
        initialDoE->show();
        //theNuggetSelection->setCurrentIndex(3);
    }
};

// SLOT function
void SurrogateDoEInputWidget::doAdvancedGP(bool tog)
{

    theGpAdvancedWidgetGroup ->setVisible(tog);


    if (tog) {
        theGpAdvancedCheckBox->setStyleSheet("font-weight: bold; color: black");

    } else {
        theGpAdvancedCheckBox->setStyleSheet("font-weight: bold; color: grey");

        gpKernel->setCurrentIndex(0);
        //theNuggetSelection->setCurrentIndex(0);
        theDoESelection->setCurrentIndex(3);
        theLinearCheckBox->setChecked(false);
        //theLogtCheckBox->setChecked(false);
    }
}
// SLOT function
void SurrogateDoEInputWidget::doAdvancedEE(bool tog)
{

    theGpAdvancedWidgetGroupEE ->setVisible(tog);


    if (tog) {
        theGpAdvancedCheckBoxEE->setStyleSheet("font-weight: bold; color: black");

    } else {
        theGpAdvancedCheckBoxEE->setStyleSheet("font-weight: bold; color: grey");
    }
}


void SurrogateDoEInputWidget::doExistingGP(bool tog)
{
    theExistingWidgetGroup->setVisible(tog);
    if (tog) {
        theExistingCheckBox->setStyleSheet("font-weight: bold; color: black");
    } else {
        theExistingCheckBox->setStyleSheet("font-weight: bold; color: grey");
        inpFileDir-> setText("");
        outFileDir-> setText("");
    }
    theExistingWidgetGroup->setVisible(tog);

//        lineB->setVisible(tog);
//        inpFileDir->setVisible(tog);
//        outFileDir->setVisible(tog);
//        theInputLabel->setVisible(tog);
//        theOutputLabel->setVisible(tog);
//        chooseInpFile->setVisible(tog);
//        chooseOutFile->setVisible(tog);
}

void
SurrogateDoEInputWidget::checkValidityData(QString name1){

    // get number of columns
    std::ifstream inFile(name1.toStdString());
    // read lines of input searching for pset using regular expression
    std::string line;
    errMSG->hide();

    int numberOfColumns_pre = -100;
    while (getline(inFile, line)) {
        int  numberOfColumns=1;
        bool previousWasSpace=false;
        bool previousWasHeader=false;

        //for(int i=0; i<line.size(); i++){
        for(size_t i=0; i<line.size(); i++){
            if(line[i] == '%' || line[i] == '#'){ // ignore header
                numberOfColumns = numberOfColumns_pre;
                previousWasHeader=false;
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
        if(previousWasSpace && !previousWasHeader)// when there is a blank space at the end of each row
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
}

bool
SurrogateDoEInputWidget::outputToJSON(QJsonObject &jsonObj){

    bool result = true;

    jsonObj["samples"]=numSamples->text().toInt();
    jsonObj["seed"]=randomSeed->text().toInt();
    jsonObj["timeLimit"]=timeMeasure->text().toDouble();
    jsonObj["accuracyLimit"]=accuracyMeasure->text().toDouble();
    jsonObj["parallelExecution"]=parallelCheckBox->isChecked();

    jsonObj["advancedOpt"]=theGpAdvancedCheckBox->isChecked();
    if (theGpAdvancedCheckBox->isChecked())
    {
        jsonObj["kernel"]=gpKernel->currentText();
        jsonObj["DoEmethod"]=theDoESelection->currentText();
        jsonObj["initialDoE"]=initialDoE->text().toDouble();
        jsonObj["linear"]=theLinearCheckBox->isChecked();
        jsonObj["logTransform"]=theLogtCheckBox->isChecked();
        jsonObj["nuggetOpt"]=theNuggetSelection->currentText();
        jsonObj["nuggetString"]=theNuggetVals->text();

        if (numSampToBeRepl->text() == "") {
            jsonObj["numSampToBeRepl"]= -1; // use default
        } else {
            if ((numSampToBeRepl->text().toInt()<2) || (numSampToBeRepl->text().toInt() > numSamples->text().toInt())) {
                //errorMessage("Error prossessing inputs - the number of samples to be replicated (A) should be greater than 1 and smaller than the number of the unique samples (" + numSamples->text() +"), a value greater than 4×#RV is recommended");
                //return 0;
                // sy- not anymore march 2023
            }

             jsonObj["numSampToBeRepl"]= numSampToBeRepl->text().toInt();
        }

        if (numRepl->text() == "") {
            jsonObj["numRepl"]= -1; // use default
         } else {
            if (numRepl->text().toInt()<2) {
                //errorMessage("Error prossessing inputs - the number of replications (B) should be greater than 1 and a value greater than 5 is recommended");
                //return 0;
                // sy- not anymore march 2023
            }
            jsonObj["numRepl"]= numRepl->text().toInt();
        }

    } else {
        jsonObj["kernel"]="Radial Basis";
        jsonObj["DoEmethod"]="None";
        jsonObj["initialDoE"]=-1;
        jsonObj["linear"]=false;
        if (typeEVT.compare("EQ") ==0) {
            jsonObj["logTransform"]=true;
            jsonObj["numSampToBeRepl"]= 1;
            jsonObj["numRepl"]= 1;
            jsonObj["nuggetOpt"]="Heteroscedastic";
        } else {
            jsonObj["logTransform"]=false;
            jsonObj["numSampToBeRepl"]= -1;
            jsonObj["numRepl"]= -1;
            jsonObj["nuggetOpt"]="Optimize";
        }
        jsonObj["nuggetString"]="NA";
    }

    jsonObj["existingDoE"]=theExistingCheckBox->isChecked();
    if (theExistingCheckBox->isChecked())
    {
        jsonObj["inpFile"]=inpFileDir->text();
        jsonObj["outFile"]=outFileDir->text();
    } else {
        jsonObj["inpFile"]="NA";
        jsonObj["outFile"]="NA";
    }

    if (im_stackedWidgets->currentIndex()==0) {
        QJsonObject imJson;
        result = theSCIMWidget->outputToJSON(imJson);
        jsonObj["IntensityMeasure"] = imJson;
        jsonObj["useGeoMean"] = useGeoMeanIM->isChecked();
    }

    return result;    
}

bool
SurrogateDoEInputWidget::inputFromJSON(QJsonObject &jsonObject){

    bool result = true;

    if (jsonObject.contains("samples") && jsonObject.contains("seed")) {
        int samples=jsonObject["samples"].toInt();
        double seed=jsonObject["seed"].toDouble();
        numSamples->setText(QString::number(samples));
        randomSeed->setText(QString::number(seed));
    } else {
        result = false;
    }

    if (jsonObject.contains("timeLimit") && jsonObject.contains("accuracyLimit")) {
        int time=jsonObject["timeLimit"].toInt();
        double accuracy=jsonObject["accuracyLimit"].toDouble();
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

    if (jsonObject.contains("advancedOpt")) {
        if (jsonObject["advancedOpt"].toBool()) {
            theGpAdvancedCheckBox->setChecked(true);
            QString method =jsonObject["kernel"].toString();
            int index = gpKernel->findText(method);
            if (index == -1) {
                return false;
            }
            gpKernel->setCurrentIndex(index);
            theLinearCheckBox->setChecked(jsonObject["linear"].toBool());
            theLogtCheckBox->setChecked(jsonObject["logTransform"].toBool());
            theDoESelection -> setCurrentText(jsonObject["DoEmethod"].toString());
            int initDoE=jsonObject["initialDoE"].toInt();
            if (initDoE!=0) {
                initialDoE->setText(QString::number(initDoE));
            } else {
                initialDoE->setText("");
            }
            if (jsonObject.contains("nuggetOpt")) {
                QString nuggetOpt =jsonObject["nuggetOpt"].toString();
                index = theNuggetSelection->findText(nuggetOpt);
                if (index == -1) {
                    return false;
                }
                theNuggetSelection->setCurrentIndex(index);
                if ((index==1) ||(index==2)){
                    theNuggetVals->setText(jsonObject["nuggetString"].toString());
                }
                else if (index==4){

                    if (jsonObject["numSampToBeRepl"].toInt()==-1)
                        numSampToBeRepl->setText("");
                    else
                        numSampToBeRepl->setText(QString::number(jsonObject["numSampToBeRepl"].toInt()));

                    if (jsonObject["numRepl"].toInt()==-1)
                        numRepl->setText("");
                    else
                        numRepl->setText(QString::number(jsonObject["numRepl"].toInt()));

                    updateSimNumber("");
                }
            } else {
                theNuggetSelection->setCurrentIndex(index);
            }
        } else {
            theGpAdvancedCheckBox->setChecked(false);
            // for compatibility. Change to give an error later
        }
    } else {
    result = false;
    }

    if (jsonObject.contains("existingDoE")) {
        if (jsonObject["existingDoE"].toBool()) {
            theExistingCheckBox->setChecked(true);
            inpFileDir -> setText(jsonObject["inpFile"].toString());
            outFileDir -> setText(jsonObject["outFile"].toString());
        } else {
            theExistingCheckBox->setChecked(false);
        }
    } else {
        result = false;
    }

    if (jsonObject.contains("IntensityMeasure")) {
        useGeoMeanIM->setChecked(jsonObject["useGeoMean"].toBool());
        theGpAdvancedCheckBoxEE->setVisible(true);
        theGpAdvancedCheckBoxEE->setChecked(true);
        im_stackedWidgets->setCurrentIndex(0);
        imChoicesComboBox->setCurrentIndex(0);
        qDebug() << "Start loading intensity measure";
        result = theSCIMWidget->inputFromJSON(jsonObject);
    }

    return result;
}


void
SurrogateDoEInputWidget::createLineEdits(QLineEdit *&a, QString defaultVal, QString type, QString toolTipText, double wid, QString placeholderText)
{
    a = new QLineEdit();
    a->setText(defaultVal);
    if (type=="Int" ) {
        a->setValidator(new QIntValidator);
    } else if (type=="Double") {
        a->setValidator(new QDoubleValidator);
    }
    a->setToolTip(toolTipText);
    a->setMaximumWidth(wid);
    a->setMinimumWidth(wid);
    a->setPlaceholderText((placeholderText));
}




void
SurrogateDoEInputWidget::createComboBox(QComboBox *&a, QStringList items, QString toolTipText, double wid, int currentIdx)
{
    a = new QComboBox();

    foreach(QString str, items)
    {
        a->addItem(str);
    }
    a->setToolTip(toolTipText);
    a->setMaximumWidth(wid);
    a->setCurrentIndex(currentIdx);
}

void
SurrogateDoEInputWidget::clear(void)
{
    theGpAdvancedCheckBox->setChecked(false);
    theExistingCheckBox->setChecked(false);
}

bool
SurrogateDoEInputWidget::copyFiles(QString &fileDir) {
    if (theExistingCheckBox->isChecked())
    {
        QFile::copy(inpFileDir->text(), fileDir + QDir::separator() + "inpFile.in");
        QFile::copy(outFileDir->text(), fileDir + QDir::separator() + "outFile.in");
    }
    return true;
}

int
SurrogateDoEInputWidget::getNumberTasks()
{
  return numSamples->text().toInt();
}

void
SurrogateDoEInputWidget::setRV_Defaults(void) {

  RandomVariablesContainer *theRVs =  RandomVariablesContainer::getInstance();
  QString classType("Uniform");
  QString engineType("SimCenterUQ");

  theRVs->setDefaults(engineType, classType, Uniform);
}

void
SurrogateDoEInputWidget::setEventType(QString type) {
    typeEVT = type;
    emit eventTypeChanged(typeEVT);
}

void
SurrogateDoEInputWidget::onEventTypeChanged(QString typeEVT) {
    if (typeEVT.compare("EQ") ==0 ) {
        // an earthquake event type
        theGpAdvancedCheckBoxEE->setVisible(true);
        theLogtCheckBox->setChecked(true);
        theNuggetSelection->setCurrentIndex(4); //heteroskedasticity
        numSampToBeRepl->setText("1");
        numRepl->setText("1");
    } else if (typeEVT.compare("WE") ==0 ) {
            theGpAdvancedCheckBoxEE->setVisible(false);
            theLogtCheckBox->setChecked(true);
            theNuggetSelection->setCurrentIndex(0); //small noise level
    } else {
        // not an earthquake event, inactivate ground motion intensity widget
        theGpAdvancedCheckBoxEE->setVisible(false);
        theGpAdvancedCheckBoxEE->setChecked(false);
        theLogtCheckBox->setChecked(false);
    }
}
