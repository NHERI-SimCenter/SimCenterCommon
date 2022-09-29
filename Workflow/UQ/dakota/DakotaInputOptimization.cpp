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

// Written: bsaakash

#include "DakotaInputOptimization.h"
#include <DakotaResultsOptimization.h>
#include <RandomVariablesContainer.h>

#include <QLabel>
#include <QJsonObject>
#include <QLineEdit>

DakotaInputOptimization::DakotaInputOptimization(QWidget *parent)
    : UQ_Engine(parent)
{
    QGridLayout *layout = new QGridLayout();

    int row=0;

    optimizationMethod = new QComboBox();
//    optimizationMethod->addItem(tr("Derivative-Based Local Search"));
    optimizationMethod->addItem(tr("Derivative-Free Local Search"));
//    optimizationMethod->addItem(tr("Derivative-Free Global Search"));
    layout->addWidget(new QLabel("Method"), row, 0);
    layout->addWidget(optimizationMethod, row++, 1, 1, 2);
    connect(optimizationMethod, SIGNAL(currentTextChanged(QString)), this, SLOT(comboboxItemChanged(QString)));

    initial_step_size_value = new QLineEdit();
    initial_step_size_value->setText(QString::number(1.0));
    layout->addWidget(new QLabel("Initial step size"), row, 0, 1, 2);
    layout->addWidget(initial_step_size_value, row++, 1, 1, 2);
    initial_step_size_value->setValidator(new QDoubleValidator);
    initial_step_size_value->setToolTip("Initial step size for derivative-free optimizer");

    contraction_factor = new QLineEdit();
    contraction_factor->setText(QString::number(0.5));
    layout->addWidget(new QLabel("Contraction factor"), row, 0, 1, 2);
    layout->addWidget(contraction_factor, row++, 1, 1, 2);
    contraction_factor->setValidator(new QDoubleValidator);
    contraction_factor->setToolTip("Amount by which step length is rescaled");

    max_function_evals = new QLineEdit();
    max_function_evals->setText(tr("1000"));
    layout->addWidget(new QLabel("Max # model evals"), row, 0, 1, 2);
    layout->addWidget(max_function_evals, row++, 1, 1, 2);
    max_function_evals->setValidator(new QIntValidator);
    max_function_evals->setToolTip("Number of model evaluations allowed for optimizer");

    maxIterations = new QLineEdit();
    maxIterations->setText(tr("100"));
    layout->addWidget(new QLabel("Max # iterations"), row, 0, 1, 2);
    layout->addWidget(maxIterations, row++, 1, 1, 2);
    maxIterations->setValidator(new QIntValidator);
    maxIterations->setToolTip("Number of iterations allowed for optimizer");

    final_step_size_value = new QLineEdit();
    final_step_size_value->setText(QString::number(1.0e-4));
    layout->addWidget(new QLabel("Variable tolerance"), row, 0, 1, 2);
    layout->addWidget(final_step_size_value, row++, 1, 1, 2);
    final_step_size_value->setValidator(new QDoubleValidator);
    final_step_size_value->setToolTip("Step length-based stopping criteria for derivative-free optimizer");

    convergenceTol = new QLineEdit();
    convergenceTol->setText(QString::number(1.0e-4));
    layout->addWidget(new QLabel("Convergence tolerance"), row, 0, 1, 2);
    layout->addWidget(convergenceTol, row++, 1, 1, 2);
    convergenceTol->setValidator(new QDoubleValidator);
    convergenceTol->setToolTip("Stopping criterion based on objective function convergence");

//    solutionTarget = new QLineEdit();
//    solutionTarget->setText(QString::number(-1.7e308));
//    layout->addWidget(new QLabel("Solution target"), row, 0, 1, 2);
//    layout->addWidget(solutionTarget, row++, 1, 1, 2);
//    solutionTarget->setValidator(new QDoubleValidator);
//    solutionTarget->setToolTip("Stopping criteria based on objective function value");

    layout->setColumnMinimumWidth(0, 130);

    layout->setColumnStretch(3, 1);

    layout->setRowStretch(row,1);

    this->setLayout(layout);
}

void
DakotaInputOptimization::comboboxItemChanged(QString value)
{
    return;
}



DakotaInputOptimization::~DakotaInputOptimization()
{

}


void DakotaInputOptimization::clear(void)
{

}


int
DakotaInputOptimization::getMaxNumParallelTasks(void){
  return 1;
}

bool
DakotaInputOptimization::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    QJsonObject uq;
    uq["method"]=optimizationMethod->currentText();
    uq["maxIterations"]=maxIterations->text().toInt();
    uq["convergenceTol"]=convergenceTol->text().toDouble();
//    uq["factors"]=scalingFactors->text();


    if(optimizationMethod->currentText()=="Derivative-Free Local Search")
    {
        uq["initialDelta"] =initial_step_size_value->text().toDouble();
        uq["thresholdDelta"] = final_step_size_value->text().toDouble() ;
        uq["contractionFactor"] = contraction_factor->text().toDouble();
        uq["maxFunEvals"] = max_function_evals->text().toInt() ;
//        uq["solutionTarget"] = solutionTarget->text().toDouble();
//        uq["patternMove"] = exploratory_moves->currentText();
    }

//    if(optimizationMethod->currentText()=="Coliny_EA")
//    {
//        uq["maxFunEvals"]=max_function_evals_ColonyEA->text().toInt();
//        uq["seed"]=seed_ColonyEA->text().toInt();
//        uq["popSize"]=pop_sizeColonyEA->text().toInt();
//        uq["fitnessType"]=fitness_typeColonyEA->currentText();
//        uq["mutationType"]=mutation_typeColonyEA->currentText();
//        uq["mutationRate"]=mutation_rateColonyEA->text().toDouble();
//        uq["crossoverType"]=crossover_typeColonyEA->currentText();
//        uq["crossoverRate"]=crossover_rateColonyEA->text().toDouble();
//        uq["replacementType"]=replacement_typeColonyEA->currentText();
//        uq["replacementValue"]=replacement_type_value_ColonyEA->text().toInt();
//    }


    jsonObject["optimizationMethodData"]=uq;

    return result;
}

bool
DakotaInputOptimization::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;
    this->clear();

    QJsonObject uq = jsonObject["optimizationMethodData"].toObject();
    QString method =uq["method"].toString();
    int maxIter=uq["maxIterations"].toInt();
    double convTol=uq["convergenceTol"].toDouble();
//    QString fact = uq["factors"].toString();


    if (method == "Derivative-Free Local Search")
    {
        double initialDelta = uq["initialDelta"].toDouble();
        double thresholdDelta = uq["thresholdDelta"].toDouble();
        double contractFac = uq["contractionFactor"].toDouble();
        int maxFuncEvals = uq["maxFunEvals"].toInt();
//        double solTarget = uq["solutionTarget"].toDouble();

        initial_step_size_value->setText(QString::number(initialDelta));
        final_step_size_value->setText(QString::number(thresholdDelta));
        contraction_factor->setText(QString::number(contractFac));
        max_function_evals->setText(QString::number(maxFuncEvals));
//        solutionTarget->setText(QString::number(solTarget));

    }

    maxIterations->setText(QString::number(maxIter));
    convergenceTol->setText(QString::number(convTol));
    int index = optimizationMethod->findText(method);
    optimizationMethod->setCurrentIndex(index);
//    scalingFactors->setText(fact);

    return result;

}

void
DakotaInputOptimization::methodChanged(const QString &arg1)
{

}

UQ_Results *
DakotaInputOptimization::getResults(void) {
  return new DakotaResultsOptimization(RandomVariablesContainer::getInstance());
}

void
DakotaInputOptimization::setRV_Defaults(void) {

  RandomVariablesContainer *theRVs = RandomVariablesContainer::getInstance();
  QString classType("Design");
  QString engineType("Dakota");
  theRVs->setDefaults(engineType, classType, ContinuousDesign);
}

QString
DakotaInputOptimization::getMethodName(void){
  return QString("optimization");
}

