#ifndef SC_RemoteAppTool_H
#define SC_RemoteAppTool_H

#include <SimCenterAppWidget.h>
#include <QList>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QMap>

class QStackedWidget;
class SimCenterAppWidget;
class WorkflowAppWidget;
class QPushButton;
class RemoteService;
class QLineEdit;
class RemoteJobManager;
class QJsonObject;

class SC_RemoteAppTool : public SimCenterAppWidget
{
  Q_OBJECT
  
public:
  SC_RemoteAppTool(QString tapisAppName,
		   QList<QString> queueNames,
		   RemoteService *theRemoteService,
		   SimCenterAppWidget* theEnclosedApp,
		   QDialog *enclosingDialog = nullptr);
  
  ~SC_RemoteAppTool();
  
  void clear(void);
  bool outputCitation(QJsonObject &jsonObject) override;
		    
public slots:
  //  void onRunRemoteButtonPressed();
  void submitButtonPressed();
  void uploadDirReturn(bool);
  void startJobReturn(QString);

  void onGetRemoteButtonPressed();
  void processResults(QString &);
    
private:

  SimCenterAppWidget *theApp;
  RemoteService *theService;  
  QString tapisAppName;
  QStringList queus;
  
  QLineEdit *nameLineEdit;
  // QLineEdit *systemLineEdit;
  QLineEdit *numCPU_LineEdit;
  QLineEdit *numGPU_LineEdit;
  QLineEdit *numProcessorsLineEdit;
  QLineEdit *runtimeLineEdit;  
  QPushButton *submitButton;


  QString tmpDirName;
  QString remoteDirectory;

  // From RemoteApplication
  // TODO: Condense and make more consistent the approach to SC_RemoteAppTool and RemoteApplication
  QLineEdit *allocation; 
  QString shortDirName;
  QMap<QString, QString> extraInputs;
  QMap<QString, QString> extraParameters;
  QString designsafeDirectory; 
  QString tempDirectory;
  QString remoteHomeDirPath;
  QJsonObject theJob;


  RemoteJobManager *theJobManager;
};

#endif // SC_RemoteAppTool_H
