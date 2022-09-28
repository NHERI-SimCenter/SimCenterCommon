#include <unordered_map>
#include <string>

class QVariant;
class MainWindowWorkflowApp;
class WorkflowAppWidget;

class WorkflowCLI {

public:
    WorkflowCLI(MainWindowWorkflowApp *windowApp, WorkflowAppWidget* inputApp);
    ~WorkflowCLI();

    int parseAndRun(int argc, char **argv);
    int runLocal(void);

private:
    int configureSimCenterApps(int argc, char **argv);
    void resetConfiguration(void);

    bool reset_config = true;
    MainWindowWorkflowApp* window;
    WorkflowAppWidget* inputApp;

    // Keys stored in "Common" QSettings
    std::unordered_map<std::string, QVariant*> common_options {
      {"pythonExePath", nullptr}
    };

    // Keys stored in application's QSettings
    std::unordered_map<std::string, QVariant*> app_options {
      {"localWorkDir",  nullptr},
      {"remoteWorkDir", nullptr},
      {"customAppDir",  nullptr},
      {"openseesPath",  nullptr},
      {"dakotaPath",    nullptr},
      {"appDir",        nullptr}
    };
};

