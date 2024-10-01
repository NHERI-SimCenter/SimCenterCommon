/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */


// Written: fmckenna

// Purpose: a widget for managing submiited jobs by WorkflowApp tool
//  - allow for refresh of status, deletion of submitted jobs, and download of results from finished job

#include "RemoteApplication.h"
#include <RemoteService.h>
#include <Utils/FileOperations.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QJsonObject>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QProcess>
#include <QSettings>
#include <SimCenterPreferences.h>
#include <QString>
//#include <AgaveInterface.h>
#include <QDebug>
#include <QDir>
#include <QUuid>
#include <QFileDialog>
#include <ZipUtils.h>
#include <QCoreApplication>
#include <QIntValidator>


// Support multiple remote HPC systems, but default to Frontera - 

static int maxProcPerNode = 56; // Frontera clx nodes: 56 cores, 56 threads, Frontera rtx nodes: 16 cores, 32 threads

// new function to be called before removeRecusivility


RemoteApplication::RemoteApplication(QString name, RemoteService *theService, QWidget *parent)
: Application(parent), theRemoteService(theService)
{


    QString appName = QCoreApplication::applicationName();
    
  
    workflowScriptName = name;
    shortDirName = appName + QString(": ");

    if (appName == "R2D" || appName == "quoFEM") {
      maxProcPerNode = 48;
    }
    //    shortDirName = workflowScriptName;
    //shortDirName = name.chopped(3); // remove .py
    //shortDirName.chop(3);

    QGridLayout *layout = new QGridLayout();
    QLabel *nameLabel = new QLabel();

    int numRow = 0;

    nameLabel->setText(QString("Job Name:"));
    layout->addWidget(nameLabel, numRow, 0);

    nameLineEdit = new QLineEdit();
    nameLineEdit->setToolTip(tr("A meaningful name to provide for you to remember run later (days and weeks from now)"));
    layout->addWidget(nameLineEdit, numRow, 1);

    numRow++;
    QLabel *numCPU_Label = new QLabel();
    numCPU_Label->setText(QString("Num Nodes:"));

    layout->addWidget(numCPU_Label,numRow,0);

    numCPU_LineEdit = new QLineEdit();
    QIntValidator* theValidatorNumC = new QIntValidator(1, 512);
    numCPU_LineEdit->setValidator(theValidatorNumC);
    
    numCPU_LineEdit->setText("1");
    numCPU_LineEdit->setToolTip(tr("Total # of nodes to use (each node has many cores)"));
    layout->addWidget(numCPU_LineEdit,numRow,1);

    numRow++;
    QLabel *numProcessorsLabel = new QLabel();
    numProcessorsLabel->setText(QString("# Cores Per Node:"));

    layout->addWidget(numProcessorsLabel,numRow,0);

    numProcessorsLineEdit = new QLineEdit();
    numProcessorsLineEdit->setText(QString::number(maxProcPerNode));
    numProcessorsLineEdit->setToolTip(tr("Total # of Processes to Start on each node"));
    layout->addWidget(numProcessorsLineEdit,numRow,1);

    // hate the validator
    //QIntValidator* theValidatorNumP = new QIntValidator(1, maxProcPerNode);
    //numProcessorsLineEdit->setValidator(theValidatorNumP);
    
    connect(numProcessorsLineEdit, &QLineEdit::textChanged, this, [=](QString newText) {
        bool ok;
        int numP = newText.toInt(&ok);
        if (ok == false) 
        {
	        numP = 1;
        }
        else if (ok && numP > maxProcPerNode) 
        {
	        ok = false;
	        numP = maxProcPerNode;
        } 
        else if (ok && numP < 1) 
        {
	        ok = false;
	        numP = 1;
        }
        
        if (ok == false) 
        {
	        numProcessorsLineEdit->setText(QString::number(numP));
        }
        
    });


    if (appName == "R2D") {
        numRow++;
        layout->addWidget(new QLabel("# Buildings Per Task:"), numRow, 0);
        buildingsPerTask=new QLineEdit("10");
        buildingsPerTask->setToolTip("Number of buildings per task when running in parallel");
        layout->addWidget(buildingsPerTask, numRow, 1);

        numRow++;
        layout->addWidget(new QLabel("Save Inter. Results:"), numRow, 0);
        saveResultsBox=new QCheckBox(); saveResultsBox->setChecked(false);
        saveResultsBox->setToolTip("Save Intermediary results to compressed folder. You typically do not want this.");
        layout->addWidget(saveResultsBox, numRow, 1);
    }

    numRow++;
    QLabel *runtimeLabel = new QLabel();
    runtimeLabel->setText(QString("Max Run Time (minutes):"));
    layout->addWidget(runtimeLabel,numRow,0);

    runtimeLineEdit = new QLineEdit();
    runtimeLineEdit->setText("30");
    runtimeLineEdit->setToolTip(tr("Run time Limit on running Job hours:Min. Job will be stopped if while running it exceeds this"));
    int maxMinutes = 60*48; // WARNING: Some Stampede3 queues only allow 60*24 minute run-times
    QIntValidator* theValidatorMinutes = new QIntValidator(1, maxMinutes);
    runtimeLineEdit->setValidator(theValidatorMinutes);
    runtimeLineEdit->setToolTip(tr("Run time Limit on running Job hours:Min. Job will be stopped if while running it exceeds this"));        
    layout->addWidget(runtimeLineEdit,numRow,1);

    numRow++;
    layout->addWidget(new QLabel("TACC Allocation"), numRow, 0);
    allocation = new QLineEdit();
    allocation->setText(SimCenterPreferences::getInstance()->getDefaultAllocation());
    layout->addWidget(allocation,numRow,1);

    numRow++;
    pushButton = new QPushButton();
    pushButton->setText("Submit");
    pushButton->setToolTip(tr("Press Submit to launch your job on a remote computer system. This window will close once the job starts."));
    layout->addWidget(pushButton,numRow,1);

    this->setLayout(layout);

    connect(pushButton,SIGNAL(clicked()), this, SLOT(onRunButtonPressed()));
}

bool
RemoteApplication::outputToJSON(QJsonObject &jsonObject)
{
    QString appName = QCoreApplication::applicationName();

    if (appName != "R2D"){
        jsonObject["localAppDir"]=SimCenterPreferences::getInstance()->getAppDir();
        jsonObject["workingDir"]=SimCenterPreferences::getInstance()->getRemoteWorkDir();
    } else {
        jsonObject["localAppDir"]=SimCenterPreferences::getInstance()->getRemoteAppDir();	
    }

    jsonObject["remoteAppDir"]=SimCenterPreferences::getInstance()->getRemoteAppDir();    
    jsonObject["runType"]=QString("runningRemote");
    int nodeCount = numCPU_LineEdit->text().toInt();
    int numProcessorsPerNode = numProcessorsLineEdit->text().toInt();
    jsonObject["nodeCount"]=nodeCount;
    jsonObject["numP"]=nodeCount*numProcessorsPerNode;    
    jsonObject["coresPerNode"]=numProcessorsPerNode;    

    return true;
}

bool
RemoteApplication::inputFromJSON(QJsonObject &dataObject) {

    Q_UNUSED(dataObject);
    return true;
}


void
RemoteApplication::onRunButtonPressed(void)
{
    QString workingDir = SimCenterPreferences::getInstance()->getRemoteWorkDir();

    QDir dirWork(workingDir);
    if (!dirWork.exists()) {
        if (!dirWork.mkpath(workingDir)) {
            emit sendErrorMessage(QString("Could not create Working Dir: ") + workingDir + QString(" . Try using an existing directory or make sure you have permission to create the working directory."));
            pushButton->setEnabled(true);
            return;
        }
    }
    QString appDir = SimCenterPreferences::getInstance()->getAppDir();
    //   QString appDir = localAppDirName->text();
    QDir dirApp(appDir);
    if (!dirApp.exists()) {
      emit sendErrorMessage(QString("The application directory, ") + appDir + QString(" specified does not exist!"));
      pushButton->setEnabled(true);
      return;
    }

    QString templateDir("templatedir");

    emit setupForRun(workingDir, templateDir);
}


//
// now use the applications Workflow Application script to run dakota and produce output files:
//    dakota.in dakota.out dakotaTab.out dakota.err
//

bool
RemoteApplication::setupDoneRunApplication(QString &tmpDirectory, QString &inputFile) {

  //Q_UNUSED(runType);
  //    QString appDir = localAppDirName->text();
  QString runType("runningRemote");
  
  QString appDir = SimCenterPreferences::getInstance()->getAppDir();
  QString pySCRIPT;
  
  QString appName = QCoreApplication::applicationName();
  
  // R2D does not have a local setup run
  
  if (appName != "R2D"){
    
        QDir scriptDir(appDir);
        scriptDir.cd("applications");
        scriptDir.cd("Workflow");
        pySCRIPT = scriptDir.absoluteFilePath(workflowScriptName);
        QFileInfo check_script(pySCRIPT);
        // check if file exists and if yes: Is it really a file and no directory?
        if (!check_script.exists() || !check_script.isFile()) {
            qDebug() << "NO SCRIPT FILE: " << pySCRIPT;
            pushButton->setEnabled(true);
            return false;
        }

        QString registryFile = scriptDir.absoluteFilePath("WorkflowApplications.json");
        QFileInfo check_registry(registryFile);
        if (!check_registry.exists() || !check_registry.isFile()) {
            qDebug() << "NO REGISTRY FILE: " << registryFile;
            pushButton->setEnabled(true);
            return false;
        }

        QStringList files;
        files << "dakota.in" << "dakota.out" << "dakotaTab.out" << "dakota.err";

        /************************************************************************
        for (int i = 0; i < files.size(); i++) {
           QString copy = files.at(i);
           QFile file(destinationDir + copy);
           file.remove();
        }
        ***********************************************************************/

        //
        // now get inputs ready
        //

        QProcess *proc = new QProcess();
        QStringList args{pySCRIPT, runType, inputFile, registryFile};
        //    proc->execute("python",args);
        SimCenterPreferences *preferences = SimCenterPreferences::getInstance();
        QString python = preferences->getPython();

        QFileInfo pythonFile(python);
        if (!pythonFile.exists()) {
            emit sendErrorMessage("NO VALID PYTHON - Read the Manual & Check your Preferences");
            pushButton->setEnabled(true);
            return false;
        }

	    qDebug() << "RUNNING: " << python << " " << args;
	
        proc->execute(python,args);
        proc->waitForStarted();

        // qDebug() << python;
        // qDebug() << args;

        //
        // in tmpDirectory we will zip up current template dir and then remove before sending (doone to reduce number of sends)
        //

        QDir templateDir(tmpDirectory);
        templateDir.cd("templatedir");
        QString templateDIR = templateDir.absolutePath();

#ifdef Q_OS_WIN
        templateDir.rename("driver.bat","driver");
#endif

        QFileInfo check_workflow(templateDir.absoluteFilePath("driver"));
        if (!check_workflow.exists() || !check_workflow.isFile()) {
            emit sendErrorMessage(("No Driver file exists, local failure in setting up driver or problem not HPC ready"));
            qDebug() << "Local Failure: No Driver file found, Either Problem will not run on HPC or Setup failure";
            pushButton->setEnabled(true);
            return false;
        }
        templateDir.cdUp();

        QString zipFile(templateDir.absoluteFilePath("tmpSimCenter.zip"));
        qDebug() << "ZIP FILE: " << zipFile;
        qDebug() << "DIR TO ZIP: " << tmpDirectory;
        QDir tmpDir(tmpDirectory);

	
        ZipUtils::ZipFolder(tmpDir, zipFile);

	//
	// remove input_data & templatedir directories before we send tmp directory across (they are now in zip file)
	//
	
        if (tmpDir.exists("input_data")) {
            QDir inputDataDir(tmpDir.absoluteFilePath("input_data"));
            inputDataDir.removeRecursively();
	} 

        QDir dirToRemove(templateDIR);
        templateDir.cd("templatedir");
	templateDir.removeRecursively();
	
    } else {

        // zip up data_dir
        QDir tmpDir(tmpDirectory);
        if (tmpDir.exists("input_data")) {
            QString zipFile = tmpDir.absoluteFilePath("input_data.zip");
            QDir inputDataDir(tmpDir.absoluteFilePath("input_data"));
            ZipUtils::ZipFolder(inputDataDir, zipFile);
            inputDataDir.removeRecursively();
        } else {
            qDebug() << "Remote App : no input_data dir";
        }
    }
  
    //
    // now upload files to remote local
    //

    // rename dir so unique at DesignSafe
    QUuid uniqueName = QUuid::createUuid();
    QString strUnique = uniqueName.toString();
    strUnique = strUnique.mid(1,36);
    QString newName = QString("tmp.SimCenter") + strUnique;

    QDir theDirectory(tmpDirectory);
    theDirectory.cdUp();
    if (theDirectory.rename("tmp.SimCenter",newName) != true) {
        emit sendErrorMessage(QString("Could not rename directory to ") + newName);
        pushButton->setEnabled(true);
        return false;
    }

    tempDirectory = theDirectory.absoluteFilePath(newName);

    theDirectory.cd(newName);
    QString dirName = theDirectory.dirName();

    remoteHomeDirPath = theRemoteService->getHomeDir();
    
    if (remoteHomeDirPath.isEmpty()) {
      qDebug() << "RemoteApplication:: - remoteHomeDir is empty!!";      
      pushButton->setEnabled(true);
      return -1;
    }
    QString remoteDirectory = remoteHomeDirPath + QString("/") + dirName;
    designsafeDirectory = remoteDirectory;    
    pushButton->setEnabled(false);

    //connect(this,SIGNAL(uploadDirCall(const QString &,const QString &)), theRemoteService, SLOT(uploadDirectoryCall(const QString &,const QString &)));
    connect(theRemoteService, SIGNAL(uploadDirectoryReturn(bool)), this, SLOT(uploadDirReturn(bool)));
    theRemoteService->uploadDirectoryCall(tempDirectory, remoteHomeDirPath);        

    return 0;
}

// this slot is invoked on return from uploadDirectory signal in pushButtonClicked slot

void
RemoteApplication::uploadDirReturn(bool result)
{
    disconnect(theRemoteService, SIGNAL(uploadDirectoryReturn(bool)), this, SLOT(uploadDirReturn(bool)));  
    
    if (result == true) {

      //
      // create the json needed to run the remote application
      //

      QJsonObject job;
      
      pushButton->setDisabled(true);
      
      job["name"]=shortDirName + nameLineEdit->text();
      int nodeCount = numCPU_LineEdit->text().toInt();
      int numProcessorsPerNode = numProcessorsLineEdit->text().toInt();
      int ramPerNodeMB = 1000; // 1 GB
      job["nodeCount"]=nodeCount;
      job["coresPerNode"]=numProcessorsPerNode;
      job["memoryMB"]=ramPerNodeMB;
      job["maxMinutes"]=runtimeLineEdit->text().toInt();



      // --- CPU queues on Frontera, TODO: implement for frontera/stampede3/lonestar6 + GPU queues
      QString queue = "small";
      if (nodeCount > 2) {
        queue = "normal";
      } else if (nodeCount > 512 && nodeCount <= 2048) {
        queue = "large";
      } else {
        // Only applicable to Texa-scale days
      }
      // ---

      QString appName = QCoreApplication::applicationName(); 

      if ((appName == QString("R2D")) || (appName == QString("quoFEM")) || (appName == QString("quoFEM_TEST")) ) 
      {
        queue = "skx"; // Stampede3 Skylake node queue, upto 60*24 minutes run-time
      }

    
      if ((appName == QString("HydroUQ")) || (appName == QString("Hydro-UQ")) || (appName == QString("HydroUQ_TEST"))) 
      {
            // NVIDIA GPU queues currently HydroUQ's default, as they include CPUs as well
            int numProcessorsPerNodeInGpuQueu = numProcessorsPerNode;
            int nodeCountInGpuQueue = nodeCount;
            const bool USE_FRONTERA_GPU = true;
            const bool USE_LONESTAR6_GPU = false;
            if constexpr (USE_FRONTERA_GPU) 
            {
                queue = "rtx"; // Frontera. 4 NVIDIA Quadro RTX 5000 GPU 16GB
                numProcessorsPerNodeInGpuQueu = 16; // 2 Intel Xeon E5-2620 v4 (“Broadwell”), 2*8 cores, or 2*8*2 threads (may not be enabled on Frontera)
                ramPerNodeMB = 128000; // 128 GB
                nodeCountInGpuQueue = (nodeCount < 22) ? nodeCount : 22; // Frontera, 22 nodes per job on rtx queue
            } 
            else if constexpr (USE_LONESTAR6_GPU) 
            {
                queue = "gpu-a100"; // Lonestar6, 3 NVIDIA A100 GPU 80 GB
                numProcessorsPerNodeInGpuQueu = 128; // 2x AMD EPYC 7763 64-Core Processor ("Milan"), 2*64 cores, or 2*64*1 threads
                ramPerNodeMB = 256000; // 256 GB
                nodeCountInGpuQueue = (nodeCount < 4) ? nodeCount : 4; // Lonestar6, 4 nodes per job on gpu-a100 queue
            }
            job["coresPerNode"] = numProcessorsPerNodeInGpuQueu;
            job["nodeCount"] = nodeCountInGpuQueue;
      }

      job["memoryMB"]               = ramPerNodeMB;
      job["execSystemLogicalQueue"] = queue;


      // Grab the tapis App information from the preferences dialog
      // To get the true Tapis App name, the version is appeneded to the app id, e.g. "simcenter-openfoam-frontera-1.0.0"
      // TODO : split the <machine> field from the tapisId, so thats its more easily swapped out and updated at run-time

      // Format for Tapis Apps (as of Aug 31, 2024): "<affiliation>-<exectutable>-<machine>""
      // Affiliation is by default "simcenter" for all our simcenter public apps
      // The executable is the name of the primary workflow / program, e.g. "uq", "claymore", "openfoam" / "weuq-openfoam"
      // The machine is the name of the HPC system, e.g. "frontera", "stampede3, "lonestar6" / "ls6".
      QString tapisAppId      = SimCenterPreferences::getInstance()->getRemoteAgaveApp();
      
      // The tapis version, <X.Y.Z>, is the iteration of the public app, e.g. "1.0.0". 
      // DOUBLE-CHECK: [Tapisv3 removes the "uX" field, (i.e. 1.0.0u2), so ignore prior Tapisv2 versioning.]
      QString tapisAppVersion = SimCenterPreferences::getInstance()->getRemoteAgaveAppVersion(); 
      job["appId"]            = tapisAppId;
      job["appVersion"]       = tapisAppVersion;      
      


      QJsonObject parameterSet;
      QJsonArray envVariables;


      //
      // app specific env variables go here
      //
      
      
      if (appName != "R2D") {

        QJsonObject inputFile;
        inputFile["key"]="inputFile";
        inputFile["value"]="scInput.json";
        envVariables.append(inputFile);

        if (appName == "quoFEM") {
            QJsonObject driverFile;
            driverFile["key"]="driverFile";
            driverFile["value"]="driver";
            envVariables.append(driverFile);
        } else {
            QJsonObject driverFile;
            driverFile["key"]="driverFile";
            driverFile["value"]="sc_driver";
            envVariables.append(driverFile);
        }
            
        for (auto parameterName : extraParameters.keys()) {

            QJsonObject paramObj;
            paramObj["key"]=parameterName;
            paramObj["value"]=extraParameters[parameterName];
            envVariables.append(paramObj);
        
        }

      } else { // R2D env variables

        QJsonObject inputFileObj;
        inputFileObj["key"]="inputFile";
        inputFileObj["value"]="inputRWHALE.json";	  
        envVariables.append(inputFileObj);
        QJsonObject inputDataObj;
        inputDataObj["key"]="inputDir";
        inputDataObj["value"]="input_data";	  
        envVariables.append(inputDataObj);	  
	  
      }

      //
      // add allocation, env variables, and inputDir info to the job
      //
      
      QJsonArray schedulerOptions;
      QJsonObject schedulerOptionsObj;
      QString allocationText = QString("-A " ) + allocation->text();
      schedulerOptionsObj["arg"] = allocationText;
      
      schedulerOptions.append(schedulerOptionsObj);
      parameterSet["schedulerOptions"]=schedulerOptions;
      parameterSet["envVariables"]=envVariables;
      job["parameterSet"]=parameterSet;
      
      QJsonArray fileInputs;
      QJsonObject inputs;
      inputs["envKey"]="inputDirectory";
      inputs["targetPath"]="*";
      inputs["sourceUrl"] = "tapis://" + designsafeDirectory;
      designsafeDirectory = "";
      for (auto inputName : extraInputs.keys())
      {
	    inputs[inputName] = extraInputs[inputName];
      }
      fileInputs.append(inputs);
      job["fileInputs"]=fileInputs;
      
      // disable the button while the job is being uploaded and started
      pushButton->setEnabled(false);


      //
      // now remove the tmp directory
      //
      
      QDir theDirectory(tempDirectory);

      //QString dirName = theDirectory.dirName();
      //QString remoteDirectory = remoteHomeDirPath + QString("/") + dirName;

      if (SCUtils::isSafeToRemoveRecursivily(tempDirectory))
	theDirectory.removeRecursively();
      
      //
      // start the remote job
      //


      // connect(this,SIGNAL(startJobCall(QJsonObject)),theRemoteService,SLOT(startJobCall(QJsonObject)));
      connect(theRemoteService,SIGNAL(startJobReturn(QString)), this, SLOT(startJobReturn(QString)));      
      theRemoteService->startJobCall(job);
    }
}

void
RemoteApplication::attemptLoginReturn(bool ok) {
  
  if (ok == true) {
    connect(theRemoteService,SIGNAL(getHomeDirPathReturn(QString)), this, SLOT(getHomeDirReturned(QString)));    
    theRemoteService->getHomeDirPathCall();
  }
  
}


void
RemoteApplication::getHomeDirReturned(QString path){
    remoteHomeDirPath = path;
    disconnect(theRemoteService,SIGNAL(getHomeDirPathReturn(QString)), this, SLOT(getHomeDirReturned(QString)));      
}

void
RemoteApplication::startJobReturn(QString result) {
   disconnect(theRemoteService,SIGNAL(startJobReturn(QString)), nullptr, nullptr);      
   Q_UNUSED(result);
   pushButton->setEnabled(true);
   emit successfullJobStart();
}

void
RemoteApplication::setNumTasks(int numTasks) {
    if (numTasks < maxProcPerNode)
        numProcessorsLineEdit->setText(QString::number(numTasks));
}

void RemoteApplication::setExtraInputs(QMap<QString, QString> extraInputs)
{
    this->extraInputs = extraInputs;
}

void RemoteApplication::setExtraParameters(QMap<QString, QString> extraParameters)
{
    this->extraParameters = extraParameters;
}

void RemoteApplication::clearExtraInputs()
{
    this->extraInputs.clear();
}

void RemoteApplication::clearExtraParameters()
{
    this->extraParameters.clear();
}

