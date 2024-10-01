#include "FileOperations.h"
#include <QJsonArray>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QFileInfo>


namespace SCUtils {

bool recursiveCopy(const QString &sourcePath, const QString &destPath)
{
    QFileInfo srcFileInfo(sourcePath);

    if (srcFileInfo.isDir())
    {
        QDir targetDir(destPath);

        if (!targetDir.mkpath(destPath))
            return false;

        QDir sourceDir(sourcePath);

        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);

        foreach (const QString &fileName, fileNames)
        {
            const QString newSrcFilePath = sourcePath + QDir::separator() + fileName;
            const QString newDestFilePath = destPath  + QDir::separator() + fileName;

            if (!recursiveCopy(newSrcFilePath, newDestFilePath))
                return false;
        }
    } else {
        if (!QFile::copy(sourcePath, destPath))
            return false;
    }

    return true;
}

bool isSafeToRemoveRecursivily(const QString &directoryPath) {
  
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
    
    // ensure user is owner of dir
    QString owner = dirInfo.owner();
    QString currentUser = QDir::home().dirName();  // This gives the user's home directory name
    QString userName = qgetenv("USER"); // On UNIX-like systems
    if (userName.isEmpty())
        userName = qgetenv("USERNAME"); // On Windows

    return owner == currentUser || owner == userName;
}


  
}
