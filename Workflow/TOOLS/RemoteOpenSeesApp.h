#ifndef REMOTE_OPENSEES_APP_H
#define REMOTE_OPENSEES_APP_H

#include <SimCenterAppWidget.h>
#include <SC_ResultsWidget.h>

class QDir;
class SC_DirEdit;
class SC_FileEdit;
class SC_ComboBox;

class RemoteOpenSeesAppResult : public SC_ResultsWidget
{
  Q_OBJECT
  
public:
  RemoteOpenSeesAppResult();
  ~RemoteOpenSeesAppResult();
  int processResults(QString &outputFile, QString &dirName);

public slots:

private:
  SC_DirEdit *resultsFolder;  
};

class RemoteOpenSeesApp : public SimCenterAppWidget
{
  Q_OBJECT
  
public:
  RemoteOpenSeesApp();
  ~RemoteOpenSeesApp();
  
  void clear(void);
  bool copyFiles(QString &destDir);
  bool outputToJSON(QJsonObject &json);
  bool outputAppDataToJSON(QJsonObject &json);
  SC_ResultsWidget * getResultsWidget(QWidget *parent);
		    
public slots:

private:
  SC_FileEdit *theScriptFile;
  SC_ComboBox *theApplication;  
  SC_ComboBox *theVersion;

  RemoteOpenSeesAppResult *theResult;
};

#endif // RemoteOpenSeesApp_H
