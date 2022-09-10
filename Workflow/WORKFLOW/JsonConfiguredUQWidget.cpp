#include "JsonConfiguredUQWidget.h"
#include <QDebug>
#include <QJsonObject>
#include <QString>
#include <QLineEdit>
#include <QIntValidator>

JsonConfiguredUQWidget::JsonConfiguredUQWidget(QJsonObject &configurationJsonObject, QWidget *parent) : SimCenterWidget(parent)
{
    QString title = configurationJsonObject["title"].toString();
    QGroupBox *inputs = new QGroupBox(tr(qPrintable(title + " Inputs")), this);
    QFormLayout *inputsFormLayout = new QFormLayout(inputs);
    inputsFormLayout->setAlignment(Qt::AlignLeft);
    inputsFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    inputsFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    QJsonObject properties = configurationJsonObject["properties"].toObject();
    foreach (const QJsonValue & v, properties)
        {
            QString title = v.toObject().value("title").toString();
            QString type = v.toObject().value("type").toString();
            QLineEdit *widget = new QLineEdit;
            if (type == "integer") {
                widget->setValidator(new QIntValidator);
            }
            qDebug() << title << type;
            inputsFormLayout->addRow(tr(qPrintable(title)), widget);
        }
}

JsonConfiguredUQWidget::~JsonConfiguredUQWidget()
{

}


