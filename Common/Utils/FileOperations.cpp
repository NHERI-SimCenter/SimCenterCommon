#include "FileOperations.h"
#include <QJsonArray>
#include <QFileInfo>
#include <QDir>
#include <QApplication>
#include <QStandardPaths>
#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>


namespace SCUtils {

  QString getAppWorkDir() {



    //
    // appWorkDir is typically in ~/Documents/appName
    //   -- if no Documents place in ~/appName
    //   -- don't want OneDrive or similar!
    //

    // find directory where appName filder to be: standardDocPath
    
    QString standardDocPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation); 
    if (standardDocPath.contains("OneDrive", Qt::CaseInsensitive)) {
        QString userProfilePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        QDir userDocumentsDir(userProfilePath + QDir::separator() + "Documents");
        // Verify that the local Documents directory exists
        if (userDocumentsDir.exists()) 
            standardDocPath = userDocumentsDir.absolutePath();
        else
	  // use users home dir	  
	  standardDocPath = userProfilePath;
    }
	
    QString workDirPath = standardDocPath + QDir::separator() + QCoreApplication::applicationName();

    // if the appName dir does not exist, create it
    QDir workDir(workDirPath);
    if (!workDir.exists())
        if (!workDir.mkpath(workDirPath)) {
	  
            qDebug() << QString("Could not create Working Dir: ") << workDirPath;

	    QMessageBox msgBox;
	    msgBox.setIcon(QMessageBox::Critical);
	    QString text = QString("FATAL: The application could not create a folder/directory necessary to run the application. Try creating the following folder on your machine and start again. ") + workDirPath; 
	    msgBox.setText(text);
	    msgBox.setWindowTitle("Fatal Error");
	    QPushButton *exitButton = msgBox.addButton(QMessageBox::Ok);
	    QObject::connect(exitButton, &QPushButton::clicked, qApp, &QApplication::quit);
	    msgBox.exec();	    
	}	    

    
    return workDirPath;
  }
  
  bool
  compareFiles(const QString &sourcePath, const QString &destPath) {

    // open up files and bytewise compare or create checksunms & compare .. needed before a file copy
    //   -- try bytewise compare as probably quicjest as checksum requires same thing
    //       -- of course the built in method for checksum might be better optimized than mine!

    // function assuming files exist!
    QFile sourceFile(sourcePath);
    QFile destFile(destPath);

    int bufferSize = 4096;
    while (!sourceFile.atEnd() && !destFile.atEnd()) {

      // read chunks ar a time and compare
      QByteArray bufferSource = sourceFile.read(bufferSize);
      QByteArray bufferDest = destFile.read(bufferSize);
      if (bufferSource != bufferDest) {
	// qDebug() << "FileOperations::compareFile: " << sourcePath << " " << destPath << " DIFFERENT";	
	return false;  
      }
    }

    //    qDebug() << "FileOperations::compareFile: " << sourcePath << " " << destPath << " SAME";
    return true;
  }

  bool
  recursiveCopy(const QString &sourcePath, const QString &destPath) {
    
    QDir sourceDir(sourcePath);
    
    // check source dir exists .. otherwise nothing to be copied, so quick return .. probably an error!
    if (!sourceDir.exists()) {
        qDebug() << "FileApplications::recursiveCopy() source directory does not exist:" << sourcePath;
        return false;
    }

    QDir destDir(destPath);

    // if destination directory does not exist, create it
    if (!destDir.exists()) {
        destDir.mkpath(destPath);
	if (!destDir.exists()) { // .. check again
	  qDebug() << "FileApplications::recursiveCopy() dest directory did not exist and unable to create it:" << destDir;
	}
    }

    // Get the list of files in the source directory & copy
    //  - by copy if file already exists compare and make sure they are the same .. if not there straight copy
    foreach (QString fileName, sourceDir.entryList(QDir::Files)) {
        QString srcFilePath = sourceDir.filePath(fileName);
        QString destFilePath = destDir.filePath(fileName);
	  
	if (QFile::exists(destFilePath)) {

	  // code to see if files are the same
	  if (!SCUtils::compareFiles(srcFilePath, destFilePath)) {
	    qDebug() << "FileOperations::recursiveCopy() dest File : " << destFilePath << " already exists and is different from source file:" << srcFilePath;
	    return false;	    
	  }
	  
	} else {
	  // Copy the file from source to destination
	  if (!QFile::copy(srcFilePath, destFilePath)) {
            qDebug() << "FileOperations::recursiveCopy() Failed to copy file:" << srcFilePath;
            return false;
	  }
	}
    }

    // recursively copy subdirectories
    foreach (QString dirName, sourceDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      
        QString srcDirPath = sourceDir.filePath(dirName);
        QString destDirPath = destDir.filePath(dirName); // destDir does not need to exist, check at start of this function
	  
        if (!recursiveCopy(srcDirPath, destDirPath)) {
            return false;
        }
    }

    return true;
  }
  
  bool
  isSafeToRemoveRecursivily(const QString &directoryPath) {
  
    // Get information about the directory
    QFileInfo dirInfo(directoryPath);
    
    // Check if the directory exists
    if (!dirInfo.exists() || !dirInfo.isDir()) {
        qWarning() << "The directory does not exist or is not a directory.";
        return false;
    }

    // make sure not one of users main dir
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString documentsDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString desktopDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString downloadDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QString appDir = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);    
    if (directoryPath == homeDir ||
	directoryPath == documentsDir ||
	directoryPath == desktopDir ||
	directoryPath == downloadDir ||
	directoryPath == appDir) {
        qWarning() << "The directory is one of users main directories and is not removable.";
        return false;
    }
    
#ifndef Q_OS_WIN
    
    // ensure user is owner of dir
    QString owner = dirInfo.owner();
    QString currentUser = QDir::home().dirName();  // This gives the user's home directory name
    QString userName = qgetenv("USER"); // On UNIX-like systems
    if (userName.isEmpty())
        userName = qgetenv("USERNAME"); // On Windows

    return owner == currentUser || owner == userName;

#else
    
    return true;
    
#endif
    
  }
  
}
  

  

