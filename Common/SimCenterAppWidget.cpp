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

    return SCUtils::recursiveCopy(sourceDir, destinationDir);
}


bool SimCenterAppWidget::areFilesIdentical(const QString& path1, const QString& path2) {
    QFileInfo info1(path1);
    QFileInfo info2(path2);

    // Check if files exist and are actual files
    if (!info1.exists() || !info1.isFile() || !info2.exists() || !info2.isFile()) {
        qDebug() << "ERROR: SimCenterAppWidget: areFilesIdentical - one or both paths are not valid files.";
        qDebug() << "Path 1:" << path1;
        qDebug() << "Path 2:" << path2;
        qDebug() << "File 1 exists:" << info1.exists() << ", is file:" << info1.isFile();
        qDebug() << "File 2 exists:" << info2.exists() << ", is file:" << info2.isFile();
        return false; // One or both paths are not valid files
    }

    // Compare sizes first for quick exit
    if (info1.size() != info2.size()) {
        qDebug() << "INFO: SimCenterAppWidget: areFilesIdentical - Files compared are different sizes, [" << info1.size() << "] and [" << info2.size() << ". Implies they are not identical.";
        return false;
    }

    // Option 1: Byte-by-byte comparison (for smaller files or when hash is not desired)
    QFile file1(path1);
    QFile file2(path2);

    if (!file1.open(QIODevice::ReadOnly) || !file2.open(QIODevice::ReadOnly)) {
        qDebug() << "ERROR: SimCenterAppWidget: areFilesIdentical - Failed to open one or both files for reading.";
        qDebug() << "Path 1:" << path1;
        qDebug() << "Path 2:" << path2;
        qDebug() << "File 1 open status:" << file1.isOpen();
        qDebug() << "File 2 open status:" << file2.isOpen();
        return false; // Failed to open one or both files
    }

    const int bufferSize = 4096; // Read in chunks
    char buffer1[bufferSize];
    char buffer2[bufferSize];

    // chunk limit to avoid memory issues with large files
    int chunkLimit = 1000000; // 1 million bytes (1 MB) allowed to be compared in total for time's sake
    qint64 totalBytesRead1 = 0;
    qint64 totalBytesRead2 = 0;
    while (!file1.atEnd() && !file2.atEnd()) {
        if (totalBytesRead1 >= chunkLimit || totalBytesRead2 >= chunkLimit) {
            qDebug() << "WARN: SimCenterAppWidget: areFilesIdentical - Chunk limit reached, stopping comparison.";
            file1.close();
            file2.close();
            return false; // Stop comparison if chunk limit is reached
        }

        // Read chunks from both files
        totalBytesRead1 += file1.pos();
        totalBytesRead2 += file2.pos();
        qint64 bytesRead1 = file1.read(buffer1, bufferSize);
        qint64 bytesRead2 = file2.read(buffer2, bufferSize);

        if (bytesRead1 != bytesRead2 || memcmp(buffer1, buffer2, bytesRead1) != 0) {
            qDebug() << "INFO: SimCenterAppWidget: areFilesIdentical - Files differ in content. Done via chunk analysis.";
            file1.close();
            file2.close();
            return false;
        }
    }
    qDebug() << "INFO: SimCenterAppWidget: areFilesIdentical - Files read are identical in content (atleast for the compared chunks, with byte limit of " << chunkLimit << " bytes).";
    file1.close();
    file2.close();
    return true; // Files are identical based on content

    /*
    // Option 2: Hash comparison (more efficient for large files)
    // Note: You would typically use one or the other, not both
    QFile file1(path1);
    QFile file2(path2);

    if (!file1.open(QIODevice::ReadOnly) || !file2.open(QIODevice::ReadOnly)) {
        return false; // Failed to open one or both files
    }

    QCryptographicHash hash1(QCryptographicHash::Sha256);
    QCryptographicHash hash2(QCryptographicHash::Sha256);

    // Add file content to hash
    hash1.addData(&file1);
    hash2.addData(&file2);

    file1.close();
    file2.close();

    return hash1.result() == hash2.result();
    */
}

bool
SimCenterAppWidget::copyFile(QString filename, QString destinationDir)
{
    qDebug() << "SimCenterAppWidget: " << filename << " " << destinationDir;
    
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

    qDebug() << "SimCenterAppWidget: " <<theFile << " " << thePath << " " << pathNewFile;
    
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
      QString msg = QString("WARNING file with same name as: ") + filename + QString(" exists in ") + destinationDir + QString(" . Checking to see if files are identical before copying...");
      qDebug() << msg;
      if (SimCenterAppWidget::areFilesIdentical(originalFileInfo.absoluteFilePath(), newFileInfo.absoluteFilePath())) {
        qDebug() << "SimCenterAppWidget: " << theFile << " already exists in " << destinationDir << " and is identical, not copied";
        return true;
      }
      // remove the file so we can copy the new one
      qDebug() << "SimCenterAppWidget: " << theFile << " already exists in " << destinationDir << " and is not identical, removing old file";
      newFile.remove();
    }

    qDebug() << "SimCenterAppWidget: " <<theFile << " " << thePath << " " << pathNewFile;;
    
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
