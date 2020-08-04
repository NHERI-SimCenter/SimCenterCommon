
// Written: fmckenna
// Purpose: to test the INputWidgetSheetBM widget

#include <QTreeView>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QDebug>
#include "MainWindowInputSheetBM.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMenuBar>
#include <QAction>
#include <QMenu>
#include <QApplication>

#include "InputWidgetSheetBM.h"
#include "SimCenterTableWidget.h"
#include "JsonValidator.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
{

    // todo: add additional model types such as SAM or EVT
    // Currently there is only one type (BIM) all the 'pages' for BIM types are collected inside the InputWidgetSheetBM
    // additional model types will require additional sheet containers InputWidgetSheetXX
    // to add additional model types: determine model type and instantiate coorect widget sheet type
    // UI provides the Model menu that allows the user to select the input model type

    inputWidget = new InputWidgetSheetBM();
    inputWidget->setMinimumWidth(800);

    this->setCentralWidget(inputWidget);

    this->createActions();

    inputWidget->setMainWindow(this);


}

MainWindow::~MainWindow()
{

}


bool MainWindow::save()
{
  if (currentFile.isEmpty()) {
    return saveAs();
  } else {
    return saveFile(currentFile);
  }
}

bool MainWindow::saveAs()
{
  //
  // get filename
  //

  QFileDialog dialog(this);
  dialog.setWindowModality(Qt::WindowModal);
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  if (dialog.exec() != QDialog::Accepted)
    return false;

  // and save the file
  return saveFile(dialog.selectedFiles().first());
}

void MainWindow::open()
{
  QString fileName = QFileDialog::getOpenFileName(this);
  if (!fileName.isEmpty())
    loadFile(fileName);
}

void MainWindow::newFile()
{
  // clear old
  inputWidget->clear();

  // set currentFile blank
  setCurrentFile(QString());
}


void MainWindow::setCurrentFile(const QString &fileName)
{
  currentFile = fileName;
  //  setWindowModified(false);

  QString shownName = currentFile;
  if (currentFile.isEmpty())
    shownName = "untitled.json";

  setWindowFilePath(shownName);
}

bool MainWindow::saveFile(const QString &fileName)
{
  //
  // open file
  //

  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text)) {
    QMessageBox::warning(this, tr("Application"),
             tr("Cannot write file %1:\n%2.")
             .arg(QDir::toNativeSeparators(fileName),
                  file.errorString()));
    return false;
  }


  //
  // create a json object, fill it in & then use a QJsonDocument
  // to write the contents of the object to the file in JSON format
  //

  QJsonObject json;
  inputWidget->outputToJSON(json);
  QJsonDocument doc(json);
  file.write(doc.toJson());

  // close file
  file.close();

  // set current file
  setCurrentFile(fileName);

  return true;
}

void MainWindow::loadFile(const QString &fileName)
{
  //
  // open file
  //

  QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
      QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    // validate the document
    // JsonValidator class already takes a model type param, add additional model types as required


    JsonValidator *jval = new JsonValidator();
    jval->validate(this, BIM, fileName);

    // place contents of file into json object
    QString val;
    val=file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObj = doc.object();

    // close file
    file.close();

    // given the json object, create the C++ objects
    if ( ! (currentFile.isNull() || currentFile.isEmpty()) ) {
        inputWidget->clear();
    }
    inputWidget->inputFromJSON(jsonObj);

    setCurrentFile(fileName);
}


void MainWindow::createActions() {
 QMenu *fileMenu = menuBar()->addMenu(tr("&File"));


 //const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
 //const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));

 //QToolBar *fileToolBar = addToolBar(tr("File"));

 QAction *newAction = new QAction(tr("&New"), this);
 newAction->setShortcuts(QKeySequence::New);
 newAction->setStatusTip(tr("Create a new file"));
 connect(newAction, &QAction::triggered, this, &MainWindow::newFile);
 fileMenu->addAction(newAction);
 //fileToolBar->addAction(newAction);

 QAction *openAction = new QAction(tr("&Open"), this);
 openAction->setShortcuts(QKeySequence::Open);
 openAction->setStatusTip(tr("Open an existing file"));
 connect(openAction, &QAction::triggered, this, &MainWindow::open);
 fileMenu->addAction(openAction);
 //fileToolBar->addAction(openAction);


 QAction *saveAction = new QAction(tr("&Save"), this);
 saveAction->setShortcuts(QKeySequence::Save);
 saveAction->setStatusTip(tr("Save the document to disk"));
 connect(saveAction, &QAction::triggered, this, &MainWindow::save);
 fileMenu->addAction(saveAction);

 QAction *saveAsAction = new QAction(tr("&Save As"), this);
 saveAsAction->setStatusTip(tr("Save the document with new filename to disk"));
 connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);
 fileMenu->addAction(saveAsAction);

 // strangely, this does not appear in menu (at least on a mac)!! ..
 // does Qt not allow as in tool menu by default?
 // check for yourself by changing Quit to drivel and it works
 QAction *exitAction = new QAction(tr("&Quit"), this);
 connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
 // exitAction->setShortcuts(QKeySequence::Quit);
 exitAction->setStatusTip(tr("Exit the application"));
 fileMenu->addAction(exitAction);

 // the Model Menu cotains is a check list of which model type is currently being edited
 // to get the current value check each menu item
 // only one menu item should be selected at a time, the dafault on startup is BIM
 QMenu *modelMenu = menuBar()->addMenu(tr("&Model"));

 QAction *bimAction = new QAction(tr("&BIM"), this);
 bimAction->setCheckable(true);
 bimAction->setChecked(true);
 bimAction->setStatusTip(tr("Create a Building Information Model"));
 modelMenu->addAction(bimAction);
 //todo: the menu should connect to a funcation that presents a confirmation dialog, saves the current model to file,
 // unchecks the previous value, reloads thhe main withdow with the appropriate InputWIdetSheetXX class

 QAction *samAction = new QAction(tr("&SAM"), this);
 samAction->setCheckable(true);
 samAction->setStatusTip(tr("Create a Structure Information Model"));
 modelMenu->addAction(samAction);
 //todo: the menu should connect to a funcation that presents a confirmation dialog, saves the current model to file,
 // unchecks the previous value, reloads thhe main withdow with the appropriate InputWIdetSheetXX class


 cutAction = new QAction(tr("Cu&t"), this);
 cutAction->setIcon(QIcon(":/images/cut.png"));
 cutAction->setShortcut(QKeySequence::Cut);
 cutAction->setStatusTip(tr("Cut the current selection's contents "
                            "to the clipboard"));
 //connect(cutAction, SIGNAL(triggered()), inputWidget->getActiveSpreadsheet(), SLOT(cut()));

 copyAction = new QAction(tr("&Copy"), this);
 copyAction->setIcon(QIcon(":/images/copy.png"));
 copyAction->setShortcut(QKeySequence::Copy);
 copyAction->setStatusTip(tr("Copy the current selection's contents "
                             "to the clipboard"));
 //connect(copyAction, SIGNAL(triggered()), inputWidget->getActiveSpreadsheet(), SLOT(copy()));

 pasteAction = new QAction(tr("&Paste"), this);
 pasteAction->setIcon(QIcon(":/images/paste.png"));
 pasteAction->setShortcut(QKeySequence::Paste);
 pasteAction->setStatusTip(tr("Paste the clipboard's contents into "
                              "the current selection"));
 //connect(pasteAction, SIGNAL(triggered()), inputWidget->getActiveSpreadsheet(), SLOT(paste()));

 deleteAction = new QAction(tr("&Delete"), this);
 deleteAction->setShortcut(QKeySequence::Delete);
 deleteAction->setStatusTip(tr("Delete the current selection's "
                               "contents"));
 //connect(deleteAction, SIGNAL(triggered()), inputWidget->getActiveSpreadsheet(), SLOT(del()));

 editMenu = menuBar()->addMenu(tr("&Edit"));
 editMenu->addAction(cutAction);
 editMenu->addAction(copyAction);
 editMenu->addAction(pasteAction);
 editMenu->addAction(deleteAction);



 selectRowAction = new QAction(tr("&Row"), this);
 selectRowAction->setStatusTip(tr("Select all the cells in the "
                                  "current row"));
 //connect(selectRowAction, SIGNAL(triggered()), inputWidget->getActiveSpreadsheet(), SLOT(selectCurrentRow()));

 selectColumnAction = new QAction(tr("&Column"), this);
 selectColumnAction->setStatusTip(tr("Select all the cells in the "
                                     "current column"));
 //connect(selectColumnAction, SIGNAL(triggered()), inputWidget->getActiveSpreadsheet(), SLOT(selectCurrentColumn()));

 selectAllAction = new QAction(tr("&All"), this);
 selectAllAction->setShortcut(QKeySequence::SelectAll);
 selectAllAction->setStatusTip(tr("Select all the cells in the "
                                  "spreadsheet"));
 //connect(selectAllAction, SIGNAL(triggered()), inputWidget->getActiveSpreadsheet(), SLOT(selectAll()));

 selectSubMenu = editMenu->addMenu(tr("&Select"));
 selectSubMenu->addAction(selectRowAction);
 selectSubMenu->addAction(selectColumnAction);
 selectSubMenu->addAction(selectAllAction);

}


// called from InputWidgetSheet on selectionChangedEvent to disconnect Edit menu from current sheet
void MainWindow::disconnectMenuItems(SimCenterTableWidget  *inputwidget)
{

    disconnect(cutAction, SIGNAL(triggered()), (const QObject *)inputwidget->getSpreadsheetWidget(), SLOT(cut()));
    disconnect(copyAction, SIGNAL(triggered()), (const QObject *)inputwidget->getSpreadsheetWidget(), SLOT(copy()));
    disconnect(pasteAction, SIGNAL(triggered()), (const QObject *)inputwidget->getSpreadsheetWidget(), SLOT(paste()));
    disconnect(deleteAction, SIGNAL(triggered()), (const QObject *)inputwidget->getSpreadsheetWidget(), SLOT(del()));

    disconnect(selectRowAction, SIGNAL(triggered()), (const QObject *)inputwidget->getSpreadsheetWidget(), SLOT(selectCurrentRow()));
    disconnect(selectColumnAction, SIGNAL(triggered()), (const QObject *)inputwidget->getSpreadsheetWidget(), SLOT(selectCurrentColumn()));
    disconnect(selectAllAction, SIGNAL(triggered()), (const QObject *)inputwidget->getSpreadsheetWidget(), SLOT(selectAll()));

}

// called from InputWidgetSheet on selectionChangedEvent to dconnect Edit menu to newly selected sheet
void MainWindow::connectMenuItems(SimCenterTableWidget  *inputwidget)
{

    connect(cutAction, SIGNAL(triggered()), (const QObject *)inputwidget->getSpreadsheetWidget(), SLOT(cut()));
    connect(copyAction, SIGNAL(triggered()), (const QObject *)inputwidget->getSpreadsheetWidget(), SLOT(copy()));
    connect(pasteAction, SIGNAL(triggered()), (const QObject *)inputwidget->getSpreadsheetWidget(), SLOT(paste()));
    connect(deleteAction, SIGNAL(triggered()), (const QObject *)inputwidget->getSpreadsheetWidget(), SLOT(del()));

    connect(selectRowAction, SIGNAL(triggered()), (const QObject *)inputwidget->getSpreadsheetWidget(), SLOT(selectCurrentRow()));
    connect(selectColumnAction, SIGNAL(triggered()), (const QObject *)inputwidget->getSpreadsheetWidget(), SLOT(selectCurrentColumn()));
    connect(selectAllAction, SIGNAL(triggered()), (const QObject *)inputwidget->getSpreadsheetWidget(), SLOT(selectAll()));

}
