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

#include "ContinuousDesignDistribution.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>


ContinuousDesignDistribution::ContinuousDesignDistribution(QWidget *parent) :RandomVariableDistribution(parent)
{
    //
    // create the main layout and add the input entries
    //
    QGridLayout *mainLayout = new QGridLayout(this);

    // set some defaults, and set layout for widget to be the horizontal layout
    mainLayout->setHorizontalSpacing(10);
    mainLayout->setVerticalSpacing(0);
    mainLayout->setMargin(0);

    min = this->createTextEntry(tr("Lower Bound"), mainLayout, 0);
    max = this->createTextEntry(tr("Upper Bound"), mainLayout, 1);
    initialPoint = this->createTextEntry(tr("Initial Point"), mainLayout, 2);

    mainLayout->setColumnStretch(3,1);
}

ContinuousDesignDistribution::~ContinuousDesignDistribution()
{

}


bool
ContinuousDesignDistribution::outputToJSON(QJsonObject &rvObject){
    // check for error condition, an entry had no value
    if (min->text().isEmpty() || max->text().isEmpty() || initialPoint->text().isEmpty()) {
        this->errorMessage("ERROR: Continuous Distribution - data has not been set");
        return false;
    }
    rvObject["lowerbound"]=min->text().toDouble();
    rvObject["upperbound"]=max->text().toDouble();
    rvObject["initialpoint"]=initialPoint->text().toDouble();

    return true;
}


bool
ContinuousDesignDistribution::inputFromJSON(QJsonObject &rvObject){

    if (rvObject.contains("lowerbound")) {
        QJsonValue theValue = rvObject["lowerbound"];
        min->setText(QString::number(theValue.toDouble()));

    } else {
        this->errorMessage("ERROR: Continuous Distribution - no \"lowerbound\" entry");
        return false;
    }


    if (rvObject.contains("upperbound")) {
        QJsonValue theValue = rvObject["upperbound"];
        max->setText(QString::number(theValue.toDouble()));
    } else {

        this->errorMessage("ERROR: Continuous Distribution - no \"upperbound\" entry");
        return false;
    }

    if (rvObject.contains("initialpoint")){
        QJsonValue theValue = rvObject["initialpoint"];
        initialPoint->setText(QString::number(theValue.toDouble()));
    } else {
        this->errorMessage("ERROR: Continuous Distribution - no \"initial point \" entry");

        return false;
    }

    return true;
}
bool
ContinuousDesignDistribution::copyFiles(QString fileDir) {
    return true;//do nothing
}

QString 
ContinuousDesignDistribution::getAbbreviatedName(void) {
  return QString("Continuous");
}
