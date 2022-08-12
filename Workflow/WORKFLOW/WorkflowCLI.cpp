#include <stdio.h>
#include <string>
#include <unordered_map>
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
#include <QSettings>

WorkflowCLI::WorkflowCLI(MainWindowWorkflowApp *w, WorkflowAppWidget *input)
  : window(w), inputApp(input)
{

}

WorkflowCLI::~WorkflowCLI()
{

}

/* static void
configureSimCenterApps(int argc, char **argv); */

int WorkflowCLI::parseAndRun(int argc, char **argv)
{
  int res = 0;
  int current_arg = 0;

  current_arg = configureSimCenterApps(argc, argv);
  if (current_arg < argc) {
    fprintf(stderr, "Running file '%s'\n", argv[current_arg]);
    inputApp->loadFile(*(new QString(argv[current_arg])));
    res  = runLocal();

  } else if (reset_config)
    resetConfiguration();

  fprintf(stderr, "status = %d\n", res);
  
  return res;
}

int
WorkflowCLI::runLocal(void)
{
    QEventLoop loop;
    QObject::connect(inputApp, &WorkflowAppWidget::sendLocalRunComplete, window, &MainWindowWorkflowApp::onExitButtonClicked);
    if (reset_config)
      QObject::connect(inputApp, &WorkflowAppWidget::sendLocalRunComplete, [=](void){this->resetConfiguration();});
    QObject::connect(inputApp, &WorkflowAppWidget::sendLocalRunComplete, &loop, &QEventLoop::quit);
    window->onRunButtonClicked();
    return loop.exec();
}

void
WorkflowCLI::resetConfiguration(void) {
    QSettings settingsCommon("SimCenter", "Common");
    QSettings settingsApp("SimCenter", QCoreApplication::applicationName());

    for (const auto &item : app_options) {
        const std::string key = item.first;
        if (app_options[key] && app_options[key]->isValid()) {
            settingsApp.setValue(QString::fromStdString(key), *app_options[key]);
        }
    }
    for (const auto &item : common_options) {
        const std::string key = item.first;
        if (common_options[key] && common_options[key]->isValid()) {
            settingsCommon.setValue(QString::fromStdString(key), *common_options[key]);
        }
    }
}

int
WorkflowCLI::configureSimCenterApps(int argc, char **argv) {
    QSettings settingsCommon("SimCenter", "Common");
    QSettings settingsApp("SimCenter", QCoreApplication::applicationName());

    int used_count = 1;
    for (int i = 1; i < argc; i++){
      if (strcmp(argv[i], "--config") == 0 ||
          strcmp(argv[i], "-c") == 0 ||
          strcmp(argv[i], "--persist-config") == 0 ||
          strcmp(argv[i], "-C") == 0 ) {

        used_count ++;
        if (strcmp(argv[i], "--persist-config") == 0 ||
            strcmp(argv[i], "-C") == 0 ) {
          reset_config = false;
        }


        if (argc > i + 1)  {
          used_count ++;
          char *tokptr = argv[++i];
          char *key = strtok_r(tokptr, "=", &tokptr);
          char *val = strtok_r(tokptr, "=", &tokptr);

          if (val) {
            if (app_options.find(key) != app_options.end()) {
              // Store current value to reset later
              app_options[key] = new QVariant(settingsApp.value(key));
              fprintf(stderr, "%s: %s -> %s\n", key, app_options[key]->toString().toStdString().c_str(), val);
              settingsApp.setValue(key, val);

              if (strcmp(key, "appDir") == 0)
                settingsApp.setValue("customAppDir", true);

            } else if (app_options.find(key) != app_options.end()) {
              // Store current value to reset later
              common_options[key] = new QVariant(settingsCommon.value(key));
              settingsCommon.setValue("pythonExePath", val);

            } else {
              fprintf(stderr, "ERROR: Unknown configuration key '%s'\n", key);
              exit(-1);
            }

          } else {
            for (const auto&k : settingsApp.allKeys()) {
              const char* value = settingsApp.value(k, "None").toString().toStdString().c_str();
              fprintf(stdout, "%s: %s\n", k.toStdString().c_str(), value);
            }
            // exit(-1);
          }
        }

      
      } else if (argv[i][0] == '-') {
          fprintf(stderr, "ERROR: Unknown option '%s'\n", argv[i]);
          exit(-1);
      }

    }
    return used_count;
}
