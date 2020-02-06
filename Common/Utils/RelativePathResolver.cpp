#include "RelativePathResolver.h"
#include <QJsonArray>
#include <QFileInfo>
#include  <QDebug>


namespace SCUtils {

const QString CurrentDirPrefix = "{Current_Dir}/";

QString ResolveAbsolutePaths(QString string, QDir baseDir)
{
    qDebug() << "string:" << string;
    qDebug() << "BaseDir:" << baseDir.path();

    if(string.startsWith(CurrentDirPrefix))
    {
        auto filepath = baseDir.absoluteFilePath(string.remove(CurrentDirPrefix));
        string = QDir(filepath).canonicalPath();
    }
    qDebug() << "new string:" << string;

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
    qDebug() << baseDir.path();
    for(QJsonValueRef jsonValueRef: jsonObject)
    {
        ResolveAbsolutePaths(jsonValueRef, baseDir);
    }
}


QString ResolveRelativePaths(QString string, QDir baseDir)
{
    qDebug() << "string:" << string;
    qDebug() << "BaseDir:" << baseDir.path();

    QFileInfo fileInfo (string);
    if(fileInfo.exists() && fileInfo.isAbsolute())
    {
        QDir rootDir(baseDir);
        QString relPath = rootDir.relativeFilePath(string);
        qDebug() << "rel Path: " << relPath;

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
    qDebug() << baseDir.path();
    for(QJsonValueRef jsonValueRef: jsonObject)
    {
        ResolveRelativePaths(jsonValueRef, baseDir);
    }
}

}
