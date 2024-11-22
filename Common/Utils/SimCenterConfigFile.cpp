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


static bool parsedInputOptions = false;
static QJsonObject *inputOptions = 0;

int parseInputOptions(void) {

  //
  // open ~/appDir config.json file if exists & read options
  //

  QJsonObject jsonObj;

  // for some unknown reason the key values fail if key not found later .. bug somewhere else!
  QJsonObject outputPos;  
  jsonObj["screenSize"]="normal";
  jsonObj["outputLocation"]=outputPos;
  
  QString appPath = QCoreApplication::applicationDirPath();
  QString configFile = appPath + QDir::separator() + QString("config.json");
  QFile jsonFile(configFile);

  qDebug() << "configFile: " << configFile;

  if (jsonFile.exists() && jsonFile.open(QFile::ReadOnly)) {
    QJsonDocument exDoc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonObj = exDoc.object();
  }

  //
  // add command line jsonObj if provided, override ones in config.json if duplicate
  //  parse -key value


  // loop over all args, app name is first of course so ignore
  QStringList argList = QCoreApplication::arguments();
  int size = argList.size();
  for (int i = 1; i < size; i++) {

    QString argi = argList[i];
    
    // if arg starts with a - if inidicates a -key value pair
    if (argi[0] == '-') {
      QString key = argi.mid(1); // strip the -

      if (i+1 < size) { // watch for empty values
	
	QString value = argList[i+1];	
	i++;

	// check if int
	bool ok;
	int intValue = value.toInt(&ok);
	
	if (ok == true)  {
	  
	  jsonObj[key] = intValue;
	 
	} else { // otherwise json or just use the string
	  
	  // check if json
	  QJsonParseError parseError;
	  QJsonDocument jsonDoc = QJsonDocument::fromJson(value.toUtf8(), &parseError);	  
	  qDebug() << "key:" << key << " value:" << value;
	  
	  if (parseError.error == QJsonParseError::NoError && jsonDoc.isObject()) {
	    jsonObj[key] = jsonDoc.object();	  

	  } else {
	    jsonObj[key] = value;
	  }
	  
	} // not int
      } // the i+1 < size
    } // the -
  }
  
  qDebug() << "application input options: " << jsonObj;

  inputOptions = new QJsonObject(jsonObj);
  parsedInputOptions = true;

  return 0; // Return int as stated by function header. Void function if desired. Noted on VSCode 2019 in appveyor. We can quiet this error with pragma warning if needed
  // C:\projects\SimCenter\SimCenterCommon\Common\Utils\SimCenterConfigFile.cpp(94) : error C4716: 'parseInputOptions': must return a value
}


QString getConfigOptionString(QString option) {

  if (parsedInputOptions == false)
    parseInputOptions();
  
  QString res = "";

  /*
  QString appPath = QCoreApplication::applicationDirPath();
  QString configFile = appPath + QDir::separator() + QString("config.json");
  QFile jsonFile(configFile);

  qDebug() << "getConfigOptionString: option=" << option;  
  qDebug() << "configFile: " << configFile;

  
  if (jsonFile.exists() && jsonFile.open(QFile::ReadOnly)) {

    QJsonDocument exDoc = QJsonDocument::fromJson(jsonFile.readAll());
    QJsonObject options = exDoc.object();
    qDebug() << "configFile JSON: " << options;

  */

  qDebug() << "getConfigString: " << option << *inputOptions;
  
  if (inputOptions->contains(option)) {
    QJsonValue value = (*inputOptions)[option];
    if (value.isString())
      return value.toString();
  }

  return res;
}


bool getConfigOptionInteger(QString option, int &returnValue) {
  
  if (parsedInputOptions == false)
    parseInputOptions();

  qDebug() << "getConfigOptionInt: " << option << *inputOptions;
  
  /*
  QString appPath = QCoreApplication::applicationDirPath();
  QString configFile = appPath + QDir::separator() + QString("config.json");
  QFile jsonFile(configFile);

  if (jsonFile.exists() && jsonFile.open(QFile::ReadOnly)) {

    QJsonDocument exDoc = QJsonDocument::fromJson(jsonFile.readAll());
    QJsonObject *inputOptions = exDoc.object();
  */
  
  if (inputOptions->contains(option)) {
    
    QJsonValue value = (*inputOptions)[option];
    returnValue = value.toInt(returnValue);
    return true;
    
  } else
    return false;
}


QJsonObject getConfigOptionJSON(QString option) {

  qDebug() << "getConfigOptionJSON: " << option << *inputOptions;
  
  if (parsedInputOptions == false)
    parseInputOptions();  

  if (inputOptions->contains(option)) {
    QJsonValue value = (*inputOptions)[option];
    if (value.isObject())
      return value.toObject();
  }
  
  /*
  QJsonObject res;
  QString appPath = QCoreApplication::applicationDirPath();
  QString configFile = appPath + QDir::separator() + QString("config.json");
  QFile jsonFile(configFile);

  if (jsonFile.exists() && jsonFile.open(QFile::ReadOnly)) {

    QJsonDocument exDoc = QJsonDocument::fromJson(jsonFile.readAll());
    QJsonObject *inputOptions = exDoc.object();
    if (inputOptions->contains(option)) {

      QJsonValue value = (*inputOptions)[option];
      if (value.isObject())
	return value.toObject();
    }
  }
  return res;  
  */

}

