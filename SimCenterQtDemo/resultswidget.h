#ifndef RESULTSWIDGET_H
#define RESULTSWIDGET_H

#include <QGroupBox>

namespace Ui {
class ResultsWidget;
}

class ResultsWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit ResultsWidget(QWidget *parent = nullptr);
    ~ResultsWidget();

    void setStress(double sigma_bottom, double sigma_neutral_axis, double sigma_top);

private:
    Ui::ResultsWidget *ui;
};

#endif // RESULTSWIDGET_H
