// Written: fmckenna

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

// Written: fmckenna
// Modified: Dimitris, Aakash

#include "UQpySubsetSimulation.h"
#include <UQpyResultsSubsetSim.h>
#include <RandomVariablesContainer.h>

#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <SectionTitle.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>

#include <QStackedWidget>
//#include <SubsetSimulationWidget.h>

UQpySubsetSimulation::UQpySubsetSimulation(QWidget *parent)
: UQ_Method(parent)
{
    layout = new QVBoxLayout();
    mLayout = new QVBoxLayout();


    //
    // create layout for selection box for method type to layout
    //

    QHBoxLayout *methodLayout= new QHBoxLayout;
    QLabel *label1 = new QLabel();
    label1->setText(QString("Method"));
    reliabilityMethod = new QComboBox();
    reliabilityMethod->setMaximumWidth(200);
    reliabilityMethod->setMinimumWidth(200);
    reliabilityMethod->addItem(tr("Subset Simulation"));

    methodLayout->addWidget(label1);
    methodLayout->addWidget(reliabilityMethod);
    methodLayout->addStretch(1);

    mLayout->addLayout(methodLayout);

    //
    // qstacked widget to hold all widgets
    //

    theStackedWidget = new QStackedWidget();

    //theSubsetSim = new SubsetSimulationWidget();
    //theStackedWidget->addWidget(theSubsetSim);

    mLayout->addWidget(theStackedWidget);
    layout->addLayout(mLayout);

    this->setLayout(layout);

}

UQpySubsetSimulation::~UQpySubsetSimulation() {

}

bool
UQpySubsetSimulation::outputToJSON(QJsonObject &jsonObject) {
    return true;
}

bool
UQpySubsetSimulation::inputFromJSON(QJsonObject &jsonObject){
    return true;
}

bool
UQpySubsetSimulation::outputAppDataToJSON(QJsonObject &jsonObject) {
    return true;
}

bool
UQpySubsetSimulation::inputAppDataFromJSON(QJsonObject &jsonObject) {
    return true;
}


void
UQpySubsetSimulation::setRV_Defaults(void) {

}


int
UQpySubsetSimulation::getMaxNumParallelTasks(void) {
    return 1;
}
