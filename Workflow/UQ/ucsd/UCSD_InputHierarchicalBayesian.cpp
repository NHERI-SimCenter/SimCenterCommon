
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


UCSD_InputHierarchicalBayesian::UCSD_InputHierarchicalBayesian()
{
    sampleSizeLineEdit = new QLineEdit();
    sampleSizeLineEdit->setText(tr("500"));
    QIntValidator *positiveIntegerValidator = new QIntValidator;
    positiveIntegerValidator->setBottom(0);
    sampleSizeLineEdit->setValidator(positiveIntegerValidator);
    sampleSizeLineEdit->setToolTip("Specify the number of sample values to be drawn from the posterior");

    srand(time(NULL));
    int randomNumber = arc4random() % 1000 + 1;
    randomStateLineEdit = new QLineEdit();
    randomStateLineEdit->setText(QString::number(randomNumber));
    randomStateLineEdit->setValidator(positiveIntegerValidator);
    randomStateLineEdit->setToolTip("Specify the random state used by the pseudo random number generator. This is used for reproducibility.");

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
    this->updateCalDataMainDirectory();
    this->updateDatasetDirectories();
    this->updateDatasetDirectoriesVector();
    this->updateDatasetGroupBox();
}

bool UCSD_InputHierarchicalBayesian::outputToJSON(QJsonObject &jsonObject)
{
    Q_UNUSED(jsonObject);
    return false;
}

bool UCSD_InputHierarchicalBayesian::inputFromJSON(QJsonObject &jsonObject)
{
    Q_UNUSED(jsonObject);
    return false;
}

int UCSD_InputHierarchicalBayesian::getNumberTasks()
{
    return 1;
}

bool UCSD_InputHierarchicalBayesian::copyFiles(QString &fileDir)
{
    Q_UNUSED(fileDir);
    return false;
}

void UCSD_InputHierarchicalBayesian::setRV_Defaults()
{
}

void UCSD_InputHierarchicalBayesian::setEventType(QString typeEVT)
{
    Q_UNUSED(typeEVT);
}

void UCSD_InputHierarchicalBayesian::clear()
{
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
    }
    this->calDataMainDirectoryLineEdit->setText(selectedDirectory);
}

void UCSD_InputHierarchicalBayesian::updateListsOfCalibrationDatasetsAndDirectories()
{
    this->datasetList.clear();
    this->datasetDirectoriesList.clear();

    QString selectedDirectory = calDataMainDirectoryLineEdit->text();
    QDir mainDir(selectedDirectory);
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
    for (QLabel* label : qAsConst(selectedDatasetDirectoriesVector)) {
      dataDirectoriesVBoxLayout->removeWidget(label);
      delete label;
    }
    selectedDatasetDirectoriesVector.clear();

    // Add new labels for the selected filenames to the vector
    for (int i=0; i < datasetList.size(); ++i) {
        QString dirPath = datasetList.at(i);
//        QLabel* label = new QLabel(dirPath + QDir::separator() + calDataFileName, this);
        QLabel* label = new QLabel(dirPath, this);
        selectedDatasetDirectoriesVector.append(label);
    }
}

void UCSD_InputHierarchicalBayesian::updateDatasetGroupBox()
{
    QString selectedDirectory = calDataMainDirectoryLineEdit->text();
    int numDataDirectories = selectedDatasetDirectoriesVector.size();
    QString groupBoxTitle;
    QDir mainDir(selectedDirectory);
    if (mainDir.exists()) {
        if (numDataDirectories == 0) {
            groupBoxTitle = "No subdirectories containing '" + calDataFileName + "' were found in the chosen directory";
        } else {
            groupBoxTitle = "The following " + QString::number(numDataDirectories) + " datasets for calibration were found in the chosen directory:";
        }
        dataDirectoriesGroupBox->setTitle(groupBoxTitle);
        for (int i=0; i<selectedDatasetDirectoriesVector.size(); ++i) {
            QLabel *label = selectedDatasetDirectoriesVector.at(i);
            dataDirectoriesVBoxLayout->addWidget(label);
        }
        if (selectedDirectory.isEmpty()) {
            dataDirectoriesGroupBox->hide();
        } else dataDirectoriesGroupBox->show();
    } else {
        groupBoxTitle = "The directory '" + selectedDirectory + "' does not exist.";
        dataDirectoriesGroupBox->setTitle(groupBoxTitle);
    }

}

void UCSD_InputHierarchicalBayesian::initialize()
{
    const QString defaultCalDataFileName = "output_data.txt";
    calDataFileLineEdit->setText(defaultCalDataFileName);
    //    this->updateCalDataFileName(defaultCalDataFileName);
}

void UCSD_InputHierarchicalBayesian::updateDisplayOfLabels()
{
    this->updateListsOfCalibrationDatasetsAndDirectories();
    this->updateVectorOfDatasetLabels();
    this->updateDatasetGroupBox();
}



