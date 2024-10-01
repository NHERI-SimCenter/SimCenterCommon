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
#include <SimCenterAppWidget.h>

SC_FileEdit::SC_FileEdit(QString theKey, QStringList fileTypes)
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


  QString fileTypeStr = QString("All files (*.*)");
  if (fileTypes.count()>0){
    fileTypeStr = QString("File (*." + fileTypes.join(" *.") + ")");
  }

  connect(chooseFile, &QPushButton::clicked, this,
            [=]() {
        //QString fileName=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
        //QString fileName=QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)"); // sy - to continue from the previously visited directory
        QString fileName=QFileDialog::getOpenFileName(this,tr("Open File"),"", fileTypeStr); // sy - to continue from the previously visited directory
        theFile->setText(fileName);
        emit fileNameChanged(fileName);
     });
    theLayout->setMargin(0);
  //connect(chooseFile, SIGNAL(clicked(bool)), this, SLOT(chooseFileName(bool)));
  //connect(chooseFile, SIGNAL(clicked(bool)), this, SLOT(chooseFileName(bool)));
}


SC_FileEdit::SC_FileEdit(QString theKey, QString toolTip, QStringList fileTypes)
  :QWidget()
{

}

SC_FileEdit::~SC_FileEdit()
{

}

bool
SC_FileEdit::outputToJSON(QJsonObject &jsonObject)
{
    QString fileName = theFile->text();
    QFileInfo fileInfo(fileName);
    QString keyPath = key + QString("Path");
    
    jsonObject[key]= fileInfo.fileName();
    jsonObject[keyPath]=fileInfo.path();  
    return true;
}

bool
SC_FileEdit::inputFromJSON(QJsonObject &jsonObject)
{
        QString fileName;
        QString filePath;

        if (jsonObject.contains(key)) {
            QJsonValue theName = jsonObject[key];
            fileName = theName.toString();
        } else {
	  qDebug() << "ERROR: SC_FileEdit: no key: " << key << " in JSON object";
	  return false;
	}

	QString keyPath = key + QString("Path");
        if (jsonObject.contains(keyPath)) {
            QJsonValue theName = jsonObject[keyPath];
            filePath = theName.toString();
	    theFile->setText(QDir(filePath).filePath(fileName));  	    
        } else {
	    theFile->setText(fileName);
	    qDebug() << "WARNING: SC_FileEdit: no key: " << keyPath << " in JSON object, just using " << key << " as full filename";
            return false;
	}


    return true;
}

bool
SC_FileEdit::copyFile(QString &destDir) {
  return SimCenterAppWidget::copyFile(theFile->text(), destDir);  
}

QString
SC_FileEdit::getFilename(void) {
  return theFile->text();
}

void
SC_FileEdit::setFilename(QString &filename) {
  return theFile->setText(filename);
}
