#ifndef RemoteAppTest_H
#define RemoteAppTest_H

#include <SimCenterAppWidget.h>
#include <SC_ResultsWidget.h>

class QLineEdit;
class QPlainTextEdit;

class RemoteAppTestResult : public SC_ResultsWidget
{
  Q_OBJECT
  
public:
  RemoteAppTestResult();
  ~RemoteAppTestResult();
  int processResults(QString &outputFile, QString &dirName);  
public slots:

private:
  QPlainTextEdit *theOutputLine;
};

class RemoteAppTest : public SimCenterAppWidget
{
  Q_OBJECT
  
public:
  RemoteAppTest();
  ~RemoteAppTest();
  
  void clear(void);
  bool copyFiles(QString &destDir);
  bool outputToJSON(QJsonObject &json);
  bool outputAppDataToJSON(QJsonObject &json);
  SC_ResultsWidget * getResultsWidget(QWidget *parent);
		    
public slots:

private:
  QLineEdit *theFile;
  RemoteAppTestResult *theResult;
};

#endif // RemoteAppTest_H
