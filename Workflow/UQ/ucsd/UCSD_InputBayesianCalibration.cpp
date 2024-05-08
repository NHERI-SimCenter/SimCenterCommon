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
//written by: bsaakash

#include "UCSD_InputBayesianCalibration.h"
#include <QJsonObject>
#include <QLabel>
#include <UCSD_InputTMCMC.h>
#include <UCSD_InputHierarchicalBayesian.h>
#include <QDebug>
#include <RandomVariablesContainer.h>
#include <UCSD_ResultsTMCMC.h>

UCSD_InputBayesianCalibration::UCSD_InputBayesianCalibration(QWidget *parent) : UQ_Engine(parent)
{
    theTMCMC = new UCSD_InputTMCMC();
    theHierarchicalBayesian = new UCSD_InputHierarchicalBayesian();
    theCurrentMethod = theTMCMC;

    modelToSamplerWidgetMap["Non-hierarchical"] = theTMCMC;
    modelToSamplerWidgetMap["Hierarchical"] = theHierarchicalBayesian;

    modelToMethodNameMap["Non-hierarchical"] = "Transitional Markov chain Monte Carlo";
    modelToMethodNameMap["Hierarchical"] = "Metropolis Within Gibbs Sampler";

    modelCombobox = new QComboBox();
    samplerStackedWidget = new QStackedWidget();

    for (auto& x : modelToSamplerWidgetMap) {
        modelCombobox->addItem(x.first);
        samplerStackedWidget->addWidget(x.second);
    }

    QLabel *methodLabel = new QLabel();
    methodLabel->setText(tr("Model"));

    methodLayout = new QHBoxLayout();
    methodLayout->addWidget(methodLabel);
    methodLayout->addWidget(modelCombobox);
    methodLayout->addStretch(2);

    layout = new QVBoxLayout();
    layout->addLayout(methodLayout);
    layout->addWidget(samplerStackedWidget);
    this->setLayout(layout);

    connect(modelCombobox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(onTextChanged(QString)));

}

void
UCSD_InputBayesianCalibration::onTextChanged(const QString &text)
{
    auto iterator = modelToSamplerWidgetMap.find(text);
    if (iterator == modelToSamplerWidgetMap.end()) {
        QString message = "ERROR .. UCSD_InputBayesianCalibration .. no method " + text;
        qDebug() << message;
        errorMessage(message);
        return;
    } else {
        theCurrentMethod = iterator->second;
        samplerStackedWidget->setCurrentWidget(iterator->second);
    }
    if (text == "Non-hierarchical")
        emit onUQ_EngineChanged("UCSD");
    else
        emit onUQ_EngineChanged("UCSD");
    emit onUQ_MethodUpdated("Bayesian Calibration");
}

UCSD_InputBayesianCalibration::~UCSD_InputBayesianCalibration()
{

}

bool UCSD_InputBayesianCalibration::outputToJSON(QJsonObject &jsonObject)
{
    QString model = modelCombobox->currentText();
    jsonObject["uqType"] = modelToMethodNameMap.find(model)->second;
    jsonObject["method"] = "Bayesian Calibration";
    this->onTextChanged(model);
    bool result = theCurrentMethod->outputToJSON(jsonObject);
    return result;
}

bool UCSD_InputBayesianCalibration::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = false;
    this->clear();
    QString modelKey = QString();

    if (jsonObject.contains("uqType")) {
        QString method = jsonObject["uqType"].toString();
        for (auto &i : modelToMethodNameMap) {
            if (i.second == method) {
                modelKey = i.first;
                break;
            }
        }
        int index = modelCombobox->findText(modelKey);
        if (index == -1) {
            return false;
        }
        modelCombobox->setCurrentIndex(index);
        this->onTextChanged(modelKey);
        result = theCurrentMethod->inputFromJSON(jsonObject);
        if (result == false)
            return result;
    }
    return result;
}

bool UCSD_InputBayesianCalibration::outputAppDataToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    jsonObject["Application"] = "UCSD-UQ";
    QJsonObject uq;
    uq["method"]=modelCombobox->currentText();
    theCurrentMethod->outputToJSON(uq);
    jsonObject["ApplicationData"] = uq;

    return result;
}

bool UCSD_InputBayesianCalibration::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    bool result = false;
    this->clear();

    if (jsonObject.contains("ApplicationData")) {
        QJsonObject uq = jsonObject["ApplicationData"].toObject();

        if (uq.contains("method")) {
          QString method = uq["method"].toString();
          int index = modelCombobox->findText(method);

          if (index == -1) {
              errorMessage(QString("ERROR: Unknown Method ") + method);
              return false;
          }
          modelCombobox->setCurrentIndex(index);
          return theCurrentMethod->inputFromJSON(uq);
        }

    } else {
        errorMessage("ERROR: Bayesian Calibration Input Widget - no \"ApplicationData\" input");
        return false;
    }

    return result;
}

bool UCSD_InputBayesianCalibration::copyFiles(QString &destDir)
{
    return theCurrentMethod->copyFiles(destDir);
}

bool UCSD_InputBayesianCalibration::supportsLocalRun()
{
    return true;
}

void UCSD_InputBayesianCalibration::clear()
{
}

//bool UCSD_InputBayesianCalibration::outputCitation(QJsonObject &jsonObject)
//{
//    return false;
//}

//SimCenterAppWidget *UCSD_InputBayesianCalibration::getClone()
//{
//    return NULL;
//}

//void UCSD_InputBayesianCalibration::setCurrentlyViewable(bool)
//{
//}

int UCSD_InputBayesianCalibration::getMaxNumParallelTasks()
{
    return theCurrentMethod->getNumberTasks();
}

void UCSD_InputBayesianCalibration::setRV_Defaults()
{
    theCurrentMethod->setRV_Defaults();
}

UQ_Results *UCSD_InputBayesianCalibration::getResults()
{
    return theCurrentMethod->getResults();
//    return new UCSD_Results(RandomVariablesContainer::getInstance());
}

//QString UCSD_InputBayesianCalibration::getProcessingScript()
//{
//}

QString UCSD_InputBayesianCalibration::getMethodName()
{
    return QString("Bayesian Calibration");
}

//bool UCSD_InputBayesianCalibration::fixMethod(QString)
//{
//}

//void UCSD_InputBayesianCalibration::setEventType(QString type)
//{
//}


bool UCSD_InputBayesianCalibration::outputCitation(QJsonObject &jsonObject)
{
    if (theCurrentMethod == theTMCMC) {
        QJsonArray citations;

        QJsonObject paper1;
        paper1.insert("citation",QString("Ching, J., & Chen, Y.-C. (2007). Transitional Markov Chain Monte Carlo Method for Bayesian Model Updating, Model Class Selection, and Model Averaging. Journal of Engineering Mechanics, 133(7), 816-832."));
        paper1.insert("description",QString("This is the paper where the TMCMC method was proposed. The implementation of the TMCMC algorithm in quoFEM is based on ideas originating in this paper."));

        QJsonObject paper2;
        paper2.insert("citation",QString("Minson, S. E., Simons, M., & Beck, J. L. (2013). Bayesian Inversion for Finite Fault Earthquake Source Models I-Theory and Algorithm. Geophysical Journal International, 194(3), 1701-1726."));
        paper2.insert("description",QString("In this paper a parallel version of the TMCMC algorithm was described. The implementation of the TMCMC algorithm in quoFEM is based on ideas proposed in this paper."));

        citations.append(paper1);
        citations.append(paper2);

        jsonObject.insert("citations", citations);

    } else if (theCurrentMethod == theHierarchicalBayesian) {
        QJsonArray citations;

        QJsonObject paper1;
        paper1.insert("citation",QString("Kurumbhati, M. M., Ramancha, M. K., Aakash, B. S., Conte, J. P., Lotfizadeh, K. H. (Year). Hierarchical Bayesian Modeling for Calibration and Uncertainty Quantification of Constitutive Material Models: Application to a Uniaxial Steel Material Model. Engineering Structures."));
        paper1.insert("description",QString("The implementation of the sampling algorithm for Bayesian calibration of hierarchial models follows the description in this paper."));

        QJsonObject paper2;
        paper2.insert("citation",QString("Engineering Risk Analysis Group, Technische Universität München. (2023). ERADist Toolbox (Version 05/2023). Retrieved from https://www.bgu.tum.de/era/software/eradist/ (Python code and documentation)."));
        paper2.insert("description",QString("The Python code in this toolbox is utilized by quoFEM to manage random variables within the analysis. The Nataf class is specifically employed for transforming sample values generated by the UQ algorithm from standard normal space to the space defined by the random variables, and vice versa."));

        citations.append(paper1);
        citations.append(paper2);

        jsonObject.insert("citations", citations);

    }
  return true;
}
