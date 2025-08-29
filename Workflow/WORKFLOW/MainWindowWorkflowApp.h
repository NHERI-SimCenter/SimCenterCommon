#ifndef MAIN_WINDOW_WORKFLOW_APP_H
#define MAIN_WINDOW_WORKFLOW_APP_H

#include <QMainWindow>
#include <QItemSelection>
#include <QTreeView>
#include <QStandardItemModel>
#include <QJsonObject>
#include <RemoteService.h>

//#include <JsonValidator.h>

class WorkflowAppWidget;
class SimCenterWidget;
class QPushButton;
class QDockWidget;
class QLabel;
class SimCenterPreferences;
class ExampleDownloader;
class ProgramOutputDialog;

class MainWindowWorkflowApp : public QMainWindow
{
  Q_OBJECT   

public:
    explicit MainWindowWorkflowApp(QString appName, WorkflowAppWidget *theApp, RemoteService *theRemoteInterface, QWidget *parent = 0, bool exampleDownloader = false);
    ~MainWindowWorkflowApp();
    void setCopyright(QString &copyright);
    void setVersion(QString &version);
    void setDocumentationURL(QString &version);
    void setAbout(QString &about);
    void setAbout(QString &, QString&);
    void setFeedbackURL(QString &feedback);
    void setCite(QString &cite);
    void updateExamplesMenu(bool placeBeforeHelp = false);

    QString outputFilePath;

 protected:

 signals:
    void attemptLogin(QString, QString);
    void logout();

    void sendStatusMessage(QString message);
    void sendErrorMessage(QString message);
    void sendFatalMessage(QString message);
    void sendInfoMessage(QString message);

  public slots:
    // for menu items
    void newFile();
    void open();
    void openFile(QString filename);
    bool save();
    bool saveAs();
    void clear();

    void about();
    void manual();
    void submitFeedback();
    void version();
    void preferences();
    void copyright();
    void cite();
    virtual void loadExamples();
  
    // for main actions
    void onRunButtonClicked();
    void onRemoteRunButtonClicked();
    void onRemoteGetButtonClicked();
    void onExitButtonClicked();

    // for login
    void onLoginButtonClicked();
    void onLoginSubmitButtonClicked();
    void attemptLoginReturn(bool);
    void logoutReturn(bool);
    void showExampleDownloader(void);
    void showCitations(void);  


 private:
    void setCurrentFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void loadFile(QString &fileName);

    void createActions();

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *selectSubMenu;
    QMenu *toolsMenu;
    QMenu *optionsMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QMenu *exampleMenu;
    bool _exampleDownloader;

    // some variables for logging in
    QWidget *loginWindow;
    QLineEdit *nameLineEdit;
    QLineEdit *passwordLineEdit;
    QPushButton *loginSubmitButton;
    bool loggedIn;
    int numTries;

    QString currentFile;
    WorkflowAppWidget *theWorkflowAppWidget;
    RemoteService *theRemoteInterface;
    //SimCenterWidget *currentWidget;

    QPushButton *loginButton;

    QString versionText;
    QString aboutText;
    QString aboutTitle;
    QString aboutSource;
    QString copyrightText;
    QString manualURL;
    QString feedbackURL;
    QString citeText;
    bool isAutoLogin;

    QDockWidget* statusDockWidget;

    SimCenterPreferences *thePreferences;
    ExampleDownloader* theExampleDownloader;
    ProgramOutputDialog* statusWidget;

    QWidget *content; // the main content widget, used for styling
    

    friend class R2DUnitTests;

};

#endif // MAIN_WINDOW_WORKFLOW_APP_H
