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

#include "MultiFidelityBuildingModel.h"
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
#include <QFormLayout>
#include <QDir>
#include <QJsonDocument>

#include "SC_FileEdit.h"
#include "MDOF_LU.h"
#include "Utils/JsonFunctions.h"
MultiFidelityBuildingModel::MultiFidelityBuildingModel(QWidget *parent)
  : SimCenterAppWidget(parent)
{

//    QVBoxLayout *layout = new QVBoxLayout();

//    QGroupBox* HighfidelityBox = new QGroupBox("Highfidelity model");
//    QHBoxLayout* HighfidelityLayout = new QHBoxLayout(HighfidelityBox);
//    model_database = new SC_FileEdit("buildingDatabase", QStringList({"json"}));
//    HighfidelityLayout->addWidget(new QLabel("Database file (.json)"));
//    HighfidelityLayout->addWidget(model_database);
//    HighfidelityLayout->addStretch();

//    QGroupBox* LowfidelityBox = new QGroupBox("Lowfidelity model (MDOF-LU)");
//    QHBoxLayout* LowfidelityLayout = new QHBoxLayout(LowfidelityBox);
//    mdofLU = new MDOF_LU();
//    LowfidelityLayout->addWidget(mdofLU);
//    LowfidelityLayout->addStretch();

//    layout->addWidget(HighfidelityBox);
//    layout->addWidget(LowfidelityBox);
//    this->setLayout(layout);

    //
    // High fidelity widget
    //

    QWidget *placeholderWidget1 = new QWidget();
    QGridLayout* HighfidelityLayout = new QGridLayout(placeholderWidget1);
    model_database = new SC_FileEdit("buildingDatabase", QStringList({"json"}));
    model_database->setMinimumWidth(600);
    connect(model_database,SIGNAL(fileNameChanged(QString)),this,SLOT(parseDatabase(QString)));

    model_details = new QVBoxLayout();
    HighfidelityLayout->addWidget(new QLabel("Database file (.json)"),0,0);
    HighfidelityLayout->addWidget(model_database,0,1);
    HighfidelityLayout->addLayout(model_details,1,1);
    HighfidelityLayout->setRowStretch(2,1);
    HighfidelityLayout->setColumnStretch(2,1);

    //
    // Low fidelity (MDOF) widget
    //

    QWidget *placeholderWidget2 = new QWidget();
    QHBoxLayout* LowfidelityLayout = new QHBoxLayout(placeholderWidget2);
    mdofLU = new MDOF_LU();
    LowfidelityLayout->addWidget(mdofLU);
    LowfidelityLayout->addStretch();

    //
    // set up tab widgets
    //

    QTabWidget *tabWidget= new QTabWidget();
    tabWidget->addTab(placeholderWidget1,"Highfidelity model");
    tabWidget->addTab(placeholderWidget2,"Lowfidelity model (MDOF-LU)");

    QVBoxLayout * placeholder = new QVBoxLayout();
    placeholder->addWidget(tabWidget);
    this->setLayout(placeholder);
}


MultiFidelityBuildingModel::~MultiFidelityBuildingModel()
{

}


void MultiFidelityBuildingModel::clear(void)
{
    //model_database->clear();
}

bool MultiFidelityBuildingModel::outputToJSON(QJsonObject &dataObj)
{
    model_database->outputToJSON(dataObj);
    mdofLU->outputToJSON(dataObj);
    return true;
}


bool MultiFidelityBuildingModel::inputFromJSON(QJsonObject &appData)
{
    model_database->inputFromJSON(appData);
    mdofLU->inputFromJSON(appData);
  return true;
}

bool MultiFidelityBuildingModel::copyFiles(QString &destDirName)
{
    //
    // copy files for MDOF Lu
    //
    mdofLU->copyFiles(destDirName);

    //
    // save the updated database.json
    //

    //model_database->copyFile(destDirName);
    QDir destDir(destDirName);
    QFileInfo jsonFile(model_database->getFilename());
    QJsonDocument jsonDoc(newJsonArr);
    QString jsonName = jsonFile.fileName();
    // Open the file for writing
    QFile file(destDir.absoluteFilePath(jsonName));
    if (!file.open(QIODevice::WriteOnly)) {
        errorMessage("Failed to open file for writing:" + file.errorString());
        return false; // Exit with error
    }
    // Write the JSON document to the file
    file.write(jsonDoc.toJson());
    file.close(); // Close the file

    //
    // copy all files
    //

    bool res;
    for (const QString &originDir : directoriesToBeCopied) {

        QFileInfo origDirInfo(originDir);
        QString destModelDir = destDir.absoluteFilePath(origDirInfo.fileName()); // each model will have separate directory

        res = SimCenterAppWidget::copyPath(originDir, destModelDir, true);

        if (res==false) {
            QString msg = QString("Failed copy folder : " ) + originDir + QString(" to " + destModelDir );
            this->errorMessage(msg);
            return false;
        }
    }

    return true;
}

bool MultiFidelityBuildingModel::outputAppDataToJSON(QJsonObject &jsonObject) {

    bool res = true;

    jsonObject["Application"] = "MultiFidelityBuildingModel";
    QJsonObject dataObj;

    jsonObject["ApplicationData"] = dataObj;

    return res;
}



void MultiFidelityBuildingModel::parseDatabase(QString jsonFilePath) {

    QJsonValue jsonVal;
    QString errorMsg;

    //
    // initialize widgets and parameters
    //

    this -> clearModelDetails(); // remove all displayed contents
    directoriesToBeCopied = QStringList();
    QFileInfo jsonFile(jsonFilePath);
    QString baseDir = jsonFile.path(); // incase path is relative

    //
    // Sanity check
    //

    bool status = parseJsonFile(jsonFilePath, jsonVal, errorMsg);
    if (status==false) {
        errorMessage(errorMsg);
        return;
    }

    QJsonObject jsonObjSchema = getDatabaseSchema();

    if (!jsonVal.isArray()) {
        errorMessage("Error: Database file should be a json array. See documentation");
        return;
    }

    if (!validateJson(jsonVal, jsonObjSchema, errorMsg, "database json")) {
        errorMessage(errorMsg);
        return;
    } else {
        infoMessage("Databse json successfully loaded");
    }


    QJsonArray jsonArr = jsonVal.toArray();


    //
    // enumerate each model
    //

    newJsonArr = QJsonArray();
    int count = 0;
    for (const QJsonValue &value : jsonArr) {
        count ++;

        QJsonObject model = value.toObject();
        //
        // Parse Files and get file name and path
        //


        QString fullPath = model["mainScript"].toString();

        QString fileName;
        QString fileDir;
         ;
        //first check the absolute path
        QFileInfo fileInfo(fullPath);
        QDir dir;
        if (!fileInfo.exists()) {
            //next check the path relative to the database.json file
            QDir dir(baseDir); // curent (json) dir
            QString fullPath2 = dir.filePath(fullPath);
            fileInfo = QFileInfo(fullPath2);
            if (!fileInfo.exists()) {
                errorMessage("File not found: " +fullPath);
                return;
            }
        }

        fileName = fileInfo.fileName();
        QDir jsonDir(fileInfo.path());
        fileDir = jsonDir.canonicalPath(); // This is a clean absolute path

        if (!directoriesToBeCopied.contains(fileDir)) {
                directoriesToBeCopied.append(fileDir);
        }

        model["mainFolder"] = jsonDir.dirName();
        model["mainScript"] = fileName;
        newJsonArr.append(model);

        if (count==20) {
            QLabel *model_limit = new QLabel("Only the first 20 buildings are displayed");
            model_limit->setObjectName(QString::fromUtf8("titleText"));
            model_details ->addWidget(model_limit);
            continue;
        }

        //
        // parse IDs
        //

        QString idStr;
        QJsonValue idValue = model["id"];
        if (idValue.isDouble()) {
            idStr = QString::number(idValue.toInt());

        } else if (idValue.isArray()) {
            QJsonArray idArray = idValue.toArray();
            QStringList stringList;  // Create a QStringList to hold the string representations
            for (const QJsonValue &value : idArray) {
                if (value.isDouble()) {  // Check if the value is a number
                    stringList << QString::number(value.toInt());  // Convert to QString
                }
            }
            idStr = stringList.join(", ");
        } else {
            errorMessage("id should be eiter an integer or a list of integer values");
        }

        //
        // Display the model info
        //

        QLabel *model_title = new QLabel("Model " + QString::number(count));
        model_title->setObjectName(QString::fromUtf8("titleText"));
        model_details ->addWidget(model_title);

        QFormLayout* modelFormLayout = new QFormLayout();
        QLabel *idStrLabel = new QLabel(idStr);
        idStrLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        QLabel *typeLabel = new QLabel(model["type"].toString());
        typeLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        QLabel *fileNameLabel = new QLabel(fileName);
        fileNameLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        QLabel *fileDirLabel = new QLabel(fileDir);
        fileDirLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        QLabel *numStoryLabel = new QLabel(QString::number(model["numStory"].toInt()));
        numStoryLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);


        modelFormLayout->addRow(tr("Bldg index: "), idStrLabel);
        modelFormLayout->addRow(tr("Type: "), typeLabel);
        modelFormLayout->addRow(tr("Main Script: "), fileNameLabel);
        modelFormLayout->addRow(tr("Main Directory: "), fileDirLabel);
        modelFormLayout->addRow(tr("Number of Stories: "), numStoryLabel);
        modelFormLayout->setLabelAlignment(Qt::AlignRight);

        model_details ->addLayout(modelFormLayout);


    }
    // parse data
}

void MultiFidelityBuildingModel::clearModelDetails() {

    if (!model_details)
        return;

    // Remove and delete each widget in the layout
    while (QLayoutItem *item = model_details->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater(); // Safely delete the widget
        } else if (QLayout *subLayout = item->layout()) {
            // If the item has a sub-layout, recursively delete its widgets
            while (QLayoutItem *subItem = subLayout->takeAt(0)) {
                if (QWidget *subWidget = subItem->widget()) {
                    subWidget->deleteLater(); // Safely delete the sub-widget
                }
                delete subItem; // Delete the sub-layout item
            }
        }
        delete item; // Delete the layout item
    }
}


bool MultiFidelityBuildingModel::inputAppDataFromJSON(QJsonObject &jsonObject) {
    return true;
}

QJsonObject MultiFidelityBuildingModel::getDatabaseSchema() {
    //
    // Testing out json schema
    //

    QJsonObject schema{{
            {"type", "array"},
            {"items", QJsonObject{
                {"type", "object"},
                {"properties", QJsonObject{
                    {"id", QJsonObject{
                        {"type", "array"},
                        {"items", QJsonObject{{"type", "integer"}}}
                    }},
                    {"mainScript", QJsonObject{{"type", "string"}}},
                    {"type", QJsonObject{{"type", "string"}}},
                    {"NodeMapping", QJsonObject{
                        {"type", "array"},
                        {"items", QJsonObject{
                            {"type", "object"},
                            {"properties", QJsonObject{
                                {"node", QJsonObject{{"type", "integer"}}},
                                {"cline", QJsonObject{{"type", "string"}}},
                                {"floor", QJsonObject{{"type", "string"}}}
                            }},
                            {"required", QJsonArray{"node", "cline", "floor"}}
                        }}
                    }},
                    {"numStory", QJsonObject{{"type", "integer"}}},
                    {"ndm", QJsonObject{{"type", "integer"}}},
                    {"ndf", QJsonObject{{"type", "integer"}}},
                    {"dampingRatio", QJsonObject{{"type", "number"}}}
                }},
                {"required", QJsonArray{"id", "mainScript", "type", "NodeMapping", "numStory", "ndm", "ndf", "dampingRatio"}}
            }}
        }
    };

    return schema;
}

bool MultiFidelityBuildingModel::outputCitation(QJsonObject &jsonObject) {
    // TBA - MDOF Lu
    mdofLU->outputCitation(jsonObject);
    return true;
}


