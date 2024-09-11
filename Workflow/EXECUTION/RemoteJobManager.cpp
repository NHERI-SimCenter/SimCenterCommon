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

// Purpose: a widget for managing submiited jobs by uqFEM tool
//  - allow for refresh of status, deletion of submitted jobs, and download of results from finished job


#include "RemoteJobManager.h"
#include <SimCenterPreferences.h>
#include <ZipUtils/ZipUtils.h>
#include <RemoteService.h>

#include <QJsonDocument>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QJsonArray>
#include <QTableWidget>
#include <QTemporaryFile>
#include <QHeaderView>
#include <QRect>
#include <QApplication>
#include <QCoreApplication>
//#include <QDesktopWidget>
#include <QScreen>
#include <QSettings>
#include <QLabel>
#include <QRegularExpression>
#include <QMenu>
#include <QDir>
#include  <QDebug>

class RemoteService;

RemoteJobManager::RemoteJobManager(RemoteService *theRemoteService, QWidget *parent)
  : QWidget(parent), triggeredRow(-1), theService(theRemoteService), callProcessResultsOnApp(true)
{
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *label = new QLabel("* Click in any job shown in table below to update the job status, download the job or delete the job. Deleting a job removes it and all files created for it.");
    
    layout->addWidget(label);
    
    jobsTable = 0;
    htmlInputDirectory = QString("agave://designsafe.storage.default/");
    headers << "Name" << "STATUS" << "ID" << "Date Created" << "Remote Started";
    jobsTable=new QTableWidget(this);
    jobsTable->setColumnCount(headers.size());
    jobsTable->setHorizontalHeaderLabels(headers);
    jobsTable->setRowCount(0);

    QHeaderView* header = jobsTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    layout->addWidget(jobsTable, 1.0);
    //jobsTable->setSizePolicy(QSizePolicy::Ignored);
    this->setLayout(layout);
    QRect rec = QGuiApplication::primaryScreen()->geometry();

    int height = 0.5*rec.height();
    int width = 0.5*rec.width();

    this->resize(width, height);

    this->setWindowFlags(Qt::WindowStaysOnTopHint);

    //jobsTable->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    this->setMinimumHeight(200);
    this->setMinimumWidth(200);
    connect(jobsTable, SIGNAL(cellClicked(int,int)),this,SLOT(bringUpJobActionMenu(int,int)));
    connect(jobsTable, SIGNAL(cellPressed(int,int)),this,SLOT(bringUpJobActionMenu(int,int)));


    // The python process for extracting the hdf5 file
    proc = new QProcess(this);
    connect(proc, &QProcess::readyReadStandardOutput, this, &RemoteJobManager::handleProcessTextOutput);
}

void
RemoteJobManager::clearTable(void){
    jobsTable->setRowCount(0);
}

void
RemoteJobManager::setFilesToDownload(QStringList fileList){
  filesToDownload = fileList;
}

void
RemoteJobManager::updateJobTable(QString appName){

  // update jobs list
  connect(theService,SIGNAL(getJobListReturn(QJsonObject)), this,SLOT(jobsListReturn(QJsonObject)));  
  theService->getJobListCall(appName);
  
  // emit getJobsList(appName);
  // jobs = theInterface->getJobList(QString(""));
}

void
RemoteJobManager::jobsListReturn(QJsonObject theJobs){
  
    disconnect(theService, SIGNAL(uploadDirectoryReturn(bool)), this, SLOT(uploadDirReturn(bool)));  
    jobs = theJobs;

    jobsTable->setRowCount(0);    
    if (jobs.contains("jobs")) {
      
        QJsonArray jobData=jobs["jobs"].toArray();
        int numJobs = jobData.count();
        // jobsTable->setRowCount(numJobs);

	/* *************** INFO PER JOB ******************
            {
            "appId": "simcenter-uq-stampede3",
            "appVersion": "1.0.0",
            "archiveSystemId": "stampede3",
            "condition": null,
            "created": "2024-05-29T16:33:33.782933Z",
            "ended": "2024-05-29T16:33:49.070294Z",
            "execSystemId": "stampede3",
            "lastUpdated": "2024-05-29T16:33:49.070294Z",
            "name": "quoFEM: tapisv3-quofem",
            "owner": "tg457427",
            "remoteStarted": null,
            "status": "FAILED",
            "tenant": "designsafe",
            "uuid": "dbe0230a-ab4b-4bba-b259-76037a9b24e9-007"
        }
	*******************************************************/

	QString appName = QCoreApplication::applicationName();
	int count = 0;
        for (int i=0; i<numJobs; i++) {
	  
            QJsonObject job=jobData.at(i).toObject();
            QString jobName = job["name"].toString();

	    if (jobName.contains(appName)) {

	      jobsTable->insertRow(count);    	      
	      QString jobID = job["uuid"].toString();
	      QString jobStatus = job["status"].toString();
	      QString jobDate = job["created"].toString();
	      QString remoteStarted = job["remoteStarted"].toString();
	      //QString lastUpdated = job["lastUpdated"].toString();
	      
	      
	      jobsTable->setItem(count, 0, new QTableWidgetItem(jobName));
	      jobsTable->setItem(count, 1, new QTableWidgetItem(jobStatus));
	      jobsTable->setItem(count, 2, new QTableWidgetItem(jobID));
	      jobsTable->setItem(count, 3, new QTableWidgetItem(jobDate));
	      
	      //Added by Abiy
	      jobsTable->setItem(count, 4, new QTableWidgetItem(remoteStarted));
	      //            jobsTable->setItem(i, 4, new QTableWidgetItem(QString::number(processorsPerNode.toInt()*nodes.toInt())));
	      //            jobsTable->setItem(i, 5, new QTableWidgetItem(maxHour));
//            jobsTable->setItem(i, 6, new QTableWidgetItem(maxHour));
	      
//            getJobRunTime(remoteStarted, lastUpdated);

	      count++;
	    }
	    
        }
    }
     //jobsTable->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
     this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

     QString msg1("Job status does not refresh automatically");
     emit sendStatusMessage(msg1);
     QString msg2("Select a job from table and use left mouse button to see options for that job");
     emit sendStatusMessage(msg2);

   // jobsTable->resizeRowsToContents();
   // jobsTable->setSizePolicy(QSizePolicy::Ignored);
    return;
}


void
RemoteJobManager::bringUpJobActionMenu(int row, int col){
    Q_UNUSED(col);

    triggeredRow = row;
    QMenu jobMenu;

    jobMenu.addAction("Refresh Job", this, SLOT(updateJobStatus()));
    jobMenu.addAction("Retrieve Data", this, SLOT(getJobData()));
    jobMenu.addSeparator();
    jobMenu.addAction("Delete Job", this, SLOT(deleteJob()));
    //    jobMenu.addAction("Delete Job And Data", this, SLOT(deleteJobAndData()));

    jobMenu.exec(QCursor::pos());
}


void
RemoteJobManager::updateJobStatus(void)
{
    if (triggeredRow != -1) {
        QTableWidgetItem *itemID = jobsTable->item(triggeredRow,2);
        QString jobID = itemID->text();
	//    connect(this,SIGNAL(getJobStatus(QString)),theService,SLOT(getJobStatusCall(QString)));
	connect(theService,SIGNAL(getJobStatusReturn(QString)),this,SLOT(jobStatusReturn(QString)));
	theService->getJobStatusCall(jobID);
        // emit getJobStatus(jobID);
    }
}

void
RemoteJobManager::jobStatusReturn(QString status) {
  
    disconnect(theService,SIGNAL(getJobStatusReturn(QString)),this,SLOT(jobStatusReturn(QString)));  
    if (triggeredRow != -1) {
        QTableWidgetItem *itemStatus=jobsTable->item(triggeredRow,1);
        itemStatus->setText(status);
    }
    triggeredRow = -1;
}


void
RemoteJobManager::deleteJob(void){

    if (triggeredRow != -1) {
        QStringList noDirToRemove;
        QTableWidgetItem *itemID=jobsTable->item(triggeredRow,2);
        QString jobID = itemID->text();
//        bool result = theInterface->deleteJob(jobID);
	// delete job
	//connect(this,SIGNAL(deleteJob(QString,QStringList)),theService,SLOT(deleteJobCall(QString,QStringList)));
	connect(theService,SIGNAL(deleteJobReturn(bool)), this,SLOT(deleteJobReturn(bool)));
	theService->deleteJobCall(jobID, noDirToRemove);
        //emit deleteJob(jobID, noDirToRemove);
    }
}

void
RemoteJobManager::deleteJobReturn(bool result) {
  disconnect(theService,SIGNAL(deleteJobReturn(bool)), this,SLOT(deleteJobReturn(bool)));  
  if (result == true)
    jobsTable->removeRow(triggeredRow);
  triggeredRow = -1;
}


void
RemoteJobManager::deleteJobAndData(void){

    if (triggeredRow != -1) {

        //
        // get jobID, from interface obtain information about where the input and archived data is located
        // & then remove the directories
        //   NOTE SHOULD probably check job status not RUNNING befre doing this
        //

        QTableWidgetItem *itemID=jobsTable->item(triggeredRow,2);

        jobIDRequest = itemID->text();
        getJobDetailsRequest = 1;
	// getJobDetails
	// connect(this,SIGNAL(getJobDetails(QString)), theService,SLOT(getJobDetailsCall(QString)));
	connect(theService,SIGNAL(getJobDetailsReturn(QJsonObject)),this,SLOT(getJobDetailsReturn(QJsonObject)));
	theService->getJobDetailsCall(jobIDRequest);
	
	// emit getJobDetails(jobIDRequest);
    }
}

void
RemoteJobManager::getJobDetailsReturn(QJsonObject job)  {

    disconnect(theService,SIGNAL(getJobDetailsReturn(QJsonObject)),this,SLOT(getJobDetailsReturn(QJsonObject)));

    //    qDebug() << "RemoteJobManager::getJobDetails job: " << job;
    
    if (getJobDetailsRequest == 1) {

        //
        // the request was from deleteJobAndData
        //   - request the deletion of archive and input directories along with the job
        //
      
        QStringList dirToRemove;
        QJsonValue archivePath = job["archivePath"];
        if (archivePath.isString()) {
            QString archiveDir = archivePath.toString();
            dirToRemove << archiveDir;
        }
        QJsonValue inputs = job["inputs"];
	
        if (inputs.isObject()) {

            QJsonObject inputObject = inputs.toObject();
            QJsonValue inputPath = inputObject["inputDirectory"];
            if (inputPath.isArray()) {
                QString inputDir = inputPath.toArray().at(0).toString();
                inputDir.remove(htmlInputDirectory);
                dirToRemove << inputDir;
            } else if (inputPath.isString()) {
                QString inputDir = inputPath.toString();
                inputDir.remove(htmlInputDirectory);
                dirToRemove << inputDir;
	    }
        }
	
	connect(theService,SIGNAL(deleteJobReturn(bool)), this,SLOT(deleteJobReturn(bool)));
	theService->deleteJobCall(jobIDRequest, dirToRemove);	
        // emit deleteJob(jobIDRequest, dirToRemove);
    }

    if (getJobDetailsRequest == 2) {

         //
         // the request was a getJobData
         //    we have to download the files & then process them
         //    note: the processing done after files have downloaded
         //

         QString archiveDir;
         QString inputDir;
         QJsonValue archivePath = job["archiveSystemDir"];
         if (archivePath.isString()) {
             archiveDir = archivePath.toString();
         }
         QJsonValue inputs = job["inputs"];
         if (inputs.isObject()) {

             QJsonObject inputObject = inputs.toObject();
             QJsonValue inputPath = inputObject["inputDirectory"];
             if (inputPath.isArray()) {
                 inputDir = inputPath.toArray().at(0).toString();
                 inputDir.remove(htmlInputDirectory);
             } else if (inputPath.isString()) {
                 inputDir = inputPath.toString();
                 inputDir.remove(htmlInputDirectory);
             }
         }
	 
        QString localDir = SimCenterPreferences::getInstance()->getRemoteWorkDir();
        QDir localWork(localDir);
        localWork.removeRecursively();
        if (!localWork.exists()) {
            if (!localWork.mkpath(localDir)) {
                emit sendErrorMessage(QString("Could not create Working Dir: ") + localDir + QString(" . Try using an existing directory or make sure you have permission to create the working directory."));
                return;
            }
        }

        QStringList localFiles;
        QStringList remoteFiles;
        QString appName = QCoreApplication::applicationName();
	
	if (filesToDownload.size() == 0) {
	  
	  if (appName != "R2D"){
	    
	    name1 = localDir + QDir::separator() + QString("templatedir.zip");
	    name2 = localDir + QDir::separator() + QString("results.zip");
	    name3 = localDir;	  
	    
	    localFiles.append(name1);
	    localFiles.append(name2);
	    
	    //
	    // download data to temp files & then process them as normal
	    //
	    
	    archiveDir = archiveDir + QString("/") + inputDir.remove(QRegularExpression(".*\\/")); // regex to remove up till last /

	    QString inputJSON = archiveDir + QString("templatedir.zip");
	    QString resultsZIP = archiveDir + QString("results.zip");
	    
	    remoteFiles.append(inputJSON);
	    remoteFiles.append(resultsZIP);

	  } else {
	    
            name1 = localDir + QDir::separator() + QString("inputRWHALE.json");
            name2 = localDir + QDir::separator() + QString("input_data.zip");
            name3 = localDir + QDir::separator() + QString("Results.zip");
	    
            localFiles.append(name1);
            localFiles.append(name2);
            localFiles.append(name3);
	    
            //
            // download data to temp files & then process them as normal
            //
	    
            archiveDir = archiveDir + QString("/") + inputDir.remove(QRegularExpression(".*\\/")); // regex to remove up till last /

            QString rName1 = archiveDir + QString("inputRWHALE.json");
            QString rName2 = archiveDir + QString("input_data.zip");
            QString rName3 = archiveDir + QString("Results.zip");
	    
            // first download the input data & load it
	    
            remoteFiles.append(rName1);
            remoteFiles.append(rName2);
            remoteFiles.append(rName3);

	  }
	  
	} else {

	  archiveDir = archiveDir + QString("/") + inputDir.remove(QRegularExpression(".*\\/")); // regex to remove up till last /

	  for (int i=0; i<filesToDownload.size(); i++) {
	    QString name = filesToDownload.at(i);
	    QString localFilePath = localDir + QDir::separator() + name;
	    QString remoteFilePath = archiveDir + QString("/") + name;
	    localFiles.append(localFilePath);
	    remoteFiles.append(remoteFilePath);
	  }
	}
	
	connect(theService,SIGNAL(downloadFilesReturn(bool, QObject*)),this,SLOT(downloadFilesReturn(bool, QObject*)));
	theService->downloadFilesCall(remoteFiles, localFiles, this);    
	
    }
}

void
RemoteJobManager::downloadFilesReturn(bool result, QObject* sender)
{
    //
    // this method called only during the loading of a remote job
    //    called as a result of method above which emitted a downloadFIles(),
    //    which itself was a result of the getJobData methid and it's emit getJobDetails signal
    //

    if (sender == this && callProcessResultsOnApp == true) {
      
      disconnect(theService,SIGNAL(downloadFilesReturn(bool, QObject*)),this,SLOT(downloadFilesReturn(bool, QObject*)));
      
      QString localDir = SimCenterPreferences::getInstance()->getRemoteWorkDir();
      QDir localWork(localDir);
      
      if (!localWork.exists())
	if (!localWork.mkpath(localDir)) {
	  emit sendErrorMessage(QString("Could not create Working Dir: ") + localDir + QString(" . Try using an existing directory or make sure you have permission to create the working directory."));
	  return;
	}
      
      if (result == true) {
	QString appName = QCoreApplication::applicationName();

	if (filesToDownload.size() == 0) {
	
	  if (appName != "R2D"){
	    
	    QString templateDir = name3 + QDir::separator() + QString("templatedir");
	    QDir templateD(templateDir);
	    if (templateD.exists())
	      templateD.removeRecursively();
	    
	    // unzip .. this places files in a new dir templatedir
	    ZipUtils::UnzipFile(name1, QDir(name3));
	    QString inputFile = templateDir + QDir::separator() + QString("scInput.json");
	    qDebug() << "LoadingFile after download .. " << inputFile;
	    emit loadFile(inputFile);
	    
	    // remove results dir if exists
	    QString resultsDir = name3 + QDir::separator() + QString("Results");
	    
	    QDir resultsD(resultsDir);
	    if (resultsD.exists())
	      resultsD.removeRecursively();
	    
	    // unzip .. this places files in a new dir results
	    ZipUtils::UnzipFile(name2, QDir(name3));
	    emit processResults(resultsDir);		
	    this->close();
	    
	  } else {
	    
	    // unzip files
	    ZipUtils::UnzipFile(name3, localDir);
	    ZipUtils::UnzipFile(name2, localDir);
	    
	    // now load inputfile and process results
	    QFileInfo fileInfo(name1);
	    QString filePath = fileInfo.absolutePath();
	    QString resultsDir = filePath + QDir::separator() + QString("Results");
	    
	    emit loadFile(name1);
	    emit processResults(resultsDir);
	  }
	  
	} else {

	  for (int i=0; i<filesToDownload.size(); i++) {
	    QString name = filesToDownload.at(i);
	    qDebug() << "name: " << name;
	    if (name.contains(".zip")) {
	      QString filePath = localDir + QDir::separator() + name;	      
	      ZipUtils::UnzipFile(filePath, localDir);
	      qDebug() << "unzipiing " << filePath << " in: " << localDir;
	    }
	  }
	  emit processResults(localDir);
	}
	
      } else {
	emit sendErrorMessage("ERROR - Failed to download File - did Job finish successfully?");
      }
    }
}

void
RemoteJobManager::getJobData(void) {

    this->close();

    if (triggeredRow != -1) {

        QString archiveDir = "";
        QString inputDir = "";

        //
        // get jobID, from interface obtain information about where the output directory will be
        //

        QTableWidgetItem *itemID=jobsTable->item(triggeredRow,2);
        jobIDRequest = itemID->text();
        getJobDetailsRequest = 2;
	connect(theService,SIGNAL(getJobDetailsReturn(QJsonObject)),this,SLOT(getJobDetailsReturn(QJsonObject)));
	theService->getJobDetailsCall(jobIDRequest);	
        // emit getJobDetails(jobIDRequest);
    }

    triggeredRow = -1;
}


void RemoteJobManager::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus, QString fileName)
{
    if(exitStatus == QProcess::ExitStatus::CrashExit)
    {
        QString errText("Error, the hdf conversion process crashed for file ");

        emit sendErrorMessage(errText + fileName);
        emit sendStatusMessage("Unpacking of hdf complete with errors");

        return;
    }

    if(exitCode != 0)
    {
        QString errText("An error occurred in the hdf conversion process, the exit code is " + QString::number(exitCode));

        emit sendErrorMessage(errText);
        emit sendStatusMessage("Unpacking of hdf complete with errors for file" + fileName);

        return;
    }

    emit sendStatusMessage("Unpacking of hdf file completed successfully for file "+ fileName);
}


void RemoteJobManager::handleProcessTextOutput(void)
{
    QByteArray output =  proc->readAllStandardOutput();
    QString outputStr = QString(output);
    emit sendStatusMessage(outputStr);
    QApplication::processEvents();
}

QString RemoteJobManager::getJobRunTime(QString startedTime, QString currentTime)
{
    QDateTime UTC(QDateTime::fromString(startedTime,Qt::ISODateWithMs));
    QDateTime local(UTC.toLocalTime());
    qDebug() << "UTC time is:" << UTC;

    return "yes";
}



