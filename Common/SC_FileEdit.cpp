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

#include <SC_FileEdit.h>

#include <QLineEdit>
#include <QPushButton>
#include <QJsonObject>
#include <QGridLayout>
#include <QFileDialog>

SC_FileEdit::SC_FileEdit(QString theKey)
  :QWidget()
{
  key = theKey;

  QGridLayout *theLayout = new QGridLayout;
  
  theFile = new QLineEdit;    
  QPushButton *chooseFile = new QPushButton();
  chooseFile->setText(tr("Choose"));

  theLayout->addWidget(theFile,0,0);
  theLayout->addWidget(chooseFile, 0,1);
  this->setLayout(theLayout);

  connect(chooseFile, SIGNAL(clicked(bool)), this, SLOT(chooseFileName()));
}


SC_FileEdit::SC_FileEdit(QString theKey, QString toolTip)
  :QWidget()
{

}

SC_FileEdit::~SC_FileEdit()
{

}

bool
SC_FileEdit::outputToJSON(QJsonObject &jsonObject)
{
    return true;
}

bool
SC_FileEdit::inputFromJSON(QJsonObject &jsonObject)
{
    return true;
}

void
SC_FileEdit::chooseFileName(void) {
  QString fileName=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
  theFile->setText(fileName);
}

bool
SC_FileEdit::copyFiles(QString &destDir) {
  return true;
}

QString
SC_FileEdit::getFilename(void) {
  return theFile->text();
}

void
SC_FileEdit::setFilename(QString &filename) {
  return theFile->setText(filename);
}
