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


#include "bendingstresscodedwidget.h"

#include <QObject>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QGridLayout>
#include <QSizePolicy>

#include "resultswidget.h"


BendingStressCodedWidget::BendingStressCodedWidget(QWidget *parent) : QFrame(parent)
{
    this->initUI();
}

void
BendingStressCodedWidget::initUI()
{
    // set background color to green for easy identification

    this->setStyleSheet(tr("QFrame {background-color: #eeffee}"));


    //
    // create the objects
    //

    MOE_label = new QLabel(tr("MOE"));
    MOE_LE    = new QLineEdit();
    MOE_units = new QLabel(tr("MPa"));

    moi_label      = new QLabel(tr("I"));
    mom_of_inertia = new QLineEdit();
    moi_units      = new QLabel(tr("cm<sup>4</sup>"));

    Area_label = new QLabel(tr("A"));
    Area_LE    = new QLineEdit();
    Area_units = new QLabel(tr("cm<sup>2</sup>"));

    c_top_label = new QLabel(tr("c<sub>top</sub>"));
    c_top_SBx   = new QDoubleSpinBox();
    c_top_units = new QLabel(tr("cm"));

    c_bottom_label = new QLabel(tr("c<sub>bottom</sub>"));
    c_bottom_SBx   = new QDoubleSpinBox();
    c_bottom_units = new QLabel(tr("cm"));


    M_label = new QLabel(tr("moment"));
    moment  = new QDoubleSpinBox();
    moment->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    M_units = new QLabel(tr("kN.m"));

    P_label = new QLabel(tr("force"));
    force   = new QDoubleSpinBox();
    force->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    P_units = new QLabel(tr("kN"));

    compute = new QPushButton(tr("compute"));
    compute->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);


    results = new ResultsWidget();

    //
    // organize objects
    //

    QGridLayout *layout = new QGridLayout(this);  // defining the parent saves the "setLayout(..)" in the end

    int row=0;

    layout->addWidget(MOE_label, row,0);
    layout->addWidget(MOE_LE, row,1);
    layout->addWidget(MOE_units, row,2);

    row++;  // start a new row

    layout->addWidget(Area_label, row,0);
    layout->addWidget(Area_LE, row,1);
    layout->addWidget(Area_units, row,2);

    row++;  // start a new row

    layout->addWidget(moi_label, row,0);
    layout->addWidget(mom_of_inertia, row,1);
    layout->addWidget(moi_units, row,2);

    row++;  // start a new row

    layout->addWidget(c_top_label, row,0);
    layout->addWidget(c_top_SBx, row,1);
    layout->addWidget(c_top_units, row,2);

    row++;  // start a new row

    layout->addWidget(c_bottom_label, row,0);
    layout->addWidget(c_bottom_SBx, row,1);
    layout->addWidget(c_bottom_units, row,2);

    row++;  // start a new row

    // organize input for moment and force
    QGridLayout *loadsLayout = new QGridLayout();  // don't make this the parent - one layout only!

    // grouping is for readability only (not needed for C++)
    {
        loadsLayout->addWidget(M_label, 0,0);
        loadsLayout->addWidget(moment, 1,0);
        loadsLayout->addWidget(M_units, 1,1);

        loadsLayout->addWidget(P_label, 0,2);
        loadsLayout->addWidget(force, 1,2);
        loadsLayout->addWidget(P_units, 1,3);

        loadsLayout->addWidget(compute, 0,4,2,1);   // the last two numbers are: 2 rows high and 1 cols wide
    }

    layout->addLayout(loadsLayout, row,0, 1,3); // the last two numbers are: 1 row high and 3 cols wide

    row++;  // start a new row

    layout->addWidget(results, row, 0, 1,3);    // the last two numbers are: 1 row high and 3 cols wide



    //
    // connect signals and slots
    //

    QObject::connect(compute, SIGNAL(clicked()),
                     this, SLOT(on_compute_btn_clicked()));
}

void BendingStressCodedWidget::on_compute_btn_clicked()
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

    MOE      = MOE_LE->text().toDouble();                        // comes in MPa
    A        = Area_LE->text().toDouble() * 100.;                // cm^2 -> mm^2
    I        = (mom_of_inertia->text()).toDouble() * 10000.;  // cm^4 -> mm^4
    c_top    = c_top_SBx->value() * 10.;                          // cm -> mm
    c_bottom = c_bottom_SBx->value() * 10.;                       // cm -> mm

    P        = force->text().toDouble() * 1000.;               // kN -> N
    M        = moment->text().toDouble() * 1000. * 1000.;      // kN.m -> N.mm

    // compute

    sigma_neutral_axis = P/A;
    sigma_top          = sigma_neutral_axis - M * c_top/I;
    sigma_bottom       = sigma_neutral_axis + M * c_bottom/I;

    // display results (requires an access function)

    results->setStress(sigma_bottom, sigma_neutral_axis, sigma_top);

}
