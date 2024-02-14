#ifndef SC_RemoteAppTool_H
#define SC_RemoteAppTool_H

#include <SimCenterAppWidget.h>
#include <QList>
#include <QString>

class QStackedWidget;
class SimCenterAppWidget;
class WorkflowAppWidget;
class QPushButton;
class RemoteService;
class QLineEdit;
class RemoteJobManager;

class SC_RemoteAppTool : public SimCenterAppWidget
{
  Q_OBJECT
  
public:
  SC_RemoteAppTool(QString tapisAppName,
		   QList<QString> queueNames,
		   RemoteService *theService,
		   SimCenterAppWidget* theEnclosedApp,
		   QDialog *enclosingDialog = nullptr);
  
  ~SC_RemoteAppTool();
  
  void clear(void);
		    
public slots:
  //  void onRunRemoteButtonPressed();
  void submitButtonPressed();
  void uploadDirReturn(bool);
  void startJobReturn(QString);

  void onGetRemoteButtonPressed();
  void processResults(QString &);
    
private:

  SimCenterAppWidget *theApp;
  QString tapisAppName;
  RemoteService *theService;

  QLineEdit *nameLineEdit;
  QLineEdit *numCPU_LineEdit;
  QLineEdit *numProcessorsLineEdit;
  QLineEdit *runtimeLineEdit;  

  QPushButton *submitButton;

  QString tmpDirName;
  QString remoteDirectory;

  RemoteJobManager *theJobManager;
};

#endif // SC_RemoteAppTool_H
