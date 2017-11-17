#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QItemSelection>
#include <QTreeView>
#include <QStandardItemModel>
#include <QJsonObject>

class InputWidgetSheetBM;

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

 private:
    void setCurrentFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void loadFile(const QString &fileName);

    void createActions();

    //Ui::MainWindow *ui;

    QString currentFile;
    InputWidgetSheetBM *inputWidget;

};

#endif // MAINWINDOW_H
