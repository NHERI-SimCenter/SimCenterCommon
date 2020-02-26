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

#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QString>

#include "SimCenterFileInput.h"

SimCenterFileInput::SimCenterFileInput(const QJsonValue &inputObject,
                                     QWidget *parent)
    : SimCenterWidget(parent)
{
  // Configure file line edit based on input JSON object
  theFileLineEdit = new QLineEdit();
  theFileLabel = new QLabel();
  theFileLabel->setText(inputObject["name"].toString());
  QPushButton * chooseFile = new QPushButton();
  chooseFile->setText("Choose");

  QHBoxLayout * layout = new QHBoxLayout();
  layout->addWidget(theFileLabel);
  layout->addWidget(theFileLineEdit);
  layout->addWidget(chooseFile);

  connect(chooseFile, &QPushButton::clicked, this, &SimCenterFileInput::chooseInputFile);

  this->setLayout(layout);
}

void SimCenterFileInput::chooseInputFile() {
  QString fileName = QFileDialog::getOpenFileName(this, "Open File", "C://",
                                                  "All files (*.*)");
  theFileLineEdit->setText(fileName);
}

bool SimCenterFileInput::inputFromJSON(QJsonObject& jsonObject) {
  bool result = true;

  theFileLabel->setText(jsonObject["name"].toString());
  theFileLineEdit->setText(jsonObject["value"].toString());

  return result;
}

bool SimCenterFileInput::outputToJSON(QJsonObject& jsonObject) {
  bool result = true;

  jsonObject.insert("name", theFileLabel->text());
  jsonObject.insert("type", "FileInput");
  jsonObject.insert("value", theFileLineEdit->text());

  return true;
}
