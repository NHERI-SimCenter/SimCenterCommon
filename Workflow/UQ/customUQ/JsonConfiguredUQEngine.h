#ifndef JSONCONFIGUREDUQENGINE_H
#define JSONCONFIGUREDUQENGINE_H

#include <QWidget>
#include "UQ_Engine.h"
#include <JsonConfiguredUQWidget.h>

#include <UQ_Results.h>

class UQ_Results;
class RandomVariablesContainer;

class JsonConfiguredUQEngine : public UQ_Engine
{
    Q_OBJECT
public:
    explicit JsonConfiguredUQEngine(UQ_EngineType type, QWidget *parent = nullptr);
    virtual ~JsonConfiguredUQEngine();

    bool outputToJSON(QJsonObject &jsonObject);
    bool inputFromJSON(QJsonObject &jsonObject);
    bool outputAppDataToJSON(QJsonObject &jsonObject);
    bool inputAppDataFromJSON(QJsonObject &jsonObject);
    QString getProcessingScript();
    QString getMethodName();
    void setEventType(QString type);
    int getMaxNumParallelTasks(void);
    void setRV_Defaults(void);

    UQ_Results *getResults(void);


private:
    JsonConfiguredUQWidget * theJsonConfiguredUQWidget;
    RandomVariablesContainer * theRandomVariables;

signals:
    void onUQ_EngineChanged();
    void onNumModelsChanged(int numModels);

};

#endif // JSONCONFIGUREDUQENGINE_H
