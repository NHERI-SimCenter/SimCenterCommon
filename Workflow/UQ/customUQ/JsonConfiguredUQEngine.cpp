#include "JsonConfiguredUQEngine.h"
#include "JsonConfiguredUQWidget.h"

#include <QFile>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "CustomUQ_Results.h"

JsonConfiguredUQEngine::JsonConfiguredUQEngine(UQ_EngineType type, QWidget *parent) : UQ_Engine(parent)
{
    // Read the schema json file for the UQ engine
    QFile file("/Users/aakash/Desktop/Research/Mike/UQpy-quoFEM_Integration/code/UQpySchema.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument document = QJsonDocument::fromJson(jsonData);
    QJsonObject object = document.object();

    JsonConfiguredUQWidget *theJsonConfiguredUQWidget = new JsonConfiguredUQWidget(object);
    theRandomVariables = RandomVariablesContainer::getInstance();
}

JsonConfiguredUQEngine::~JsonConfiguredUQEngine()
{

}



bool
JsonConfiguredUQEngine::outputToJSON(QJsonObject &rvObject) {
  return theJsonConfiguredUQWidget->outputToJSON(rvObject);
}

bool
JsonConfiguredUQEngine::inputFromJSON(QJsonObject &rvObject) {
  return theJsonConfiguredUQWidget->inputFromJSON(rvObject);
}

bool
JsonConfiguredUQEngine::outputAppDataToJSON(QJsonObject &jsonObject) {
  //jsonObject["Application"] = theApplicationName->text();
  jsonObject["Application"] = QString("CustomUQ");
  QJsonObject dataObj;
  jsonObject["ApplicationData"] = dataObj;

  return true;
}

bool
JsonConfiguredUQEngine::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    Q_UNUSED(jsonObject);
    return true;
}

QString
JsonConfiguredUQEngine::getProcessingScript() {
    return QString("UNKNOWN.py");
}


QString
JsonConfiguredUQEngine::getMethodName() {
    return QString("UNKNOWN");
}

void
JsonConfiguredUQEngine::setEventType(QString type) {

}

int JsonConfiguredUQEngine::getMaxNumParallelTasks(void) {
    return 1;
}

void JsonConfiguredUQEngine::setRV_Defaults(void) {

}

UQ_Results *
JsonConfiguredUQEngine::getResults(void) {
  return new CustomUQ_Results(theRandomVariables);
}
