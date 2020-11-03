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


#include "bendingstresswidget.h"
#include "ui_bendingstresswidget.h"

BendingStressWidget::BendingStressWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::BendingStressWidget)
{
    ui->setupUi(this);
}

BendingStressWidget::~BendingStressWidget()
{
    delete ui;
}

void BendingStressWidget::on_compute_btn_clicked()
{
    /*
     * length is given in cm
     * forces are given in kN
     * moments are given in kN.m
     * stresses are requested in MPa
     *
     * consistent units:
     *   force N
     *   length mm
     *   stress N/mm^2 == MPa
     */

    // input

    MOE      = ui->MOE->text().toDouble();                        // comes in MPa
    A        = ui->area->text().toDouble() * 100.;                // cm^2 -> mm^2
    I        = (ui->mom_of_inertia->text()).toDouble() * 10000.;  // cm^4 -> mm^4
    c_top    = ui->c_top->value() * 10.;                          // cm -> mm
    c_bottom = ui->c_bottom->value() * 10.;                       // cm -> mm

    P        = ui->force->text().toDouble() * 1000.;              // kN -> N
    M        = ui->moment->text().toDouble() * 1000. * 1000.;     // kN.m -> N.mm

    // compute

    sigma_neutral_axis = P/A;
    sigma_top          = sigma_neutral_axis - M * c_top/I;
    sigma_bottom       = sigma_neutral_axis + M * c_bottom/I;

    // display results (requires an access function)

    ui->results->setStress(sigma_bottom, sigma_neutral_axis, sigma_top);

}
