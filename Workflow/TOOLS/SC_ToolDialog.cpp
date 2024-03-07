#include "SC_ToolDialog.h"
#include "Utils/ProgramOutputDialog.h"
#include "WorkflowAppWidget.h"
#include "SimCenterAppWidget.h"

#include <QDebug>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGuiApplication>
#include <QScreen>

SC_ToolDialog::SC_ToolDialog(WorkflowAppWidget* parent)
  : QDialog(parent)
{
  QVBoxLayout *theMainLayout = new QVBoxLayout(this);
  theStackedWidget = new QStackedWidget();
  theMainLayout->addWidget(theStackedWidget);

  QRect rec = QGuiApplication::primaryScreen()->geometry();
  int height = this->height()<int(rec.height())?int(rec.height()):this->height();
  int width  = this->width()<int(rec.width())?int(rec.width()):this->width();
  height = abs(0.75*height);
  width = abs(0.75*width);
  this->resize(width, height);
  this->hide();

  setWindowFlag(Qt::WindowStaysOnTopHint);
}


SC_ToolDialog::~SC_ToolDialog()
{
  qDebug() << "SC_ToolDialog::Destructor";
}

void SC_ToolDialog::clear()
{
  qDebug() << "SC_ToolDialog::clear - method yest to be implemented";
}

void SC_ToolDialog::addTool(SimCenterAppWidget *theAppWidget, QString text) {

    if (theApplicationNames.indexOf(text) == -1) {
        theApplicationNames.append(text);
        theStackedWidget->addWidget(theAppWidget);
    }
}

void SC_ToolDialog::showTool(QString text) {
  
  int index = theApplicationNames.indexOf(text);
  if (index != -1) {
    
    theStackedWidget->setCurrentIndex(index);
    this->show();
    this->activateWindow();
    this->raise();

  }
}  

	     





