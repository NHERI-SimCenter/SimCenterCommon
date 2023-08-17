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

// Written: fmckenna, sangri

#include <MultiFidelitySimWidget.h>
#include <RandomVariablesContainer.h>
#include "SC_DoubleLineEdit.h"
#include "SC_IntLineEdit.h"

#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>
#include <QCheckBox>


MultiFidelitySimWidget::MultiFidelitySimWidget(QWidget *parent)
: UQ_Method(parent)
{
    auto Layout = new QGridLayout();
    this->setLayout(Layout);
    int npilot_default = 40;

    //
    // Basic layouts
    //

    maxTime = new SC_DoubleLineEdit("maxTime",20);
    seed = new SC_IntLineEdit("seed",42);
    helpText = new QLabel("Note: At least "+ QString::number(npilot_default) +" samples will be simulated for each model even if the time exceed the specified limit.\n");
    helpText->setStyleSheet("color: grey");
    advancedCheckBox = new QCheckBox("Advanced Options");
    advancedGroup = new QWidget();

    Layout -> addWidget(new QLabel("Max Computation Time (min)"),0,0);
    Layout -> addWidget(maxTime,0,1);
    Layout -> addWidget(new QLabel("seed"),1,0);
    Layout -> addWidget(seed,1,1);
    Layout -> addWidget(helpText,2,0,1,-1);
    Layout -> addWidget(advancedCheckBox,3,0,1,-1);
    Layout -> addWidget(advancedGroup,4,0,1,-1);
    Layout -> setRowStretch(5,1);
    Layout -> setColumnStretch(2,1);

    //
    // Advanced Options
    //

    advancedCheckBox ->setStyleSheet("font-weight: bold");

    QGridLayout *advancedOptions= new QGridLayout();
    numPilot = new SC_IntLineEdit("numPilot", npilot_default);
    advancedOptions -> addWidget(new QLabel("Minimum # simulations per model"),0,0);
    advancedOptions -> addWidget(numPilot,0,1);
    advancedOptions -> setColumnStretch(2,1);
    advancedOptions -> setMargin(0);

    advancedGroup ->setLayout(advancedOptions);
    advancedGroup -> setVisible(false);

    connect(advancedCheckBox,SIGNAL(toggled(bool)),advancedGroup,SLOT(setVisible(bool)));
}

MultiFidelitySimWidget::~MultiFidelitySimWidget()
{

}

void
MultiFidelitySimWidget::updateHelpText(void)
{

    helpText->setText("Note: At least "+ QString::number(numPilot->text().toInt()) +" samples will be simulated for each model even if the time exceed the specified limit.\n");
}

bool
MultiFidelitySimWidget::outputToJSON(QJsonObject &jsonObj){
    bool result = true;

    maxTime->outputToJSON(jsonObj);
    seed->outputToJSON(jsonObj);

    jsonObj["advancedCheckBox"] = advancedCheckBox->isChecked();
    if (advancedCheckBox->isChecked()) {
        numPilot->outputToJSON(jsonObj);
    } else {
        jsonObj["numPilot"] = 40;
    }
    return result;    
}

bool
MultiFidelitySimWidget::inputFromJSON(QJsonObject &jsonObject){
    bool result = true;

    maxTime->inputFromJSON(jsonObject);
    seed->inputFromJSON(jsonObject);

    advancedCheckBox->setChecked(jsonObject["advancedCheckBox"].toBool());

    if (jsonObject.contains("numPilot")) {
        numPilot->inputFromJSON(jsonObject);
   }
    return result;
}

void
MultiFidelitySimWidget::clear(void)
{
    advancedCheckBox->setChecked(false);
}



int
MultiFidelitySimWidget::getNumberTasks()
{
  return 0; // what's this?
}
