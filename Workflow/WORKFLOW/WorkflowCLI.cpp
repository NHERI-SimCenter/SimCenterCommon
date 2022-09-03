#include <stdio.h>
#include <LocalApplication.h>
#include <WorkflowAppWidget.h>
#include <MainWindowWorkflowApp.h>
#include "WorkflowCLI.h"

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QTranslator>
#include <QEventLoop>
#include <QApplication>

WorkflowCLI::WorkflowCLI(MainWindowWorkflowApp *w, WorkflowAppWidget *input)
  : window(w), inputApp(input)
{

}

WorkflowCLI::~WorkflowCLI()
{

}

int WorkflowCLI::parseAndRun(int argc, char **argv)
{
  if (argc > 2)
    // printf(window->version());
    // printf(window->about());
    return 1;
  else {
    inputApp->loadFile(*(new QString(argv[1])));
    int res  = runLocal();
    return res;
  }
}

int WorkflowCLI::runLocal(void)
{
    QEventLoop loop;
    QObject::connect(inputApp, &WorkflowAppWidget::sendLocalRunComplete, window, &MainWindowWorkflowApp::onExitButtonClicked);
    QObject::connect(inputApp, &WorkflowAppWidget::sendLocalRunComplete, &loop, &QEventLoop::quit);
    window->onRunButtonClicked();
    return loop.exec();
}

