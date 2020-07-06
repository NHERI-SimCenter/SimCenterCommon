
// Written: fmckenna

#include "MainWindowRV.h"
#include "ui_mainwindowRV.h"

#include "RandomVariable.h"
#include "RandomVariableInputWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QScrollArea>
#include <QPushButton>
#include "RandomVariableInputWidget.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QTextStream>
#include <QAction>
#include <QJsonDocument>
#include <QJsonObject>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->createActions();
    this->makeRV();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::makeRV(void) {
   rvWidget = new RandomVariableInputWidget();
   this->ui->verticalLayout->addWidget(rvWidget);
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

void MainWindow::about()
{
  QMessageBox::about(this, tr("About Application"),
		     tr("The <b>Application</b> tests the RandomVariableWidget class"));

}

void MainWindow::newFile()
{
  // clear old
  rvWidget->clear();

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
  rvWidget->outputToJSON(json);
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
    
    // place contents of file into json object
    QString val;
    val=file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObj = doc.object();

    // close file
    file.close();
    
    // given the json object, create the C++ objects
    rvWidget->inputFromJSON(jsonObj);
    
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
 saveAction->setStatusTip(tr("Save the document with new filename to disk"));
 connect(saveAction, &QAction::triggered, this, &MainWindow::save);
 fileMenu->addAction(saveAsAction);

 // strangely, this does not appear in menu (at least on a mac)!! .. 
 // does Qt not allow as in tool menu by default?
 // check for yourself by changing Quit to drivel and it works
 QAction *exitAction = new QAction(tr("&Quit"), this);
 connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
 // exitAction->setShortcuts(QKeySequence::Quit);
 exitAction->setStatusTip(tr("Exit the application"));
 fileMenu->addAction(exitAction);
}
