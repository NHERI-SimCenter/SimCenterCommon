#include "resultswidget.h"
#include "ui_resultswidget.h"

ResultsWidget::ResultsWidget(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ResultsWidget)
{
    ui->setupUi(this);
}

ResultsWidget::~ResultsWidget()
{
    delete ui;
}

void ResultsWidget::setStress(double sigma_bottom, double sigma_neutral_axis, double sigma_top)
{
    ui->bottom_stress->setText(tr("%1").arg(sigma_bottom,12,'f',2));
    ui->NA_stress->setText(tr("%1").arg(sigma_neutral_axis,12,'f',2));
    ui->top_stress->setText(tr("%1").arg(sigma_top,12,'f',2));
}
