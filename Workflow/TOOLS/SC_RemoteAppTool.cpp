#include "SC_RemoteAppTool.h"
#include "Utils/ProgramOutputDialog.h"
#include "SimCenterAppWidget.h"
#include <RemoteService.h>
#include <SimCenterPreferences.h>
#include <ZipUtils/ZipUtils.h>
#include <RemoteJobManager.h>
#include <SC_ResultsWidget.h>
#include <Utils/RelativePathResolver.h>

#include <QDebug>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QDir>
#include <QApplication>
#include <QCoreApplication>
#include <QLineEdit>
#include <QJsonDocument>
#include <QMessageBox>
#include <QFileDialog>


SC_RemoteAppTool::SC_RemoteAppTool(QString appName,
				   QList<QString> queus,
				   RemoteService *theRemoteService,
				   SimCenterAppWidget* theEnclosedApp,
				   QDialog *enclosingDialog)
:SimCenterAppWidget(), theApp(theEnclosedApp), theService(theRemoteService), tapisAppName(appName)
{
  QVBoxLayout *theMainLayout = new QVBoxLayout(this);
  theMainLayout->addWidget(theApp);


  QGridLayout *theButtonLayout = new QGridLayout();
  QPushButton *fileLoadButton = new QPushButton("LOAD File");
  QPushButton *fileSaveButton = new QPushButton("SAVE File");  
  QPushButton *runRemoteButton = new QPushButton("RUN at DesignSafe");
  QPushButton *getRemoteButton = new QPushButton("GET from DesignSafe");

  theButtonLayout->addWidget(fileLoadButton,0,0);
  theButtonLayout->addWidget(fileSaveButton,0,1);  
  theButtonLayout->addWidget(runRemoteButton,0,2);
  theButtonLayout->addWidget(getRemoteButton,0,3);
  
  theMainLayout->addLayout(theButtonLayout);

  // now create the Dialog for the Remote Applications

  QDialog *theRemoteDialog = new QDialog(this);
  // QString workflowScriptName = "FMK-NAME";
  QString shortDirName = QCoreApplication::applicationName() + "BLAH";

  //shortDirName = workflowScriptName;
  //shortDirName = name.chopped(3); // remove .py
  //shortDirName.chop(3);
  
  QGridLayout *remoteLayout = new QGridLayout();
  QLabel *nameLabel = new QLabel();
  
  int numRow = 0;

  remoteLayout->addWidget(new QLabel("Job Name:"), numRow,0);
  nameLineEdit = new QLineEdit();
  nameLineEdit->setToolTip(tr("A meaningful name to provide for you to remember run later (days and weeks from now)"));
  remoteLayout->addWidget(nameLineEdit,numRow,1);
  
  numRow++;
  QLabel *numCPU_Label = new QLabel();
  remoteLayout->addWidget(new QLabel("Num Nodes:"),numRow,0);
  
  numCPU_LineEdit = new QLineEdit();
  numCPU_LineEdit->setText("1");
  numCPU_LineEdit->setToolTip(tr("Total # of nodes to use (each node has many cores)"));
  remoteLayout->addWidget(numCPU_LineEdit,numRow,1);
  
  numRow++;
  remoteLayout->addWidget(new QLabel("# Processors Per Node"),numRow,0);

  int maxProcPerNode = 56; //theApp->getMaxNumProcessors(56);
  numProcessorsLineEdit = new QLineEdit();
  numProcessorsLineEdit->setText(QString::number(maxProcPerNode));
  numProcessorsLineEdit->setText(QString::number(maxProcPerNode));  
  numProcessorsLineEdit->setToolTip(tr("Total # of Processes to Start"));
  remoteLayout->addWidget(numProcessorsLineEdit,numRow,1);
  
  //  QString appName = QCoreApplication::applicationName();
  
  numRow++;
  remoteLayout->addWidget(new QLabel("Max Run Time:"),numRow,0);
  runtimeLineEdit = new QLineEdit();
  runtimeLineEdit->setText("00:20:00");
  runtimeLineEdit->setToolTip(tr("Run time Limit on running Job hours:Min:Sec. Job will be stopped if while running it exceeds this"));
  remoteLayout->addWidget(runtimeLineEdit,numRow,1);
  
  numRow++;
  submitButton = new QPushButton();
  submitButton->setText("Submit");
  submitButton->setToolTip(tr("Press to launch job on remote machine. After pressing, window closes when Job Starts"));
  remoteLayout->addWidget(submitButton,numRow,1);
  
  theRemoteDialog->setLayout(remoteLayout);
  theRemoteDialog->hide();

  
  connect(fileLoadButton, &QPushButton::clicked, this,
	  [=]() {
	    //QString fileName=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
        QString fileName=QFileDialog::getOpenFileName(this,tr("Open JSON File"),"", "JSON file (*.json)"); // sy - to continue from the previously visited directory
	    QFile file(fileName);
	    if (!file.open(QFile::ReadOnly | QFile::Text)) {
	      emit errorMessage(QString("Could Not Open File: ") + fileName);
	      return -1;
	    }

	    //
	    // place contents of file into json object
	    //
	    
	    QString val;
	    val=file.readAll();
	    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
	    QJsonObject jsonObj = doc.object();

	    // close file
	    file.close();
	    
	    //Resolve absolute paths from relative ones
	    QFileInfo fileInfo(fileName);
	    SCUtils::ResolveAbsolutePaths(jsonObj, fileInfo.dir());    
    
	    //
	    // clear current and input from new JSON
	    //
	    
	    theApp->clear();	    
	    theApp->inputFromJSON(jsonObj);
	  });


  connect(fileSaveButton, &QPushButton::clicked, this,
	  [=]() {

        QString fileName=QFileDialog::getSaveFileName(this, tr("Save JSON File"),"", "JSON file (*.json)"); // sy - to continue from the previously visited directory
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
	    theApp->outputToJSON(json);
	    
	    //Resolve relative paths before saving
	    QFileInfo fileInfo(fileName);
	    SCUtils::ResolveRelativePaths(json, fileInfo.dir());
	    
	    QJsonDocument doc(json);
	    file.write(doc.toJson());
	    
	    // close file
	    file.close();	    
	    
	  });
  
  connect(getRemoteButton,&QPushButton::clicked, this, [=](){
    this->onGetRemoteButtonPressed();
  });
  
  connect(runRemoteButton,&QPushButton::clicked, this, [=](){
    theRemoteDialog->show();
  });    

  connect(submitButton,&QPushButton::clicked,this, [=]() {
    this->submitButtonPressed();
    theRemoteDialog->close();
  });  

  if (enclosingDialog != nullptr) {
    QPushButton *closeButton = new QPushButton("Close");  
    theButtonLayout->addWidget(closeButton,0,4);
    connect(closeButton,&QPushButton::clicked,enclosingDialog,&QDialog::close);
    connect(closeButton,&QPushButton::clicked,theRemoteDialog,&QDialog::close);
    connect(closeButton,&QPushButton::clicked,[=]() {
      theJobManager->hide();
    });
  }


  
  QStringList filesToDownload; filesToDownload << "scInput.json" << "results.zip";
  theJobManager = new RemoteJobManager(theService);
  theJobManager->setFilesToDownload(filesToDownload);
  theJobManager->hide();
  connect(theJobManager,SIGNAL(processResults(QString&)), this, SLOT(processResults(QString&)));
}

SC_RemoteAppTool::~SC_RemoteAppTool()
{
  qDebug() << "SC_RemoteAppTool::Destructor";
}

void SC_RemoteAppTool::clear()
{
  theApp->clear();
}

// this method display the RemoteApp dialog widget

void
SC_RemoteAppTool::submitButtonPressed() {

  //
  // first check if logged in
  //
  
  bool loggedIn = theService->isLoggedIn();

  if (loggedIn != true) {
    errorMessage("ERROR - You Need to Login");
    QMessageBox msg;
    msg.setText("You need to go back to the Main Application and Login");
    msg.show();
    return;
  }

  //
  // create tmp directory in which we will place files to be sent
  // 
  

  QDir workDir(SimCenterPreferences::getInstance()->getRemoteWorkDir());

  QUuid uniqueName = QUuid::createUuid();
  QString strUnique = uniqueName.toString();
  strUnique = strUnique.mid(1,36);
  tmpDirName = QString("tmp.SimCenter") + strUnique;
  
  //QString tmpDirName = QString("tmp.SimCenter");  
  QString tmpDirectory = workDir.absoluteFilePath(tmpDirName);
  QDir destinationDirectory(tmpDirectory);

  if(destinationDirectory.exists()) {
    destinationDirectory.removeRecursively();
  } else
    destinationDirectory.mkpath(tmpDirectory);

  //
  // in tmpDir, create another "inputs" in which we will place all the files needed by the app & then zip it up
  //
  
  QString inputsDirectory  = destinationDirectory.absoluteFilePath("inputData");
  destinationDirectory.mkpath(inputsDirectory);

  QString zipFile(destinationDirectory.absoluteFilePath("inputData.zip"));
  QDir inputDataDir(destinationDirectory.absoluteFilePath("inputData"));  
  
  qDebug() << "destinationDir: " << destinationDirectory;  
  qDebug() << "ZIP FILE: " << zipFile;
  qDebug() << "DIR TO ZIP: " << inputsDirectory;
  
  theApp->copyFiles(inputsDirectory);  
  
  ZipUtils::ZipFolder(inputDataDir, zipFile);
  
  //
  // in tmpDir create the input file
  //

  QString inputFile = destinationDirectory.absoluteFilePath("scInput.json");
  
  QFile file(inputFile);
  if (!file.open(QFile::WriteOnly | QFile::Text)) {
    //errorMessage();
    return;
  }
  
  QJsonObject json;

  QJsonObject appData;
  theApp->outputAppDataToJSON(json);
  json["ApplicationData"]=appData;
  
  theApp->outputToJSON(json);

  json["workingDir"]=SimCenterPreferences::getInstance()->getRemoteWorkDir(); 
  json["runDir"]=tmpDirectory;
  json["remoteAppDir"]=SimCenterPreferences::getInstance()->getRemoteAppDir();    
  json["runType"]=QString("runningRemote");
  int nodeCount = numCPU_LineEdit->text().toInt();
  int numProcessorsPerNode = numProcessorsLineEdit->text().toInt();
  json["nodeCount"]=nodeCount;
  json["numP"]=nodeCount*numProcessorsPerNode;    
  json["processorsOnEachNode"]=numProcessorsPerNode;    

  QJsonDocument doc(json);
  file.write(doc.toJson());
  file.close();


  // remove inputData so not sent
  
  inputDataDir.removeRecursively();

  
  //
  // now send directory across
  //

  QString dirName = destinationDirectory.dirName();
  
  QString remoteHomeDirPath = theService->getHomeDir();
  if (remoteHomeDirPath.isEmpty()) {
    qDebug() << "RemoteApplication:: - remoteHomeDir is empty!!";
    return;
  }
  remoteDirectory = remoteHomeDirPath + QString("/") + dirName;
  submitButton->setEnabled(false);
  
  connect(theService, SIGNAL(uploadDirectoryReturn(bool)), this, SLOT(uploadDirReturn(bool)));
  theService->uploadDirectoryCall(tmpDirectory, remoteHomeDirPath);        

}  


void
SC_RemoteAppTool::uploadDirReturn(bool result)
{
  disconnect(theService, SIGNAL(uploadDirectoryReturn(bool)), this, SLOT(uploadDirReturn(bool)));
    
  //
  // now start the app
  //

  if (result == true) {
    
    //
    // create the json needed to run the remote application
    //
    
    QJsonObject job;
    
    //submitButton->setDisabled(true);

    QString shortDirName = QCoreApplication::applicationName() + "BLAH";
    
    job["name"]=shortDirName + nameLineEdit->text();
    int nodeCount = numCPU_LineEdit->text().toInt();
    int numProcessorsPerNode = numProcessorsLineEdit->text().toInt();
    job["nodeCount"]=nodeCount;
    //job["processorsPerNode"]=nodeCount*numProcessorsPerNode; // DesignSafe has inconsistant documentation
    job["processorsOnEachNode"]=numProcessorsPerNode;
    job["maxRunTime"]=runtimeLineEdit->text();

    QString queue = "small";
    if (nodeCount > 2)
      queue = "normal";
    if (nodeCount > 512)
      queue = "large";
    
    job["appId"]=tapisAppName;
    
    job["memoryPerNode"]= "1GB";
    job["archive"]=true;
    job["batchQueue"]=queue;      
    job["archivePath"]="";
    job["archiveSystem"]="designsafe.storage.default";  

    QJsonObject parameters;    
    parameters["inputFile"]="scInput.json";
    parameters["modules"]="petsc,python3";
    theApp->outputAppDataToJSON(parameters);
    job["parameters"]=parameters;

    QJsonObject inputs;
    inputs["inputDirectory"]=remoteDirectory;
    job["inputs"]=inputs;    

    connect(theService,SIGNAL(startJobReturn(QString)), this, SLOT(startJobReturn(QString)));      
    theService->startJobCall(job);
    
  } else {
    submitButton->setEnabled(true);          
  }
}

void
SC_RemoteAppTool::startJobReturn(QString result)
{
  //
  // job started
  //

  disconnect(theService,SIGNAL(startJobReturn(QString)), nullptr, nullptr);      
  Q_UNUSED(result);
  submitButton->setEnabled(true);      
}


void
SC_RemoteAppTool::onGetRemoteButtonPressed() {

    this->errorMessage("");

    bool loggedIn = theService->isLoggedIn();

    if (loggedIn == true) {

        theJobManager->hide();
        theJobManager->updateJobTable("");
        theJobManager->show();

    } else {
        errorMessage("ERROR - You Need to Login");
    }  
}

void
SC_RemoteAppTool::processResults(QString &dirName){

  //
  // get results widget from app and process
  //

  QString localDir = SimCenterPreferences::getInstance()->getRemoteWorkDir();
  QDir localWork(localDir);
  
  if (!localWork.exists())
    if (!localWork.mkpath(localDir)) {
      emit errorMessage(QString("Could not create Working Dir: ") + localDir + QString(" . Try using an existing directory or make sure you have permission to create the working directory."));
      return;
    }
  
    
  SC_ResultsWidget *theResults=theApp->getResultsWidget();
  if (theResults == NULL) {
    this->errorMessage("FATAL - App cannot process Results");
    return;
  }
  QString blankFileName("");
  theResults->processResults(blankFileName,dirName);
}



	     





