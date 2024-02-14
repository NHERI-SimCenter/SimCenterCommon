
#include "RemoteAppTest.h"
#include <SC_FileEdit.h>

#include <QPushButton>
#include <QGridLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QWidget>
#include <QLabel>
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QFileDialog>
#include <QJsonObject>

RemoteAppTestResult::RemoteAppTestResult()
  :SC_ResultsWidget()
{
  QGridLayout *theLayout = new QGridLayout();
  this->setLayout(theLayout);
  
  theOutputLine = new QPlainTextEdit("NO RESULT .. YOU HAVE TO RUN TOOL & GET RESULTS BACK");
  theLayout->addWidget(theOutputLine,0,0);  
}

RemoteAppTestResult::~RemoteAppTestResult()
{

}

int
RemoteAppTestResult::processResults(QString &inputFile, QString &resultsDir)
{
  QString resultsFile = resultsDir + QDir::separator() + QString("results") + QDir::separator() + QString("results.out");

  QFile file(resultsFile);

  if ( !file.exists() ){
    theOutputLine->setPlainText("SOME PROBLEM With JOB as results.out file non-existant");    
  } else {
    file.open(QFile::ReadOnly | QFile::Text);
    theOutputLine->setPlainText(file.readAll());
  }

  return 0;
}


RemoteAppTest::RemoteAppTest()
  :SimCenterAppWidget()
{
  QGridLayout *theLayout = new QGridLayout();
  this->setLayout(theLayout);

  theFile = new QLineEdit("");
  QPushButton *chooseFile = new QPushButton("Browse");

  connect(chooseFile, &QPushButton::clicked, this,
	  [=]() {
	    //QString fileName=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
	    QString fileName=QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)"); // sy - to continue from the previously visited directory
	    theFile->setText(fileName);
	  });  
  theResult = new RemoteAppTestResult();
  
  theLayout->addWidget(new QLabel("MPI Program Source Code"),0,0);      
  theLayout->addWidget(theFile, 0,1);
  theLayout->addWidget(chooseFile,0,2);
  theLayout->addWidget(theResult, 1,1);
  theLayout->setRowStretch(2,1);
  
}

RemoteAppTest::~RemoteAppTest()
{

}

void RemoteAppTest::clear()
{

}

bool RemoteAppTest::copyFiles(QString &destDir)
{
  return SimCenterAppWidget::copyFile(theFile->text(), destDir);
}

bool
RemoteAppTest::outputToJSON(QJsonObject &json) {
  return true;
}

bool
RemoteAppTest::outputAppDataToJSON(QJsonObject &json)
{
  QString fileName = theFile->text();
  QFileInfo fileInfo(fileName);
  json["programFile"]= fileInfo.fileName();  
  return true;
}


SC_ResultsWidget *
RemoteAppTest::getResultsWidget(QWidget *parent)
{
  return theResult;
}





	     





