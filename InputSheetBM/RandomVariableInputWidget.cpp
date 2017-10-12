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

#include "RandomVariableInputWidget.h"
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QDebug>

RandomVariableInputWidget::RandomVariableInputWidget(QWidget *parent) 
: SimCenterWidget(parent)
{
    verticalLayout = new QVBoxLayout();
    this->setLayout(verticalLayout);
    this->makeRV();
}

RandomVariableInputWidget::~RandomVariableInputWidget()
{

}

void
RandomVariableInputWidget::makeRV(void)
{
    // text and add button at top
    QHBoxLayout *textRandom = new QHBoxLayout();
    QLabel *textRV = new QLabel();
    textRV->setText(tr("Random Variables"));
    textRV->setMinimumWidth(250);
    textRV->setMaximumWidth(250);

    QPushButton *addRV = new QPushButton();
    addRV->setMinimumWidth(75);
    addRV->setMaximumWidth(75);
    addRV->setText(tr("Add"));
    connect(addRV,SIGNAL(clicked()),this,SLOT(addRandomVariable()));

    textRandom->addWidget(textRV);
    textRandom->addWidget(addRV);
    textRandom->addStretch();

    verticalLayout->addLayout(textRandom);

    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);

    rv = new QGroupBox(tr(""));

    rvLayout = new QVBoxLayout;
    rvLayout->addStretch();
    //setLayout(layout);
    rv->setLayout(rvLayout);

    this->addRandomVariable();

     sa->setWidget(rv);
     verticalLayout->addWidget(sa);

     verticalLayout->setSpacing(0);
     verticalLayout->setMargin(0);
}

void RandomVariableInputWidget::addRandomVariable(void)
{
   RandomVariable *theRV = new RandomVariable();
   theRandomVariables.append(theRV);
   rvLayout->insertWidget(rvLayout->count()-1, theRV);
}

void RandomVariableInputWidget::clear(void)
{
  // loop over random variables, removing from layout & deleting

  for (int i = 0; i <theRandomVariables.size(); ++i) {
    RandomVariable *theRV = theRandomVariables.at(i);
    rvLayout->removeWidget(theRV);
    delete theRV;
  }
  theRandomVariables.clear();
}


void
RandomVariableInputWidget::outputToJSON(QJsonObject &rvObject)
{
    QJsonArray rvArray;
    for (int i = 0; i <theRandomVariables.size(); ++i) {
        QJsonObject rv;
        theRandomVariables.at(i)->outputToJSON(rv);
        rvArray.append(rv);
    }
    rvObject["randomVariables"]=rvArray;
}


void
RandomVariableInputWidget::inputFromJSON(QJsonObject &rvObject)
{
  this->clear();

    // add the new
    QJsonArray rvArray = rvObject["randomVariables"].toArray();
    foreach (const QJsonValue &rvValue, rvArray) {
        QJsonObject rvObject = rvValue.toObject();
        RandomVariable *theRV = new RandomVariable();
        theRV->inputFromJSON(rvObject);
        theRandomVariables.append(theRV);
        rvLayout->insertWidget(rvLayout->count()-1, theRV);
    }
}
