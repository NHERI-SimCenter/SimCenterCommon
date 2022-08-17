#ifndef DAKOTA_INPUT_BAYESIAN_CALIBRATION_H
#define DAKOTA_INPUT_BAYESIAN_CALIBRATION_H

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

// Written: fmckenna

#include <UQ_Engine.h>

class DakotaResultsBayesianCalibration;
class UQ_Results;
class QGridLayout;
class QComboBox;
class QPushButton;
class QLineEdit;
class QCheckBox;
class QLabel;
class QHBoxLayout;


class DakotaInputBayesianCalibration : public UQ_Engine
{
    Q_OBJECT
public:
    explicit DakotaInputBayesianCalibration(QWidget *parent = 0);
    ~DakotaInputBayesianCalibration();

    int getMaxNumParallelTasks(void);
    bool outputToJSON(QJsonObject &jsonObject);
    bool inputFromJSON(QJsonObject &jsonObject);

    int processResults(QString &filenameResults, QString &filenameTab);

    UQ_Results *getResults(void);
    void setRV_Defaults(void);
    QString getMethodName();

signals:

public slots:
   void clear(void);
   void onMethodChanged(const QString &arg1);
   void showNumExp(bool);
   void showCalibrateCovMultiplierCheckBox(bool);

private:
    QGridLayout *layout;
    QWidget     *methodSpecific;
    QComboBox   *calibrationMethod;
    //QComboBox   *mcmcMethod;
    QLineEdit   *chains;
    QLineEdit   *chainSamples;
    QLineEdit   *randomSeed;
    QLineEdit   *burnInSamples;
    QLineEdit   *jumpStep;
//    QLineEdit   *calibrationData;
    QCheckBox   *readCalibrationDataCheckBox;
    QLineEdit   *numExperiments;
    QLabel      *numExperimentsLabel;
    QHBoxLayout *readCalDataWidget;
    QLabel      *readCovarianceDataLabel;
    QCheckBox   *readCovarianceDataCheckBox;
    QLabel      *calibrateCovarianceMultiplierLabel;
    QCheckBox   *calibrateCovarianceMultiplierCheckBox;
    QHBoxLayout *readErrorCovarianceDataWidget;
    QLabel      *calibratePerResponseLabel;
    QCheckBox   *calibratePerResponseCheckBox;
    QLabel      *calibratePerExperimentLabel;
    QCheckBox   *calibratePerExperimentCheckBox;
    QLabel      *calibrateMultiplierText;
    QLineEdit   *calDataFileEdit;

    DakotaResultsBayesianCalibration *results;
};

#endif // DAKOTA_INPUT_BAYESIAN_CALIBRATION_H
