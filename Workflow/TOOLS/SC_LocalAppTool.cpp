#include "SC_ToolDialog.h"
#include "Utils/ProgramOutputDialog.h"
#include "WorkflowAppWidget.h"
#include "SimCenterAppWidget.h"

#include <QDebug>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>

SC_ToolDialog::SC_ToolDialog(WorkflowAppWidget* parent)
  : QDialog(parent)
{
  QVBoxLayout *theMainLayout = new QVBoxLayout(this);
  theStackedWidget = new QStackedWidget();
  theMainLayout->addWidget(theStackedWidget);

  QGridLayout *theButtonLayout = new QGridLayout();
  closeButton = new QPushButton("Close");
  runLocalButton = new QPushButton("Run");
  runRemoteButton = new QPushButton("RUN at DesignSafe");
  getRemoteButton = new QPushButton("GET from DesignSafe");
  
  theButtonLayout->addWidget(runLocalButton,0,0);
  theButtonLayout->addWidget(runRemoteButton,0,1);
  theButtonLayout->addWidget(getRemoteButton,0,2);
  theButtonLayout->addWidget(closeButton,0,3);
  
  theMainLayout->addLayout(theButtonLayout);
  runLocalButton->hide();
  runRemoteButton->hide();  
  getRemoteButton->hide();
  //  closeButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);
  connect(closeButton,&QPushButton::clicked,this,&QDialog::close);
  // theMainLayout->addWidget(closeButton);
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
	this->hide();	
    }
}

void SC_ToolDialog::showTool(QString text) {
  int index = theApplicationNames.indexOf(text);
  if (index != -1) {
    theStackedWidget->setCurrentIndex(index);
    this->showMaximized();
  }
}  

	     





