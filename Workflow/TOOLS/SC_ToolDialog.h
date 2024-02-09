#ifndef SC_ToolDialog_H
#define SC_ToolDialog_H

#include <QDialog>
#include <QList>
#include <QString>

class QStackedWidget;
class SimCenterAppWidget;
class WorkflowAppWidget;
class QPushButton;

class SC_ToolDialog : public QDialog
{
  Q_OBJECT
  
public:
  SC_ToolDialog(WorkflowAppWidget* parent);
  ~SC_ToolDialog();
  
  void addTool(SimCenterAppWidget *tool, QString toolName);
  void clear(void);
		    
public slots:
  void showTool(QString name);

private:

  QStackedWidget* theStackedWidget = nullptr;
  QList<QString> theApplicationNames;

  QPushButton *closeButton;
  QPushButton *runLocalButton;
  QPushButton* runRemoteButton;
  QPushButton* getRemoteButton;  
};

#endif // SC_ToolDialog_H
