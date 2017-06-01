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

//
// headers for RandomVariableDistribution subclasses that user can select
//

#include "NormalDistribution.h"
#include "LognormalDistribution.h"
#include "BetaDistribution.h"

RandomVariable::RandomVariable(QWidget *parent)
    :QWidget(parent)
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
    nameLayout->addWidget(variableLabel);
    nameLayout->addWidget(variableName);
    nameLayout->setSpacing(10);
    nameLayout->setMargin(0);

    //
    // another vertical layout to deal with distribution selection
    //

    QVBoxLayout *distributionLayout = new QVBoxLayout();
    distributionLabel = new QLabel();
    distributionLabel->setText(QString("Distribution"));
    distributionComboBox = new QComboBox();
    distributionComboBox->setMaximumWidth(100);
    distributionComboBox->setMinimumWidth(100);
    distributionLayout->addWidget(distributionLabel);
    distributionLayout->addWidget(distributionComboBox);
    distributionLayout->setSpacing(10);
    distributionLayout->setMargin(0);

    //
    // provide the user selectable options & connect the combo boxes selection
    // signal to this classes distributionChange slot method
    //

    distributionComboBox->addItem(tr("Normal"));
    distributionComboBox->addItem(tr("Lognormal"));
    distributionComboBox->addItem(tr("Beta"));

    connect(distributionComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(distributionChanged(QString)));

    //
    // implementation note:
    // we needed to place the RandomVariable info inside another widget so as to ensure that it and
    // the RandomVAriableDistribution widget line up visually, there could be a Qt way to deal with this
    //

    // create this widget & a hozizontal layout and place our 2 verticaal layouts inside
    QWidget *theWidget = new QWidget();
    QHBoxLayout *widgetLayout = new QHBoxLayout;

    // create the main layout inside which we place a spacer & main widget
    // implementation note: spacer added first to ensure it always lines up on left

    mainLayout = new QHBoxLayout;

    mainLayout->addLayout(nameLayout);
    mainLayout->addLayout(distributionLayout);
    mainLayout->addStretch();

    theDistribution = new NormalDistribution(this);
    mainLayout->insertWidget(mainLayout->count()-1, theDistribution);

    mainLayout->setSpacing(10);
    mainLayout->setMargin(0);

    this->setLayout(mainLayout);

}

RandomVariable::~RandomVariable()
{

}

void RandomVariable::outputToJSON(QJsonObject &rvObject){
    if (!variableName->text().isEmpty()) {
       rvObject["name"]=variableName->text();
       rvObject["distribution"]=distributionComboBox->currentText();
       theDistribution->outputToJSON(rvObject);
    }
}

void RandomVariable::inputFromJSON(QJsonObject &rvObject){
    QJsonValue theName = rvObject["name"];
    variableName->setText(theName.toString());
    QJsonValue theDistributionValue = rvObject["distribution"];
    QString distributionType = theDistributionValue.toString();

    if (distributionType == QString("Normal")) {
        int index = distributionComboBox->findText(tr("Normal"));
        distributionComboBox->setCurrentIndex(index);
        theDistribution->inputFromJSON(rvObject);

    } else if (distributionType == QString("Lognormal")) {
        int index = distributionComboBox->findText(tr("Lognormal"));
        distributionComboBox->setCurrentIndex(index);
        theDistribution->inputFromJSON(rvObject);

    } else if (distributionType == QString("Beta")) {
        int index = distributionComboBox->findText(tr("Beta"));
        distributionComboBox->setCurrentIndex(index);
        theDistribution->inputFromJSON(rvObject);
    }

   // rvObject["distribution"]=distributionComboBox->currentText();
   // theDistribution->outputToJSON(rvObject);
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
    }
}
