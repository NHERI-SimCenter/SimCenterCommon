
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
#include <QIntValidator>
#include <time.h>
#include <QFileDialog>
#include <QDebug>

UCSD_InputHierarchicalBayesian::UCSD_InputHierarchicalBayesian()
{
    sampleSize = new QLineEdit();
    sampleSize->setText(tr("500"));
    QIntValidator *positiveIntegerValidator = new QIntValidator;
    positiveIntegerValidator->setBottom(0);
    sampleSize->setValidator(positiveIntegerValidator);
    sampleSize->setToolTip("Specify the number of sample values to be drawn from the posterior");

    srand(time(NULL));
    int randomNumber = arc4random() % 1000 + 1;
    randomState = new QLineEdit();
    randomState->setText(QString::number(randomNumber));
    randomState->setValidator(positiveIntegerValidator);
    randomState->setToolTip("Specify the random state used by the pseudo random number generator. This is used for reproducibility.");

//    calDataFileEdit = new QLineEdit();
//    calDataFileEdit->setText(tr("output_data.txt"));
//    calDataFileEdit->setToolTip("Enter the name of the file containing the output data from one of the datasets used for calibrating the model parameters. The same file name must be used across datasets. This file is read from each calibration data directory to find the number of data points in that dataset.");
//    connect(calDataFileEdit, &QLineEdit::textChanged, this, &UCSD_InputHierarchicalBayesian::updateCalDataFileName);

    calDataMainDirectory = new QLineEdit();
    calDataMainDirectory->setToolTip("Select the directory containing the datasets used for calibration of the hierarchical model. Each dataset must be placed in a separate sub-directory of this directory.");
    selectDataDirectoryButton = new QPushButton("Choose");
    connect(selectDataDirectoryButton, &QPushButton::clicked, this, &UCSD_InputHierarchicalBayesian::updateSelectedDatasets);

//    connect(selectDataDirectoryButton, &QPushButton::clicked, this, [=](){
//        QFileDialog dialog(this);
//        dialog.setFileMode(QFileDialog::Directory);
//        dialog.setViewMode(QFileDialog::Detail);
//        dialog.setOption(QFileDialog::ShowDirsOnly, true);
//        dialog.setOption(QFileDialog::ReadOnly, true);
//        QStringList selectedDirectoriesList;
//        if (dialog.exec())
//            selectedDirectoriesList = dialog.selectedFiles();
//        QString selectedDirectory = selectedDirectoriesList.at(0);
//        calDataMainDirectory->setText(selectedDirectory);

//        QString selectedDirectory = calDataMainDirectory->text();
//        QDir mainDir(selectedDirectory);
//        datasetDirectories = mainDir.entryList(QDir::NoDot | QDir::NoDotDot | QDir::Dirs);

        // Remove any previously added labels from the layout and delete them
//        for (QLabel* label : qAsConst(selectedDatasetDirectoriesVector)) {
//          dataDirectoriesBoxLayout->removeWidget(label);
//          delete label;
//        }
//        selectedDatasetDirectoriesVector.clear();

//        // Add new labels for the selected filenames to the form layout
//        for (const QString& dirPath : datasetDirectories) {
////            QString calFile = calDataFileEdit->text();
////            qDebug() <<"calDataFileName: " << calDataFileName << "calFile: " << calFile;
//            QLabel* label = new QLabel(dirPath + QDir::separator() + calDataFileName, this);
//            dataDirectoriesBoxLayout->addWidget(label);
//            selectedDatasetDirectoriesVector.append(label);
//        }
//        if (selectedDirectory.isEmpty()) {
//            dataDirectoriesGroupBox->hide();
//        } else dataDirectoriesGroupBox->show();
//    });

    dataDirectoriesGroupBox = new QGroupBox();
    dataDirectoriesGroupBox->hide();
    dataDirectoriesBoxLayout = new QVBoxLayout(dataDirectoriesGroupBox);

    int row = 0;
    layout = new QGridLayout();
    layout->addWidget(new QLabel("Sample Size"), row, 0);
    layout->addWidget(sampleSize, row++, 1);
    layout->addWidget(new QLabel("Random State"), row, 0);
    layout->addWidget(randomState, row++, 1);
//    layout->addWidget(new QLabel("Calibration Data File Name"), row, 0);
//    layout->addWidget(calDataFileEdit, row++, 1);
//    //   layout->addWidget(chooseCalDataFile, row++, 3);
    layout->addWidget(new QLabel("Calibration Datasets Directory"), row, 0);
    layout->addWidget(calDataMainDirectory, row, 1, 1, 3);
    layout->addWidget(selectDataDirectoryButton, row++, 4);
//    layout->setRowStretch(row++, 1);
    layout->setColumnStretch(3, 1);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(layout);
    mainLayout->addWidget(dataDirectoriesGroupBox);
    mainLayout->addStretch();
    //   layout->addWidget(dataDirectoriesGroupBox, row++, 0, 1, 4);
    this->setLayout(mainLayout);

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
    calDataFileName = text;
    if (this->calDataMainDirectory)
        this->updateSelectedDatasets();
    qDebug() << "text: " << text <<"calDataFileName: " << calDataFileName;
}

void UCSD_InputHierarchicalBayesian::updateCalDataMainDirectory()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    dialog.setOption(QFileDialog::ReadOnly, true);
    QStringList selectedDirectoriesList;
    if (dialog.exec())
        selectedDirectoriesList = dialog.selectedFiles();
    QString selectedDirectory = selectedDirectoriesList.at(0);
    calDataMainDirectory->setText(selectedDirectory);
}

void UCSD_InputHierarchicalBayesian::updateDatasetDirectories()
{
    QString selectedDirectory = calDataMainDirectory->text();
    QDir mainDir(selectedDirectory);
    datasetDirectories = mainDir.entryList(QDir::NoDot | QDir::NoDotDot | QDir::Dirs);
}

void UCSD_InputHierarchicalBayesian::updateDatasetDirectoriesVector()
{
    // Remove any previously added labels from the layout and delete them
    for (QLabel* label : qAsConst(selectedDatasetDirectoriesVector)) {
      dataDirectoriesBoxLayout->removeWidget(label);
      delete label;
    }
    selectedDatasetDirectoriesVector.clear();

    // Add new labels for the selected filenames to the vector
    for (const QString& dirPath : datasetDirectories) {
//        QLabel* label = new QLabel(dirPath + QDir::separator() + calDataFileName, this);
        QLabel* label = new QLabel(dirPath, this);
        selectedDatasetDirectoriesVector.append(label);
    }
}

void UCSD_InputHierarchicalBayesian::updateDatasetGroupBox()
{
    QString selectedDirectory = calDataMainDirectory->text();
    int numDataDirectories = selectedDatasetDirectoriesVector.size();
    QString groupBoxTitle = "The following " + QString::number(numDataDirectories) + " datasets were found in the chosen directory:";

    dataDirectoriesGroupBox->setTitle(groupBoxTitle);
    for (QLabel *label : selectedDatasetDirectoriesVector)
        dataDirectoriesBoxLayout->addWidget(label);

    if (selectedDirectory.isEmpty()) {
        dataDirectoriesGroupBox->hide();
    } else dataDirectoriesGroupBox->show();
}



