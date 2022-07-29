// #include <QObject>
// #include <QWidget>
// #include <QString>
// #include <WorkflowAppWidget.h>
// #include <RemoteService.h>

class MainWindowWorkflowApp;
class WorkflowAppWidget;

class WorkflowCLI {
  // Q_OBJECT

public:
    WorkflowCLI(MainWindowWorkflowApp *windowApp, WorkflowAppWidget* inputApp);
    ~WorkflowCLI();

  int parseAndRun(int argc, char **argv);
  int runLocal(void);

private:
  MainWindowWorkflowApp* window;
  WorkflowAppWidget* inputApp;
};

