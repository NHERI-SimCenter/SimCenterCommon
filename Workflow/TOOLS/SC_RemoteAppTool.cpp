#include "SC_RemoteAppTool.h"
#include "Utils/ProgramOutputDialog.h"
#include "SimCenterAppWidget.h"
#include <RemoteService.h>
#include <SimCenterPreferences.h>
#include <ZipUtils/ZipUtils.h>
#include <RemoteJobManager.h>
#include <SC_ResultsWidget.h>
#include <Utils/RelativePathResolver.h>
#include <TapisMachine.h>

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
#include <QJsonArray>


SC_RemoteAppTool::SC_RemoteAppTool(QString appName,
				   QString appVersion,
				   QString hpcMachine, 
				   QList<QString> theQueus,
				   RemoteService *theRemoteService,
				   SimCenterAppWidget* theEnclosedApp,
				   QDialog *enclosingDialog)
  :SimCenterAppWidget(), theApp(theEnclosedApp), theService(theRemoteService),
   tapisAppName(appName), tapisAppVersion(appVersion), machine(hpcMachine),
   queus(theQueus), theMachine(0)
{

  this->initialize(enclosingDialog);

}

SC_RemoteAppTool::SC_RemoteAppTool(QString appName,
				   QString appVersion,
				   TapisMachine *machine, 
				   RemoteService *theRemoteService,
				   SimCenterAppWidget* theEnclosedApp,
				   QDialog *enclosingDialog)
  :SimCenterAppWidget(), theApp(theEnclosedApp), theService(theRemoteService),
   tapisAppName(appName), tapisAppVersion(appVersion), theMachine(machine)
{
  this->initialize(enclosingDialog);
}

void
SC_RemoteAppTool::initialize(QDialog *enclosingDialog) {

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
  // QLabel *nameLabel = new QLabel();
  
  int numRow = 0;

  remoteLayout->addWidget(new QLabel("Job Name:"), numRow,0);
  nameLineEdit = new QLineEdit();
  nameLineEdit->setToolTip(tr("A meaningful name to provide for you to remember run later (days and weeks from now)"));
  remoteLayout->addWidget(nameLineEdit,numRow,1);


  if (theMachine ==  0) {
    
    int numNode = 1;
    int maxProcPerNode = 56; //theApp->getMaxNumProcessors(56);
    if (machine == "stampede3") {
      maxProcPerNode = 48;
    }

    numRow++;
    numCPU_LineEdit = new QLineEdit();
    remoteLayout->addWidget(new QLabel("Num Nodes:"),numRow,0);  
    remoteLayout->addWidget(numCPU_LineEdit,numRow,1);
    numCPU_LineEdit->setText("1");
    
    numGPU_LineEdit = NULL;
    if (queus.first() == "gpu-a100" || queus.first() == "gpu-a100-dev" || queus.first() == "gpu-a100-small") {
      numGPU_LineEdit = new QLineEdit();
      numRow++;
      
      // lonestar 6 .. only set up for gpu-a100
      remoteLayout->addWidget(new QLabel("Num GPUs:"),numRow,0);
      
      if (queus.first() == "gpu-a100-small") {
	numGPU_LineEdit->setText("1"); // Special queue using just 1 NVIDIA A100
	maxProcPerNode = 32;
      } else {
	numGPU_LineEdit->setText("3"); // 3 NVIDIA A100s
	maxProcPerNode = 128;
      }
      numGPU_LineEdit->setToolTip(tr("# of GPUs requested per node"));
      remoteLayout->addWidget(numGPU_LineEdit, numRow, 1);
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

  } else {

    numRow++;
    remoteLayout->addWidget(theMachine,numRow,0, 2, 3);
    numRow += 2;
  }

  numRow++;
  remoteLayout->addWidget(new QLabel("TACC Allocation"), numRow, 0);
  allocation = new QLineEdit();
  allocation->setText(SimCenterPreferences::getInstance()->getDefaultAllocation());
  allocation->setToolTip(tr("Specify which of your TACC allocations to use when running the job."));
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
	      return -1; // Double-check if it makes sense to use -1 here, might be implicitly cast to bool for Qt connect()?
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
      return 0; // implicit return for Qt connect()?
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
	    theApp->outputAppDataToJSON(json);
	    //theApp->outputToJSON(json);	    
	    
	    //Resolve relative paths before saving
	    QFileInfo fileInfo(fileName);
	    SCUtils::ResolveRelativePaths(json, fileInfo.dir());
	    
	    QJsonDocument doc(json);
	    file.write(doc.toJson());
	    
	    // close file
	    file.close();	    
	    return true;
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

  if (theMachine == 0) {
    int numProcessorsPerNode = numProcessorsLineEdit->text().QString::toInt();
    int nodeCount = numCPU_LineEdit->text().QString::toInt();
    json["nodeCount"]=nodeCount;
    json["numP"]=nodeCount*numProcessorsPerNode; 

    if (numGPU_LineEdit != NULL) {
      int gpuCount = numGPU_LineEdit->text().QString::toInt();
      json["gpus"]=gpuCount;
    }
  } else {
    theMachine->outputToJSON(json);
  }
  
  QJsonDocument doc(json);
  file.write(doc.toJson());
  file.close();
  
  //
  // now send directory containing inputFile and inputData.zip across
  //
  
  submitButton->setEnabled(false);
  
  connect(theService, SIGNAL(uploadDirectoryReturn(bool)), this, SLOT(uploadDirReturn(bool)));

  
  theService->uploadDirectoryCall(tempDirectory, remoteHomeDirPath);         
}  


void
SC_RemoteAppTool::uploadDirReturn(bool result)
{
  qDebug() << "UPLOAD DIR RETURN";
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
    
    QString shortDirName = QCoreApplication::applicationName() + ": ";
    
    job["appId"]=tapisAppName;
    job["appVersion"]=tapisAppVersion;         
    job["name"]=shortDirName + nameLineEdit->text();
    int ramPerNodeMB = 128000;    
    job["memoryMB"]= ramPerNodeMB;


    if (theMachine == 0) {

      int nodeCount = numCPU_LineEdit->text().QString::toInt();
      int numProcessorsPerNode = numProcessorsLineEdit->text().QString::toInt();
      
      job["nodeCount"]=nodeCount;
      job["coresPerNode"]=numProcessorsPerNode;
      job["maxMinutes"]=runtimeLineEdit->text().QString::toInt();
      
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
	
	
	job["execSystemLogicalQueue"]=queue;

      } else if (machine == "stampede3") {
	
	queue = "icx";
	
      }

    } else {
      
      theMachine->outputToJSON(job);

    }
    

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

      constexpr bool TAPISV3_PARAMETERS_STRICTLY_STRINGS = true; // All extra parameters are required to be strings
      if constexpr (TAPISV3_PARAMETERS_STRICTLY_STRINGS) {
        constexpr bool TAPISV3_PARAMETERS_ALLOW_NULL_CONVERSION   = false; // Convert nulls to strings for the submit json
        constexpr bool TAPISV3_PARAMETERS_ALLOW_NUMBER_CONVERSION = true; // Convert numbers to strings for the submit json
        constexpr bool TAPISV3_PARAMETERS_ALLOW_BOOL_CONVERSION   = true; // Convert booleans to strings for the submit json
        constexpr bool TAPISV3_PARAMETERS_ALLOW_OBJECT_CONVERSION = false; // Allow objects (i.e. their contents) to be converted to strings for the submit json
        constexpr bool TAPISV3_PARAMETERS_ALLOW_ARRAY_CONVERSION  = false; // Allow arrays (i.e. their contents) to be converted to strings for the submit json
        
        if (extraParameters[parameterName].isNull() || extraParameters[parameterName].isUndefined()) { 
          if constexpr (TAPISV3_PARAMETERS_ALLOW_NULL_CONVERSION) {
            paramObj["key"]   = parameterName;
            paramObj["value"] = QString(""); // or maybe use "None", "Null", etc
            envVariables.append(paramObj);
          }
        } else if (extraParameters[parameterName].isString()) {
          paramObj["key"]   = parameterName;
          if (extraParameters[parameterName].toString().isEmpty()) {
            paramObj["value"] = QString(""); // or maybe use "None", "Null", etc
          } else {
            paramObj["value"] = extraParameters[parameterName];
          }
          envVariables.append(paramObj);
        } else if (extraParameters[parameterName].isDouble()) {
          if constexpr (TAPISV3_PARAMETERS_ALLOW_NUMBER_CONVERSION) {
            paramObj["key"]   = parameterName;
            paramObj["value"] = QString::number(extraParameters[parameterName].toDouble());
            envVariables.append(paramObj);
          }
        } else if (extraParameters[parameterName].isBool()) {
          if constexpr (TAPISV3_PARAMETERS_ALLOW_BOOL_CONVERSION) {
            paramObj["key"]   = parameterName;
            paramObj["value"] = extraParameters[parameterName].toBool() ? "true" : "false";
            envVariables.append(paramObj);
          }
        } else if (extraParameters[parameterName].isArray()) {
            if constexpr (TAPISV3_PARAMETERS_ALLOW_ARRAY_CONVERSION) {
              // Not yet implemented, but convert the array to a single string with brackets and commas
            }
        } else if (extraParameters[parameterName].isObject()) {
          if constexpr (TAPISV3_PARAMETERS_ALLOW_OBJECT_CONVERSION) {
            // Not yet implemented, but enter the object and convert its contents to key-value pairs (recursively?)  
          }
        }
      } else {
        paramObj["key"]=parameterName;
        paramObj["value"]=extraParameters[parameterName];
        envVariables.append(paramObj);
      }
    }
    
    // Remove duplicate parameters from envVariables
    QSet<QString> keys;
    QJsonArray newEnvVariables;
    for (auto envVar : envVariables) {
      if (!envVar.isObject()) continue;
      if (!envVar.toObject().contains("key") || !envVar.toObject().contains("value")) continue;
      if (!envVar.toObject()["key"].isString()) continue;
      // if (!envVar.toObject()["value"].isString()) continue; // Only if all extra parameters are required to be strings 
      
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
    fileInputs.append(inputs);
    
    // for (auto extraInput : extraInputs.keys()) {
    //   QJsonObject data;
    //   data["sourceUrl"]   = "tapis://" + QString("designsafe.storage.default/") + extraInputs[extraInput];
    //   data["targetPath"]  = "*";
    //   data["envKey"]      = extraInput;
    //   fileInputs.append(data);
    // }

    // QJsonArray extraInputs;
    // if (extraParameters.contains("fileInputs")) {
    //   if (extraParameters["fileInputs"].isArray()) {
    //     extraInputs = extraParameters["fileInputs"].toArray();
    //     for (auto extraInput : extraInputs) {
    //       if (extraInput.isObject()) {
    //         if (extraInput.toObject().contains("envKey") && extraInput.toObject().contains("sourceUrl") && extraInput.toObject().contains("targetPath")) {
    //           // QString envKey = extraInput.toObject()["envKey"].toString();
    //           // QString sourceUrl = extraInput.toObject()["sourceUrl"].toString();
    //           // QString targetPath = extraInput.toObject()["targetPath"].toString();
    //           fileInputs.append(extraInput);
    //         }
    //       }
    //     }
    //   }
    // }

    // Currently, extraInputs supports just one extra fileInputs object as it is a QMap<QString, QString>
    // Can be vectorized, etc.,   if needed
    // QJsonObject extraInput;
    // if (extraInputs.contains("envKey")) {
    //   extraInput["envKey"]=extraInputs["envKey"];
    // }
    // if (extraInputs.contains("targetPath")) {
    //   extraInput["targetPath"]=extraInputs["targetPath"];
    // }
    // if (extraInputs.contains("sourceUrl")) {
    //   extraInput["sourceUrl"]=extraInputs["sourceUrl"];
    // }
    // fileInputs.append(extraInput);

    // // Refashion for use with qmap<qstring, qstring>     
    // for (auto extraInput : extraInputs.keys()) {
    //   QJsonObject data;
    //   data["sourceUrl"]   = "tapis://" + QString("designsafe.storage.default/") + extraInputs[extraInput];
    //   data["targetPath"]  = "*";
    //   data["envKey"]      = extraInput;
    //   fileInputs.append(data);
    // }

    // /* 
    // Temporary hardcoding for the MPM app
    QJsonObject data;
    if (tapisAppName == "simcenter-claymore-lonestar6") {       
      data["sourceUrl"]   = "tapis://" + QString("designsafe.storage.default/") + QString("bonusj/mpm-public-ls6");
      data["targetPath"]  = "*";
      data["envKey"]      = "dataDirectory";
      fileInputs.append(data);
    } else if (tapisAppName == "simcenter-claymore-frontera") {       
      data["sourceUrl"]   = "tapis://" + QString("designsafe.storage.default/") + QString("bonusj/mpm-public-frontera");
      data["targetPath"]  = "*";
      data["envKey"]      = "dataDirectory";
      fileInputs.append(data);
    }
    // */
    
    
    job["fileInputs"]=fileInputs;          

    //
    // now remove the tmp directory
    //
    
    theDirectory.removeRecursively();
    
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
  
    
  SC_ResultsWidget *theResults = theApp->getResultsWidget();
  if (theResults == NULL) {
    this->errorMessage("FATAL - SC_RemoteAppTool received NULL pointer theResults from theApp->getResultsWidget()... skipping theResults->processResults()");
    return;
  }

  QString blankFileName("scInput.json");
  theResults->processResults(blankFileName,dirName);

}

void SC_RemoteAppTool::setExtraInputs(QMap<QString, QString> extraInputs)
{
    this->extraInputs = extraInputs;
}

void SC_RemoteAppTool::setExtraParameters(QMap<QString, QString> extraParameters)
{
    this->extraParameters = extraParameters;
}

bool SC_RemoteAppTool::outputCitation(QJsonObject &jsonObject) {
  return theApp->outputCitation(jsonObject);
}

void
SC_RemoteAppTool::setFilesToDownload(QStringList filesToDownload, bool unzipZip) {
  theJobManager->setFilesToDownload(filesToDownload, unzipZip);
}

	     





