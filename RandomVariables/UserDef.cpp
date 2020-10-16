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

#include "UserDef.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDebug>


UserDef::UserDef(QWidget *parent) :RandomVariableDistribution(parent)
{
    //
    // create the main layout and add the input entries
    //
    QGridLayout *mainLayout = new QGridLayout(this);

    // set some defaults, and set layout for widget to be the horizontal layout
    mainLayout->setSpacing(10);
    mainLayout->setMargin(0);

    QPushButton *showPlotButton = new QPushButton("Show PDF");

    //alphaparam = this->createTextEntry(tr("Alpha"), mainLayout, 0);   // column 0
    //betaparam = this->createTextEntry(tr("Beta"), mainLayout, 1);     // column 1

    mainLayout->addWidget(showPlotButton,1,2);

    mainLayout->setColumnStretch(3,1);   // create a space.  Index is numper of columns == one past the last text/data column

}
UserDef::~UserDef()
{

}

bool
UserDef::outputToJSON(QJsonObject &rvObject){

    Q_UNUSED(rvObject);
    // check for error condition, an entry had no value
   // if (alphaparam->text().isEmpty() || betaparam->text().isEmpty()) {
     //   emit sendErrorMessage("ERROR: UserDef - data has not been set");
     //   return false;
   // }
   // rvObject["alphaparam"]=alphaparam->text().toDouble();
   // rvObject["betaparam"]=betaparam->text().toDouble();
    return true;
}

bool
UserDef::inputFromJSON(QJsonObject &rvObject){

    Q_UNUSED(rvObject);
    //
    // for all entries, make sure i exists and if it does get it, otherwise return error
    //

    /*if (rvObject.contains("alphaparam")) {
        QJsonValue theAlphaValue = rvObject["alphaparam"];
        alphaparam->setText(QString::number(theAlphaValue.toDouble()));
    } else {
        emit sendErrorMessage("ERROR: GumbelDistribution - no \"Alpha\" entry");
        return false;
    }

    if (rvObject.contains("betaparam")) {
        QJsonValue theBetaValue = rvObject["betaparam"];
        betaparam->setText(QString::number(theBetaValue.toDouble()));
    } else {
        emit sendErrorMessage("ERROR: GumbelDistribution - no \"Beta\" entry");
        return false;
    }*/

    return true;
}

QString
UserDef::getAbbreviatedName(void) {
  return QString("UserDef");
}
