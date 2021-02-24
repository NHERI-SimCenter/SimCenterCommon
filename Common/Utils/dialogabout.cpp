#include "dialogabout.h"
#include "ui_dialogabout.h"
#include <QApplication>
#include <QFile>
#include <QDebug>

DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);

}

DialogAbout::~DialogAbout()
{
    delete ui;
}

void DialogAbout::on_btn_backToTop_clicked()
{
    ui->textBrowser->setSource(QUrl("#top"));
}

void DialogAbout::setTextSource(const QString source)
{
    QFile file(source);
    if (file.open(QFile::ReadOnly))
    {
        ui->textBrowser->setHtml(file.readAll());
    } else {
      qDebug() << "DialogAbout no file: " << source << "exists";
    }

}

void DialogAbout::setTitle(const QString theTitle)
{
    ui->label_about->setText(theTitle);
}
