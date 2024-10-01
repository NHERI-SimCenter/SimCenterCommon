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
class TapisMachine;
class RemoteJobManager;
class QJsonObject;

class SC_RemoteAppTool : public SimCenterAppWidget
{
  Q_OBJECT
  
public:
  SC_RemoteAppTool(QString tapisAppName,
		   QString tapisAppVersion,
		   QString machine,   		   
		   QList<QString> queueNames,
		   RemoteService *theRemoteService,
		   SimCenterAppWidget* theEnclosedApp,
		   QDialog *enclosingDialog = nullptr);

  SC_RemoteAppTool(QString tapisAppName,
		   QString tapisAppVersion,
		   TapisMachine *theMachine,
		   RemoteService *theRemoteService,
		   SimCenterAppWidget* theEnclosedApp,
		   QDialog *enclosingDialog = nullptr);  
  
  ~SC_RemoteAppTool();

  void initialize(QDialog *enclosingDialog);
  void clear(void);
  bool outputCitation(QJsonObject &jsonObject) override;

  void setFilesToDownload(QStringList, bool unzipZip = true);
  void setExtraInputs(QMap<QString, QString> extraInputs);
  void setExtraParameters(QMap<QString, QString> extraParameters);

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
  QString tapisAppVersion;
  QString machine;  
  QStringList queus; 
  
  QLineEdit *nameLineEdit;
  // QLineEdit *systemLineEdit;
  QLineEdit *numCPU_LineEdit;
  QLineEdit *numGPU_LineEdit;
  QLineEdit *numProcessorsLineEdit;
  QLineEdit *runtimeLineEdit;  
  QLineEdit *allocation; 
  QPushButton *submitButton;


  QString tmpDirName;
  QString remoteDirectory;

  // From RemoteApplication
  // TODO: Condense and make more consistent the approach to SC_RemoteAppTool and RemoteApplication
  QString shortDirName;
  QMap<QString, QString> extraInputs;
  QMap<QString, QString> extraParameters;
  QString designsafeDirectory; 
  QString tempDirectory;
  QString remoteHomeDirPath;
  QJsonObject theJob;

  TapisMachine *theMachine;
  RemoteJobManager *theJobManager;
};

#endif // SC_RemoteAppTool_H
