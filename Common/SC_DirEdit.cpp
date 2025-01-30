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

// Written: fmckenna, Sina Naeimi

#include <SC_DirEdit.h>

#include <QLineEdit>
#include <QPushButton>
#include <QJsonObject>
#include <QGridLayout>
#include <QFileDialog>
#include <SimCenterAppWidget.h>

SC_DirEdit::SC_DirEdit(QString theKey, bool copyFiles)
  :QWidget()
{
  key = theKey;

  copyFilesWhenCalled = copyFiles;
  
  QGridLayout *theLayout = new QGridLayout;
  
  theDirectory = new QLineEdit;    
  QPushButton *chooseFile = new QPushButton();
  chooseFile->setText(tr("Choose"));

  theLayout->addWidget(theDirectory,0,0);
  theLayout->addWidget(chooseFile, 0,1);
  this->setLayout(theLayout);
    connect(chooseFile, &QPushButton::clicked, this,
            [=]() {
        QString fileName=QFileDialog::getExistingDirectory(this,tr("Select Directory"),"", QFileDialog::ShowDirsOnly);

        if (fileName.isEmpty() == false){
          theDirectory->setText(fileName);
          emit dirNameChanged(fileName);
        }
     });
}


SC_DirEdit::SC_DirEdit(QString theKey, QString toolTip, bool copyFiles)
  :QWidget()
{

}

SC_DirEdit::~SC_DirEdit()
{

}

bool
SC_DirEdit::outputToJSON(QJsonObject &jsonObject)
{
    theDirectory->text();
    jsonObject[key]= theDirectory->text();

    return true;
}

bool
SC_DirEdit::inputFromJSON(QJsonObject &jsonObject)
{
  QString fileName;

  if (jsonObject.contains(key)) {
    QJsonValue theName = jsonObject[key];
    fileName = theName.toString();
    theDirectory->setText(fileName);
  }
  
  return true;
}

bool
SC_DirEdit::copyFile(QString &destDir, bool intoSubDirOfSameName) {

  QString dest = destDir;
  QDir destDirectory(destDir);
  
  if (copyFilesWhenCalled == true) {
    if (intoSubDirOfSameName == true) {
      
      //
      // before copyingg make dir of same name and set dest so copy into that new dir
      //
      
      QDir theOrigDir(theDirectory->text());
      QString dirName = theOrigDir.dirName();
      destDirectory.mkdir(dirName);
      dest = destDirectory.filePath(dirName);
    }
    
    return SimCenterAppWidget::copyPath(theDirectory->text(), dest, true);
    
  }
  
  else
    return true;
}

QString
SC_DirEdit::getDirName(void) {
  return theDirectory->text();
}

void
SC_DirEdit::setDirName(QString &filename) {
  return theDirectory->setText(filename);
}
