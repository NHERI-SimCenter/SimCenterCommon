/*
 * Qt UI and Layout Demo  (NHERI-SimCenter)
 * ==================================================================
 *
 * This application demonstrates the creation of UI Widgets using
 * both direct coding (.h, .cpp) and the Qt Creator (.ui, .h, .cpp).
 *
 * The same widget is implemented both ways.  The CentralWidget displays
 * both implementations side-by-side.  Computations are independent as part
 * of the respective implementation.
 *
 * created: Peter Mackenzie-Helnwein, University of Washington, Seattle, WA
 * date:    September 24, 2020
 */


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFrame>
#include <QHBoxLayout>

#include "bendingstresswidget.h"
#include "bendingstresscodedwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFrame *theFrame = new QFrame(this);
    QHBoxLayout *layout = new QHBoxLayout(theFrame);
    layout->setMargin(0);
    layout->setSpacing(0);   // no additional  space between widgets

    layout->addWidget(new BendingStressWidget(this), 1);
    layout->addWidget(new BendingStressCodedWidget(this), 1);

    this->setCentralWidget(theFrame);
}

MainWindow::~MainWindow()
{
    delete ui;
}

