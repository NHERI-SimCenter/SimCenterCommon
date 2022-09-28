#include <stdio.h>
#include <string>
#include <unordered_map>
#include <LocalApplication.h>
#include "WorkflowAppWidget.h"
#include "MainWindowWorkflowApp.h"
#include "WorkflowCLI.h"

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QTranslator>
#include <QEventLoop>
#include <QApplication>
#include <QSettings>
#include <QTextStream>

// Replicate qDebug() behavior with standard terminal streams.
// The " \b" string quiets zero-length format string warnings.
QTextStream qstderr(stderr);
QTextStream qstdout(stdout);
#define qStdErr(...) (fprintf(stderr, " \b" __VA_ARGS__), qstderr)
#define qStdOut(...) (fprintf(stdout, " \b" __VA_ARGS__), qstdout)


static void print_help(const char* app_name);
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
  
  if (argc == 2 && strcmp(argv[1], "--help") == 0) {
    print_help(argv[0]);
    exit(0);
  }
  current_arg = configureSimCenterApps(argc, argv);
  if (current_arg < argc) {
    qStdErr("Running file '%s'\n", argv[current_arg]);
    inputApp->loadFile(*(new QString(argv[current_arg])));
    res  = runLocal();

  } else if (reset_config)
    resetConfiguration();

  // qStdErr("status = %d\n", res);
  
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

    // Count consumed arguments
    int used_count = 1; 

    for (int i = 1; i < argc; i++){
      if (strcmp(argv[i], "--config") == 0 ||
          strcmp(argv[i], "-c") == 0 ||
          strcmp(argv[i], "--persist-config") == 0 ||
          strcmp(argv[i], "-C") == 0 ) {

        if (strcmp(argv[i], "--persist-config") == 0 ||
            strcmp(argv[i], "-C") == 0 ) {
          reset_config = false;
        }
        used_count ++;

        if ((argc > i + 1) && (argv[i + 1][0] != '-')) {
          used_count ++;
          char *tokptr = argv[++i];
          char *key = strtok_r(tokptr, "=", &tokptr);
          char *val = strtok_r(tokptr, "=", &tokptr);

          if (val) {
            // 
            // --config KEY=VALUE ; set KEY with VALUE
            //
            if (app_options.find(key) != app_options.end()) {
              // Store current value to reset later
              app_options[key] = new QVariant(settingsApp.value(key));
              // qStdErr("%s: %s -> %s\n", key, app_options[key]->toString().toUtf8().data(), val);
              settingsApp.setValue(key, val);

              if (strcmp(key, "appDir") == 0)
                settingsApp.setValue("customAppDir", true);

            } else if (app_options.find(key) != app_options.end()) {
              // Store current value to reset later
              common_options[key] = new QVariant(settingsCommon.value(key));
              settingsCommon.setValue("pythonExePath", val);

            } else {
              qStdErr("ERROR: Unknown configuration key '%s'\n", key);
              exit(-1);
            }

          } else {
            // --config VAR ; print out var
              qStdOut() << key << ": " << settingsCommon.value(key,settingsApp.value(key, "Key unknown")).toString().toUtf8().data() << "\n";
          }

        } else {
            // --config ; print out all key/value pairs
            qStdOut() << "Application Settings:\n";
            for (const auto&k : settingsApp.allKeys()) {
              const char* svalue = settingsApp.value(k, "None").toString().toUtf8().data();
              qStdOut() << "\t" << k.toUtf8().data() << ": " << svalue << "\n";
            }
            qStdOut() << "SimCenter Common Settings:\n";
            for (const auto&k : settingsCommon.allKeys()) {
              const char* svalue = settingsApp.value(k, "None").toString().toUtf8().data();
              qStdOut() << "\t" << k.toUtf8().data() << ": " << svalue << "\n";
            }
        }
 
      } else if (argv[i][0] == '-') {
          qStdErr("ERROR: Unknown key '%s'\n", argv[i]);
          exit(-1);
      }

    }
    return used_count;
}

static void print_help(const char* app_name) {
  static const char *options = R"EOF(
Options
--config/-c          [<key>=<value>]    Set config value for <key> to <value> 
                                        for current run.
--persist-config/-C   <key>=<value>     Set config value for <key> to <value>, 
                                        and store changes for app.

Examples

  ./quoFEM -c appDir=~/simcenter/SimCenterBackendApplications/ Examples/qfem-0001/src/input.json

)EOF";

  qStdOut("usage: %s [options] <input.json>\n%s", app_name, options);
}

