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

#include <SimCenterAppWidget.h>

#include <QJsonObject>
#include <QDir>
#include <QDebug>

#include <SC_ResultsWidget.h>
#include <Utils/FileOperations.h>

SimCenterAppWidget::SimCenterAppWidget(QWidget *parent)
    :SimCenterWidget(parent)
{
}

SimCenterAppWidget::~SimCenterAppWidget()
{

}


bool
SimCenterAppWidget::outputAppDataToJSON(QJsonObject &jsonObject)
{
    Q_UNUSED(jsonObject);
    return true;
}

bool
SimCenterAppWidget::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    Q_UNUSED(jsonObject);
    return true;
}


bool
SimCenterAppWidget::copyFiles(QString &destDir)
{
    Q_UNUSED(destDir);
    return true;
}

bool
SimCenterAppWidget::supportsLocalRun()
{
    return true;
}


void
SimCenterAppWidget::clear(void)
{

}


bool
SimCenterAppWidget::copyPath(QString sourceDir, QString destinationDir, bool overWriteDirectory)
{
  // don't copy self!
  if (sourceDir == destinationDir)
    return true;

  // don't copy entire quoFEM - sy
  if (sourceDir==QString("."))
    return true;

   QDir sourceDirectory(sourceDir);

    if (!sourceDirectory.exists()) {
        qDebug() << "Source Directory: " << sourceDir << " Does not exist";
        return false;
    }

    QDir destinationDirectory(destinationDir);

    // remove if existing
    if(destinationDirectory.exists() && overWriteDirectory) {
      if (SCUtils::isSafeToRemoveRecursivily(destinationDir))
        destinationDirectory.removeRecursively();
    }

    // now create
    if(!destinationDirectory.exists()) {
        destinationDirectory.mkdir(".");
    }

    SCUtils::recursiveCopy(sourceDir, destinationDir);
}


bool
SimCenterAppWidget::copyFile(QString filename, QString destinationDir)
{
    QFileInfo originalFileInfo(filename);
    
    if (filename.size() < 1){
      QString msg = QString("WARNING file to copy does not exist");
      qDebug() << msg;
      return false;
    }

    if (!originalFileInfo.exists()) {
      QString msg = QString("WARNING file to copy: ") + filename + QString(" does not exist!");
      qDebug() << msg;
      return false;
    }

    QString theFile = originalFileInfo.fileName();
    QString thePath = originalFileInfo.path();
    QString pathNewFile = QString(destinationDir + QDir::separator() + theFile);

    QFile originalFile(filename);    
    QFile newFile(pathNewFile);
    QFileInfo newFileInfo(pathNewFile);
    
    //
    // don't overwrite same file with itself
    //
    
    if (originalFileInfo.absoluteFilePath() == newFileInfo.absoluteFilePath())
      return true;

    //
    // don't overwrite file
    //
    
    if (newFileInfo.exists()) {
      QString msg = QString("WARNING file with same name as: ") + filename + QString(" exists in ") + destinationDir + QString(" file not copied");
      qDebug() << msg;
      return true;
    }

    return originalFile.copy(pathNewFile);
}

bool
SimCenterAppWidget::outputCitation(QJsonObject &jsonObject)
{
    Q_UNUSED(jsonObject);
  return true;
}

SimCenterAppWidget *
SimCenterAppWidget::getClone()
{
  return NULL;
}

SC_ResultsWidget *
SimCenterAppWidget::getResultsWidget(QWidget* parent)
{
  Q_UNUSED(parent);
  return NULL;
}

#ifdef _R2D
SC_ResultsWidget *
SimCenterAppWidget::getResultsWidget(QWidget *parent, QWidget *R2DresWidget, QMap<QString, QList<QString>> assetTypeToType)
{
  Q_UNUSED(parent);
  return NULL;
}
#endif
