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

#include <LineEditRV.h>
#include <QJsonObject>
#include <QString>
#include <RandomVariablesContainer.h>

LineEditRV::LineEditRV(RandomVariablesContainer *theRandomVariableContainer, QWidget *parent)
:QLineEdit(parent), theRVC(theRandomVariableContainer)
{
    //connect(this,SIGNAL(editingFinished()),this,SLOT(on_editingFinished()));
    connect(this,SIGNAL(textChanged(QString)),this,SLOT(on_editingFinished()));
}

LineEditRV::~LineEditRV()
{

    bool ok;
    QString currentText = this->text();
    double value = currentText.toDouble(&ok);
    Q_UNUSED(value);
    if (!ok)
      theRVC->removeRandomVariable(currentText);
}

bool
LineEditRV::outputToJSON(QJsonObject &jsonObject, QString key)
{
    QString valueText = this->text();
    bool ok;

    if (valueText.isEmpty()) {
        qDebug() << "ERROR: LineRV: no key: " << key << " in JSON object";
        return false;
    }

    double valueDouble = valueText.QString::toDouble(&ok);
    if (ok == true)
        jsonObject[key]=valueDouble;
    else
        jsonObject[key]= QString("RV.") + valueText;

    return true;
}

bool
LineEditRV::inputFromJSON(QJsonObject &jsonObject, QString key)
{
    if (jsonObject.contains(key)) {

        QJsonValue theValue = jsonObject[key];
        if (theValue.isString()) {
            oldText = theValue.toString();
            oldText.remove(0,3); // remove RV.
            this->setText(oldText);
        } else if (theValue.isDouble()) {
            oldText = QString::number(theValue.toDouble());
            this->setText(oldText);
        }
    } else {
        qDebug() << "LineEditRV::iinputFRomJSON - key not found, key: " << key;
        return false;
    }

    return true;
}


void 
LineEditRV::on_editingFinished() {

  QString currentText = this->text();
  
  if (oldText != currentText) {
    bool ok;

    // if old text not double, remove random Variable
    double value = oldText.toDouble(&ok);
    Q_UNUSED(value);
    if (!ok) {
      theRVC->removeRandomVariable(oldText);
    }

    // if new text not double, add random variable
    value = currentText.toDouble(&ok);
    Q_UNUSED(value);
    if (!ok) {
      theRVC->addRandomVariable(currentText);
    }
    oldText = currentText;
  }
}
