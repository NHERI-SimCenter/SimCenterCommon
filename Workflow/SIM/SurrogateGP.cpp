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

// Written: fmckenna,sangri



#include "SurrogateGP.h"
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <QFileInfo>
#include <string>
#include <sstream>
#include <iostream>
//using namespace std;
#include <QGridLayout>
#include <QJsonDocument>

//#include <surrogateGpParser.h>
#include <RandomVariablesContainer.h>
#include "SurrogateEDP.h"


surrogateGP::surrogateGP(QWidget *parent)
  : SimCenterAppWidget(parent)
{

    QGridLayout *layout = new QGridLayout();

    QLabel *label1 = new QLabel("SurrogateGP Info (.json)");
    inputScript = new QLineEdit;
    QPushButton *chooseMS_Button = new QPushButton();
    chooseMS_Button->setText(tr("Choose"));
    connect(chooseMS_Button, &QPushButton::clicked, this, [this](){
        QString mainScriptName=QFileDialog::getOpenFileName(this,tr("Open File"),
                              "",
                              "Json files (*.json)");
        if(!mainScriptName.isEmpty()) {
            this->setMainScript(mainScriptName);
        }
    });
    //connect(inputScript, SIGNAL(textEdited(QString)),SLOT(showGpOptions(QString)));
    connect(inputScript, SIGNAL(textChanged(QString)),SLOT(showGpOptions(QString)));

    layout->addWidget(label1,0,0);
    layout->addWidget(inputScript,0,1);
    layout->addWidget(chooseMS_Button,0,2);

    label2 = new QLabel("SurrogateGP Model (.pkl)");
    postprocessScript = new QLineEdit;
    postprocessScript->setPlaceholderText("");
    choosePostprocessScript = new QPushButton();
    choosePostprocessScript->setText(tr("Choose"));
    connect(choosePostprocessScript, &QPushButton::clicked, this, [this](){
      QString selectedFile = QFileDialog::getOpenFileName(this,
                              tr("Postprocess Script"),
                              "",
                              "Pickle files (*.pkl)");

        if(!selectedFile.isEmpty()) {
            postprocessScript->setText(selectedFile);
        }
    });

    groupBox = new QGroupBox("Options");
    QGridLayout *optionsLayout = new QGridLayout(groupBox);

    QCheckBox *advanced = new QCheckBox("Advanced Options");
    QGroupBox * advancedOptions = new QGroupBox();
    QVBoxLayout * advancedLayout = new QVBoxLayout(advancedOptions);

    // Set Threshold value
    QHBoxLayout * labelVarThresLayout = new QHBoxLayout();
    thresVal = new QLineEdit("0");
    thresVal->setMaximumWidth(100);
    thresVal->setValidator(new QDoubleValidator);
    connect(thresVal,SIGNAL(textEdited(QString)), this, SLOT(updateMessage(QString)));
    labelVarThresLayout->addWidget( new QLabel("Maximum Allowable Normalized Variance  "));
    labelVarThresLayout->addWidget(thresVal);
    labelVarThresLayout->addStretch(1);

    labelThresMsg = new QLabel(" ");
    labelThresMsg->setStyleSheet("color: red");
    advancedLayout->addLayout(labelVarThresLayout);
    advancedLayout->addWidget(labelThresMsg);

    // Threshold exceeded
    QLabel *optionsLabel = new QLabel("When surrogate model gives imprecise prediction at certain sample locations");
    option1Button = new QRadioButton("Stop Analysis");
    option2Button = new QRadioButton("Give warning and Continue (recommended)");
    option3Button = new QRadioButton("Run Exact FEM Simulation");
    option2Button->setChecked(true);
    femWidget = new QWidget();

    QHBoxLayout *tmpDirLayout = new QHBoxLayout(femWidget);
    QLabel *labelTempDir = new QLabel("Template Directory");
    tempDir = new QLineEdit;
    QPushButton *chooseTD_Button = new QPushButton();
    chooseTD_Button->setText(tr("Choose"));
    connect(chooseTD_Button, &QPushButton::clicked, this, [this](){
        QString mainScriptName=QFileDialog::getExistingDirectory(this,tr("Open Folder"),"");
        if(!mainScriptName.isEmpty()) {
            tempDir->setText(mainScriptName);
        }
    });
    tmpDirLayout->addWidget(labelTempDir);
    tmpDirLayout->addWidget(tempDir);
    tmpDirLayout->addWidget(chooseTD_Button);

    //QVBoxLayout *femLayout = new QVBoxLayout(femWidget);
    //labelProgName=new QLabel();
    //labelProgDir1=new QLabel();
    //labelProgDir2=new QLabel();

    //femLayout->addLayout(tmpDirLayout);
    //femLayout->addWidget(labelProgName);
    //femLayout->addWidget(labelProgDir1);
    //femLayout->addWidget(labelProgDir2);
    femWidget->setVisible(false);


    advancedLayout->addWidget(optionsLabel);
    advancedLayout->addWidget(option1Button);
    advancedLayout->addWidget(option2Button);
    advancedLayout->addWidget(option3Button);
    advancedLayout->addWidget(femWidget);

    advancedOptions->hide();
    connect(advanced, &QCheckBox::clicked, this, [=](bool tog){
        if (tog==false)
        {
            advancedOptions->hide();
        } else {
            advancedOptions->show();
        }
    });
    connect(option3Button, &QRadioButton::toggled, this, [=](bool tog){
        if (tog==false)
        {
            tempDir->setText("");
            femWidget->hide();
        } else {
            femWidget->show();
        }
    });
    //femWidget = new QWidget();
    //QVBoxLayout *femLayout = new QVBoxLayout();
    //femWidget->setLayout(femLayout);
    //labelProgName=new QLabel();
    //labelProgDir1=new QLabel();
    //labelProgDir2=new QLabel();
    //femLayout->addWidget(labelProgName);
    //femLayout->addWidget(labelProgDir1);
    //femLayout->addWidget(labelProgDir2);
    //femWidget->setVisible(false);

    QHBoxLayout * gpOutputLayout = new QHBoxLayout();
    gpOutputComboBox= new QComboBox;
    gpOutputComboBox->addItem("Median (representative) prediction");
    gpOutputComboBox->addItem("Random sample under prediction uncertainty");
    gpOutputComboBox->setCurrentIndex(1);
    gpOutputLayout->addWidget( new QLabel("GP output   "));
    gpOutputLayout->addWidget(gpOutputComboBox);
    gpOutputComboBox->setMinimumWidth(400);
    gpOutputLayout->addStretch(1);


    qoiNames = new QLabel();
    qoiNames->setTextInteractionFlags(Qt::TextSelectableByMouse);
    //optionsLayout->addWidget(qoiNames, 2,0);
    optionsLayout->addLayout(gpOutputLayout, 3,0,1,-1);
//    optionsLayout->addWidget(optionsLabel, 2,0,1,-1);
//    optionsLayout->addWidget(option1Button, 3,0,1,-1);
//    optionsLayout->addWidget(option2Button, 4,0,1,-1);
//    optionsLayout->addWidget(option3Button, 5,0,1,-1);
    optionsLayout->addWidget(advanced, 4,0,1,-1);
    optionsLayout->addWidget(advancedOptions, 5,0,1,-1);

    //optionsLayout->addWidget(femWidget, 6,0);

    optionsLayout->setColumnStretch(6,1.0);
    //optionsLayout->setSpacing(0);
    //optionsLayout->setHorizontalSpacing(0);
    //optionsLayout->setContentsMargins(0,0,0,0);
    layout->addWidget(label2,1,0);
    layout->addWidget(postprocessScript,1,1);
    layout->addWidget(choosePostprocessScript,1,2);
    label2->hide();
    postprocessScript->hide();
    choosePostprocessScript->hide();
    layout->addWidget(groupBox,2,0,1,3);
    groupBox->setVisible(false);
    layout->setRowStretch(3,1.0);
    layout->setColumnStretch(1,4);
    layout->setColumnStretch(4,1);
    this->setLayout(layout);


    theSurrogateEDPs = SurrogateEDP::getInstance();
}

surrogateGP::~surrogateGP()
{

}


void surrogateGP::clear(void)
{
  QStringList names;
  for (int i=0; i<varNamesAndValues.size()-1; i+=2) {
         names.append(varNamesAndValues.at(i));
  }

  RandomVariablesContainer *theRVs=RandomVariablesContainer::getInstance();
  theRVs->removeRandomVariables(names);
  postprocessScript->setText("");
  inputScript->setText("");
}
void surrogateGP::updateMessage(QString textval){
    double thres=textval.toDouble();
    double percEst = this->interpolateForGP(thrsVals,percVals,thres)*100;
    if (thres>thrsVals[thrsVals.size()-1]) {
        percEst=percVals[percVals.size()-1]*100;
    } else if (thres<thrsVals[0]) {
        percEst=percVals[0]*100;
    }
    if (!isData) {
        labelThresMsg->setVisible(true);
        labelThresMsg->setText("Note: around " + QString::number(percEst, 'f', 1) + "% of new samples in training range will exceed the tolerance limit.");
    } else {
        labelThresMsg->setVisible(false);
    }
};


bool
surrogateGP::inputFromJSON(QJsonObject &jsonObject)
{

//  varNamesAndValues.clear();

//  if (jsonObject.contains("randomVariables")) {
//    QJsonArray randomVars = jsonObject["randomVariables"].toArray();
//    foreach (const QJsonValue & value, randomVars) {
//      QJsonObject theRV = value.toObject();
//      QString name = theRV["name"].toString();
//      QString zero = "0";
//      varNamesAndValues.append(name);
//      varNamesAndValues.append(zero);
//    }
//  }

  return true;
}

bool
surrogateGP::outputToJSON(QJsonObject &jsonObject) {
     Q_UNUSED(jsonObject);
  return true;
}


bool
surrogateGP::outputAppDataToJSON(QJsonObject &jsonObject) {

    bool result = true;
    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    jsonObject["type"]="SurrogateGPBuildingModel";
    jsonObject["Application"] = "SurrogateGPBuildingModel";
    QJsonObject dataObj;

    QString mainScriptName = inputScript->text();
    QFileInfo fileInfo(mainScriptName);

    if (fileInfo.exists() && fileInfo.isFile()) {
        dataObj["mainScript"]=fileInfo.fileName();
        dataObj["MS_Path"]=fileInfo.path();
    } else {
        QString msg = QString("surrogateGP - mainScript " ) + mainScriptName + QString(" does not exist!");
        this->errorMessage(msg);
        dataObj["mainScript"]=mainScriptName;
        dataObj["MS_Path"]=QString("");
        result = false;
    }

    QString mainScriptName1 = postprocessScript->text();
    QFileInfo fileInfo1(mainScriptName1);

    if (fileInfo1.exists() && fileInfo1.isFile()) {
        dataObj["postprocessScript"]=fileInfo1.fileName();
        dataObj["PS_Path"]=fileInfo1.path();
    } else {
        if (mainScriptName1.isEmpty()) {
            dataObj["postprocessScript"]=QString("");
        } else {
            QString msg = QString("surrogateGP - postprocessScript " ) + mainScriptName1 + QString(" does not exist!");
            this->errorMessage(msg);
            dataObj["postprocessScript"]=mainScriptName1;
            dataObj["PS_Path"]=QString("");
            result = false;
        }
    }

    jsonObject["ApplicationData"] = dataObj;

    jsonObject["varThres"]=thresVal->text().toDouble();
    if (option1Button->isChecked()) {
        jsonObject["femOption"]="giveError";
    } else if (option2Button->isChecked()) {
        jsonObject["femOption"]="continue";
    } else if (option3Button->isChecked()) {
        jsonObject["femOption"]="doSimulation";
    }
    jsonObject["predictionOption"]=gpOutputComboBox->currentText();
    jsonObject["gpSeed"]=1;
    return true; // needed for json file to save, copyFiles will return false
}

 bool
surrogateGP::inputAppDataFromJSON(QJsonObject &jsonObject) {

  //
  // from ApplicationData
  //

  if (jsonObject.contains("ApplicationData")) {
    QJsonObject dataObject = jsonObject["ApplicationData"].toObject();

    //
    // retrieve mainScriptName and path, set the QLIne Edit
    //

    QString mainScriptName;
    QString filePath;

    if (dataObject.contains("mainScript")) {
      QJsonValue theName = dataObject["mainScript"];
      mainScriptName = theName.toString();
    } else
        return false;

    if (dataObject.contains("MS_Path")) {
        QJsonValue theName = dataObject["MS_Path"];
        filePath = theName.toString();
    } else
        return false;

    inputScript->setText(QDir(filePath).filePath(mainScriptName));
    setMainScript(inputScript->text());

    if (dataObject.contains("postprocessScript")) {
        QJsonValue theName = dataObject["postprocessScript"];
        mainScriptName = theName.toString();

        if (dataObject.contains("PS_Path")) {
            QJsonValue theName = dataObject["PS_Path"];
            filePath = theName.toString();
            postprocessScript->setText(QDir(filePath).filePath(mainScriptName));
        } else
            postprocessScript->setText(mainScriptName);
    } else {
        postprocessScript->setText("");
    }


      thresVal->setText(QString::number(dataObject["varThres"].toDouble()));
      QString femOpt=dataObject["femOption"].toString();
      if (femOpt == "giveError") {
          option1Button ->setChecked(true);
      } else if (femOpt == "continue") {
          option2Button ->setChecked(true);
      } else if (femOpt == "doSimulation") {
          option3Button ->setChecked(true);
      }
      gpOutputComboBox->setCurrentText(dataObject["predictionOption"].toString());
  } else
      return false;

  return true;
}


void
surrogateGP::setMainScript(QString name1){

    // remove old random variables
    QStringList names;
    for (int i=0; i<varNamesAndValues.size()-1; i+=2) {
        names.append(varNamesAndValues.at(i));
    }

    RandomVariablesContainer *theRVs= RandomVariablesContainer::getInstance();
    theRVs->removeRandomVariables(names);

    // set file name & ebtry in qLine edit

    inputScript->setText(name1);

    //
    // parse file for random variables and add them
    //

    bool isMultiFidelity = true;

    QFile file(name1);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString val;
        val=file.readAll();
        file.close();
        val.replace(QString("NaN"),QString("null"));
        val.replace(QString("Infinity"),QString("inf"));

        QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
        QJsonObject jsonSur = doc.object();


        auto GPidentifier = jsonSur.find("kernName"); // should be the right .json file
        if (!jsonSur.isEmpty() && GPidentifier != jsonSur.end()) {
            QJsonArray RVArray = jsonSur["randomVariables"].toArray();
            foreach (const QJsonValue & v, RVArray){
                 QJsonObject jsonRV = v.toObject();
                 QString vname = jsonRV["name"].toString();
                 if (vname!=QString("MultipleEvent"))  {
                     varNamesAndValues.push_back(vname);
                     varNamesAndValues.push_back(QString::number(jsonRV["value"].toDouble()));
                 }
            }
            isMultiFidelity = jsonSur["doMultiFidelity"].toBool();
        }
    }
   theRVs->addRVsWithValues(varNamesAndValues);
   if (isMultiFidelity) {
       label2->show();
       postprocessScript->show();
       choosePostprocessScript->show();
        //postprocessScript->setDisabled(false);
        //choosePostprocessScript->setStyleSheet("background-color: dodgerblue;border-color:dodgerblue");
        //choosePostprocessScript->setDisabled(false);
        //postprocessScript->setStyleSheet("QLineEdit { background: white; selection-background-color: white; }");
   } else {
       label2->hide();
       postprocessScript->hide();
       choosePostprocessScript->hide();
       postprocessScript->setText("");
       //postprocessScript->setDisabled(true);
       //postprocessScript->setStyleSheet("QLineEdit { background: lightgrey; selection-background-color: lightgrey; }");
       //choosePostprocessScript->setDisabled(true);
       //choosePostprocessScript->setStyleSheet({ "background-color: lightgrey; border: none;" });

   }

    return;
}
void
surrogateGP::showGpOptions(QString name1) {

    option2Button->setChecked(true);

    QFile file(name1);
    QString appName, mainScriptDir,postScriptDir;
    QStringList qoiNames_list;


    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString val;
        val=file.readAll();
        val.replace(QString("NaN"),QString("null"));
        val.replace(QString("Infinity"),QString("inf"));
        QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
        QJsonObject jsonSur = doc.object();
        file.close();

        auto GPidentifier = jsonSur.find("kernName"); // should be right .json file
        QJsonObject jsonHif = jsonSur["highFidelityInfo"].toObject();
        if (!jsonSur.isEmpty() && GPidentifier != jsonSur.end()) {
            bool from_data=!jsonHif["doSimulation"].toBool();
            QJsonArray RVArray = jsonSur["randomVariables"].toArray();
            QJsonArray QoIArray = jsonSur["ylabels"].toArray();

            foreach (const QJsonValue & v, QoIArray){
                 qoiNames_list.push_back(v.toString());
            }
            //qoiNames->setText(QString("QoI list: ") + qoiNames_list.join(", "));

            if (from_data) {
                isData = true;
                appName = "data";
                option3Button->setDisabled(true);
                thresVal->setText("0.02");
                groupBox->setVisible(true);
            } else {
                isData = false;
                option3Button->setDisabled(false);
                QJsonObject jsonPred = jsonSur["predError"].toObject();
                QJsonArray precArray = jsonPred["percent"].toArray();
                QJsonArray valsArray = jsonPred["value"].toArray();
                //QJsonObject jsonFEM = jsonSur["fem"].toObject();
                // interpolate
                QVector<double> percVal_tmp, thrsVal_tmp;
                foreach (const QJsonValue & v, precArray)
                     percVal_tmp.push_back(v.toDouble());
                foreach (const QJsonValue & v, valsArray)
                     thrsVal_tmp.push_back(v.toDouble());
                percVals=percVal_tmp;
                thrsVals=thrsVal_tmp;
                thresVal->setText(QString::number(this->interpolateForGP(percVal_tmp,thrsVal_tmp,0.5)));
                this->updateMessage( thresVal->text());
                // save names
                //appName = jsonFEM["program"].toString();
                //mainScriptDir = jsonFEM["inputFile"].toString();
                //postScriptDir = jsonFEM["postprocessScript"].toString();
                //labelProgName->setText("      • Application Name: " + appName);
                //labelProgDir1->setText("      • Main Script: "+ mainScriptDir);
                //labelProgDir2->setText("      • Postprocess Script: "+ postScriptDir );
                groupBox->setVisible(true);
            }
        } else {
            appName = "NA";
            thresVal->setText("0");
            groupBox->setVisible(false);
        }
    } else {
        appName = "NA";
        thresVal->setText("0");
        groupBox->setVisible(false);
    }



    theSurrogateEDPs->addEDPs(qoiNames_list);
}

void
surrogateGP::specialCopyMainScript(QString mainScriptName, QStringList varNames) {
    // if surrogateGP or FEAP parse the file for the variables
    if (varNames.size() > 0) {
//        surrogateGpParser theParser;
//        theParser.writeFile(inputScript->text(), mainScriptName, varNames);
    }
}

bool
surrogateGP::copyFiles(QString &dirName) {

    //
    // main script
    //

     QString mainScriptName = inputScript->text();

     if (mainScriptName.isEmpty()) {
         this->errorMessage("surrogateGPInput - no file set");
         return false;
     }
     QFileInfo fileInfo(mainScriptName);

     QString theFile = fileInfo.fileName();
     //QString thePath = fileInfo.path();

     QFile::copy(mainScriptName, dirName + QDir::separator() + theFile);

     //
     // postprocess script
     //

     QString pklName = postprocessScript->text();

     if (!pklName.isEmpty()) {
         QFileInfo pfileInfo(pklName);

         QString thePklFile = pfileInfo.fileName();
         //QString thePath = fileInfo.path();

         QFile::copy(pklName, dirName + QDir::separator() + thePklFile);
     }

     //
     // template dir
     //

     QString theTmpDir = tempDir->text();
     qDebug() << theTmpDir;
     bool res = true;
     if (theTmpDir!="") {
        res = SimCenterAppWidget::copyPath(theTmpDir, dirName + QDir::separator() + "templatedir_SIM", true);
     }

//     RandomVariablesContainer *theRVs=RandomVariablesContainer::getInstance();
//     QStringList varNames = theRVs->getRandomVariableNames();

//     // now create special copy of original main script that handles the RV
//     QString copiedFile = dirName + QDir::separator() + theFile;

     if (res==false) {
         QString msg = QString("Failed copy folder : " ) + theTmpDir;
         this->errorMessage(msg);
     }
     return res;
}

double
surrogateGP::interpolateForGP(QVector<double> X, QVector<double> Y, double Xval){
    int N = X.count();

    if (X.size()==1) {
        return 0; // make it %
    }

    double estY=0;
    if (X[0]<X[1]) {
        for (int np=0; np<N; np++) {
            if(X[np] > Xval) {
                if (np==0)
                    estY = X[0];
                else
                    estY = Y[np-1]+(Y[np]-Y[np-1])*(Xval-X[np-1])/(X[np]-X[np-1]);
                break;
            }
        }
    } else {
        for (int np=N-1; np>-1; np--) {
            if(X[np] > Xval) {
                if (np==0)
                    estY = X[0];
                else
                    estY = Y[np-1]+(Y[np]-Y[np-1])*(Xval-X[np-1])/(X[np]-X[np-1]);
                break;
            }
        }
    }

    return (estY);
}
