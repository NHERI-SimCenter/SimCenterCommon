class MainWindowWorkflowApp;
class WorkflowAppWidget;

class WorkflowCLI {

public:
    WorkflowCLI(MainWindowWorkflowApp *windowApp, WorkflowAppWidget* inputApp);
    ~WorkflowCLI();

  int parseAndRun(int argc, char **argv);
  int runLocal(void);

private:
  MainWindowWorkflowApp* window;
  WorkflowAppWidget* inputApp;
};

