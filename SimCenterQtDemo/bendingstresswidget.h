/*
 * Qt UI and Layout Demo  (NHERI-SimCenter)
 * ==================================================================
 *
 * Version: created using Qt Creator (.ui, .h, .cpp)
 * -------------------------------------------------------------
 *
 * UI Object Definition Table can be found at
 *
 *    URL:  https://docs.google.com/spreadsheets/d/1lgKdMNwKY8mPqwPaz08-37ELX7Qx1nCUvbRzc684JpI/edit?usp=sharing
 *
 * created: Peter Mackenzie-Helnwein, University of Washington, Seattle, WA
 * date:    September 24, 2020
 */


#ifndef BENDINGSTRESSWIDGET_H
#define BENDINGSTRESSWIDGET_H

#include <QFrame>

namespace Ui {
class BendingStressWidget;
}

class BendingStressWidget : public QFrame
{
    Q_OBJECT

public:
    explicit BendingStressWidget(QWidget *parent = nullptr);
    ~BendingStressWidget();

private slots:
    void on_compute_btn_clicked();

private:
    Ui::BendingStressWidget *ui;

    double MOE;
    double A;
    double I;
    double c_top;
    double c_bottom;
    double M;
    double P;

    double sigma_top;
    double sigma_bottom;
    double sigma_neutral_axis;
};

#endif // BENDINGSTRESSWIDGET_H
