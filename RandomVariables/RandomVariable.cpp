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
#include "ConstantDistribution.h"
#include "ContinuousDesignDistribution.h"
#include "UserDef.h"
#include <QDebug>

RandomVariable::RandomVariable()
    :SimCenterWidget(0), refCount(0), variableClass(QString(""))
{

}

RandomVariable::RandomVariable(const QString &type, QWidget *parent)
    :SimCenterWidget(parent), refCount(0), variableClass(type)
{
    //
    // create a vertical layout to deal with variable name
    //

    QVBoxLayout *nameLayout = new QVBoxLayout();
    variableLabel = new QLabel();
    variableLabel->setText(QString("Variable Name"));
    variableName = new QLineEdit();
    variableName->setMaximumWidth(100);
    variableName->setMinimumWidth(100);
   // variableLabel->setContentsMargins(1,1,1,1);
   // variableName->setContentsMargins(1,1,1,1);
    nameLayout->addWidget(variableLabel);
    nameLayout->addWidget(variableName);
    nameLayout->setSpacing(1);
    nameLayout->setMargin(0);
    //nameLayout->addStretch();

    //
    // another vertical layout to deal with distribution selection
    //

    QVBoxLayout *distributionLayout = new QVBoxLayout();
    distributionLabel = new QLabel();
    distributionLabel->setText(QString("Distribution"));
    distributionComboBox = new QComboBox();
    distributionComboBox->setMaximumWidth(200);
    distributionComboBox->setMinimumWidth(200);
    distributionLayout->addWidget(distributionLabel);
    distributionLayout->addWidget(distributionComboBox);
    distributionLayout->setSpacing(0);
    distributionLayout->setMargin(0);
    //distributionLayout->addStretch();

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
        distributionComboBox->addItem(tr("Constant"));
        distributionComboBox->addItem(tr("Weibull"));
        distributionComboBox->addItem(tr("Gumbel"));
        //distributionComboBox->addItem(tr("UserDef"));
    }
    connect(distributionComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(distributionChanged(QString)));

    //
    // implementation note:
    // we needed to place the RandomVariable info inside another widget so as to ensure that it and
    // the RandomVAriableDistribution widget line up visually, there could be a Qt way to deal with this
    //

    //QHBoxLayout *widgetLayout = new QHBoxLayout;

    // create the main layout inside which we place a spacer & main widget
    // implementation note: spacer added first to ensure it always lines up on left

    mainLayout = new QHBoxLayout;

    button = new QRadioButton();
    mainLayout->addWidget(button);

    mainLayout->addLayout(nameLayout);
    mainLayout->addLayout(distributionLayout);
    mainLayout->addStretch();

    theDistribution = new NormalDistribution(this);
    mainLayout->insertWidget(mainLayout->count()-1, theDistribution);
     connect(theDistribution,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));

    mainLayout->setSpacing(10);
    mainLayout->setMargin(0);

    this->setLayout(mainLayout);
  // mainLayout->setSizeConstraint(QLayout::SetMaximumSize);
}

RandomVariable::~RandomVariable()
{

}


RandomVariable::RandomVariable(const QString &type,
                               const QString &rvName,
                               QWidget *parent)
    :RandomVariable(type, parent)
{
    variableName->setText(rvName);

}

RandomVariable::RandomVariable(const QString &type,
                               const QString &rvName,
                               RandomVariableDistribution &theD,
                               QWidget *parent)
    :RandomVariable(type, parent)
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
    mainLayout->insertWidget(mainLayout->count()-1, theDistribution);
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
    QString distributionType;
    if (rvObject.contains("name")) {
        QJsonValue theName = rvObject["name"];
        variableName->setText(theName.toString());
    } else {
        return false;
    }

    if (rvObject.contains("distribution")) {
        QJsonValue theDistributionValue = rvObject["distribution"];
        distributionType = theDistributionValue.toString();
    } else {
        return false;
    }

    if (rvObject.contains("refCount")) {
        QJsonValue theCount= rvObject["refCount"];
        refCount = theCount.toInt();
    } else {
        return false;
    }

    int index = distributionComboBox->findText(distributionType);
    this->distributionChanged(distributionType);
    distributionComboBox->setCurrentIndex(index);
    return theDistribution->inputFromJSON(rvObject);

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

    if (arg1 == QString("Normal")) {
        theDistribution = new NormalDistribution();
        mainLayout->insertWidget(mainLayout->count()-1, theDistribution);

    } else if (arg1 == QString("Lognormal")) {
        theDistribution = new LognormalDistribution();
        mainLayout->insertWidget(mainLayout->count()-1, theDistribution);

    } else if (arg1 == QString("Beta")) {
         theDistribution = new BetaDistribution();
         mainLayout->insertWidget(mainLayout->count()-1, theDistribution);
         // padhye added 4/32/18
          //qDebug () <<"Hello world I am degbuging this is BetaDistribution"; // padhye
    } else if (arg1 == QString("Uniform")) {
         theDistribution = new UniformDistribution();
         mainLayout->insertWidget(mainLayout->count()-1, theDistribution);
           // padhye added 4/32/18
            //qDebug () <<"Hello world I am degbuging this is Uniform"; // padhye
            //qDebug () <<QString("Uniform"); // padhy
    } else if (arg1 == QString("Constant")) {
         theDistribution = new ConstantDistribution();
         mainLayout->insertWidget(mainLayout->count()-1, theDistribution);
    } else if (arg1 == QString("ContinuousDesign")) {
         theDistribution = new ContinuousDesignDistribution();
         mainLayout->insertWidget(mainLayout->count()-1, theDistribution);
    } else if (arg1 == QString("Weibull")) {
        theDistribution = new WeibullDistribution();
        mainLayout->insertWidget(mainLayout->count()-1, theDistribution);
    } else if (arg1 == QString("Gumbel")) {
        theDistribution = new GumbelDistribution();
        mainLayout->insertWidget(mainLayout->count()-1, theDistribution);
    } else if (arg1 == QString("UserDef")) {
        theDistribution = new UserDef();
        mainLayout->insertWidget(mainLayout->count()-1, theDistribution);
    }

    connect(theDistribution,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
}

   void
   RandomVariable::errorMessage(QString message) {
       emit sendErrorMessage(message);
   }
