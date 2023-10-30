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

#include "UCSD_InputTMCMC.h"
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>
#include <QPushButton>
#include <QFileDialog>
#include <time.h>
#include <QDir>
#include <QDebug>
#include <QCheckBox>

#include <fstream>
#include <sstream>

UCSD_InputTMCMC::UCSD_InputTMCMC(QWidget *parent)
:UQ_Method(parent)
{
    auto layout = new QGridLayout();
    int row = 0;

    // create layout label and entry for # samples
    numParticles = new QLineEdit();
    numParticles->setText(tr("500"));
    QIntValidator *numSamplesValidator = new QIntValidator;
    numSamplesValidator->setBottom(0);
    numParticles->setValidator(numSamplesValidator);
    numParticles->setToolTip("Specify the number of samples");

    layout->addWidget(new QLabel("Sample Size (> 100)"), row, 0);
    layout->addWidget(numParticles, row, 1);

    numSamplesWarning = new QLabel;
    numSamplesWarning->setText("A sample size of at least 200 is recommended");
    numSamplesWarning->setStyleSheet("QLabel { color : blue; }");
    layout->addWidget(numSamplesWarning, row, 2);
    numSamplesWarning->setVisible(false);

    numSamplesError = new QLabel;
    numSamplesError->setText("A sample size of at least 100 is required");
    numSamplesError->setStyleSheet("QLabel { color : red; }");
    layout->addWidget(numSamplesError, row++, 2);
    numSamplesError->setVisible(false);

    connect(numParticles, &QLineEdit::textChanged, this, [=] {
        if (numParticles->text().isEmpty()) {
            numSamplesError->setVisible(true);
            numSamplesWarning->setVisible(false);
        } ;
    });

    connect(numParticles, &QLineEdit::editingFinished, this, [=]  {
        int numPar = numParticles->text().toInt();
        checkSampleSize(numPar);
    });

    // create label and lineedit for max run time
    maxRunTime = new QLineEdit();
    maxRunTime->setText(tr("60"));
    QDoubleValidator *maxRunTimeValidator = new QDoubleValidator;
    maxRunTime->setValidator(maxRunTimeValidator);
    maxRunTime->setPlaceholderText(tr("2880"));
    maxRunTime->setToolTip(tr("This will be used to stop TMCMC algorithm run to facilitate restarts if required"));

//    layout->addWidget(new QLabel("Max. Computation Time (minutes)"), row, 0);
//    layout->addWidget(maxRunTime, row++, 1);


    // create label and entry for seed to layout
    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;
    //int randomNumber = arc4random() % 1000 + 1;
    randomSeed = new QLineEdit();
    randomSeed->setText(QString::number(randomNumber));
    randomSeed->setValidator(new QIntValidator);
    randomSeed->setToolTip("Specify the random seed value");

    layout->addWidget(new QLabel("Seed"), row, 0);
    layout->addWidget(randomSeed, row++, 1);

    // create label and lineedit for calibration data file and add to layout
    calDataFileEdit = new QLineEdit();
    layout->addWidget(new QLabel("Calibration Data File"), row, 0);
    layout->addWidget(calDataFileEdit, row, 1, 1, 2);

    QPushButton *chooseCalDataFile = new QPushButton("Choose");
    connect(chooseCalDataFile, &QPushButton::clicked, this, [=](){
          QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*)");
          if (!fileName.isEmpty()) {
              calDataFileEdit->setText(fileName);
              numExperiments = getNumExp(fileName);
          }
    });
    layout->addWidget(chooseCalDataFile, row++, 3);

    // create label and lineedit for loglikelihood script and add to layout
    logLikelihoodScript = new QLineEdit();
    logLikelihoodScript->setPlaceholderText("(Optional)");
    layout->addWidget(new QLabel("Log Likelihood Script"), row, 0);
    layout->addWidget(logLikelihoodScript, row, 1, 1, 2);

    QPushButton *chooseFile = new QPushButton("Choose");
    connect(chooseFile, &QPushButton::clicked, this, [=](){
        logLikelihoodScript->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.py)"));
    });
    layout->addWidget(chooseFile, row++, 3);

    readCovarianceDataCheckBox = new QCheckBox();
    readCovarianceDataCheckBox->setChecked(false);


    // Adding advanced options

    advancedOptionsCheckBox = new QCheckBox();
    advancedOptionsCheckBox->setChecked(false);
    advancedOptionsTitle=new QLabel("Advanced Options for TMCMC algorithm ");
    advancedOptionsTitle->setStyleSheet("font-weight: bold; color: grey");
//    layout->addWidget(advancedOptionsCheckBox, row, 0, 1, 3,Qt::AlignBottom);
//    layout->addWidget(advancedOptionsTitle, row++, 1, 1, 3, Qt::AlignBottom);

    lineA= new QFrame;
    lineA->setFrameShape(QFrame::HLine);
    lineA->setFrameShadow(QFrame::Sunken);
//    layout->addWidget(lineA, row++, 0, 1, 3);
    lineA->setVisible(false);

    // create layout label and entry for minimum # MCMC steps
    numMCMCStepsMinimum = new QLineEdit();
    numMCMCStepsMinimum->setText(tr("2"));
    numMCMCStepsMinimum->setValidator(new QIntValidator);
    numMCMCStepsMinimum->setToolTip("Specify the minimum number of steps in the random walk");

//    layout->addWidget(new QLabel("Min. # MCMC Steps"), row, 0);
//    layout->addWidget(numMCMCStepsMinimum, row++, 1);

    // create layout label and entry for maximum # MCMC steps
    numMCMCStepsMaximum = new QLineEdit();
    numMCMCStepsMaximum->setText(tr("5"));
    numMCMCStepsMaximum->setValidator(new QIntValidator);
    numMCMCStepsMaximum->setToolTip("Specify the maximum number of steps in the random walk");

//    layout->addWidget(new QLabel("Max. # MCMC Steps"), row, 0);
//    layout->addWidget(numMCMCStepsMaximum, row++, 1);

    layout->setRowStretch(row++, 2);
    layout->setColumnStretch(2, 1);
    this->setLayout(layout);

    connect(advancedOptionsCheckBox,SIGNAL(toggled(bool)),this,SLOT(advancedOptionsSlotFunction(bool)));
}

UCSD_InputTMCMC::~UCSD_InputTMCMC()
{

}

bool UCSD_InputTMCMC::checkSampleSize(int sampleSize) {
    bool sizeOk = true;
    if (sampleSize < requiredSampleSize) {
        sizeOk = false;
        numSamplesError->setVisible(true);
        numParticles->setText(tr("100"));
        numSamplesWarning -> setVisible(false);
    } else if (sampleSize >= recommendedSampleSize) {
        numSamplesError->setVisible(false);
        numSamplesWarning -> setVisible(false);
    }   else {
        numSamplesError->setVisible(false);
        numSamplesWarning->setVisible(true);
    }
    return sizeOk;
}

// SLOT function
void UCSD_InputTMCMC::advancedOptionsSlotFunction(bool tog)
{

    lineA->setVisible(tog);

    if (tog) {
        advancedOptionsTitle->setStyleSheet("font-weight: bold; color: black");

    } else {
        advancedOptionsTitle->setStyleSheet("font-weight: bold; color: grey");

    }
}

bool
UCSD_InputTMCMC::outputToJSON(QJsonObject &jsonObj){

    bool result = true;

    if (numParticles->text().isEmpty()) {
        result = checkSampleSize(0);
    } else {
        result = checkSampleSize(numParticles->text().toInt());
    }

    jsonObj["numParticles"]=numParticles->text().toInt();
    jsonObj["seed"]=randomSeed->text().toInt();
//    jsonObj["maxRunTime"]=maxRunTime->text().toDouble();

    QString logLike = logLikelihoodScript->text();
    QFileInfo fileInfo(logLike);

    jsonObj["logLikelihoodFile"]=fileInfo.fileName();
    QString path = fileInfo.absolutePath();
    jsonObj["logLikelihoodPath"]=path;

    QString calFileName = calDataFileEdit->text();
    QFileInfo calFileInfo(calFileName);

    jsonObj["calDataFile"]=calFileInfo.fileName();
    QString calFilePath = calFileInfo.absolutePath();
    jsonObj["calDataFilePath"]=calFilePath;

    jsonObj["readUserDefinedCovarianceData"]=readCovarianceDataCheckBox->isChecked();

    jsonObj["numExperiments"] = numExperiments;

    return result;    
}

bool
UCSD_InputTMCMC::inputFromJSON(QJsonObject &jsonObject){

  bool result = true;
  this->clear();

  if (jsonObject.contains("numParticles") && jsonObject.contains("logLikelihoodFile") && jsonObject.contains("logLikelihoodPath") && jsonObject.contains("seed")
          && jsonObject.contains("calDataFile") && jsonObject.contains("calDataFilePath")) {

    int particles=jsonObject["numParticles"].toInt();
    numParticles->setText(QString::number(particles));
    if (numParticles->text().isEmpty()) {
        numSamplesError->setVisible(true);
        numSamplesWarning->setVisible(false);
    } else {
        int numPar = numParticles->text().toInt();
        checkSampleSize(numPar);
    };

    int seed=jsonObject["seed"].toInt();
    randomSeed->setText(QString::number(seed));

    QString file = jsonObject["logLikelihoodFile"].toString();
    QString path = jsonObject["logLikelihoodPath"].toString();
    if (!(file.trimmed().isEmpty() && path.trimmed().isEmpty())) {
        logLikelihoodScript->setText(path + "/" + file);
    }

//    double maxTime = jsonObject["maxRunTime"].toDouble();
//    maxRunTime->setText(QString::number(maxTime));

    QString calFile = jsonObject["calDataFile"].toString();
    QString calFilePath = jsonObject["calDataFilePath"].toString();
    if (!(calFile.trimmed().isEmpty() && calFilePath.trimmed().isEmpty())) {
        calDataFileEdit->setText(calFilePath + "/" + calFile);
    }

    if (jsonObject.contains("numExperiments")){
        numExperiments = jsonObject["numExperiments"].toInt();
    }
    else {
        QString calFileName = calDataFileEdit->text();
        numExperiments = getNumExp(calFileName);
    }


  }
  else {
          errorMessage("ERROR: TMCMC input - not all data specified");
          result = false;
      }

  return result;
}

void
UCSD_InputTMCMC::clear(void)
{

}


int
UCSD_InputTMCMC::getNumberTasks()
{
  return numParticles->text().toInt();
}

/*
int
UCSD_TMMC::getNumberTasks() {
    QFileDialog dialog(this, "Save Simulation Model");
    //dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList filters;
    filters << "Json files (*.json)"
            << "All files (*)";
    dialog.setNameFilters(filters);


    if (dialog.exec() != QDialog::Accepted)
        return false;

    // and save the file
    logLikelihoodScript->setText(dialog.selectedFiles().first());
}
*/

bool
UCSD_InputTMCMC::copyFiles(QString &fileDir) {

    QString calFileName = calDataFileEdit->text();
    if (calFileName != "") {

        QFileInfo calFileInfo(calFileName);
        QDir calDir = calFileInfo.dir();

        QFileInfo childDir(fileDir);
        //QDir dstDir = childDir.dir();
        QString dst = fileDir  + "/" + calFileInfo.fileName();

        qDebug() << "FileDir is: " << fileDir;
        //qDebug() << "tmp.SimCenter path is: " << dstDir.absolutePath();
        qDebug() << "calFileName is: " << calFileName;
        qDebug() << "Copying " << calFileName << " to " << dst << "\n";

        bool fileCopyResult = QFile::copy(calFileName, dst);

        qDebug() << "File copy result: " << QVariant(fileCopyResult).toString();

        int numExp = getNumExp(calFileName);

        qDebug() << "Number of experiments: " << numExp;

//        if (readCovarianceDataCheckBox->isChecked())
//        {
	  /************************ FMK *************************************
            qDebug() << "Looking for user-defined covariance files";

            QVector<EDP *> theEDPs = theEdpWidget->theEDPs;
            int numEDPs = theEDPs.size();

            qDebug() << "The number of response quantities: " << numEDPs;

            for (int i = 0; i < numEDPs; i++) {
                EDP *theEDP = theEDPs.at(i);
                QString edpName = theEDP->variableName->text().simplified();

                qDebug() << "Looping over EDPs. i: " << i << " edpName: " << edpName;

                for (int expNum = 1; expNum <= numExp; expNum++) {
                    QString covFileName = edpName + "." + QVariant(expNum).toString() + "." + "sigma";

                    qDebug() << "covFileName: " << covFileName;

                    QString srcFileName = calDir.absolutePath() + "/" + covFileName;

                    qDebug() << "srcFileName: " << srcFileName;

                    std::ifstream checkFile(srcFileName.toStdString());
                    if (checkFile.good()) {

                        qDebug() << "covFile found. Copying to tmp.SimCenter";

                        QString dstFileName = dstDir.absolutePath() + "/" + covFileName;

                        qDebug() << "dstFileName: " << dstFileName;

                        bool fileCopyResult = QFile::copy(srcFileName, dstFileName);

                        qDebug() << "File copy result: " << QVariant(fileCopyResult).toString();

                    }
                    else {
                        qDebug() << "Check failed. Not copying the file.";
                    }
                }
            }
	    ****************************** FMK ********************************************/
	  
//        }

    }

    QString loglikeFileName = logLikelihoodScript->text();

    if (loglikeFileName != "") {

        QFileInfo loglikeFileInfo(loglikeFileName);
        QDir loglikeDir = loglikeFileInfo.dir();

        QFileInfo childDir(fileDir);
        //QDir dstDir = childDir.dir();
        QString dst = fileDir  + "/" + loglikeFileInfo.fileName();

        qDebug() << "loglikeFileName is: " << loglikeFileName;
        qDebug() << "Copying " << loglikeFileName << " to " << dst << "\n";

        bool fileCopyResult = QFile::copy(loglikeFileName, dst);

        qDebug() << "File copy result: " << QVariant(fileCopyResult).toString();
    }

    return true;
}

int
UCSD_InputTMCMC::getNumExp(QString &calFileName) {

    std::ifstream calDataFile(calFileName.toStdString());
    int numExperiments = 0;
    std::string line;
    while (getline(calDataFile, line)) {
        if (!line.empty()) {
            ++numExperiments;
        }
    }
    return numExperiments;
}

