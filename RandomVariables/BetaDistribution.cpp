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

#include "BetaDistribution.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

BetaDistribution::BetaDistribution(QWidget *parent)
    : RandomVariableDistribution(parent)
{

    //
    // create the main horizontal layout and add the input entries
    //

    QHBoxLayout *mainLayout = new QHBoxLayout();

    alphas = this->createTextEntry(tr("alphas"), mainLayout);
    betas = this->createTextEntry(tr("betas"), mainLayout);
    lowerBound = this->createTextEntry(tr("lowerbound"), mainLayout, 100, 100);
    upperBound = this->createTextEntry(tr("upperbound"), mainLayout, 100, 100);
    mainLayout->addStretch();

    // set some defaults, and set layout for widget to be the horizontal layout
    mainLayout->setSpacing(10);
    mainLayout->setMargin(0);
    this->setLayout(mainLayout);
}

BetaDistribution::~BetaDistribution()
{

}


bool
BetaDistribution::outputToJSON(QJsonObject &rvObject){
    // check for error condition, an entry had no value
    if (alphas->text().isEmpty() || betas->text().isEmpty() ||
            upperBound->text().isEmpty() || lowerBound->text().isEmpty()) {
        emit sendErrorMessage("ERROR: BetaDistribution - data has not been set");
        return false;
    }

    rvObject["alphas"]=alphas->text();
    rvObject["betas"]=betas->text();
    rvObject["lowerbound"]=lowerBound->text();
    rvObject["upperbound"]=upperBound->text();
    return true;
}

bool
BetaDistribution::inputFromJSON(QJsonObject &rvObject){

    //
    // for all entries, make sure i exists and if it does get it, otherwise return error
    //

    if (rvObject.contains("alphas")) {
        QJsonValue theValue = rvObject["alphas"];
        alphas->setText(QString::number(theValue.toDouble()));
    } else {
        emit sendErrorMessage("ERROR: Beta Distribution - no \"alphas\" entry");
        return false;
    }

    if (rvObject.contains("betas")) {
        QJsonValue theValue = rvObject["betas"];
        betas->setText(QString::number(theValue.toDouble()));
    } else {
        emit sendErrorMessage("ERROR: Beta Distribution - no \"betas\" entry");
        return false;
    }

    if (rvObject.contains("lowerbound")) {
        QJsonValue theValue = rvObject["lowerbound"];
        lowerBound->setText(QString::number(theValue.toDouble()));
    } else {
        emit sendErrorMessage("ERROR: Beta Distribution - no \"lowerbound\" entry");
        return false;
    }

    if (rvObject.contains("upperbound")) {
        QJsonValue theValue = rvObject["upperbound"];
        upperBound->setText(QString::number(theValue.toDouble()));
    } else {
        emit sendErrorMessage("ERROR: Beta Distribution - no \"lowerbound\" entry");
        return false;
    }

    return true;
}

QString 
BetaDistribution::getAbbreviatedName(void) {
  return QString("Beta");
}
