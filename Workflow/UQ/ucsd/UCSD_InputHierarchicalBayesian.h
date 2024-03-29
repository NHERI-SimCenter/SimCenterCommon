#ifndef UCSD_INPUTHIERARCHICALBAYESIAN_H
#define UCSD_INPUTHIERARCHICALBAYESIAN_H

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

#include <UQ_Results.h>
#include <QFileInfo>
#include <QLabel>
#include <UCSD_UQ_Method.h>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QJsonObject>

class UCSD_InputHierarchicalBayesian : public UCSD_UQ_Method
{
    Q_OBJECT
public:
    explicit UCSD_InputHierarchicalBayesian(QWidget *parent=0);
    ~UCSD_InputHierarchicalBayesian();

    // SimCenterWidget interface
public:
    bool outputToJSON(QJsonObject &jsonObject);
    bool inputFromJSON(QJsonObject &jsonObject);

    // UQ_Method interface
public:
    int getNumberTasks();
    bool copyFiles(QString &fileDir);
    void setRV_Defaults();
    void setEventType(QString typeEVT);
    void clear();

    // UCSD_UQ_Method interface
public:
    UQ_Results *getResults();

public:
    // Methods
    void selectCalDataMainDirectory();
    void updateSelectedDatasets();
    void updateVectorOfDatasetLabels();
    void updateDatasetGroupBox();
    void initialize();
    void updateDisplayOfLabels();
    bool handleInputFromJSONError(QString &msg);

public slots:
    void updateCalDataFileName(const QString &text);
    void updateListsOfCalibrationDatasetsAndDirectories();

private:
    // User interface elements
    QLineEdit *sampleSizeLineEdit;
    QLineEdit *randomStateLineEdit;
    QLineEdit *restartFileLineEdit;
    QLineEdit *proposalAdaptationDurationLineEdit;
    QLineEdit *proposalAdaptationIntervalLineEdit;
    QLineEdit *calDataFileLineEdit;
    QLineEdit *calDataMainDirectoryLineEdit;
    QGridLayout *userInputsGridLayout;
    QPushButton *selectDataDirectoryButton;
    QVBoxLayout* dataDirectoriesVBoxLayout;
    QGroupBox *dataDirectoriesGroupBox;

    // Data
    QString calDataFileName;
    QString calDatasetsMainDirectory;
    QStringList datasetList;
    QStringList datasetDirectoriesList;
    QVector<QLabel*> datasetLabelsVector;
};

#endif // UCSD_INPUTHIERARCHICALBAYESIAN_H
