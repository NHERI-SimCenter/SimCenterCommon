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

//#include <AgaveInterface.h>
#include <QDebug>
#include <QDir>
#include <QUuid>
#include <QFileDialog>
#include <ZipUtils.h>
#include <QCoreApplication>

RemoteApplication::RemoteApplication(QString name, RemoteService *theService, QWidget *parent)
: Application(parent), theRemoteService(theService)
{
    workflowScriptName = name;
    shortDirName = QCoreApplication::applicationName() + QString(": ");

    //    shortDirName = workflowScriptName;
    //shortDirName = name.chopped(3); // remove .py
    //shortDirName.chop(3);

    QGridLayout *layout = new QGridLayout();
    QLabel *nameLabel = new QLabel();

    int numRow = 0;

    nameLabel->setText(QString("job Name:"));
    layout->addWidget(nameLabel, numRow,0);

    nameLineEdit = new QLineEdit();
    nameLineEdit->setToolTip(tr("A meaningful name to provide for you to remember run later (days and weeks from now)"));
    layout->addWidget(nameLineEdit,numRow,1);

    numRow++;
    QLabel *numCPU_Label = new QLabel();
    numCPU_Label->setText(QString("Num Nodes:"));

    layout->addWidget(numCPU_Label,numRow,0);

    numCPU_LineEdit = new QLineEdit();
    numCPU_LineEdit->setText("1");
    numCPU_LineEdit->setToolTip(tr("Total # of nodes to use (each node has many cores)"));
    layout->addWidget(numCPU_LineEdit,numRow,1);

    numRow++;
    QLabel *numProcessorsLabel = new QLabel();
    numProcessorsLabel->setText(QString("# Processes Per Node:"));

    layout->addWidget(numProcessorsLabel,numRow,0);

    numProcessorsLineEdit = new QLineEdit();
    numProcessorsLineEdit->setText("32");
        numProcessorsLineEdit->setToolTip(tr("Total # of Processes to Start"));
    layout->addWidget(numProcessorsLineEdit,numRow,1);


    QString appName = QCoreApplication::applicationName();
    if (appName == "R2D"){
        numRow++;
        layout->addWidget(new QLabel("# Buildings Per Task:"), numRow, 0);
        buildingsPerTask=new QLineEdit("10");
        buildingsPerTask->setToolTip("Number of buildings per task when runnig in parallel");
        layout->addWidget(buildingsPerTask, numRow, 1);

        numRow++;
        layout->addWidget(new QLabel("Save Inter. Results:"), numRow, 0);
        saveResultsBox=new QCheckBox(); saveResultsBox->setChecked(false);
        saveResultsBox->setToolTip("Save Intermediary results to compressed folder. You typically do not want this.");
        layout->addWidget(saveResultsBox, numRow, 1);
    }

    numRow++;
    QLabel *runtimeLabel = new QLabel();
    runtimeLabel->setText(QString("Max Run Time:"));
    layout->addWidget(runtimeLabel,numRow,0);

    runtimeLineEdit = new QLineEdit();
    runtimeLineEdit->setText("00:20:00");
    runtimeLineEdit->setToolTip(tr("Run time Limit on running Job hours:Min:Sec. Job will be stopped if while running it exceeds this"));
    layout->addWidget(runtimeLineEdit,numRow,1);

    numRow++;
    pushButton = new QPushButton();
    pushButton->setText("Submit");
    pushButton->setToolTip(tr("Press to launch job on remote machine. After pressing, window closes when Job Starts"));
    layout->addWidget(pushButton,numRow,1);

    this->setLayout(layout);

    //
    // set up connections
    //

    // on login from interface to set up homeDirPath
    //    connect(theService,SIGNAL(loginReturn(bool)),this,SLOT(attemptLoginReturn(bool)));
    connect(this,SIGNAL(getHomeDirCall()),theService,SLOT(getHomeDirPathCall()));
    connect(theService,SIGNAL(getHomeDirPathReturn(QString)), this, SLOT(getHomeDirReturned(QString)));

    // to start job need to connect uploadDir and start job
    connect(this,SIGNAL(uploadDirCall(const QString &,const QString &)), theService, SLOT(uploadDirectoryCall(const QString &,const QString &)));
    connect(theService, SIGNAL(uploadDirectoryReturn(bool)), this, SLOT(uploadDirReturn(bool)));
    connect(this,SIGNAL(startJobCall(QJsonObject)),theService,SLOT(startJobCall(QJsonObject)));
    connect(theService,SIGNAL(startJobReturn(QString)), this, SLOT(startJobReturn(QString)));

    connect(pushButton,SIGNAL(clicked()), this, SLOT(onRunButtonPressed()));
}

bool
RemoteApplication::outputToJSON(QJsonObject &jsonObject)
{
    QString appName = QCoreApplication::applicationName();

    if (appName != "R2D"){
        jsonObject["localAppDir"]=SimCenterPreferences::getInstance()->getAppDir();
        jsonObject["remoteAppWorkingDir"]=SimCenterPreferences::getInstance()->getRemoteAppDir();
        jsonObject["workingDir"]=SimCenterPreferences::getInstance()->getRemoteWorkDir();
    } else {
        jsonObject["remoteAppDir"]=SimCenterPreferences::getInstance()->getRemoteAppDir();
    }

    jsonObject["remoteAppDir"]=SimCenterPreferences::getInstance()->getRemoteAppDir();    
    jsonObject["runType"]=QString("runningRemote");

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
    if (!dirWork.exists())
        if (!dirWork.mkpath(workingDir)) {
            emit sendErrorMessage(QString("Could not create Working Dir: ") + workingDir + QString(" . Try using an existing directory or make sure you have permission to create the working directory."));
            return;
        }

    QString appDir = SimCenterPreferences::getInstance()->getAppDir();
    //   QString appDir = localAppDirName->text();
    QDir dirApp(appDir);
   if (!dirApp.exists()) {
       emit sendErrorMessage(QString("The application directory, ") + appDir +QString(" specified does not exist!"));
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
            return false;
        }

        QString registryFile = scriptDir.absoluteFilePath("WorkflowApplications.json");
        QFileInfo check_registry(registryFile);
        if (!check_registry.exists() || !check_registry.isFile()) {
            qDebug() << "NO REGISTRY FILE: " << registryFile;
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
            return false;
        }

        proc->execute(python,args);
        proc->waitForStarted();

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
            emit sendErrorMessage(("Local Failure Setting up Dakota"));
            qDebug() << "Local Failure Setting Up Dakota ";
            return false;
        }
        templateDir.cdUp();

        QString zipFile(templateDir.absoluteFilePath("templatedir.zip"));
        qDebug() << "ZIP FILE: " << zipFile;
        qDebug() << "DIR TO ZIP: " << templateDIR;
        QDir tmpDir(templateDIR);

        ZipUtils::ZipFolder(tmpDir, zipFile);
        //ZipUtils::ZipFolder(QDir(templateDIR), zipFile);

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
            qDebug() << "Remote App : no inpout_data dir";
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
        return false;
    }

    tempDirectory = theDirectory.absoluteFilePath(newName);

    theDirectory.cd(newName);
    QString dirName = theDirectory.dirName();
    
    QString remoteDirectory = remoteHomeDirPath + QString("/") + dirName;
    pushButton->setEnabled(false);

    emit uploadDirCall(tempDirectory, remoteHomeDirPath);

    return 0;
}

// this slot is invoked on return from uploadDirectory signal in pushButtonClicked slot

void
RemoteApplication::uploadDirReturn(bool result)
{
    if (result == true) {

      //
      // create the json needed to run the remote application
      //

      QJsonObject job;
      
      pushButton->setDisabled(true);
      
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
	
      job["appId"]=SimCenterPreferences::getInstance()->getRemoteAgaveApp();
      job["memoryPerNode"]= "1GB";
      job["archive"]=true;
      job["batchQueue"]=queue;      
      job["archivePath"]="";
      job["archiveSystem"]="designsafe.storage.default";
      

      QString appName = QCoreApplication::applicationName();
      if (appName != "R2D"){

          QJsonObject parameters;
          parameters["inputFile"]="scInput.json";
	  
	  if (appName == "quoFEM")
	    parameters["driverFile"]="driver";
	  else
	    parameters["driverFile"]="sc_driver";
	  
          parameters["modules"]="petsc,python3";
          for (auto parameterName : extraParameters.keys())
          {
              parameters[parameterName] = extraParameters[parameterName];
          }
          job["parameters"]=parameters;

          QDir theDirectory(tempDirectory);
          QString dirName = theDirectory.dirName();

          QString remoteDirectory = remoteHomeDirPath + QString("/") + dirName;

          QJsonObject inputs;
          inputs["inputDirectory"]=remoteDirectory;
          for (auto inputName : extraInputs.keys())
          {
              inputs[inputName] = extraInputs[inputName];
          }
          job["inputs"]=inputs;

          // now remove the tmp directory
          theDirectory.removeRecursively();

      } else {

          QDir theDirectory(tempDirectory);
          QString dirName = theDirectory.dirName();

          QString remoteDirectory = remoteHomeDirPath + QString("/") + dirName;
          QString configFile = remoteDirectory + "/inputRWHALE.json";
          QString inputData = remoteDirectory + "/input_data.zip";

          QJsonObject inputs;
          inputs["configFile"]=configFile;
          inputs["dataFile"]=inputData;
          job["inputs"]=inputs;

          QJsonObject parameters;


          int numBldg = buildingsPerTask->text().toInt();
          if (numBldg != 0 ) {
              parameters["buildingsPerTask"]=QString::number(numBldg);
              parameters["saveResults"]=saveResultsBox->isChecked();
          }
          job["parameters"]=parameters;

          // now remove the tmp directory
          theDirectory.removeRecursively();

      }

      // disable the button while the job is being uploaded and started
      pushButton->setEnabled(false);

      qDebug() << "JOBS_SUBMIT: " << job;

      qDebug() << "JOB: " << job;
      //
      // start the remote job
      //

      emit startJobCall(job);
      


    }
}

void
RemoteApplication::attemptLoginReturn(bool ok) {
    if (ok == true) {
        emit getHomeDirCall();
    }
}


void
RemoteApplication::getHomeDirReturned(QString path){
    remoteHomeDirPath = path;
}

void
RemoteApplication::startJobReturn(QString result) {
    Q_UNUSED(result);
   pushButton->setEnabled(true);
   emit successfullJobStart();
}

void
RemoteApplication::setNumTasks(int numTasks) {
    if (numTasks < 32)
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

