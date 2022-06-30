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

#include "UserDefVec.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDebug>
#include <QFileDialog>


UserDefVec::UserDefVec(QWidget *parent) :RandomVariableDistribution(parent)
{
    //
    // create the main layout and add the input entries
    //
    QGridLayout *mainLayout = new QGridLayout(this);

    // set some defaults, and set layout for widget to be the horizontal layout
    mainLayout->setHorizontalSpacing(10);
    mainLayout->setVerticalSpacing(0);
    mainLayout->setMargin(0);
    scriptDir = this->createTextEntry(tr("Script File"), mainLayout,0);
    scriptDir->setMinimumWidth(200);
    scriptDir->setMaximumWidth(200);
    QPushButton *chooseFileButton = new QPushButton("Choose");
    mainLayout->addWidget(chooseFileButton,1,1);

    connect(chooseFileButton, &QPushButton::clicked, this, [=](){
              QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*)");
              if (!fileName.isEmpty()) {
                  scriptDir->setText(fileName);
              }
          });

    length = this->createTextEntry(tr("Length"), mainLayout,2);

    mainLayout->setColumnStretch(2,1);

}
UserDefVec::~UserDefVec()
{

}

bool
UserDefVec::outputToJSON(QJsonObject &rvObject){

    if (scriptDir->text().isEmpty()) {
        this->errorMessage("ERROR:User Defined Vector Distribution - script path has not been set");
        return false;
    }

    if (length->text().isEmpty()) {
        this->errorMessage("ERROR:User Defined Vector Distribution - length not specified");
        return false;
    }

    rvObject["scriptDir"]=QString(scriptDir->text());
    rvObject["length"]=QString(length->text()).toInt();


    return true;

}

bool
UserDefVec::inputFromJSON(QJsonObject &rvObject){


    if (rvObject.contains("scriptDir")) {
        QString theDataDir = rvObject["scriptDir"].toString();
        scriptDir->setText(theDataDir);
    } else {
        this->errorMessage("ERROR:  User Defined Vector Distribution - no \"scriptDir\" entry");
        return false;
    }

    if (rvObject.contains("length")) {
        //auto aa = rvObject["length"].toInt();
        length->setText( QString::number(rvObject["length"].toInt()));
    } else {
        this->errorMessage("ERROR: User Defined Vector Distribution - no \"length\" entry");
        return false;
    }


    return true;
}

bool
UserDefVec::copyFiles(QString fileDir) {
        return QFile::copy(scriptDir->text(), fileDir);
}

QString
UserDefVec::getAbbreviatedName(void) {
  return QString("UserDefVec");
}
