#include "helpwidget.h"
#include "ui_helpwidget.h"

HelpWidget::HelpWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::HelpWidget)
{
    ui->setupUi(this);
    m_resourceName = "";
}

HelpWidget::~HelpWidget()
{
    delete ui;
}

void HelpWidget::setHelpResource(const QString &resFile)
{
    // try to load resource

    // if successful, check for help index file

    // set help index

    // remember name of resource file for later reference/debugging
    m_resourceName = resFile;
}

void HelpWidget::lookup(const QString &lookfor)
{
    // search for information on lookfor
    //
    // if nothing found, show index page

}


/*
 *     SLOTS
 */

void HelpWidget::on_btn_goToIndex_clicked()
{

}

void HelpWidget::on_btn_goBack_clicked()
{

}

void HelpWidget::on_btn_goForward_clicked()
{

}

void HelpWidget::on_btn_OK_clicked()
{

}

void HelpWidget::on_searchTextBox_textEdited(const QString &arg1)
{

}
