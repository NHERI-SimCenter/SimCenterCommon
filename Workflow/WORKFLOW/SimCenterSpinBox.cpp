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

// Written: Michael Gardner

#include <QSpinBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QString>

#include "SimCenterSpinBox.h"

SimCenterSpinBox::SimCenterSpinBox(const QJsonValue &inputObject,
                                     QWidget *parent)
    : SimCenterWidget(parent)
{
  // Configure SpinBox based on input JSON object
  theSpinBoxLabel = new QLabel();  
  theSpinBox = new QSpinBox();
  theSpinBoxLabel->setText(inputObject["name"].toString());  

  auto layout = new QHBoxLayout();
  layout->addWidget(theSpinBoxLabel);
  layout->addWidget(theSpinBox);
  layout->addStretch();

  this->setLayout(layout);
}

bool SimCenterSpinBox::inputFromJSON(QJsonObject& jsonObject) {
  bool result = true;

  theSpinBox->setObjectName(jsonObject["name"].toString());
  theSpinBox->setValue(jsonObject["value"].toDouble());

  return result;
}

bool SimCenterSpinBox::outputToJSON(QJsonObject& jsonObject) {

  jsonObject.insert("name", theSpinBoxLabel->text());
  jsonObject.insert("type", "SpinBox");
  jsonObject.insert("value", theSpinBox->value());

  return true;
}
