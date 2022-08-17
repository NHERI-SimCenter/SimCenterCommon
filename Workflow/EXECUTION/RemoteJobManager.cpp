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
#include <QDesktopWidget>
#include <QScreen>
#include <SimCenterPreferences.h>
#include <QSettings>
#include <QLabel>

#include <QMenu>
#include <QDir>

#include  <QDebug>
#include <ZipUtils/ZipUtils.h>
class RemoteService;

RemoteJobManager::RemoteJobManager(RemoteService *theRemoteInterface, QWidget *parent)
    : QWidget(parent), triggeredRow(-1)
{
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *label = new QLabel("* Click in any job shown in table below to update the job status, download the job or delete the job. Deleting a job removes it and all files created for it.");
    
    layout->addWidget(label);
    
    jobsTable = 0;
    htmlInputDirectory = QString("agave://designsafe.storage.default/");
    headers << "Name" << "STATUS" << "ID" << "Date Created";
    jobsTable=new QTableWidget(this);
    jobsTable->setColumnCount(4);
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

    // update jobs list
    connect(this,SIGNAL(getJobsList(QString)),theRemoteInterface,SLOT(getJobListCall(QString)));
    connect(theRemoteInterface,SIGNAL(getJobListReturn(QJsonObject)), this,SLOT(jobsListReturn(QJsonObject)));

    // update job status
    connect(this,SIGNAL(getJobStatus(QString)),theRemoteInterface,SLOT(getJobStatusCall(QString)));
    connect(theRemoteInterface,SIGNAL(getJobStatusReturn(QString)),this,SLOT(jobStatusReturn(QString)));

    // getJobDetails
    connect(this,SIGNAL(getJobDetails(QString)), theRemoteInterface,SLOT(getJobDetailsCall(QString)));
    connect(theRemoteInterface,SIGNAL(getJobDetailsReturn(QJsonObject)),this,SLOT(getJobDetailsReturn(QJsonObject)));

    // delete job
    connect(this,SIGNAL(deleteJob(QString,QStringList)),theRemoteInterface,SLOT(deleteJobCall(QString,QStringList)));
    connect(theRemoteInterface,SIGNAL(deleteJobReturn(bool)), this,SLOT(deleteJobReturn(bool)));

    // download files
    connect(this,&RemoteJobManager::downloadFiles, theRemoteInterface,
            [this, theRemoteInterface](QStringList remoteFiles, QStringList localFiles)
    {
        theRemoteInterface->downloadFilesCall(remoteFiles, localFiles, this);
    });
    connect(theRemoteInterface,SIGNAL(downloadFilesReturn(bool, QObject*)),this,SLOT(downloadFilesReturn(bool, QObject*)));

    // The python process for extracting the hdf5 file
    proc = new QProcess(this);

    connect(proc, &QProcess::readyReadStandardOutput, this, &RemoteJobManager::handleProcessTextOutput);
}

void
RemoteJobManager::clearTable(void){
    jobsTable->setRowCount(0);

}

void
RemoteJobManager::updateJobTable(QString appName){

    emit getJobsList(appName);
   // jobs = theInterface->getJobList(QString(""));
}

void
RemoteJobManager::jobsListReturn(QJsonObject theJobs){

    jobs = theJobs;

    if (jobs.contains("jobs")) {
        QJsonArray jobData=jobs["jobs"].toArray();
        int numJobs = jobData.count();
        jobsTable->setRowCount(numJobs);

        for (int i=0; i<numJobs; i++) {
            QJsonObject job=jobData.at(i).toObject();
            QString jobID = job["id"].toString();
            QString jobName = job["name"].toString();
            QString jobStatus = job["status"].toString();
            QString jobDate = job["created"].toString();
            jobsTable->setItem(i, 2, new QTableWidgetItem(jobID));
            jobsTable->setItem(i, 1, new QTableWidgetItem(jobStatus));
            jobsTable->setItem(i, 3, new QTableWidgetItem(jobDate));
            jobsTable->setItem(i, 0, new QTableWidgetItem(jobName));
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
    jobMenu.addAction("Delete Job And Data", this, SLOT(deleteJobAndData()));

    jobMenu.exec(QCursor::pos());
}


void
RemoteJobManager::updateJobStatus(void)
{
    if (triggeredRow != -1) {
        QTableWidgetItem *itemID=jobsTable->item(triggeredRow,2);
        QString jobID = itemID->text();
        //QString status=theInterface->getJobStatus(jobID);
        emit getJobStatus(jobID);
    }
}

void
RemoteJobManager::jobStatusReturn(QString status) {
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
        emit deleteJob(jobID, noDirToRemove);
    }
}

void
RemoteJobManager::deleteJobReturn(bool result) {
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
        emit getJobDetails(jobIDRequest);
    }
}

void
RemoteJobManager::getJobDetailsReturn(QJsonObject job)  {


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
        emit deleteJob(jobIDRequest, dirToRemove);
    }

     if (getJobDetailsRequest == 2) {

         //
         // the request was a getJobData
         //    we have to download the files & then process them
         //    note: the processing done after files have downloaded
         //

         QString archiveDir;
         QString inputDir;
         QJsonValue archivePath = job["archivePath"];
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
        QStringList filesToDownload;

        QString appName = QCoreApplication::applicationName();
        if (appName != "R2D"){

	  name1 = localDir + QDir::separator() + QString("templatedir.zip");
	  name2 = localDir + QDir::separator() + QString("results.zip");
	  name3 = localDir;	  
	  
	  localFiles.append(name1);
	  localFiles.append(name2);
	  
	  //
	  // download data to temp files & then process them as normal
	  //
	  
	  archiveDir = archiveDir + QString("/") + inputDir.remove(QRegExp(".*\\/")); // regex to remove up till last /

         QString inputJSON = archiveDir + QString("/templatedir.zip");
         QString resultsZIP = archiveDir + QString("/results.zip");

	 filesToDownload.append(inputJSON);
	 filesToDownload.append(resultsZIP);

        } else {

            name1 = localDir + QDir::separator() + QString("inputRWHALE.json");
            name2 = localDir + QDir::separator() + QString("input_data.zip");
            name3 = localDir + QDir::separator() + QString("hdf.zip");

            localFiles.append(name1);
            localFiles.append(name2);
            localFiles.append(name3);

            //
            // download data to temp files & then process them as normal
            //

            archiveDir = archiveDir + QString("/") + inputDir.remove(QRegExp(".*\\/")); // regex to remove up till last /

            QString rName1 = archiveDir + QString("/inputRWHALE.json");
            QString rName2 = archiveDir + QString("/input_data.zip");
            QString rName3 = archiveDir + QString("/hdf.zip");

            // first download the input data & load it

            filesToDownload.append(rName1);
            filesToDownload.append(rName2);
            filesToDownload.append(rName3);
        }

        emit downloadFiles(filesToDownload, localFiles);
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

    if (sender == this)
    {
        QString localDir = SimCenterPreferences::getInstance()->getRemoteWorkDir();
        QDir localWork(localDir);

        if (!localWork.exists())
            if (!localWork.mkpath(localDir)) {
                emit sendErrorMessage(QString("Could not create Working Dir: ") + localDir + QString(" . Try using an existing directory or make sure you have permission to create the working directory."));
                return;
            }

        if (result == true) {
            QString appName = QCoreApplication::applicationName();
            if (appName != "R2D"){
		QString templateDir = name3 + QDir::separator() + QString("templatedir");
		QDir templateD(templateDir);
		if (templateD.exists())
		  templateD.removeRecursively();

		// unzip .. this places files in a new dir templatedir
		ZipUtils::UnzipFile(name1, QDir(name3));
		QString inputFile = templateDir + QDir::separator() + QString("scInput.json");
		qDebug() << "loadingFile after download .. " << inputFile;
                emit loadFile(inputFile);

		// remove results dir if exists
		QString resultsDir = name3 + QDir::separator() + QString("results");
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

                // convert any hdf files to csv files
                QStringList theFiles = localWork.entryList(QStringList() << "*.hdf5" << "*.hdf",QDir::Files);
                QString pyScript = SimCenterPreferences::getInstance()->getAppDir() + QDir::separator() +
                        "applications" + QDir::separator() + "performDL" + QDir::separator() + "pelicun" + QDir::separator() + "HDF_to_CSV.py";
                foreach(QString filename, theFiles) {

                    QString pathToFile = localWork.filePath(filename);
                    QStringList args{pyScript, pathToFile};

                    QString python = SimCenterPreferences::getInstance()->getPython();
                    //QSettings settings("SimCenter", "Common"); //These names will need to be constants to be shared
                    //QVariant  pythonLocationVariant = settings.value("pythonExePath");
                    //if (pythonLocationVariant.isValid()) {
                    //    python = pythonLocationVariant.toString();
                    //}

                    QString msg = "Unpacking hdf file "+filename +" to the local directory.";
                    emit sendStatusMessage(msg);
                    proc->start(python,args);

                    proc->waitForFinished();

                    handleProcessFinished(proc->exitCode(), proc->exitStatus(), filename);

                    if(proc->exitCode() != 0)
                        return;
                }

                // now load inputfile and process results
                QFileInfo fileInfo(name1);
                QString filePath = fileInfo.absolutePath();
                //QString name2("");
                //QString name3("");

                emit loadFile(name1);
                emit processResults(filePath);
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
        emit getJobDetails(jobIDRequest);
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



