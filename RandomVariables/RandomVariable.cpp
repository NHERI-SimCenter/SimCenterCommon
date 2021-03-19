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

// Written: fmckenna

#include "RandomVariable.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QDebug>

//
// headers for RandomVariableDistribution subclasses that user can select
//

#include "GumbelDistribution.h"
#include "WeibullDistribution.h"
#include "NormalDistribution.h"
#include "LognormalDistribution.h"
#include "BetaDistribution.h"
#include "UniformDistribution.h"
#include "ExponentialDistribution.h"
#include "DiscreteDistribution.h"
#include "GammaDistribution.h"
#include "ChiSquaredDistribution.h"
#include "TruncatedExponentialDistribution.h"

#include "ConstantDistribution.h"
#include "ContinuousDesignDistribution.h"
#include "UserDef.h"


RandomVariable::RandomVariable()
    :SimCenterWidget(0), refCount(0), variableClass(QString(""))
{

}

RandomVariable::RandomVariable(const QString &type, QString uqengin, QWidget *parent)
    :SimCenterWidget(parent), refCount(0), variableClass(type), uqEngineName(uqengin)
{  
    // create the main layout inside which we place a spacer & main widget
    // implementation note: spacer added first to ensure it always lines up on left

    mainLayout = new QGridLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->setHorizontalSpacing(10);

    //
    // create radio button
    //
    button = new QRadioButton();
    mainLayout->addWidget(button,1,0);

    //
    // create variable name block
    //
    variableLabel = new QLabel(tr("Variable Name"));
    variableName  = new QLineEdit();
    variableName->setMaximumWidth(100);
    variableName->setMinimumWidth(100);
    variableName->setText("");
    mainLayout->addWidget(variableLabel,0,1);
    mainLayout->addWidget(variableName,1,1);

    //
    // create data type block
    //
    typeLabel    = new QLabel(tr("Input Type"));
    typeComboBox = new QComboBox();
    typeComboBox->setMaximumWidth(200);
    typeComboBox->setMinimumWidth(200);

    mainLayout->addWidget(typeLabel,0,2);
    mainLayout->addWidget(typeComboBox,1,2);


    typeComboBox->addItem(tr("Parameters"));
    typeComboBox->addItem(tr("Moments"));
    typeComboBox->addItem(tr("Dataset"));


    connect(typeComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(typeChanged(QString)));
    typeComboBox->setCurrentIndex(0);

    //
    // create distribution block
    //
    distributionLabel    = new QLabel(tr("Distribution"));
    distributionComboBox = new QComboBox();
    distributionComboBox->setMaximumWidth(200);
    distributionComboBox->setMinimumWidth(200);

    mainLayout->addWidget(distributionLabel,0,3);
    mainLayout->addWidget(distributionComboBox,1,3);

    //
    // provide the user selectable options & connect the combo boxes selection
    // signal to this classes distributionChange slot method
    //

    if (variableClass == QString("Design")) {

        distributionComboBox->addItem(tr("ContinuousDesign"));
        distributionComboBox->addItem(tr("Constant"));

    } else if (variableClass == QString("Uncertain")) {

        distributionComboBox->addItem(tr("Normal"));
        distributionComboBox->addItem(tr("Lognormal"));
        distributionComboBox->addItem(tr("Beta"));
        distributionComboBox->addItem(tr("Uniform"));
        distributionComboBox->addItem(tr("Weibull"));
        distributionComboBox->addItem(tr("Gumbel"));
        distributionComboBox->addItem(tr("Constant"));
    }

    if (uqEngineName==QString("SimCenterUQ")){

        distributionComboBox->addItem(tr("Exponential"));
        distributionComboBox->addItem(tr("Discrete"));
        distributionComboBox->addItem(tr("Gamma"));
        distributionComboBox->addItem(tr("Chisquare"));
        distributionComboBox->addItem(tr("Truncated exponential"));
    }

    connect(distributionComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(distributionChanged(QString)));

    if (uqengin!=QString("SimCenterUQ")){
        typeLabel->setVisible(false);
        typeComboBox->setVisible(false);
    }

    //mainLayout->addStretch();

    theDistribution = new NormalDistribution();
    mainLayout->addWidget(theDistribution,0,4,2,1);
    connect(theDistribution,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));

    //this->setLayout(mainLayout);
  // mainLayout->setSizeConstraint(QLayout::SetMaximumSize);
}

RandomVariable::~RandomVariable()
{

}

RandomVariable::RandomVariable(const QString &type,
                               const QString &rvName,
                               QString uqengin,
                               QWidget *parent)
    :RandomVariable(type, uqengin, parent)
{
    variableName->setText(rvName);

}

RandomVariable::RandomVariable(const QString &type,
                               const QString &rvName,
                               RandomVariableDistribution &theD,
                               QString uqengin,
                               QWidget *parent)
    :RandomVariable(type, uqengin, parent)
{
    variableName->setText(rvName);

    // now change the distribution to constant and set value
    int index = distributionComboBox->findText(theD.getAbbreviatedName());
    distributionComboBox->setCurrentIndex(index);

    // remove old
    mainLayout->removeWidget(theDistribution);
    delete theDistribution;

    // set new
    theDistribution = &theD;
    mainLayout->addWidget(theDistribution,0,4,2,1);
    connect(theDistribution,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
}


 bool
 RandomVariable::isSelectedForRemoval(void)
{
  return button->isChecked();
 }


 QString
 RandomVariable::getVariableName(void){
     return variableName->text();
 }

bool
RandomVariable::outputToJSON(QJsonObject &rvObject){
    bool result = false;
    if (!variableName->text().isEmpty()) {
        rvObject["name"]=variableName->text();
        rvObject["value"]=QString("RV.") + variableName->text();
        rvObject["distribution"]=distributionComboBox->currentText();
        rvObject["inputType"]=typeComboBox->currentText();
        rvObject["variableClass"]=variableClass;
        rvObject["refCount"]=refCount;
        result = theDistribution->outputToJSON(rvObject);
    } else {
        emit sendErrorMessage("ERROR: RandomVariable - cannot output as no \"name\" entry!");
        return false;
    }
    return result;
}

bool
RandomVariable::inputFromJSON(QJsonObject &rvObject){

    QString distributionType, inputType;
    if (rvObject.contains("name")) {
        QJsonValue theName = rvObject["name"];
        variableName->setText(theName.toString());
    } else {
        return false;
    }

    bool typeSpecified = false;
    if (rvObject.contains("inputType")) {
        QJsonValue theInputTypeValue = rvObject["inputType"];
        inputType = theInputTypeValue.toString();
        typeSpecified = true;
    } else {
        inputType = "Parameters";
    }

    if (rvObject.contains("distribution")) {
        QJsonValue theDistributionValue = rvObject["distribution"];
        distributionType = theDistributionValue.toString();
        if (distributionType == "Lognormal" && typeSpecified == false)
            inputType = "Moments";
    } else {
        return false;
    }

    if (rvObject.contains("refCount")) {
        QJsonValue theCount= rvObject["refCount"];
        refCount = theCount.toInt();
    } else {
        return false;
    }

    if ((distributionType==tr("Exponential")) ||
        (distributionType==tr("Discrete")) ||
        (distributionType==tr("Gamma")) ||
        (distributionType==tr("Chisquare")) ||
        (distributionType==tr("Truncated exponential")))
    {
        distributionComboBox->addItem(tr("Exponential"));
        distributionComboBox->addItem(tr("Discrete"));
        distributionComboBox->addItem(tr("Gamma"));
        distributionComboBox->addItem(tr("Chisquare"));
        distributionComboBox->addItem(tr("Truncated exponential"));
    }

    int index1 = typeComboBox->findText(inputType);
    this->typeChanged(inputType);
    typeComboBox->setCurrentIndex(index1);
    typeOpt = QString(inputType);


    int index2 = distributionComboBox->findText(distributionType);
    this->distributionChanged(distributionType);
    distributionComboBox->setCurrentIndex(index2);
    return theDistribution->inputFromJSON(rvObject);

}

void RandomVariable::typeChanged(const QString &arg1) {

    if (theDistribution != 0) {
        delete theDistribution;
        theDistribution = 0;
    }
    theDistribution = new NormalDistribution(arg1);
    mainLayout->addWidget(theDistribution,0,4,2,1);
    distributionComboBox->setCurrentIndex(0);

    typeOpt = arg1;
}

// distributionChanged()
// this is the method called when the user changes the item in the selecetion box
// it's purpose is to change the input options the user is presented with depending on the
// distribution ..
void RandomVariable::distributionChanged(const QString &arg1)
{
    if (theDistribution != 0) {
        delete theDistribution;
        theDistribution = 0;
    }
    typeOpt = typeComboBox->currentText();

    if (arg1 == QString("Normal")) {
        theDistribution = new NormalDistribution(typeOpt);
    } else if (arg1 == QString("Lognormal")) {
        if (this->uqEngineName==QString("Dakota")) {
            typeComboBox->setCurrentIndex(1);
            theDistribution = new LognormalDistribution(QString("Moments"));
            // Dakota gets moments for lognormal
        } else {
            theDistribution = new LognormalDistribution(typeOpt);
        }
    } else if (arg1 == QString("Beta")) {
         theDistribution = new BetaDistribution(typeOpt);
    } else if (arg1 == QString("Uniform")) {
         theDistribution = new UniformDistribution(typeOpt);
    } else if (arg1 == QString("Constant")) {
         theDistribution = new ConstantDistribution();
    } else if (arg1 == QString("Weibull")) {
        theDistribution = new WeibullDistribution(typeOpt);
    } else if (arg1 == QString("Gumbel")) {
        theDistribution = new GumbelDistribution(typeOpt);
    } else if (arg1 == QString("Exponential")) {
        theDistribution = new ExponentialDistribution(typeOpt);
    } else if (arg1 == QString("Discrete")) {
        theDistribution = new DiscreteDistribution(typeOpt);
        //mainLayout->insertWidget(mainLayout->count()-1, theDistribution);
    } else if (arg1 == QString("Gamma")) {
        theDistribution = new GammaDistribution(typeOpt);
    } else if (arg1 == QString("Chisquare")) {
        theDistribution = new ChiSquaredDistribution(typeOpt);
    } else if (arg1 == QString("Truncated exponential")) {
        theDistribution = new TruncatedExponentialDistribution(typeOpt);
    } else if (arg1 == QString("ContinuousDesign")) {
         theDistribution = new ContinuousDesignDistribution();
    }

    if (theDistribution) {
      mainLayout->addWidget(theDistribution,0,4,2,1);
    }

    connect(theDistribution,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
}

   void
   RandomVariable::errorMessage(QString message) {
       emit sendErrorMessage(message);
   }
