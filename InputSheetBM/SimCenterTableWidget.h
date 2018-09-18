#ifndef SIMCENTERTABLEWIDGET_H
#define SIMCENTERTABLEWIDGET_H



#include <SimCenterWidget.h>
#include <SpreadsheetWidget.h>

class QJsonObject;

// Common class for all input widget sheets

class SimCenterTableWidget : public SimCenterWidget
{
    Q_OBJECT
public:
    explicit SimCenterTableWidget(QWidget *parent = 0);
    virtual ~SimCenterTableWidget();

    const SpreadsheetWidget * getSpreadsheetWidget();

signals:

public slots:

protected:
    SpreadsheetWidget* theSpreadsheet;

private:
    QStringList   tableHeader;

};
#endif // SIMCENTERTABLEWIDGET_H
