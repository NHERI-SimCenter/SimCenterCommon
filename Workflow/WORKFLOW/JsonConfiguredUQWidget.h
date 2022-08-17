#ifndef JSONCONFIGUREDUQWIDGET_H
#define JSONCONFIGUREDUQWIDGET_H

#include <SimCenterWidget.h>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFormLayout>

class JsonConfiguredUQWidget : public SimCenterWidget
{
public:
    explicit JsonConfiguredUQWidget(QJsonObject &configurationJsonObject, QWidget *parent = 0);
    ~JsonConfiguredUQWidget();
    QVBoxLayout* layout;
    QGroupBox* inputs;
    QFormLayout* inputsFormLayout;
};

#endif // JSONCONFIGUREDUQWIDGET_H
