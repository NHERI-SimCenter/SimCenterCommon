#ifndef RELATIVEPATHRESOLVER_H
#define RELATIVEPATHRESOLVER_H

#include <QJsonObject>
#include <QDir>

namespace SCUtils {

QString ResolveAbsolutePaths(QString string, QDir baseDir);

void ResolveAbsolutePaths(QJsonObject& jsonObject, QDir baseDir);

void ResolveAbsolutePaths(QJsonValueRef jsonValueRef, QDir baseDir);

QString ResolveRelativePaths(QString string, QDir baseDir);

void ResolveRelativePaths(QJsonObject& jsonObject, QDir baseDir);

void ResolveRelativePaths(QJsonValueRef jsonValueRef, QDir baseDir);


}


#endif // RELATIVEPATHRESOLVER_H
