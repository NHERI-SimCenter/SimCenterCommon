// Written: fmk 04/24

#include <QObject>
#include <QString>
#include <QStringList>
#include <QThread>
//class QProcess;
#include <QProcess>


class RunPython : public QObject
{
  Q_OBJECT

public:
  RunPython(const QString &script, const QStringList &args, const QString &workDir);
  virtual ~RunPython();

public slots:

  void processScript();
  void handleProcessTextOutput(void);
  void handleProcessErrorOutput(void);    
  void handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void terminate();
  
signals:
  void processFinished(int errorCode);
  void errorMessage(const QString &msg);
  void statusMessage(const QString &msg);

private:
  QString script;
  QStringList args;
  QString workDir;
  
  QProcess *process;  
};

class RunPythonInThread : public QObject
{
  Q_OBJECT

public:
  RunPythonInThread(const QString &script, const QStringList &args, const QString &);
  virtual ~RunPythonInThread();
  void runProcess(void);
  void terminateProcess(void);
			      			      

public slots:
  void handleFinished(int errorCode);
  void errorMessage(const QString &);
  void statusMessage(const QString &);  

signals:
  void processFinished(int errorCode);

private:
  QThread workerThread;  
  QString script;
  QStringList args;
  QString workDir;
  RunPython *worker {nullptr};
};
