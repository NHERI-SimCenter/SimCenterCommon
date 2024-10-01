#include <WorkflowAppWidget.h>
#include "MainWindowWorkflowApp.h"
#include <QWidget>
#include <RemoteService.h>
#include <Utils/ProgramOutputDialog.h>
#include <GoogleAnalytics.h>

#include <QMenuBar>
#include <QDebug>
#include <QCoreApplication>
#include <QApplication>
#include <QMessageBox>

ProgramOutputDialog *WorkflowAppWidget::progressDialog = nullptr;

WorkflowAppWidget::WorkflowAppWidget(RemoteService *theService, QWidget *parent)
    :QWidget(parent), theRemoteService(theService)
{

  //
  // check we are not dealing with those who may not have read the manual and/or do not really know how a computer works
  //
  
  bool okToRun = true;
  QString errorMessage="";

  QString appDir(QCoreApplication::applicationDirPath());
  qDebug() << appDir << " -> " << appDir.indexOf(QString("Volumes"));
  if (appDir.indexOf("Volumes") == 1) {
    
    okToRun = false;
    errorMessage = "You cannot run the application from a mounted volume.\n\n If you are seeing this message after clicking on the image shown when you opened the DMG file, it is because you must must first copy the application to another folder, e.g. Applications or Desktop, before you can run it.";
  } else if (appDir.indexOf("OneDrive") != -1) {
    
    okToRun = false;
    errorMessage = "You cannot run the application from OneDrive. \n\n The application must be copied from OneDrive to a folder on your local hard drive and run from there. This is because files in OneDrive are not actually on your computer but in the cloud somewehere.";
  }

  if (okToRun == false) {
    QString appName = QCoreApplication::applicationName();
    int ret = QMessageBox::critical(this,appName,errorMessage,QMessageBox::Ok);
    QApplication::quit();
  }

 
  this->setContentsMargins(0,0,0,0);
  
  progressDialog = ProgramOutputDialog::getInstance(this);
}


WorkflowAppWidget::~WorkflowAppWidget()
{

}


void
WorkflowAppWidget::setMainWindow(MainWindowWorkflowApp* window) {
    theMainWindow = window;
}


void
WorkflowAppWidget::statusMessage(const QString msg){
    qDebug() << "WorkflowAppWidget::statusMessage" << msg;
    progressDialog->appendText(msg);
    emit sendStatusMessage(msg);
}


void
WorkflowAppWidget::errorMessage(const QString msg){
    qDebug() << "WorkflowAppWidget::errorMessage" << msg;
    progressDialog->appendErrorMessage(msg);
    GoogleAnalytics::Report("WorkflowAppWidgetErrorMessage", msg);
    emit sendErrorMessage(msg);
}


void
WorkflowAppWidget::fatalMessage(const QString msg){
    progressDialog->fatalMessage(msg);
}

MainWindowWorkflowApp *WorkflowAppWidget::getTheMainWindow() const
{
    return theMainWindow;
}


ProgramOutputDialog *WorkflowAppWidget::getProgressDialog()
{
    return progressDialog;
}


void WorkflowAppWidget::runComplete()
{
    qDebug() << "Task Completed";
}

int
WorkflowAppWidget::createCitation(QJsonObject &citationToAddTo, QString citeFile)
{
  // does nothing
  return 0;
}
