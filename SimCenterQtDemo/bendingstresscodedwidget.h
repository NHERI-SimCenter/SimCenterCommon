/*
 * Qt UI and Layout Demo  (NHERI-SimCenter)
 * ==================================================================
 *
 * Version: created by user coding only (.h, .cpp)
 * -------------------------------------------------------------
 *
 * UI Object Definition Table can be found at
 *
 *    URL:  https://docs.google.com/spreadsheets/d/1lgKdMNwKY8mPqwPaz08-37ELX7Qx1nCUvbRzc684JpI/edit?usp=sharing
 *
 * created: Peter Mackenzie-Helnwein, University of Washington, Seattle, WA
 * date:    September 24, 2020
 */


#ifndef BENDINGSTRESSCODEDWIDGET_H
#define BENDINGSTRESSCODEDWIDGET_H

#include <QObject>
#include <QFrame>

class QLabel;
class QLineEdit;
class QDoubleSpinBox;
class QPushButton;
class QGroupBox;
class ResultsWidget;

class BendingStressCodedWidget : public QFrame
{
    Q_OBJECT

public:
    explicit BendingStressCodedWidget(QWidget *parent = nullptr);

signals:

private slots:
    void on_compute_btn_clicked();

private:
    void initUI();

    // UI objects

    QLabel    *MOE_label;
    QLineEdit *MOE_LE;
    QLabel    *MOE_units;

    QLabel    *moi_label;
    QLineEdit *mom_of_inertia;
    QLabel    *moi_units;

    QLabel    *Area_label;
    QLineEdit *Area_LE;
    QLabel    *Area_units;

    QLabel         *c_top_label;
    QDoubleSpinBox *c_top_SBx;
    QLabel         *c_top_units;

    QLabel         *c_bottom_label;
    QDoubleSpinBox *c_bottom_SBx;
    QLabel         *c_bottom_units;

    QLabel         *M_label;
    QDoubleSpinBox *moment;
    QLabel         *M_units;

    QLabel         *P_label;
    QDoubleSpinBox *force;
    QLabel         *P_units;

    QPushButton    *compute;

    ResultsWidget  *results;

    // computational variables

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

#endif // BENDINGSTRESSCODEDWIDGET_H
