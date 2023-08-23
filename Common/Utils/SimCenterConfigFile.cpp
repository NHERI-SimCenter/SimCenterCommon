#include "SimCenterConfigFile.h"
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

// functions to return a config option if set
// if file "config.json" exists function opens it, looks for option and returns value
// if no option, or no file empty string is returned

#include <QCoreApplication>

QString getConfigOptionString(QString option) {
  QString res = "";
  
  QString appPath = QCoreApplication::applicationDirPath();
  QString configFile = appPath + QDir::separator() + QString("config.json");
  QFile jsonFile(configFile);

  if (jsonFile.exists() && jsonFile.open(QFile::ReadOnly)) {

    QJsonDocument exDoc = QJsonDocument::fromJson(jsonFile.readAll());
    QJsonObject jsonObject = exDoc.object();
    if (jsonObject.contains(option)) {

      QJsonValue value = jsonObject[option];
      if (value.isString())
	return value.toString();
    }
  } 

  return res;
}


QJsonObject getConfigOptionJSON(QString option) {
  QJsonObject res;
  
  QString appPath = QCoreApplication::applicationDirPath();
  QString configFile = appPath + QDir::separator() + QString("config.json");
  QFile jsonFile(configFile);

  if (jsonFile.exists() && jsonFile.open(QFile::ReadOnly)) {

    QJsonDocument exDoc = QJsonDocument::fromJson(jsonFile.readAll());
    QJsonObject jsonObject = exDoc.object();
    if (jsonObject.contains(option)) {

      QJsonValue value = jsonObject[option];
      if (value.isObject())
	return value.toObject();
    }
  } 

  return res;
}

