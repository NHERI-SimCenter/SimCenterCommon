// Written: fmckenna

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

// Written: fmckenna, bsaakash

#include <GoogleAnalytics.h>

#include "UCSD_Engine.h"
#include "RandomVariablesContainer.h"
#include <QStackedWidget>
#include <QComboBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QLabel>

#include <QDebug>

#include <UCSD_InputTMCMC.h>
#include <UCSD_InputHierarchicalBayesian.h>
#include <UCSD_InputBayesianCalibration.h>

UCSD_Engine::UCSD_Engine(UQ_EngineType type, QWidget *parent)
: UQ_Engine(parent), theCurrentEngine(0)
{

    Q_UNUSED(type);

    QVBoxLayout *layout = new QVBoxLayout();

    //
    // the selection part
    //

    QHBoxLayout *theSelectionLayout = new QHBoxLayout();
    label = new QLabel();
    label->setText(QString("Method"));
    theMethodSelectionBox = new QComboBox();
    theMethodSelectionBox->addItem(tr("Bayesian Calibration"));
    int width = theMethodSelectionBox->minimumSizeHint().width();
    theMethodSelectionBox->view()->setMinimumWidth(width);

    theSelectionLayout->addWidget(label);
    theSelectionLayout->addWidget(theMethodSelectionBox);
    theSelectionLayout->addStretch();
    layout->addLayout(theSelectionLayout);

    parallelCheckBox = new QCheckBox();
    parallelCheckBox->setChecked(true);

    //
    // create the stacked widget
    //

    theStackedWidget = new QStackedWidget();

    //
    // create the individual widgets add to stacked widget
    //
    theBayesianCalibration = new UCSD_InputBayesianCalibration();
    theStackedWidget->addWidget(theBayesianCalibration);

    layout->addWidget(theStackedWidget);
    this->setLayout(layout);
    theCurrentEngine=theBayesianCalibration;

    connect(theMethodSelectionBox,
	    SIGNAL(currentTextChanged(QString)),
	    this,
        SLOT(methodChanged(QString)));
}

UCSD_Engine::~UCSD_Engine()
{

}


void UCSD_Engine::methodChanged(const QString &arg1)
{
    if ((arg1 == QString("Bayesian Calibration"))) {
        theStackedWidget->setCurrentIndex(0);
        theCurrentEngine = theBayesianCalibration;
    } else {
        qDebug() << "ERROR .. UCSDEngine selection .. type unknown: " << arg1;
    }
    emit onUQ_EngineChanged("UCSD-UQ");
    emit onUQ_MethodUpdated("Bayesian Calibration");
}


int
UCSD_Engine::getMaxNumParallelTasks(void) {
    return theCurrentEngine->getMaxNumParallelTasks();
}

bool
UCSD_Engine::outputToJSON(QJsonObject &jsonObject) {

    jsonObject["parallelExecution"] = parallelCheckBox->isChecked();
    return theCurrentEngine->outputToJSON(jsonObject);
}

bool
UCSD_Engine::inputFromJSON(QJsonObject &jsonObject) {
    bool result = false;

    QString uqMethod = jsonObject["method"].toString();
    if (uqMethod.isEmpty())
        uqMethod = "Bayesian Calibration";

    emit onUQ_MethodUpdated("Bayesian Calibration");
    emit onUQ_EngineChanged("UCSD-UQ");

    parallelCheckBox->setChecked(true);
    if (jsonObject.contains("parallelExecution")) {
        bool checkedState = jsonObject["parallelExecution"].toBool();
        parallelCheckBox->setChecked(checkedState);
    }

    int index = theMethodSelectionBox->findText(uqMethod);
    theMethodSelectionBox->setCurrentIndex(index);
    this->methodChanged(uqMethod);
    if (theCurrentEngine != 0)
        result = theCurrentEngine->inputFromJSON(jsonObject);
    else
        result = false; // don't emit error as one should have been generated

    return result;
}


bool
UCSD_Engine::outputAppDataToJSON(QJsonObject &jsonObject)
{
    jsonObject["Application"] = "UCSD-UQ";
    QJsonObject dataObj;
    jsonObject["ApplicationData"] = dataObj;

    return true;
}

bool
UCSD_Engine::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    Q_UNUSED(jsonObject);
    return true;
}

void
UCSD_Engine::setRV_Defaults(void) {
    return theCurrentEngine->setRV_Defaults();
}

UQ_Results *
UCSD_Engine::getResults(void) {
    return theCurrentEngine->getResults();
//    return new UCSD_Results(theRandomVariables);
}

QString
UCSD_Engine::getProcessingScript() {
    return QString("parseUCSD_UQ.py");
}

void
UCSD_Engine::numModelsChanged(int newNum) {
    emit onNumModelsChanged(newNum);
}

QString
UCSD_Engine::getMethodName() {

    return theCurrentEngine->getMethodName();
}

bool
UCSD_Engine::fixMethod(QString Methodname) {
    int res = theMethodSelectionBox->findText(Methodname);
    if (res == -1) {
        return false;
    } else {
        theMethodSelectionBox->setCurrentIndex(res);
        theMethodSelectionBox->hide();
        label->hide();
        return true;
    }
}

bool
UCSD_Engine::copyFiles(QString &fileName) {
    QString googleString=QString("UQ_UCSD_") + this->getMethodName();
    GoogleAnalytics::ReportAppUsage(googleString);

    return theCurrentEngine->copyFiles(fileName);
}
