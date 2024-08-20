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
#include <QJsonObject>

SC_RemoteAppTool::SC_RemoteAppTool(QString appName,
				   QString appVersion,
				   QString hpcMachine, 
				   QList<QString> theQueus,
				   RemoteService *theRemoteService,
				   SimCenterAppWidget* theEnclosedApp,
				   QDialog *enclosingDialog)
  :SimCenterAppWidget(), theApp(theEnclosedApp), theService(theRemoteService),
   tapisAppName(appName), tapisAppVersion(appVersion), machine(hpcMachine),
   queus(theQueus)
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
  QString shortDirName = QCoreApplication::applicationName() + ":";

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

  int numNode = 1;
  int maxProcPerNode = 56; //theApp->getMaxNumProcessors(56);
  if (machine == "stampede3")
    maxProcPerNode =48;
  
  numRow++;
  numCPU_LineEdit = new QLineEdit();
  remoteLayout->addWidget(new QLabel("Num Nodes:"),numRow,0);  
  remoteLayout->addWidget(numCPU_LineEdit,numRow,1);
  numCPU_LineEdit->setText("1");
    
  if (queus.first() == "gpu-a100" || queus.first() == "gpu-a100-dev") {
    numRow++;
    
    // lonestar 6 .. only set up for gpu-a100
    maxProcPerNode = 128; //theApp->getMaxNumProcessors(56);

    QLabel *numGPU_Label = new QLabel();
    remoteLayout->addWidget(new QLabel("Num GPUs:"),numRow,0);
    
    numGPU_LineEdit = new QLineEdit();
    numGPU_LineEdit->setText("3");
    numGPU_LineEdit->setToolTip(tr("Total # of GPUs to use (across all nodes)"));
    remoteLayout->addWidget(numGPU_LineEdit,numRow,1);
    
    
  } else {
    numGPU_LineEdit = NULL;
  }

  numRow++;
  remoteLayout->addWidget(new QLabel("Num Processors Per Node:"),numRow,0);    
  numProcessorsLineEdit = new QLineEdit();
  numProcessorsLineEdit->setText(QString::number(maxProcPerNode));
  numProcessorsLineEdit->setToolTip(tr("Total # of Processes to Start"));

  remoteLayout->addWidget(numProcessorsLineEdit,numRow,1);

  numRow++;
  remoteLayout->addWidget(new QLabel("Max Run Time (minutes):"),numRow,0);
  runtimeLineEdit = new QLineEdit();
  if (queus.first() == "gpu-a100" || queus.first() == "gpu-a100-dev") { 
    runtimeLineEdit->setText("120");
  }
  else {
    runtimeLineEdit->setText("120");
  }
  
  runtimeLineEdit->setToolTip(tr("Run time limit on running Job (minutes). Job will be stopped if while running it exceeds this"));
  remoteLayout->addWidget(runtimeLineEdit,numRow,1);

  numRow++;
  remoteLayout->addWidget(new QLabel("TACC Allocation"), numRow, 0);
  allocation = new QLineEdit();
  allocation->setText(SimCenterPreferences::getInstance()->getDefaultAllocation());
  remoteLayout->addWidget(allocation,numRow,1);

  numRow++;
  submitButton = new QPushButton();
  submitButton->setText("Submit");
  submitButton->setToolTip(tr("Press to launch job on remote machine. After pressing, window closes when Job Starts"));
  remoteLayout->addWidget(submitButton,numRow,1);
  
  theRemoteDialog->setLayout(remoteLayout);
  theRemoteDialog->hide();

  
  connect(fileLoadButton, &QPushButton::clicked, this, [=]() {
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


  connect(fileSaveButton, &QPushButton::clicked, this, [=]() {

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


  
  QStringList filesToDownload; filesToDownload << "scInput.json" << "results.zip" << "inputData.zip";
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
    errorMessage("ERROR - You Need to Login to DesignSafe to run a remote application.");
    QMessageBox msg;
    msg.setText("You need to go back to the Main Application and Login to DesignSafe to run a remote application.");
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

  workDir.mkdir(tmpDirName);
  tempDirectory = workDir.absoluteFilePath(tmpDirName);

  remoteHomeDirPath = theService->getHomeDir();
  if (remoteHomeDirPath.isEmpty()) {
    qDebug() << "RemoteApplication:: - remoteHomeDir is empty!!";
    return;
  }
  
  QString remoteDirectory = remoteHomeDirPath + QString("/") + tmpDirName;
  designsafeDirectory = remoteDirectory;

  //
  // in tmpDir, create another "inputs" in which we will place all the files needed by the app & then zip it up
  //

  QDir destinationDir(tempDirectory);  
  QString inputsDirectory  = destinationDir.absoluteFilePath("inputData");
  destinationDir.mkpath(inputsDirectory);

  QString zipFile(destinationDir.absoluteFilePath("inputData.zip"));
  QDir inputDataDir(destinationDir.absoluteFilePath("inputData"));  
  
  theApp->copyFiles(inputsDirectory);  
  
  ZipUtils::ZipFolder(inputDataDir, zipFile);

  // remove inputData so not sent
  inputDataDir.removeRecursively();
  
  //
  // in tmpDir create the input file
  //

  QString inputFile = destinationDir.absoluteFilePath("scInput.json");
  
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
  json["runDir"]=tempDirectory;
  json["remoteAppDir"]=SimCenterPreferences::getInstance()->getRemoteAppDir();    
  json["runType"]=QString("runningRemote");
  int nodeCount = numCPU_LineEdit->text().toInt();
  int numProcessorsPerNode = numProcessorsLineEdit->text().toInt();
  json["nodeCount"]=nodeCount;
  json["numP"]=nodeCount*numProcessorsPerNode; 

  if (numGPU_LineEdit != NULL) {
    int gpuCount = numGPU_LineEdit->text().toInt();
    json["gpus"]=gpuCount;
  }

  QJsonDocument doc(json);
  file.write(doc.toJson());
  file.close();
  
  //
  // now send directory containing inputFile and inputData.zip across
  //

  submitButton->setEnabled(false);
  
  connect(theService, SIGNAL(uploadDirectoryReturn(bool)), this, SLOT(uploadDirReturn(bool)));
  // qDebug() << "localDIR: "  << tempDirectory;
  // qDebug() << "remoteDIR: " << remoteHomeDirPath;
  
  theService->uploadDirectoryCall(tempDirectory, remoteHomeDirPath);         
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

    //
    // file in the json job object to invoke app with
    //
    
    int nodeCount = numCPU_LineEdit->text().toInt();
    int numProcessorsPerNode = numProcessorsLineEdit->text().toInt();
    QString shortDirName = QCoreApplication::applicationName() + ": ";
    
    job["appId"]=tapisAppName;
    job["appVersion"]=tapisAppVersion;         
    job["name"]=shortDirName + nameLineEdit->text();
    job["nodeCount"]=nodeCount;
    job["coresPerNode"]=numProcessorsPerNode;
    job["maxMinutes"]=runtimeLineEdit->text().toInt();
    
    int ramPerNodeMB = 128000;    
    job["memoryMB"]= ramPerNodeMB;

    //
    // figure out queue
    //
    
    QString queue; // queuu to send job to
    QString firstQueue = queus.first();
    if (firstQueue == "gpu-a100" ||
	firstQueue == "gpu-a100-dev" ||
	firstQueue == "gpu-h100" ||
	firstQueue == "rtx" ||
	firstQueue == "rtx-dev" ||
	firstQueue == "gpu-a100-small") {
      
      queue = firstQueue;
      
    } else if (machine == "frontera") {
      
      queue = "small";
      if (nodeCount > 2)
	queue = "normal";
      if (nodeCount > 512)
        queue = "large";
      
    } else if (machine == "stampede3") {
      queue = "icx";
    }
    
    
    job["execSystemLogicalQueue"]=queue;  


    //
    // Job Parameters (envVariables & allocation)
    //

    QJsonObject parameterSet;    
    QJsonArray envVariables;

    // envVariables: we set the inputFile as scInput.json
    QJsonObject inputFileObj;
    inputFileObj["key"]="inputFile";
    inputFileObj["value"]="scInput.json";
    envVariables.append(inputFileObj);
    
    // any additional envVariables variables are app dependent
    QJsonObject extraParameters;
    theApp->outputAppDataToJSON(extraParameters); //< sets maxRunTime in parameters
    for (auto parameterName : extraParameters.keys()) {
      QJsonObject paramObj;
      paramObj["key"]=parameterName;
      paramObj["value"]=extraParameters[parameterName];
      envVariables.append(paramObj);
    }
    
    // Remove duplicate parameters from envVariables
    QSet<QString> keys;
    QJsonArray newEnvVariables;
    for (auto envVar : envVariables) {
      QJsonObject envVarObj = envVar.toObject();
      QString key = envVarObj["key"].toString();
      if (!keys.contains(key)) {
	keys.insert(key);
	newEnvVariables.append(envVarObj);
      }
    }
    envVariables = newEnvVariables;

    // allocation to use
    QJsonArray schedulerOptions;
    QJsonObject schedulerOptionsObj;
    QString allocationText = QString("-A " ) + allocation->text();
    schedulerOptionsObj["arg"]=allocationText;
    
    schedulerOptions.append(schedulerOptionsObj);
    parameterSet["schedulerOptions"]=schedulerOptions;
    parameterSet["envVariables"]=envVariables;
    job["parameterSet"]=parameterSet;

    //
    // file inputs
    //
    
    QDir theDirectory(tempDirectory);
    QString dirName = theDirectory.dirName();
    
    QJsonArray fileInputs;
    QJsonObject inputs;
    
    inputs["envKey"]="inputDirectory";    
    inputs["targetPath"]="*";
    inputs["sourceUrl"] = "tapis://" + designsafeDirectory; //storage;
    
    /* NO JUSTIN THIS ONLY WORKS FOR YOU
       if (tapisAppName == "simcenter-claymore-lonestar6") {
       inputs["sourceUrl"] = "tapis://" + QString("designsafe.storage.default/") + QString("bonusj/HydroUQ/") + tmpDirName;           
       data["sourceUrl"]   = "tapis://" + QString("designsafe.storage.default/") + QString("bonusj/mpm-public-ls6");
       }
    */
    
    
    fileInputs.append(inputs);
    job["fileInputs"]=fileInputs;          

    //
    // now remove the tmp directory
    //
    theDirectory.removeRecursively();
    
    qDebug() << "SUBMIT.json:  " << job;

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
    this->errorMessage("FATAL - SC_RemoteAppTool received NULL pointer theResults from theApp->getResultsWidget()... skipping theResults->processResults()");
    return;
  }

  QString blankFileName("scInput.json");
  theResults->processResults(blankFileName,dirName);

}

bool SC_RemoteAppTool::outputCitation(QJsonObject &jsonObject) {
  return theApp->outputCitation(jsonObject);
}

	     





