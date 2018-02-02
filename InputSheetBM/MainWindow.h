#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QItemSelection>
#include <QTreeView>
#include <QStandardItemModel>
#include <QJsonObject>

class InputWidgetSheetBM;
class SimCenterTableWidget;

class MainWindow : public QMainWindow
{
  Q_OBJECT
    
    public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
  public slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();


  //void selectionChangedSlot(const QItemSelection &, const QItemSelection &);
    void connectMenuItems(SimCenterTableWidget  *widget);
    void disconnectMenuItems(SimCenterTableWidget  *widget);


 private:
    void setCurrentFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void loadFile(const QString &fileName);

    void createActions();


    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *selectSubMenu;
    QMenu *toolsMenu;
    QMenu *optionsMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exitAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *deleteAction;
    QAction *selectRowAction;
    QAction *selectColumnAction;
    QAction *selectAllAction;
    QAction *findAction;
    QAction *goToCellAction;
    QAction *recalculateAction;
    QAction *sortAction;
    QAction *showGridAction;
    QAction *autoRecalcAction;
    QAction *aboutAction;
    QAction *aboutQtAction;


    //Ui::MainWindow *ui;

    QString currentFile;
    InputWidgetSheetBM *inputWidget;

};

#endif // MAINWINDOW_H
