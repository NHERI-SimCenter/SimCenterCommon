
#include "RemoteOpenSeesApp.h"
#include <SC_FileEdit.h>
#include <SC_DirEdit.h>
#include <SC_ComboBox.h>

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

RemoteOpenSeesAppResult::RemoteOpenSeesAppResult()
  :SC_ResultsWidget()
{
  QGridLayout *theLayout = new QGridLayout();
  this->setLayout(theLayout);

  /*
  QStringList theExe; theExe << "OpenSees" << "OpenSeesMP" << "OpenSeesSP";
  QStringList theVersions; theExe << "v3.6.0";  
  theScriptFile = new SC_FileEdit("inputScript");
  theApplication = new SC_ComboBox("openSeesExecutable", theExe);
  theVersion = new SC_ComboBox("openSeesVersion", theVersions);    

  theLayout->addWidget(theScriptFile,0,0);
  theLayout->addWidget(theApplication,0,0);
  theLayout->addWidget(theVersion,0,0);    
  */
  
}

RemoteOpenSeesAppResult::~RemoteOpenSeesAppResult()
{

}

int
RemoteOpenSeesAppResult::processResults(QString &inputFile, QString &resultsDir)
{
  if (resultsDir != theOutputDir->getDirName()) {
    // move files
  }
  /*
  QString resultsFile = resultsDir + QDir::separator() + QString("results") + QDir::separator() + QString("results.out");

  QFile file(resultsFile);

  if ( !file.exists() ){
    theOutputLine->setPlainText("SOME PROBLEM With JOB as results.out file non-existant");    
  } else {
    file.open(QFile::ReadOnly | QFile::Text);
    theOutputLine->setPlainText(file.readAll());
  }
  */
  return 0;
}


RemoteOpenSeesApp::RemoteOpenSeesApp()
  :SimCenterAppWidget()
{
  QGridLayout *theLayout = new QGridLayout();
  this->setLayout(theLayout);

  QStringList theExe; theExe << "OpenSees" << "OpenSeesMP" << "OpenSeesSP";
  QStringList theVersions; theVersions << "v3.6.0";
  
  theScriptFile = new SC_FileEdit("inputScript");
  theApplication = new SC_ComboBox("openSeesExecutable", theExe);
  theVersion = new SC_ComboBox("openSeesVersion", theVersions);    

  theLayout->addWidget(new QLabel("scripitFile"),0,0);
  theLayout->addWidget(new QLabel("OpenSees Application"),1,0);
  theLayout->addWidget(new QLabel("OpenSees Version"),2,0);    
  theLayout->addWidget(theScriptFile,0,1);
  theLayout->addWidget(theApplication,1,1);
  theLayout->addWidget(theVersion,2,1);    

  /*
  theScriptFile = new QLineEdit("");
  QPushButton *chooseFile = new QPushButton("Browse");

  connect(chooseFile, &QPushButton::clicked, this,
	  [=]() {
	    //QString fileName=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
	    QString fileName=QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)"); // sy - to continue from the previously visited directory
	    theScriptFile->setText(fileName);
	  });  
  theResult = new RemoteOpenSeesAppResult();
  
  theLayout->addWidget(new QLabel("Input Script"),0,0);      
  theLayout->addWidget(theScriptFile, 0,1,1,2);
  theLayout->addWidget(chooseFile,0,3);
  //theLayout->setColStretch(2,1);  
  
  QStringList applications; applications<< "OpenSees" << "OpenSeesSP" << "OpenSeesMP" << "OpenSees Py";
  QStringList versions; versions<< "Current" << "v3.6.0" << "v3.5.1";
  theApplication = new SC_ComboBox("application", applications);
  theApplication = new SC_ComboBox("version", versions);
  theLayout->addWidget(theApplication,1,1,1,1);
  theLayout->addWidget(theVersion,2,1,1,1);

  theResult = new RemoteOpenSeesAppResult();  
  theLayout->addWidget(theResult, 3,1,1,4);
  */
  
  theResult = new RemoteOpenSeesAppResult();  
  theLayout->addWidget(theResult, 3,1,1,4);  
  theLayout->setRowStretch(4,1);  
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
    QString dirName = fileDir.dirName();
    
    // mkdir
    destDir.mkdir(dirName);

    // copy files
    QString sourcePath = fileDir.absolutePath();
    QString destPath = destDir.absoluteFilePath(dirName);

    SimCenterAppWidget::copyPath(sourcePath, destDirectory, true);
    return true;
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
    json["scriptFile"]= fileInfo.fileName();
    json["scriptDir"]= fileDir.dirName();
    return true;
  }
  return false;
}


SC_ResultsWidget *
RemoteOpenSeesApp::getResultsWidget(QWidget *parent)
{
  return theResult;
}





	     





