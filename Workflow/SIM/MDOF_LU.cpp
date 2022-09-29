/* *****************************************************************************
Copyright (c) 2016-2021, The Regents of the University of California (Regents).
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

// Written by: Frank McKenna

#include "MDOF_LU.h"
#include "SimCenterPreferences.h"

#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

MDOF_LU::MDOF_LU(QWidget *parent)
  : SimCenterAppWidget(parent)
{
    QGridLayout *layout = new QGridLayout();

    QLabel *hazusLabel = new QLabel("Hazus Data File");
    hazusDataFile = new QLineEdit;
    QPushButton *hazusButton = new QPushButton("Choose");

    auto SCPrefs = SimCenterPreferences::getInstance();

    QString filePath = SCPrefs->getAppDir() + QDir::separator()
            + "applications" + QDir::separator() + "createSAM"
            + QDir::separator() + "MDOF-LU" + QDir::separator()
            + "data" + QDir::separator() + "HazusData.txt";
    QFileInfo hFile(filePath);
    if (hFile.exists()) {
        hazusDataFile->setText(filePath);
    }

    connect(hazusButton,SIGNAL(clicked()),this,SLOT(chooseFileName1()));
    layout->addWidget(hazusLabel,    0,0);
    layout->addWidget(hazusDataFile, 0,1);
    layout->addWidget(hazusButton,   0,2);

    QLabel *stiffnessLabel= new QLabel("std deviation Stiffness:");
    stdStiffness = new QLineEdit;
    stdStiffness->setText("0.1");
    layout->addWidget(stiffnessLabel, 1,0);
    layout->addWidget(stdStiffness,   1,1);

    QLabel *dampingLabel= new QLabel("std deviation Damping:");
    stdDamping = new QLineEdit;
    stdDamping->setText("0.1");
    layout->addWidget(dampingLabel, 2,0);
    layout->addWidget(stdDamping,   2,1);

    storyHeight = new QLineEdit;
    storyHeight->setText("");
    layout->addWidget(new QLabel("Default Story Height:"), 3,0);
    layout->addWidget(storyHeight,   3,1);

    auto citation1 = new QLabel("This backend application used by this selection was provided by Prof. Xinzheng Lu, Tsinghua University, China. "
                                       "Users should cite the work as follows:");

    auto citation2 = new QLabel("(1) Lu X.Z., McKenna F., Cheng Q.L., Xu Z., Zeng X., and Mahin S.A.,"
                " \"An open-source framework for regional earthquake loss estimation using the city-scale nonlinear time-history analysis\" "
                ", Earthquake Spectra, 2020, 36(2): 806-831.");
    auto citation3 = new QLabel("(2) Lu X.Z., Han B., Hori M., Xiong C., and Xu Z.,"
                                " \"A coarse-grained parallel approach for seismic damage simulations of urban areas based on refined models"
                                "and GPU/CPU cooperative computing\" , Advances in Engineering Software,"
                                "2014, 70: 90-103.");
    citation1->setWordWrap(true);
    citation2->setWordWrap(true);
    citation3->setWordWrap(true);

    QLabel *spacer = new QLabel("");
    layout->addWidget(spacer,4,0);
    layout->addWidget(citation1, 5, 0, 1,2);
    layout->addWidget(citation2, 6, 0, 1, 2);
    layout->addWidget(citation3, 7, 0, 1, 2);
    layout->setRowStretch(8,1);

    this->setLayout(layout);
}


MDOF_LU::~MDOF_LU()
{

}


void MDOF_LU::clear(void)
{
    hazusDataFile->clear();
    stdStiffness->clear();
    stdDamping->clear();
}


bool MDOF_LU::outputToJSON(QJsonObject &dataObj)
{
    dataObj["stdStiffness"] = stdStiffness->text().toDouble();
    dataObj["stdDamping"] = stdDamping->text().toDouble();

    if(storyHeight->text() != "") {
        dataObj["storyHeight"] = stdDamping->text();
    }

    QFileInfo theFile(hazusDataFile->text());
    if (theFile.exists()) {
        dataObj["hazusData"]=theFile.fileName();
        dataObj["pathToHazusFile"]=theFile.path();
    } else {
        dataObj["hazusData"]=QString("None");
        dataObj["pathToHazusFile"]=QString("");
        return false;
    }
  
    return true;
}


bool MDOF_LU::inputFromJSON(QJsonObject &appData)
{

  QFileInfo fileInfo;
  QString fileName;
  QString pathToFile;
  
  if (appData.contains("stdStiffness"))
    stdStiffness->setText(QString::number(appData["stdStiffness"].toDouble()));
  
  if (appData.contains("stdDamping"))
    stdDamping->setText(QString::number(appData["stdDamping"].toDouble()));
  
  if (appData.contains("hazusData"))
    fileName = appData["hazusData"].toString();
  
  if (appData.contains("storyHeight"))
    storyHeight->setText(QString::number(appData["storyHeight"].toDouble()));
  
  //
  // hazus file .. a number of options
  //  1 is user created input the full path can be specified
  //  2 if user specified, it can be relative to current dir
  //  3 if use specified it can be in input_Data
  //  4 if R2D created the path is seperate
  //
  
  if (fileInfo.exists(fileName)) {
    
    // option 1
    hazusDataFile->setText(fileName);
    return true;
    
  } else {
    
    if (appData.contains("pathToHazusFile"))
      pathToFile = appData["pathToHazusFile"].toString();
    else
      pathToFile = QDir::currentPath();
    
    QString hazusFile = pathToFile + QDir::separator() + fileName;
    
    if (fileInfo.exists(hazusFile)) {
      
      // option 2 or 4
      hazusDataFile->setText(hazusFile);
      return true;
      
    } else {
      
      // option 3
      // adam .. adam .. adam
      hazusFile = pathToFile + QDir::separator()
	+ "input_data" + QDir::separator() + fileName;
      
      if (fileInfo.exists(hazusFile)) {
	hazusDataFile->setText(hazusFile);
	return true;
      }
    }
  }
  
  return true;
}


bool MDOF_LU::outputAppDataToJSON(QJsonObject &jsonObject) {

    bool res = true;

    jsonObject["Application"] = "MDOF-LU";
    QJsonObject dataObj;


    jsonObject["ApplicationData"] = dataObj;

    return res;
}


bool MDOF_LU::inputAppDataFromJSON(QJsonObject &jsonObject) {

    return true;
}


void MDOF_LU::chooseFileName1(void) {
    QString filePath=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All Files (*.*)");
    hazusDataFile->setText(filePath);
}


bool MDOF_LU::copyFiles(QString &dirName) {

    QString fileName = hazusDataFile->text();
    QFileInfo fileInfo(fileName);

    if (fileInfo.exists()) {
        return this->copyFile(fileName, dirName);
    }

    return true;
}

