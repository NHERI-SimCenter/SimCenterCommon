
#include "RemoteOpenSeesApp.h"
#include <SC_FileEdit.h>
#include <SC_DirEdit.h>
#include <SC_ComboBox.h>
#include <QGroupBox>
#include <QPushButton>
#include <QGridLayout>
#include <QLineEdit>
#include <QWidget>
#include <QLabel>
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QJsonObject>
#include <QApplication>
#include <QStandardPaths>
#include <QMessageBox>
#include <ZipUtils.h>
#include <QFileDialog>
#include <QLineEdit>

RemoteOpenSeesAppResult::RemoteOpenSeesAppResult()
  :SC_ResultsWidget()
{
  QString pathToFolder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
    + QDir::separator() + QCoreApplication::applicationName() + QDir::separator() +
    "DesignSafeFiles";

  QDir resDir(pathToFolder);
  if (!resDir.exists())
    if (!resDir.mkpath(pathToFolder)) {
      QString msg("PEER_NGA_Records could not create local work dir: "); msg += pathToFolder;
      errorMessage(msg);
    }

  resultsFolder = new SC_DirEdit("outputDir");
  resultsFolder->setDirName(pathToFolder);
  
  QGridLayout *layout = new QGridLayout();
  layout->addWidget(new QLabel("Output Folder"),0,0);
  layout->addWidget(resultsFolder,0,1);
  this->setLayout(layout);
}
  

RemoteOpenSeesAppResult::~RemoteOpenSeesAppResult()
{

}

int
RemoteOpenSeesAppResult::processResults(QString &inputFile, QString &resultsDir)
{

  QString directoryToUnzipIn = resultsFolder->getDirName();
  QDir theFolder(directoryToUnzipIn);

  // create folder
  if (!theFolder.exists())
    if (theFolder.mkpath(directoryToUnzipIn))
      if (!theFolder.exists())
	errorMessage("Could not create a Folder to place results in");

  
  QString resultsFile = resultsDir + QDir::separator() + QString("results.zip");
  QString moveFile = directoryToUnzipIn + QDir::separator() + QString("results.zip");  

  /*********** just copy don't unzip
  if (QFile::rename(resultsFile, moveFile)) {
    QMessageBox msgBox;
    QString msg("The Files have been downloaded from DesignSafe into ");
    msg+=moveFile;
    msgBox.setText(msg);
    msgBox.exec();
  }
  *************************************/
  
  if (ZipUtils::UnzipFile(resultsFile, directoryToUnzipIn)) {
    QMessageBox msgBox;
    QString msg("The Files have been downloaded from DesignSafe into ");
    msg+=directoryToUnzipIn;
    msgBox.setText(msg);
    msgBox.exec();
  } else {
    errorMessage("Unzipping failed");
  }

  return 0;
}


RemoteOpenSeesApp::RemoteOpenSeesApp()
  :SimCenterAppWidget()
{

  QGroupBox *theInputs = new QGroupBox("Input Information");
  QGridLayout *theLayout = new QGridLayout();
  theInputs->setLayout(theLayout);

  QStringList theExe; theExe << "OpenSees" << "OpenSeesMP" << "OpenSeesSP" << "OpenSeesPy";
  QStringList theVersions; theVersions << "v3.6.0";
  
  theScriptFile = new SC_FileEdit("inputScript");
  theApplication = new SC_ComboBox("openSeesExecutable", theExe);
  theVersion = new SC_ComboBox("openSeesVersion", theVersions);    

  theLayout->addWidget(new QLabel("scriptFile"),0,0);
  theLayout->addWidget(new QLabel("OpenSees Application"),1,0);
  theLayout->addWidget(new QLabel("OpenSees Version"),2,0);    
  theLayout->addWidget(theScriptFile,0,1);
  theLayout->addWidget(theApplication,1,1);
  theLayout->addWidget(theVersion,2,1);    

  QGroupBox *theOutputs = new QGroupBox("Result Download");
  QGridLayout *theOutputsLayout = new QGridLayout(theOutputs);
  theResult = new RemoteOpenSeesAppResult();  
  theOutputsLayout->addWidget(theResult,0,0,1,4);
  
  QVBoxLayout *theMainLayout = new QVBoxLayout(this);
  theMainLayout->addWidget(theInputs);
  theMainLayout->addWidget(theOutputs);
  theMainLayout->addStretch();    
}

RemoteOpenSeesApp::~RemoteOpenSeesApp()
{

}

void RemoteOpenSeesApp::clear()
{

}

bool RemoteOpenSeesApp::copyFiles(QString &destDirectory)
{
  //
  // copy file and all files in dir to a dir in destDir of same name
  //   => mkdir and then copy the files
  
  QDir destDir(destDirectory);
  QFileInfo fileInfo(theScriptFile->getFilename());
  if (fileInfo.exists()) {
    QDir fileDir(fileInfo.dir());
    
    QString sourcePath = fileDir.absolutePath();
    SimCenterAppWidget::copyPath(sourcePath, destDirectory, true);    
  }
  return true;
}    


bool
RemoteOpenSeesApp::outputToJSON(QJsonObject &json) {
  return true; 
}

bool
RemoteOpenSeesApp::outputAppDataToJSON(QJsonObject &json)
{
  theApplication->outputToJSON(json);
  theVersion->outputToJSON(json);  
  QFileInfo fileInfo(theScriptFile->getFilename());
  if (fileInfo.exists()) {
    QDir fileDir(fileInfo.dir());  
    json["inputScript"]= fileInfo.fileName();
    json["scriptDir"]= fileDir.absolutePath();
    json["remoteAPP"]= "true";    
    return true;
  }  
  return false;
}


SC_ResultsWidget *
RemoteOpenSeesApp::getResultsWidget(QWidget *parent)
{
  return theResult;
}





	     





