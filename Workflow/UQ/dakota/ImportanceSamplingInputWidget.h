#ifndef IMPORTANCESAMPLINGINPUTWIDGET_H
#define IMPORTANCESAMPLINGINPUTWIDGET_H

#include <UQ_Method.h>
#include <QLineEdit>
#include <QComboBox>

class ImportanceSamplingInputWidget : public UQ_Method
{
    Q_OBJECT
public:
    explicit ImportanceSamplingInputWidget(QWidget *parent = nullptr);

signals:

public slots:

    // SimCenterWidget interface
public:
    bool outputToJSON(QJsonObject &jsonObject);
    bool inputFromJSON(QJsonObject &jsonObject);

public:
    int getNumberTasks();

private:
    QLineEdit *randomSeed;
    QLineEdit *numSamples;
    QComboBox* isMethod;
    QLineEdit *reliabilityLevel;
    QLineEdit *probabilityLevel;
    QComboBox *levelType;
};

#endif // IMPORTANCESAMPLINGINPUTWIDGET_H
