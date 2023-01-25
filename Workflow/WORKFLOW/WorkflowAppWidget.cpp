#include <WorkflowAppWidget.h>
#include "MainWindowWorkflowApp.h"
#include <QWidget>
#include <RemoteService.h>
#include <Utils/ProgramOutputDialog.h>

#include <QMenuBar>
#include <QDebug>

ProgramOutputDialog *WorkflowAppWidget::progressDialog = nullptr;

WorkflowAppWidget::WorkflowAppWidget(RemoteService *theService, QWidget *parent)
    :QWidget(parent), theRemoteService(theService)
{
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
    emit sendErrorMessage(msg);
}


void
WorkflowAppWidget::fatalMessage(const QString msg){
    progressDialog->appendErrorMessage(msg);
    emit sendFatalMessage(msg);
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

