#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QList>
#include <simfigure.h>

#include <QDebug>
#include <math.h>

#define PI 3.141592653590

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QStandardItemModel *model = new QStandardItemModel();
    QList<QStandardItem *> items;
    items.append(new QStandardItem("Demo 1"));
    items.append(new QStandardItem("Demo 2"));
    //items.append(new QStandardItem("Demo 3"));
    items.append(new QStandardItem("Clear"));
    model->appendColumn(items);
    ui->selectionView->setModel(model);
    ui->selectionView->header()->hide();
    ui->selectionView->setColumnWidth(0,30);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_selectionView_clicked(const QModelIndex &index)
{
    QVector<double> x;
    QVector<double> y;
    QVector<double> z;
    QVector<double> d;
    QVector<double> u;

    ui->theFigure->clear();

    int cnt = index.row();
    switch (cnt)
    {
    case 0: {
        for (double s=5.;s<=128.9;s+=0.1)
        {
            x.append(s);
            y.append( 2.+sin(s) );
            z.append( 2.+cos(s) );
        }
        int idx;
        idx = ui->theFigure->plot(x,y);
        idx = ui->theFigure->plot(x,z, LineType::Solid, QColor(Qt::blue));

        ui->theFigure->showLegend();

        break; }
    case 1: {
        for (double s=0.;s<=2.*PI;s+=PI/20)
        {
            x.append( s );
            y.append( 200. + 150*sin(s) );
            z.append( 200. + 150*cos(s) );
            d.append( 200. + 175*0.5*s/PI * cos(s));
            u.append( 200. + 175*0.5*s/PI * sin(s));
        }
        int idx;
        idx = ui->theFigure->plot(y,z, LineType::DashDotted, Qt::green);
        idx = ui->theFigure->plot(d,u, LineType::Solid, Qt::blue, Marker::Triangle);

        ui->theFigure->showLegend();

        break; }
    case 2:
    case 3:
    default:
        x.clear();
        y.clear();
        z.clear();
        d.clear();
        u.clear();
        ui->theFigure->showLegend(false);
        break;
    }

}

void MainWindow::on_btn_option1_clicked()
{
    ui->theFigure->showLegend(!ui->theFigure->legendVisible());
}

void MainWindow::on_btn_option2_clicked()
{
    QVector<Location> locList({Location::Top,
                               Location::TopRight,
                               Location::Right,
                               Location::BottomRight,
                               Location::Bottom,
                               Location::BottomLeft,
                               Location::Left,
                               Location::TopLeft
                              });

    currentLocation++;
    if (currentLocation >= locList.length()) currentLocation -= locList.length();
    ui->theFigure->moveLegend(locList[currentLocation]);
}

void MainWindow::on_btn_option3_clicked()
{

}
