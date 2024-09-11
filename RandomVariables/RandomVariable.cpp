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
#include <QDir>
#include <QPushButton>
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
#include "UserDefVec.h"


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
    // mainLayout->setMargin(0);
    mainLayout->setHorizontalSpacing(10);

    //
    // create radio button (not used any more - sy)
    //
    //button = new QRadioButton();
    //button->setToolTip("Select to remove");
    //mainLayout->addWidget(button,1,0);
    //button->setDisabled(true);
    //button->setVisible(false);  // not used any more



    //
    // create remove button - sy
    //
    removeButton = new QPushButton("×");
    const QSize BUTTON_SIZE = QSize(15, 15);
    removeButton->setFixedSize(BUTTON_SIZE);
    removeButton->setStyleSheet("QPushButton { font-size:15px;  font-weight: bold;padding: 0px 0px 2px 0px; }");
    mainLayout->addWidget(removeButton,1,0,2,1);
    connect(removeButton, SIGNAL(clicked()), this, SLOT(xButtonClicked()) );


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
    typeComboBox->setMaximumWidth(100);
    typeComboBox->setMinimumWidth(100);

    mainLayout->addWidget(typeLabel,0,2);
    mainLayout->addWidget(typeComboBox,1,2);

    //if (uqEngineName!=QString("SimCenterUQ")){
        typeComboBox->addItem(tr("Parameters"));
        typeComboBox->addItem(tr("Moments"));
        typeComboBox->addItem(tr("Dataset"));
    //}

    connect(typeComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(typeChanged(QString)));
    typeComboBox->setCurrentIndex(0);

    //
    // create distribution block
    //
    distributionLabel    = new QLabel(tr("Distribution"));
    distributionComboBox = new QComboBox();
    distributionComboBox->setMaximumWidth(150);
    distributionComboBox->setMinimumWidth(150);

    mainLayout->addWidget(distributionLabel,0,3);
    mainLayout->addWidget(distributionComboBox,1,3);

    //
    // provide the user selectable options & connect the combo boxes selection
    // signal to this classes distributionChange slot method
    //

    if (variableClass == QString("Design")) {

        distributionComboBox->addItem(tr("ContinuousDesign"));
        distributionComboBox->addItem(tr("Constant"));
        theDistribution = new ContinuousDesignDistribution();

    } else if (variableClass == QString("Uniform")) {

        distributionComboBox->addItem(tr("Uniform"));
        theDistribution = new UniformDistribution(0);

    } else if (variableClass == QString("NA")) {

        distributionComboBox->addItem(tr("None"));
        theDistribution = new UniformDistribution(0);
        distributionLabel->setVisible(false);
        distributionComboBox->setVisible(false);

    } else if (variableClass == QString("Uncertain")) {

        distributionComboBox->addItem(tr("Normal"));
        distributionComboBox->addItem(tr("Lognormal"));
        distributionComboBox->addItem(tr("Beta"));
        distributionComboBox->addItem(tr("Uniform"));
        distributionComboBox->addItem(tr("Weibull"));
        distributionComboBox->addItem(tr("Gumbel"));
        distributionComboBox->addItem(tr("Constant"));

        theDistribution = new NormalDistribution();

        if ((uqEngineName!=QString("Dakota")))
        {
            distributionComboBox->addItem(tr("Truncated exponential"));
            distributionComboBox->addItem(tr("Exponential"));
            distributionComboBox->addItem(tr("Discrete"));
            distributionComboBox->addItem(tr("Gamma"));
            distributionComboBox->addItem(tr("Chisquare"));
        }
        if ((uqEngineName==QString("Custom"))){
            distributionComboBox->addItem(tr("User Defined"));
            distributionComboBox->addItem(tr("User Defined Vector"));
        }





    }
    connect(distributionComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(distributionChanged(QString)));
    distributionComboBox->setCurrentIndex(0);
    if (! ((uqengin==QString("SimCenterUQ")) && (variableClass==QString("Uncertain")))){
        typeLabel->setVisible(false);
        typeComboBox->setVisible(false);
    }
    //mainLayout->addStretch();


    if (variableClass!=QString("NA")){
        mainLayout->addWidget(theDistribution,0,4,2,1);
    }// connect(theDistribution,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));



    //this->setLayout(mainLayout);
  // mainLayout->setSizeConstraint(QLayout::SetMaximumSize);
    mainLayout->setColumnStretch(6,1);
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
  variableName->setText(rvName.trimmed());
}

RandomVariable::RandomVariable(const QString &type,
                               const QString &rvName,
                               RandomVariableDistribution &theD,
                               QString uqengin,
                               QWidget *parent)
    :RandomVariable(type, uqengin, parent)
{
  variableName->setText(rvName.trimmed());

    // now change the distribution to constant and set value
    int index = distributionComboBox->findText(theD.getAbbreviatedName());
    distributionComboBox->setCurrentIndex(index);

    // remove old
    mainLayout->removeWidget(theDistribution);
    delete theDistribution;

    // set new
    theDistribution = &theD;

    if (variableClass == QString("NA")) {
        distributionLabel->setVisible(false);
        distributionComboBox->setVisible(false);
        //auto idx = mainLayout->indexOf(theDistribution);
        //if (idx>=0){
        //    mainLayout->removeItem(mainLayout->itemAt(idx));
        //    connect(removeButton, SIGNAL(clicked()), this, SLOT(xButtonClicked()) );
        //}

        // show only name
    } else {

        // set new
        mainLayout->addWidget(theDistribution,0,4,2,1);
    }


}

void
RandomVariable::xButtonClicked(void){
    emit removeRVclicked(this);
}

// bool
// RandomVariable::isSelectedForRemoval(void)
//{
//  return button->isChecked();
// }


 QString
 RandomVariable::getVariableName(void){
   return variableName->text().trimmed();
 }

 bool
 RandomVariable::copyFiles(QString fileDir){
 if (theDistribution!=NULL) {
     auto filePath = fileDir + QDir::separator() + variableName->text() +".in";
     if (QFile::exists(filePath))
         QFile::remove(filePath); // we will replace if it is already in the template dir

     return theDistribution->copyFiles(filePath);
     } else {
         return true;
     }
 }


bool
RandomVariable::outputToJSON(QJsonObject &rvObject){
    bool result = false;
    if (!variableName->text().isEmpty()) {
        rvObject["name"]=variableName->text().trimmed();
        rvObject["value"]=QString("RV.") + variableName->text().trimmed();
        rvObject["distribution"]=distributionComboBox->currentText();
        rvObject["inputType"]=typeComboBox->currentText();
        rvObject["variableClass"]=variableClass;
        rvObject["refCount"]=refCount;
        if (theDistribution!=NULL) {
            result = theDistribution->outputToJSON(rvObject);
        } else {
            result = true;
        }
    } else {
        this->errorMessage("ERROR: RandomVariable - cannot output as no \"name\" entry!");
        return false;
    }
    return result;
}

bool
RandomVariable::inputFromJSON(QJsonObject &rvObject){

    QString distributionType, inputType;
    if (rvObject.contains("name")) {
        QJsonValue theName = rvObject["name"];
        variableName->setText(theName.toString().trimmed());
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

//    if ((distributionType==tr("Exponential")) ||
//        (distributionType==tr("Discrete")) ||
//        (distributionType==tr("Gamma")) ||
//        (distributionType==tr("Chisquare")) ||
//        (distributionType==tr("Truncated exponential")))
//    {
//        distributionComboBox->addItem(tr("Exponential"));
//        distributionComboBox->addItem(tr("Discrete"));
//        distributionComboBox->addItem(tr("Gamma"));
//        distributionComboBox->addItem(tr("Chisquare"));
//        distributionComboBox->addItem(tr("Truncated exponential"));
//    }

//    if ((distributionType==tr("User defined vector")) ||
//         distributionType==tr("Truncated exponential")   )
//    {
//        distributionComboBox->addItem(tr("User defined vector"));
//        distributionComboBox->addItem(tr("User defined"));
//    }

    int index1 = typeComboBox->findText(inputType);
    this->typeChanged(inputType);
    typeComboBox->setCurrentIndex(index1);
    typeOpt = QString(inputType);


    int index2 = distributionComboBox->findText(distributionType);
    if (index2>=0) {
        this->distributionChanged(distributionType);
        distributionComboBox->setCurrentIndex(index2);
        if (distributionType!=QString("None"))
            theDistribution->inputFromJSON(rvObject);
    }
    if (distributionType==QString("")) {
        delete theDistribution;
        theDistribution = 0;
    }


//    if (rvObject.contains("variableClass")) {
//        QString oldVariableClass = variableClass;
//        variableClass = rvObject["variableClass"].toString(0);
//        uqEngineChanged( uqEngineName,  oldVariableClass);
//    } else {
//        return false;
//    }



    return true;
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
    if ((theDistribution != 0)) {
        delete theDistribution;
        theDistribution = 0;
    }

    if (this->uqEngineName==QString("Dakota")) {
        typeOpt = "Parameters";
    } else {
        typeOpt = typeComboBox->currentText();
    }

    if (arg1 == QString("Normal")) {
        theDistribution = new NormalDistribution(typeOpt);
    } else if (arg1 == QString("Lognormal")) {
        if (this->uqEngineName==QString("Dakota")) {
            //typeComboBox->setCurrentIndex(1);
            theDistribution = new LognormalDistribution(QString("Moments"));
            // Dakota gets moments for lognormal
            typeComboBox->setCurrentText("Moments");
            distributionComboBox->setCurrentText("Lognormal");

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
    } else if (arg1 == QString("User defined")) {
         theDistribution = new UserDef();
    } else if (arg1 == QString("User defined vector")) {
         theDistribution = new UserDefVec();
    }

    if (theDistribution) {
      mainLayout->addWidget(theDistribution,0,4,2,1);
    }

    // connect(theDistribution,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
}

//void RandomVariable::fixToUniform(double dValue)
//{
//    distributionComboBox->setCurrentIndex(3); // Uniform
//    distributionComboBox->setDisabled(1);
//    typeComboBox->setDisabled(1);

//    delete theDistribution;
//    theDistribution = 0;
//    theDistribution = new UniformDistribution(dValue);
//    mainLayout->addWidget(theDistribution,0,4,2,1);

//}

void RandomVariable::uqEngineChanged(QString newUqEngineName, QString newClass) {

    if ((newClass==variableClass) &&(newUqEngineName==uqEngineName)){
        return;
    }

    QString currentType = distributionComboBox->currentText();

    if ((newClass == QString("NA")) && (newClass != variableClass)) {
        // NA is for GP (data) - sy
        typeLabel->setVisible(false);
        typeComboBox->setVisible(false);
        distributionLabel->setVisible(false);
        distributionComboBox->setVisible(false);

        if (typeComboBox->currentText()!="Parameters"){
            typeComboBox->setCurrentText("Parameters");
        }
        for (int i=distributionComboBox->count()-1; i>=0; i--) {
            distributionComboBox->removeItem(i);
        }
        distributionComboBox->insertItem(0,tr("Uniform"));

        uqEngineName=newUqEngineName;
        variableClass=newClass;

        auto idx = mainLayout->indexOf(theDistribution);
        if (idx>=0){
            // RandomVariableDistribution *theD = theDistribution;
            // mainLayout->removeWidget(theDistribution);
            mainLayout->removeItem(mainLayout->itemAt(idx));
            delete theDistribution;
            theDistribution = NULL;
            typeChanged("Parameters"); // redefine RV
        }
        return;

    } else {
        distributionLabel->setVisible(true);
        distributionComboBox->setVisible(true);

        auto idx = mainLayout->indexOf(theDistribution);
        if (idx>0){
            mainLayout->addWidget(theDistribution,0,4,2,1);
        }
    }

    if ((newClass == QString("Uniform")) && (newClass != variableClass)) {

        // Uniform only is for GP (Only bounds matters) - sy
        typeLabel->setVisible(false);
        typeComboBox->setVisible(false);
        if (typeComboBox->currentText()!="Parameters"){
            typeComboBox->setCurrentText("Parameters");
        }

        int unifIdx=-1;
        if (currentType==QString("Uniform")){
            unifIdx = distributionComboBox->findText("Uniform");
            distributionComboBox->blockSignals(true);
        }

        for (int i=distributionComboBox->count()-1; i>=0; i--) {
            if (i!=unifIdx) {
                distributionComboBox->removeItem(i);
            }
        }

        if (unifIdx<0) {
            distributionComboBox->insertItem(0,tr("Uniform"));
        }

        uqEngineName=newUqEngineName;
        variableClass=newClass;
        return;
    }

    if ((newClass == QString("Design")) && (newClass != variableClass)) {
        // Design only is for Deterministic Optimization - sy
            typeLabel->setVisible(false);
            typeComboBox->setVisible(false);

            for (int i=distributionComboBox->count()-1; i>=0; i--) {
                if (distributionComboBox->itemText(i) == QString("Constant")) {
                    // pass
                } else if (distributionComboBox->itemText(i) == QString("Uniform")) {
                    //if uniform, convert it to ContinuousDesign
                    distributionComboBox->removeItem(i);
                } else {
                    distributionComboBox->removeItem(i);
                }
            }
            distributionComboBox->insertItem(0,tr("ContinuousDesign"));
            if (currentType == QString("Constant")) {
                distributionComboBox->setCurrentIndex(1);
            } else {
                distributionComboBox->setCurrentIndex(0);
            }
            //theDistribution = new ContinuousDesignDistribution();

            uqEngineName=newUqEngineName;
            variableClass=newClass;
            return;

    }

    if ((newClass == QString("Uncertain")) && (newClass != variableClass)) {
                int unifIdx=-1;
                if (currentType==QString("Uniform")){
                    unifIdx = distributionComboBox->findText("Uniform");
                    distributionComboBox->blockSignals(true);
                }

                for (int i=distributionComboBox->count()-1; i>=0; i--) {
                    if (i!=unifIdx) {
                        distributionComboBox->removeItem(i);
                    }
                }

                distributionComboBox->insertItem(0,tr("Normal"));
                distributionComboBox->insertItem(1,tr("Lognormal"));
                distributionComboBox->insertItem(2,tr("Beta"));
                if (unifIdx<0) {
                    distributionComboBox->insertItem(3,tr("Uniform"));
                }
                distributionComboBox->insertItem(4,tr("Weibull"));
                distributionComboBox->insertItem(5,tr("Gumbel"));
                distributionComboBox->insertItem(6,tr("Constant"));
                distributionComboBox->blockSignals(false);
                // 6 is continuous, 7 is constant.
                // if continuousDesign, convert it to uniform
                // remove
                //int idx = distributionComboBox->findText("ContinuousDesign");
                //distributionComboBox->removeItem(idx);
    }


    if (newUqEngineName==QString("SimCenterUQ")){
        typeLabel->setVisible(true);
        typeComboBox->setVisible(true);
        if (currentType==QString("Uniform")){
            distributionComboBox->blockSignals(true);
        }
        if (distributionComboBox->count()<8) {
            distributionComboBox->addItem(tr("Exponential"));
            distributionComboBox->addItem(tr("Discrete"));
            distributionComboBox->addItem(tr("Gamma"));
            distributionComboBox->addItem(tr("Chisquare"));
            distributionComboBox->addItem(tr("Truncated exponential"));
            distributionComboBox->blockSignals(false);
        }
    }
    if (newUqEngineName==QString("Dakota")){
        typeLabel->setVisible(false);
        typeComboBox->setVisible(false);
        for (int i=distributionComboBox->count(); i>6; i--) {
            distributionComboBox->removeItem(i);
        }

        QString distName = distributionComboBox->currentText();
        if (distName=="Lognormal") {
            //if (typeComboBox->currentText()!="Moments"){
            typeComboBox->setCurrentText("Moments");
            distributionComboBox->setCurrentText(distName);
            //}
        } else {
            //if (typeComboBox->currentText()!="Parameters"){
            typeComboBox->setCurrentText("Parameters");
            distributionComboBox->setCurrentText(distName);
            //}
        }
    }

    if (newUqEngineName==QString("UCSD")){

        typeLabel->setVisible(false);
        typeComboBox->setVisible(false);
        for (int i=distributionComboBox->count(); i>6; i--) {
            distributionComboBox->removeItem(i);
        }
        if (distributionComboBox->count()<8) {
            distributionComboBox->addItem(tr("Exponential"));
            distributionComboBox->addItem(tr("Discrete"));
            distributionComboBox->addItem(tr("Gamma"));
            distributionComboBox->addItem(tr("Chisquare"));
            distributionComboBox->addItem(tr("Truncated exponential"));
        }
        typeComboBox->setCurrentText("Parameters");
    }

    if (newUqEngineName==QString("Custom")){

        typeLabel->setVisible(false);
        typeComboBox->setVisible(false);
        for (int i=distributionComboBox->count(); i>6; i--) {
            distributionComboBox->removeItem(i);
        }
        if (distributionComboBox->count()<8) {
            distributionComboBox->addItem(tr("Exponential"));
            distributionComboBox->addItem(tr("Discrete"));
            distributionComboBox->addItem(tr("Gamma"));
            distributionComboBox->addItem(tr("Chisquare"));
            distributionComboBox->addItem(tr("Truncated exponential"));
            distributionComboBox->addItem(tr("User defined"));
            distributionComboBox->addItem(tr("User defined vector"));
        }
        typeComboBox->setCurrentText("Parameters");
    }

    uqEngineName=newUqEngineName;
    variableClass=newClass;
    return;
}

QString
RandomVariable::getAbbreviatedName(void) {
    if (theDistribution!=NULL) {
        return theDistribution->getAbbreviatedName();
    } else {
        return QString("");
    }
}

