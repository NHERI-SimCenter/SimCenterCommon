#include "RelativePathResolver.h"
#include <QJsonArray>
#include <QFileInfo>
#include <QRegularExpression>


namespace SCUtils {

const QString CurrentDirPrefix = "{Current_Dir}/";

QString ResolveAbsolutePaths(QString string, QDir baseDir)
{
  // remove any spaces from begin or end of string
  string = string.trimmed();
  
    if(string.startsWith(CurrentDirPrefix))
    {
        auto filepath = baseDir.absoluteFilePath(string.remove(CurrentDirPrefix));
        string = QDir(filepath).canonicalPath();
    }

    return string;
}

void ResolveAbsolutePaths(QJsonValueRef jsonValueRef, QDir baseDir)
{
    if(jsonValueRef.isString())
    {
        jsonValueRef = ResolveAbsolutePaths(jsonValueRef.toString(), baseDir);
    }
    else if(jsonValueRef.isObject())
    {
        auto jsonObjectCopy = jsonValueRef.toObject();
        for(QJsonValueRef jsonValueRef: jsonObjectCopy)
        {
            ResolveAbsolutePaths(jsonValueRef, baseDir);
        }
        jsonValueRef = jsonObjectCopy;
    }
    else if(jsonValueRef.isArray())
    {
        auto jsonArrayCopy = jsonValueRef.toArray();
        for(QJsonValueRef jsonValueRef: jsonArrayCopy)
        {
            ResolveAbsolutePaths(jsonValueRef, baseDir);
        }
        jsonValueRef = jsonArrayCopy;
    }

}

void ResolveAbsolutePaths(QJsonObject& jsonObject, QDir baseDir)
{
    for(QJsonValueRef jsonValueRef: jsonObject)
    {
        ResolveAbsolutePaths(jsonValueRef, baseDir);
    }
}

QString ResolveRelativePaths(QString string, QDir baseDir)
{
    // remove any spaces from begin or end of string
    string = string.trimmed();
    
    QFileInfo fileInfo (string);
    if(fileInfo.exists() && fileInfo.isAbsolute())
    {
        QDir rootDir(baseDir);
        QString relPath = rootDir.relativeFilePath(string);

        if(!relPath.startsWith(".."))
            string = relPath.prepend(CurrentDirPrefix);
    }
    return string;
}

void ResolveRelativePaths(QJsonValueRef jsonValueRef, QDir baseDir)
{
    if(jsonValueRef.isString())
    {
        jsonValueRef = ResolveRelativePaths(jsonValueRef.toString(), baseDir);
    }
    else if(jsonValueRef.isObject())
    {
        auto jsonObjectCopy = jsonValueRef.toObject();
        for(QJsonValueRef jsonValueRef: jsonObjectCopy)
        {
            ResolveRelativePaths(jsonValueRef, baseDir);
        }
        jsonValueRef = jsonObjectCopy;
    }
    else if(jsonValueRef.isArray())
    {
        auto jsonArrayCopy = jsonValueRef.toArray();
        for(QJsonValueRef jsonValueRef: jsonArrayCopy)
        {
            ResolveRelativePaths(jsonValueRef, baseDir);
        }
        jsonValueRef = jsonArrayCopy;
    }

}

void ResolveRelativePaths(QJsonObject& jsonObject, QDir baseDir)
{
    for(QJsonValueRef jsonValueRef: jsonObject)
    {
        ResolveRelativePaths(jsonValueRef, baseDir);
    }
}

QString FindPath(QString string, QDir baseDir)
{
    // Returns path with directory separators normalized (that is, platform-native separators converted to "/") and redundant ones removed, and "."s and ".."s resolved (as far as possible).
    QString cleanPath = QDir::cleanPath(string);

    QDir dir(cleanPath);
    QFile file(cleanPath);

    // Quick return if the path or file already exists
    if(dir.exists() || file.exists())
        return cleanPath;

    QRegularExpression rx("[^\\/]+(?=\\/)*");

    QRegularExpressionMatchIterator i = rx.globalMatch(cleanPath);

    QStringList folders;
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString folder = match.captured(0);
        folders << folder;
    }

    // Return the string if no folders are found
    if(folders.empty())
        return string;

    auto baseDirPath = baseDir.canonicalPath();

    // Find the relative path or the file if it exists on the basedir
    QString relativePath;
    for(auto&& it : folders)
    {
        auto pathToTry = baseDirPath + relativePath + "/" + it;

        dir.setPath(pathToTry);
        file.setFileName(pathToTry);

        if(dir.exists())
            relativePath += "/" + it;
        else if(file.exists())
            return pathToTry; // Return the absolute path
        //  return  relativePath + "/" + it; // Return a relative path
    }

    if(!relativePath.isEmpty())
        return baseDirPath + relativePath;

    // If no relative paths are found return the string
    return string;
}

void FindPath(QJsonValueRef jsonValueRef, QDir baseDir)
{
    if(jsonValueRef.isString())
    {
        jsonValueRef = FindPath(jsonValueRef.toString(), baseDir);
    }
    else if(jsonValueRef.isObject())
    {
        auto jsonObjectCopy = jsonValueRef.toObject();
        for(QJsonValueRef jsonValueRef: jsonObjectCopy)
        {
            FindPath(jsonValueRef, baseDir);
        }
        jsonValueRef = jsonObjectCopy;
    }
    else if(jsonValueRef.isArray())
    {
        auto jsonArrayCopy = jsonValueRef.toArray();
        for(QJsonValueRef jsonValueRef: jsonArrayCopy)
        {
            FindPath(jsonValueRef, baseDir);
        }
        jsonValueRef = jsonArrayCopy;
    }
}

void PathFinder(QJsonObject& jsonObject, QDir baseDir)
{
    for(QJsonValueRef jsonValueRef: jsonObject)
    {
        FindPath(jsonValueRef, baseDir);
    }
}

}
