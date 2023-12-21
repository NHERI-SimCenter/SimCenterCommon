
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

// Written: fmckenna, bsaakash

#include "UCSD_InputHierarchicalBayesian.h"
#include <QCollator>
#include <QIntValidator>
#include <time.h>
#include <QFileDialog>
#include <QDebug>
#include <QJsonValue>
#include <QJsonArray>
#include <SimCenterAppWidget.h>
#include "RandomVariablesContainer.h"
#include "UCSD_ResultsHierarchicalBayesian.h"

UCSD_InputHierarchicalBayesian::UCSD_InputHierarchicalBayesian(QWidget *parent) : UCSD_UQ_Method(parent)
{
    sampleSizeLineEdit = new QLineEdit();
    sampleSizeLineEdit->setText(tr("500"));
    QIntValidator *positiveIntegerValidator = new QIntValidator;
    positiveIntegerValidator->setBottom(0);
    sampleSizeLineEdit->setValidator(positiveIntegerValidator);
    sampleSizeLineEdit->setToolTip("Specify the number of sample values to be drawn from the posterior");

    srand(time(NULL));
    int randomNumber = arc4random_uniform(1000) + 1;
    randomStateLineEdit = new QLineEdit();
    randomStateLineEdit->setText(QString::number(randomNumber));
    randomStateLineEdit->setValidator(positiveIntegerValidator);
    randomStateLineEdit->setToolTip("Specify the random state used by the pseudo random number generator. This is used for reproducibility.");

    restartFileLineEdit = new QLineEdit();
    restartFileLineEdit->setToolTip("Enter the name of the file containing the data from a previous run of the sampling algorithm to restart the Markov chains. This file must be in the directory containing the model scripts or the directory containing the calibration datasets.");
//    connect(restartFileLineEdit, &QLineEdit::textChanged, this, &UCSD_InputHierarchicalBayesian::updateCalDataFileName);


    calDataFileLineEdit = new QLineEdit();
    calDataFileLineEdit->setToolTip("Enter the name of the file containing the output data from one of the datasets used for calibrating the model parameters. The same file name must be used across datasets. This file is read from each calibration data directory to find the number of data points in that dataset.");
    connect(calDataFileLineEdit, &QLineEdit::textChanged, this, &UCSD_InputHierarchicalBayesian::updateCalDataFileName);

    calDataMainDirectoryLineEdit = new QLineEdit();
    calDataMainDirectoryLineEdit->setToolTip("Select the directory containing the datasets used for calibration of the hierarchical model. Each dataset must be placed in a separate sub-directory of this directory.");
    selectDataDirectoryButton = new QPushButton("Choose");
    connect(selectDataDirectoryButton, &QPushButton::clicked, this, &UCSD_InputHierarchicalBayesian::updateSelectedDatasets);
    connect(calDataMainDirectoryLineEdit, &QLineEdit::textChanged, this, &UCSD_InputHierarchicalBayesian::updateDisplayOfLabels);

    dataDirectoriesGroupBox = new QGroupBox();
    dataDirectoriesGroupBox->hide();
    dataDirectoriesVBoxLayout = new QVBoxLayout(dataDirectoriesGroupBox);

    int row = 0;
    userInputsGridLayout = new QGridLayout();
    userInputsGridLayout->addWidget(new QLabel("Sample Size"), row, 0);
    userInputsGridLayout->addWidget(sampleSizeLineEdit, row++, 1);
    userInputsGridLayout->addWidget(new QLabel("Random State"), row, 0);
    userInputsGridLayout->addWidget(randomStateLineEdit, row++, 1);
    userInputsGridLayout->addWidget(new QLabel("Restart File Name"), row, 0);
    userInputsGridLayout->addWidget(restartFileLineEdit, row++, 1);
    userInputsGridLayout->addWidget(new QLabel("Calibration Data File Name"), row, 0);
    userInputsGridLayout->addWidget(calDataFileLineEdit, row++, 1);
    userInputsGridLayout->addWidget(new QLabel("Calibration Datasets Directory"), row, 0);
    userInputsGridLayout->addWidget(calDataMainDirectoryLineEdit, row, 1, 1, 3);
    userInputsGridLayout->addWidget(selectDataDirectoryButton, row++, 4);
    userInputsGridLayout->setColumnStretch(3, 1);

    QVBoxLayout *mainVBoxLayout = new QVBoxLayout();
    mainVBoxLayout->addLayout(userInputsGridLayout);
    mainVBoxLayout->addWidget(dataDirectoriesGroupBox);
    mainVBoxLayout->addStretch();
    this->setLayout(mainVBoxLayout);

    this->initialize();

}

UCSD_InputHierarchicalBayesian::~UCSD_InputHierarchicalBayesian()
{

}

void UCSD_InputHierarchicalBayesian::updateSelectedDatasets()
{
    this->selectCalDataMainDirectory();
    this->updateDisplayOfLabels();
}

bool UCSD_InputHierarchicalBayesian::outputToJSON(QJsonObject &jsonObject)
{
    jsonObject["Sample Size"] = sampleSizeLineEdit->text().toInt();
    jsonObject["Random State"] = randomStateLineEdit->text().toInt();
    jsonObject["Restart File Name"] = restartFileLineEdit->text();
    jsonObject["Calibration Data File Name"] = calDataFileName;
    jsonObject["Calibration Datasets Directory"] = calDataMainDirectoryLineEdit->text();
    jsonObject["List Of Dataset Subdirectories"] = QJsonArray::fromStringList(this->datasetDirectoriesList);
    return true;
}

bool UCSD_InputHierarchicalBayesian::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;
    QString msg;
    QString key;

    key = "Sample Size";
    if (jsonObject.contains(key)) {
        QJsonValue value = jsonObject[key];
        if (value.isDouble()) {
            this->sampleSizeLineEdit->setText(QString::number(value.toInt()));
        } else {
            msg = "The value in the JSON file corresponding to '" + key + "' is not a number.";
            result = this->handleInputFromJSONError(msg);
        }
    } else {
        msg = "The JSON file does not contain the key '" + key + "'.";
        result = this->handleInputFromJSONError(msg);
    }

    key = "Random State";
    if (jsonObject.contains(key)) {
        QJsonValue value = jsonObject[key];
        if (value.isDouble()) {
            this->randomStateLineEdit->setText(QString::number(value.toInt()));
        } else {
            msg = "The value in the JSON file corresponding to '" + key + "' is not a number.";
            result = this->handleInputFromJSONError(msg);
        }
    } else {
        msg = "The JSON file does not contain the key '" + key + "'.";
        result = this->handleInputFromJSONError(msg);
    }

    key = "Restart File Name";
    if (jsonObject.contains(key)) {
        QJsonValue value = jsonObject[key];
        if (value.isString()) {
            this->restartFileLineEdit->setText(value.toString());
        } else {
            msg = "The value in the JSON file corresponding to '" + key + "' is not a string.";
            result = this->handleInputFromJSONError(msg);
        }
    } else {
        msg = "The JSON file does not contain the key '" + key + "'.";
        result = this->handleInputFromJSONError(msg);
    }

    key = "Calibration Data File Name";
    if (jsonObject.contains(key)) {
        QJsonValue value = jsonObject[key];
        if (value.isString()) {
            this->calDataFileLineEdit->setText(value.toString());
        } else {
            msg = "The value in the JSON file corresponding to '" + key + "' is not a string.";
            result = this->handleInputFromJSONError(msg);
        }
    } else {
        msg = "The JSON file does not contain the key '" + key + "'.";
        result = this->handleInputFromJSONError(msg);
    }

    key = "Calibration Datasets Directory";
    if (jsonObject.contains(key)) {
        QJsonValue value = jsonObject[key];
        if (value.isString()) {
            this->calDataMainDirectoryLineEdit->setText(value.toString());
        } else {
            msg = "The value in the JSON file corresponding to the key'" + key + "' is not a string.";
            result = this->handleInputFromJSONError(msg);
        }
    } else {
        msg = "The JSON file does not contain the key '" + key + "'.";
        result = this->handleInputFromJSONError(msg);
    }

    key = "List Of Dataset Subdirectories";
    if (jsonObject.contains(key)) {
        msg = "The value corresponding to the key '" + key + "' is not used. Instead, all the datasets found under the 'Calibration Datasets Directory' are included in the analysis.";
        infoMessage(msg);
    }

    return result;
}

int UCSD_InputHierarchicalBayesian::getNumberTasks()
{
    return this->datasetList.size();
}

bool UCSD_InputHierarchicalBayesian::copyFiles(QString &fileDir)
{
    QDir templatedir(fileDir);
    QString destinationDir = templatedir.canonicalPath();
    QString sourceDir = this->calDatasetsMainDirectory;
    return SimCenterAppWidget::copyPath(sourceDir, destinationDir, false);
}

void UCSD_InputHierarchicalBayesian::setRV_Defaults()
{
    RandomVariablesContainer * theRVs = RandomVariablesContainer::getInstance();
    QString classType("Uncertain");
    QString engineType("UCSD_UQ"); // This will show correlation matrix ("UCSD" will not)

    theRVs->setDefaults(engineType, classType, Normal);
}

void UCSD_InputHierarchicalBayesian::setEventType(QString typeEVT)
{
    Q_UNUSED(typeEVT);
}

void UCSD_InputHierarchicalBayesian::clear()
{
    datasetLabelsVector.clear();
    datasetDirectoriesList.clear();
    datasetList.clear();
}

UQ_Results *UCSD_InputHierarchicalBayesian::getResults()
{
    return new UCSD_ResultsHierarchicalBayesian(RandomVariablesContainer::getInstance());
}

void UCSD_InputHierarchicalBayesian::updateCalDataFileName(const QString &text)
{
    this->calDataFileName = text;
    if (!(this->calDataMainDirectoryLineEdit->text().isEmpty())) {
        this->updateDisplayOfLabels();
    }
}

void UCSD_InputHierarchicalBayesian::selectCalDataMainDirectory()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    dialog.setOption(QFileDialog::ReadOnly, true);
    QStringList selectedDirectoriesList;
    if (dialog.exec())
        selectedDirectoriesList = dialog.selectedFiles();
    QString selectedDirectory;
    if (selectedDirectoriesList.size() > 0) {
        selectedDirectory = selectedDirectoriesList.at(0);
        this->calDatasetsMainDirectory = selectedDirectory;
    }
    this->calDataMainDirectoryLineEdit->setText(selectedDirectory);
}

void UCSD_InputHierarchicalBayesian::updateListsOfCalibrationDatasetsAndDirectories()
{
    this->datasetList.clear();
    this->datasetDirectoriesList.clear();

    this->calDatasetsMainDirectory = calDataMainDirectoryLineEdit->text();
    QDir mainDir(this->calDatasetsMainDirectory);
    if (mainDir.exists()) {
        mainDir.setFilter(QDir::NoDot | QDir::NoDotDot | QDir::Dirs);
        QFileInfoList tempDatasetDirectories = mainDir.entryInfoList();

        for (int i=0; i<tempDatasetDirectories.size(); ++i) {
            QString dirPath = tempDatasetDirectories.at(i).canonicalFilePath();
            QString calDataOutputFile = dirPath + QDir::separator() + this->calDataFileName;
            QFileInfo calFile(calDataOutputFile);
            calFile.setCaching(false);
            if (calFile.exists() && calFile.isFile()) {
                this->datasetList.append(calDataOutputFile);
                this->datasetDirectoriesList.append(dirPath);
            }
        }
        QCollator collator;
        collator.setNumericMode(true);
        std::sort(this->datasetList.begin(), this->datasetList.end(), collator);
        std::sort(this->datasetDirectoriesList.begin(), this->datasetDirectoriesList.end(), collator);
    }
}


void UCSD_InputHierarchicalBayesian::updateVectorOfDatasetLabels()
{
    // Remove any previously added labels from the layout and delete them
    for (QLabel* label : qAsConst(datasetLabelsVector)) {
      dataDirectoriesVBoxLayout->removeWidget(label);
      delete label;
    }
    datasetLabelsVector.clear();

    // Add new labels for the selected filenames to the vector
    for (int i=0; i < datasetList.size(); ++i) {
        QString dirPath = datasetList.at(i);
//        QLabel* label = new QLabel(dirPath + QDir::separator() + calDataFileName, this);
        QLabel* label = new QLabel(dirPath, this);
        datasetLabelsVector.append(label);
    }
}

void UCSD_InputHierarchicalBayesian::updateDatasetGroupBox()
{
    this->calDatasetsMainDirectory = calDataMainDirectoryLineEdit->text();
    int numDataDirectories = datasetLabelsVector.size();
    QString groupBoxTitle;
    QDir mainDir(this->calDatasetsMainDirectory);
    if (mainDir.exists()) {
        if (numDataDirectories == 0) {
            groupBoxTitle = "No subdirectories containing '" + calDataFileName + "' were found in the chosen directory";
        } else {
            groupBoxTitle = "The following " + QString::number(numDataDirectories) + " datasets for calibration were found in the chosen directory:";
        }
        dataDirectoriesGroupBox->setTitle(groupBoxTitle);
        for (int i=0; i<datasetLabelsVector.size(); ++i) {
            QLabel *label = datasetLabelsVector.at(i);
            dataDirectoriesVBoxLayout->addWidget(label);
        }
        if (this->calDatasetsMainDirectory.isEmpty()) {
            dataDirectoriesGroupBox->hide();
        } else dataDirectoriesGroupBox->show();
    } else {
        groupBoxTitle = "The directory '" + this->calDatasetsMainDirectory + "' does not exist.";
        dataDirectoriesGroupBox->setTitle(groupBoxTitle);
    }

}

void UCSD_InputHierarchicalBayesian::initialize()
{
    const QString defaultCalDataFileName = "output_data.txt";
    calDataFileLineEdit->setText(defaultCalDataFileName);
}

void UCSD_InputHierarchicalBayesian::updateDisplayOfLabels()
{
    this->updateListsOfCalibrationDatasetsAndDirectories();
    this->updateVectorOfDatasetLabels();
    this->updateDatasetGroupBox();
}

bool UCSD_InputHierarchicalBayesian::handleInputFromJSONError(QString &msg)
{
    qDebug() << msg;
    errorMessage(msg);
    return false;
}

