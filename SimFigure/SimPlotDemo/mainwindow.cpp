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
    items.append(new QStandardItem("Demo 3"));
    items.append(new QStandardItem("Demo 4"));
    items.append(new QStandardItem("Clear"));
    model->appendColumn(items);
    ui->selectionView->setModel(model);
    ui->selectionView->header()->hide();
    ui->selectionView->setColumnWidth(0,30);

    connect(ui->theFigure, SIGNAL(curve_selected(int)), this, SLOT(on_selection_changed(int)));
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
            y.append( 2.+sin(0.1*s) );
            z.append( 2.+cos(0.1*s) );
        }
        int idx;
        idx = ui->theFigure->plot(x,y);
        idx = ui->theFigure->plot(x,z, SimFigure::LineType::Solid, QColor(Qt::blue));

        ui->theFigure->setTitle("Demo #1: simple functions");
        ui->theFigure->setXLabel("x -->");
        ui->theFigure->setYLabel("f(x) -->");
        ui->theFigure->showLegend();
        ui->theFigure->setAxisType(SimFigure::AxisType::LogX);

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
        idx = ui->theFigure->plot(y,z, SimFigure::LineType::DashDotted, Qt::green);
        idx = ui->theFigure->plot(d,u, SimFigure::LineType::Solid, Qt::blue, SimFigure::Marker::Triangle);

        ui->theFigure->setTitle("Demo #2: parametric plot");
        ui->theFigure->setXLabel("x(s) -->");
        ui->theFigure->setYLabel("y(s) -->");
        ui->theFigure->showLegend();
        ui->theFigure->setAxisType(SimFigure::AxisType::Default);

        break; }
    case 2: {
        for (double s=0.;s<=2.*PI;s+=PI/20)
        {
            x.append( s );
            y.append( 200. + 150*sin(s) );
            z.append( 200. + 150*cos(s) );
            d.append( 200. - 125. + 125*s/PI );
            u.append( 200. + 125 * sin(s));
        }
        int idx;
        idx = ui->theFigure->plot(y,z, SimFigure::LineType::DashDotted, Qt::darkGreen);
        idx = ui->theFigure->scatter(d,u, Qt::red, SimFigure::Marker::Circle);

        ui->theFigure->setTitle("Demo #3: scatter plot");
        ui->theFigure->setXLabel("x -->");
        ui->theFigure->setYLabel("y = f(x) -->");
        ui->theFigure->showLegend();
        ui->theFigure->setAxisType(SimFigure::AxisType::Default);

        break; }
    case 3: {
        for (double s=0.;s<=2.*PI;s+=PI/20)
        {
            x.append( s );
            y.append( 200. + 150*sin(s) );
            z.append( 200. + 150*cos(s) );
            d.append( 200. - 125. + 125*s/PI );
            u.append( 200. + 125 * sin(s));
        }
        int idx;
        idx = ui->theFigure->plot(y,z, SimFigure::LineType::DashDotted, Qt::green);
        idx = ui->theFigure->plot(d,u, SimFigure::LineType::Solid, Qt::blue, SimFigure::Marker::Triangle);

        ui->theFigure->setTitle("Demo #4: ");
        ui->theFigure->setTitleFontSize(24);
        ui->theFigure->setXLabel("index i");
        ui->theFigure->setYLabel("element a_i");
        ui->theFigure->setLabelFontSize(20);
        ui->theFigure->showLegend();
        ui->theFigure->setAxisType(SimFigure::AxisType::Default);

        break; }
    default:
        x.clear();
        y.clear();
        z.clear();
        d.clear();
        u.clear();
        ui->theFigure->showLegend(false);
        break;
    }

    ui->showLegend->setCheckState(Qt::CheckState::Checked);
}

void MainWindow::on_selection_changed(int ID)
{
    if (ID >= 0)
    {
        ui->btn_option3->setText(tr("ID = %1\n\nclick to\nclear\nselection").arg(ID));
        ui->btn_option3->setStyleSheet("color: #cc4444");

        switch (ID)
        {
        case 1:
            ui->btn_1st->setChecked(true);
            break;
        case 2:
            ui->btn_2nd->setChecked(true);
            break;
        default:
            ui->btn_none->setChecked(true);
        }
    }
    else {
        ui->btn_option3->setText(tr("none\nselected"));
        ui->btn_option3->setStyleSheet("color: #cc4444");

        ui->btn_none->setChecked(true);
    }
}

void MainWindow::on_btn_option1_clicked()
{
    if (ui->theFigure->legendVisible()) {
        ui->showLegend->setCheckState(Qt::CheckState::Unchecked);
        //ui->theFigure->showLegend(false);
    } else {
        ui->showLegend->setCheckState(Qt::CheckState::Checked);
        //ui->theFigure->showLegend(true);
    }
}

void MainWindow::on_btn_option2_clicked()
{
    QVector<SimFigure::Location> locList({
                                             SimFigure::Location::BottomRight,
                                             SimFigure::Location::Bottom,
                                             SimFigure::Location::BottomLeft,
                                             SimFigure::Location::Left,
                                             SimFigure::Location::TopLeft,
                                             SimFigure::Location::Top,
                                             SimFigure::Location::TopRight,
                                             SimFigure::Location::Right
                                         });

    currentLocation++;
    if (currentLocation >= locList.length()) currentLocation -= locList.length();
    ui->theFigure->moveLegend(locList[currentLocation]);
}

void MainWindow::on_btn_option3_clicked()
{
    ui->theFigure->clearSelection();
    ui->btn_none->setChecked(true);
}

void MainWindow::on_btn_1st_clicked()
{
    ui->theFigure->select(1);
}

void MainWindow::on_btn_2nd_clicked()
{
    ui->theFigure->select(2);
}

void MainWindow::on_btn_none_clicked()
{
    ui->theFigure->clearSelection();
}

void MainWindow::on_action_Save_triggered()
{
    ui->theFigure->saveToFile("SimPlotDemo1.png", SimFigure::FileType::PNG);
}

void MainWindow::on_actionSave_As_triggered()
{
    ui->theFigure->exportToFile("SimPlotDemo2", SimFigure::FileType::PNG);
}

void MainWindow::on_actionSave_Hi_res_triggered()
{
    ui->theFigure->saveToFile("SimPlotDemo3", SimFigure::FileType::PNG, QSizeF(300,200), 200);
}

void MainWindow::on_actionSave_PDF_triggered()
{
    ui->theFigure->saveToFile("SimPlotDemo4.pdf", SimFigure::FileType::PNG, QSizeF(300,200), 150);
}

void MainWindow::on_actionShow_axis_controls_triggered()
{
    ui->theFigure->showAxisControls(true);
}

void MainWindow::on_actionHide_axis_controls_triggered()
{
    ui->theFigure->showAxisControls(false);
}

void MainWindow::on_btn_option4_clicked()
{
    ui->theFigure->fit_data();
}

void MainWindow::on_showLegend_stateChanged(int arg1)
{
    ui->theFigure->showLegend((ui->showLegend->checkState()==Qt::CheckState::Checked));
}

void MainWindow::on_zoomOut_clicked()
{
    double xmax = ui->theFigure->maxX();
    double xmin = ui->theFigure->minX();
    double ymax = ui->theFigure->maxY();
    double ymin = ui->theFigure->minY();

    double w=xmax-xmin;
    double h=ymax-ymin;

    ui->theFigure->setXlimits(xmin-w/10.,xmax+w/10.);
    ui->theFigure->setYlimits(ymin-h/10.,ymax+h/10.);
}

void MainWindow::on_zoomIn_clicked()
{
    double xmax = ui->theFigure->maxX();
    double xmin = ui->theFigure->minX();
    double ymax = ui->theFigure->maxY();
    double ymin = ui->theFigure->minY();

    double w=xmax-xmin;
    double h=ymax-ymin;

    ui->theFigure->setXlimits(xmin+w/10.,xmax-w/10.);
    ui->theFigure->setYlimits(ymin+h/10.,ymax-h/10.);
}
