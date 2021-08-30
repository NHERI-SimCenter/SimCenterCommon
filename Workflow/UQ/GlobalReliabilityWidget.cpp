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

#include <GlobalReliabilityWidget.h>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>
#include <QButtonGroup>
#include <QCheckBox>
#include <QIntValidator>


GlobalReliabilityWidget::GlobalReliabilityWidget(QWidget *parent) 
: UQ_MethodInputWidget(parent)
{
     QGridLayout *layout = new QGridLayout();

    // create layout label and entry for # samples
    QHBoxLayout *methodLayout= new QHBoxLayout;
    QLabel *label2 = new QLabel();
    label2->setText(QString("GP Approximation"));
    gpApproximation = new QComboBox();
    gpApproximation->addItem(tr("x_gaussian_process"));
    gpApproximation->addItem(tr("u-gaussian_process"));
    gpApproximation->setToolTip("Create Gaussian Proccess Approximation in x-space or u-space");

    layout->addWidget(label2, 0,1);
    layout->addWidget(gpApproximation, 0,2);

    /*
    QButtonGroup *theButtonGroup = new QButtonGroup();
    checkedResponseLevel = new QCheckBox();
    checkedProbabilityLevel = new QCheckBox();
    checkedProbabilityLevel->setChecked(true);
    theButtonGroup->addButton(checkedResponseLevel);
    theButtonGroup->addButton(checkedProbabilityLevel);

    responseLevel = new QLineEdit();
    layout->addWidget(label3, 1,1);
    layout->addWidget(mppMethod, 1,2);

    layout->addWidget(checkedResponseLevel, 2,0);
    layout->addWidget(new QLabel("Response Levels"), 2, 1);
    layout->addWidget(responseLevel, 2, 2);
    layout->addWidget(checkedProbabilityLevel, 3,0);
    */

    /*
    probabilityLevel = new QLineEdit();
    probabilityLevel->setText(".02 .20 .40 .60 0.80 0.99");

    layout->addWidget(new QLabel("Probability Levels"), 1, 1);
    layout->addWidget(probabilityLevel, 1, 2);
    */

    responseLevel = new QLineEdit();
    responseLevel->setText("");
    //responseLevel->setText(".02 .20 .40 .60 0.80 0.99");
    responseLevel->setToolTip("Response level for which probability of exceedence is to be computed");
    layout->addWidget(new QLabel("Response Levels"), 1, 1);
    layout->addWidget(responseLevel, 1, 2);

    layout->addWidget(new QLabel("Seed"), 2, 1);

    srand(time(NULL));
    int randomNumber = rand() % 1000 + 1;
    seedEdit = new QLineEdit();
    seedEdit->setText(QString::number(randomNumber));
    seedEdit->setValidator(new QIntValidator);
    seedEdit->setToolTip("seed value, use of same seed in different studies will result in identical results");
    layout->addWidget(seedEdit, 2, 2);

    layout->setColumnStretch(3,4);
    layout->setRowStretch(3,1);

    this->setLayout(layout);
}

GlobalReliabilityWidget::~GlobalReliabilityWidget()
{

}

bool
GlobalReliabilityWidget::outputToJSON(QJsonObject &jsonObj){

    bool result = true;
    jsonObj["gpApproximation"]=gpApproximation->currentText();

    QJsonArray probLevel;
    if (responseLevel->text() == "") {
        qDebug() << "GlobalReliability - At least one response value must be set";
        this->errorMessage("GlobalReliability - At least one response value must be set");
        return false;
    }
    QStringList probLevelList = QStringList(responseLevel->text().split(" "));
    if (probLevelList.size() == 0) {
        this->errorMessage("GlobalReliability - At least one response value must be set");
        return false;
    }

    for (int i = 0; i < probLevelList.size(); ++i)
        probLevel.push_back(probLevelList.at(i).toDouble());
    jsonObj["responseLevel"]=probLevel;
    jsonObj["seed"]=seedEdit->text().toInt();

    /*
    QJsonArray respLevel;
    QStringList respLevelList = QStringList(responseLevel->text().split(" "));
    for (int i = 0; i < respLevelList.size(); ++i)
        respLevel.push_back(respLevelList.at(i).toDouble());
    jsonObj["responseLevel"]=respLevel;

    if (checkedResponseLevel->isChecked())
        jsonObj["activeLevel"]=QString("ResponseLevel");
    else
        jsonObj["activeLevel"]=QString("ProbabilityLevel");
    */

    return result;    
}

bool
GlobalReliabilityWidget::inputFromJSON(QJsonObject &jsonObject){

    bool result = false;
    if ( (jsonObject.contains("gpApproximation"))
         && (jsonObject.contains("responseLevel"))
         && (jsonObject.contains("seed"))
         ) {

        //responseLevel->setText("");
        responseLevel->setText("");

        QString scheme=jsonObject["gpApproximation"].toString();
        gpApproximation->setCurrentIndex(gpApproximation->findText(scheme));

        /*
         QString activeLevel=jsonObject["activeLevel"].toString();
         if (activeLevel ==  QString("ProbabilityLevel"))
             checkedProbabilityLevel->setChecked(true);
         else
             checkedResponseLevel->setChecked(true);

        QStringList respLevelList;
        */

        QJsonValue seedVal = jsonObject["seed"];
        int seedV = seedVal.toInt(); qDebug() << "SEED" << seedV;
        seedEdit->setText(QString::number(seedVal.toInt()));

        QJsonArray probLevels;

        QJsonValue probLevelVal = jsonObject["responseLevel"];
        if (probLevelVal.isArray()) {

            QStringList levelList;
            QJsonArray levels = probLevelVal.toArray();

            for (int i=0; i<levels.count(); i++) {
                QJsonValue value = levels.at(i);
                double levelV = value.toDouble();
                levelList << QString::number(levelV);
            }
            responseLevel->setText(levelList.join(" "));
        } else {
            qDebug() << "FORM Input: Response level not a json array";
        }


    /*
        QJsonValue respLevelVal = jsonObject["responseLevel"];
        if (respLevelVal.isArray()) {

            QStringList levelList;
            QJsonArray levels = respLevelVal.toArray();

            for (int i=0; i<levels.count(); i++) {
                QJsonValue value = levels.at(i);
                double levelV = value.toDouble();
                levelList << QString::number(levelV);
            }
            responseLevel->setText(levelList.join(" "));
        } else {
            qDebug() << "FORM Input: Response level not json array";
        }
*/

        return true;
    }

    return result;
}

void
GlobalReliabilityWidget::clear(void)
{

}



int
GlobalReliabilityWidget::getNumberTasks()
{
    return 1;
}
