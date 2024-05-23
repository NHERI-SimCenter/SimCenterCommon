#ifndef UCSD_INPUTBAYESIANCALIBRATION_H
#define UCSD_INPUTBAYESIANCALIBRATION_H

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
// written by: bsaakash

#include <UQ_Engine.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QComboBox>
#include <QStringList>
#include <UCSD_UQ_Method.h>


class UCSD_InputBayesianCalibration : public UQ_Engine
{
    Q_OBJECT
public:
    explicit UCSD_InputBayesianCalibration(QWidget *parent = 0);
    ~UCSD_InputBayesianCalibration();

    // SimCenterWidget interface
public:
    bool outputToJSON(QJsonObject &jsonObject);
    bool inputFromJSON(QJsonObject &jsonObject);

    // SimCenterAppWidget interface
public:
    bool outputAppDataToJSON(QJsonObject &jsonObject);
    bool inputAppDataFromJSON(QJsonObject &jsonObject);
    bool copyFiles(QString &destDir);
    bool supportsLocalRun();
    void clear();
//    bool outputCitation(QJsonObject &jsonObject);
//    SimCenterAppWidget *getClone();

public slots:
//    void setCurrentlyViewable(bool);
    void onTextChanged(const QString &text);

    // UQ_Engine interface

public:
    int getMaxNumParallelTasks();
    void setRV_Defaults();
    UQ_Results *getResults();
//    QString getProcessingScript();
    QString getMethodName();
//    bool fixMethod(QString);
//    void setEventType(QString type);

    QVBoxLayout *layout;
    QHBoxLayout *methodLayout;
    QStackedWidget *samplerStackedWidget;
    QComboBox *modelCombobox;
    std::map<QString, UCSD_UQ_Method *> modelToSamplerWidgetMap;
    std::map<QString, QString> modelToMethodNameMap;
    UCSD_UQ_Method *theTMCMC;
    UCSD_UQ_Method *theHierarchicalBayesian;
    UCSD_UQ_Method *theCurrentMethod;

    // SimCenterAppWidget interface
public:
    bool outputCitation(QJsonObject &jsonObject);
};

#endif // UCSD_INPUTBAYESIANCALIBRATION_H
