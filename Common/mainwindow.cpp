#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->header->setHeadingText(tr("Hello World!"));
}

MainWindow::~MainWindow()
{
    delete ui;
}
