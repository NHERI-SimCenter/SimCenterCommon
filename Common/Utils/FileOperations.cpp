#include "FileOperations.h"
#include <QJsonArray>
#include <QFileInfo>
#include <QDir>


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

}
