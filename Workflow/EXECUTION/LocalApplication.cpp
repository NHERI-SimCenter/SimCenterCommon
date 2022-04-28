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

#include "LocalApplication.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QJsonObject>
#include <QStandardPaths>
#include <QApplication>
#include <QProcess>
#include <QStringList>
#include <QString>
#include <QStringRef>
#include <QSettings>
#include <SimCenterPreferences.h>
#include <AgaveCurl.h>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QProcessEnvironment>
#include <QCoreApplication>
#include <SimCenterPreferences.h>

LocalApplication::LocalApplication(QString workflowScriptName, QWidget *parent)
    : Application(parent)
{
    proc = new QProcess(this);
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &LocalApplication::handleProcessFinished);
    connect(proc, &QProcess::readyReadStandardOutput, this, &LocalApplication::handleProcessTextOutput);
    connect(proc, &QProcess::started, this, &LocalApplication::handleProcessStarted);

    this->workflowScript = workflowScriptName;
}

bool
LocalApplication::outputToJSON(QJsonObject &jsonObject)
{
    jsonObject["localAppDir"]=SimCenterPreferences::getInstance()->getAppDir();
    jsonObject["remoteAppDir"]=SimCenterPreferences::getInstance()->getAppDir();
    jsonObject["workingDir"]=SimCenterPreferences::getInstance()->getLocalWorkDir();

    jsonObject["runType"]=QString("runningLocal");

    return true;
}

bool
LocalApplication::inputFromJSON(QJsonObject &dataObject) {

    Q_UNUSED(dataObject);
    return true;
}




void
LocalApplication::onRunButtonPressed(void)
{
    sendStatusMessage("Setting up temporary directory.");

    QApplication::processEvents();

    QString workingDir = SimCenterPreferences::getInstance()->getLocalWorkDir();
    QDir dirWork(workingDir);
    if (!dirWork.exists())
        if (!dirWork.mkpath(workingDir)) {
            QString errorMessage = QString("Could not create Working Dir: ") + workingDir
                    + QString(". Change the Local Jobs Directory location in preferences.");

            emit sendErrorMessage(errorMessage);;

            return;
        }


    //   QString appDir = appDirName->text();
    QString appDir = SimCenterPreferences::getInstance()->getAppDir();
    QDir dirApp(appDir);
    if (!dirApp.exists()) {
        QString errorMessage = QString("The application directory, ") + appDir +QString(" specified does not exist!. Check Local Application Directory in Preferences");
        emit sendErrorMessage(errorMessage);;
        return;
    }

    QString templateDir("templatedir");

    emit sendStatusMessage("Gathering files to local workdir.");
    emit setupForRun(workingDir, templateDir);
}


//
// now use the applications Workflow Application EE-UQ.py  to run dakota and produce output files:
//    dakota.in dakota.out dakotaTab.out dakota.err
//

bool
LocalApplication::setupDoneRunApplication(QString &tmpDirectory, QString &inputFile)
{
    tempDirectory = tmpDirectory;
    inputFilePath = inputFile;

    // qDebug() << "RUNTYPE" << runType;
    QString runType("runningLocal");
    qDebug() << "RUNTYPE" << runType;
    QString appDir = SimCenterPreferences::getInstance()->getAppDir();
    QString appName = QCoreApplication::applicationName();

    //TODO: recognize if it is PBE or EE-UQ -> probably smarter to do it inside the python file
    QString pySCRIPT;

    QDir scriptDir(appDir);
    scriptDir.cd("applications");
    scriptDir.cd("Workflow");
    pySCRIPT = scriptDir.absoluteFilePath(workflowScript);

    // pySCRIPT = scriptDir.absoluteFilePath("EE-UQ.py");
    QFileInfo check_script(pySCRIPT);
    // check if file exists and if yes: Is it really a file and no directory?
    if (!check_script.exists() || !check_script.isFile()) {
        emit sendErrorMessage(QString("NO SCRIPT FILE: ") + pySCRIPT);
        emit runComplete();
        return false;
    }

    QString registryFile = scriptDir.absoluteFilePath("WorkflowApplications.json");
    QFileInfo check_registry(registryFile);
    if (!check_registry.exists() || !check_registry.isFile()) {
        emit sendErrorMessage(QString("NO REGISTRY FILE: ") + registryFile);
        emit runComplete();
        return false;
    }

    qDebug() << "SCRIPT: " << pySCRIPT;
    qDebug() << "REGISTRY: " << registryFile;

    QStringList files;
    files << "dakota.in" << "dakota.out" << "dakotaTab.out" << "dakota.err";

    //
    // now invoke dakota, done via a python script in tool app dircetory
    //
    proc->setProcessChannelMode(QProcess::SeparateChannels);
    auto procEnv = QProcessEnvironment::systemEnvironment();
    QString pathEnv = procEnv.value("PATH");
    QString pythonPathEnv = procEnv.value("PYTHONPATH");


    QString python = QString("python");
    QString exportPath("export PATH=");
    bool colonYes = false;

    SimCenterPreferences *preferences = SimCenterPreferences::getInstance();
    python = preferences->getPython();

    QFileInfo pythonFile(python);
    if (pythonFile.exists()) {
        QString pythonPath = pythonFile.absolutePath();
        colonYes=true;
        exportPath += pythonPath;
        pathEnv = pythonPath + ';' + pathEnv;
    } else {
        emit sendErrorMessage("NO VALID PYTHON - Read the Manual & Check your Preferences");
        emit runComplete();
        return false;
    }

    /* ****************************************** removing python checks ********************

    // Check if python and required packages exist at the given path
    {
        QProcess pythonProcess;

        QString commandCheckPythonVersion = python + " -V";
        pythonProcess.start(commandCheckPythonVersion);

        bool started = pythonProcess.waitForStarted();
        if (!pythonProcess.waitForFinished(30000)) // 3 Second timeout
            pythonProcess.kill();

        if(!started)
        {
            emit errorMessage("Could not find Python on the system");
            emit runComplete();
            return false; // Not found or which does not work
        }

        auto processOutput = QString::fromLocal8Bit(pythonProcess.readAll());
        processOutput = processOutput.trimmed();

        auto resultList = processOutput.split(" ");

        if(resultList.size() == 2)
        {
            QString pythonVersionStr = resultList.at(1);

            int locFirst =  pythonVersionStr.indexOf(".");
            int locSecond =  pythonVersionStr.indexOf(".", locFirst+1);	    


            QStringRef pythonVersion(&pythonVersionStr, locFirst-1, locSecond);
	    
            double verNum = pythonVersion.toDouble();

            if(verNum != 3.8 && verNum != 3.1) // 3.1 being 3.10
            {
                emit errorMessage("Python version 3.8 is required, you have Python version "+QString::number(verNum)+" installed");
                emit runComplete();
                return false;
            }
        }
        else
        {
            emit errorMessage("Could not parse the python version");
            emit runComplete();
            return false; // Not found or which does not work
        }
    }

    {

        QProcess pythonProcess;
        // The command below will return a "1" if nheri_simcenter package is not found on the system and a zero if it is found
        QString commandToStart;

#ifdef Q_OS_WIN
        commandToStart = python + " -c \"import pkgutil; print('0' if pkgutil.find_loader('nheri_simcenter') else '1')\"";
#else
        commandToStart = python + " -c 'import pkgutil; print('0' if pkgutil.find_loader(\"nheri_simcenter\") else '1')'";
#endif


#ifdef Q_OS_WIN
        pythonProcess.start(commandToStart);
#else
        pythonProcess.start("bash", QStringList() << "-c" <<  commandToStart);
#endif

        bool started = pythonProcess.waitForStarted();
        if (!pythonProcess.waitForFinished(30000)) // 3 Second timeout
            pythonProcess.kill();

        if(!started)
        {
            emit errorMessage("Could not find Python on the system");
            emit runComplete();
            return false;
        }

        auto processOutput = QString::fromLocal8Bit(pythonProcess.readAll());
        processOutput = processOutput.trimmed();

        if(processOutput.compare("0") != 0)
        {
            emit errorMessage("Could not find the NHERI-SimCenter Python package on the system. Run the command --"+python +" -m pip install nheri-simcenter-- in Terminal or Command Prompt to install.");
            emit runComplete();
            return false;
        }
    }

    ******************************************************************************** */

    QString openseesExe = preferences->getOpenSees();
    QFileInfo openseesFile(openseesExe);
    if (openseesFile.exists()) {
        QString openseesPath = openseesFile.absolutePath();
#ifdef Q_OS_WIN
        pathEnv = openseesPath + ';' + pathEnv;
#else
        pathEnv = openseesPath + ':' + pathEnv;
#endif
        if (colonYes == false) {
            colonYes = true;
        } else {
#ifdef Q_OS_WIN
            exportPath += ";";
#else
            exportPath += ":";
#endif
        }
        exportPath += openseesPath;
    }


    QString dakotaExe = preferences->getDakota();

    qDebug() << "DAKOTA: " << dakotaExe;

    QFileInfo dakotaFile(dakotaExe);
    if (dakotaFile.exists()) {
        QString dakotaPath = dakotaFile.absolutePath();
        if (colonYes == false) {
            colonYes = true;
        } else {
#ifdef Q_OS_WIN
            exportPath += ";";
#else
            exportPath += ":";
#endif
        }

#ifdef Q_OS_WIN
        pathEnv = dakotaPath + ';' + pathEnv;
#else
        pathEnv = dakotaPath + ':' + pathEnv;
#endif
        exportPath += dakotaPath;
        QString dakotaPathPath = QFileInfo(dakotaPath).absolutePath() + QDir::separator() +
                "share" + QDir::separator() + "Dakota" + QDir::separator() + "Python";
        pythonPathEnv = dakotaPathPath + ";" + pythonPathEnv;

    }

    if (colonYes == true) {
#ifdef Q_OS_WIN
        exportPath += ";";
#else
        exportPath += ":";
#endif
    }

    exportPath += "$PATH";

    procEnv.insert("PATH", pathEnv);
    procEnv.insert("PYTHONPATH", pythonPathEnv);
    proc->setProcessEnvironment(procEnv);

    // qDebug() << "PATH: " << pathEnv;
    // qDebug() << "PYTHON_PATH" << pythonPathEnv;
    // QString appName = QCoreApplication::applicationName();

    QStringList args;
    QString inputDir = tmpDirectory + QDir::separator() + "input_data";
    if (appName == "RDT" || appName == "R2D") {
        args << pySCRIPT << inputFile << "--registry" << registryFile
             << "--referenceDir" << inputDir
             << "-w" << tmpDirectory+QDir::separator() + "Results";
    } else {
        args << pySCRIPT << runType << inputFile << registryFile;
    }
    /*
    command = sourceBash + exportPath + "; \"" + python + QString("\" \"" ) + pySCRIPT + QString("\" " )
            + QString(" \"" ) + inputFile + QString("\" ") +"--registry"
            + QString(" \"") + registryFile + QString("\" ") + "--referenceDir" + QString(" \"")
            + tmpDirectory + QString("/input_data\" ") + "-w" + QString(" \"")
                + tmpDirectory + QDir::separator() + "Results" + QString("\"");
   */


#ifdef Q_OS_WIN
    python = QString("\"") + python + QString("\"");

    qDebug() << python;
    qDebug() << args;

    proc->start(python,args);

    bool failed = false;
    if (!proc->waitForStarted(-1))
    {
        qDebug() << "Failed to start the workflow!!! exit code returned: " << proc->exitCode();
        qDebug() << proc->errorString().split('\n');
        emit sendStatusMessage("Failed to start the workflow!!!");
        failed = true;
    }

    if(!proc->waitForFinished(-1))
    {
        qDebug() << "Failed to finish running the workflow!!! exit code returned: " << proc->exitCode();
        qDebug() << proc->errorString();
        emit sendStatusMessage("Failed to finish running the workflow!!!");
        failed = true;
    }


    if(0 != proc->exitCode())
    {
        qDebug() << "Failed to run the workflow!!! exit code returned: " << proc->exitCode();
        qDebug() << proc->errorString();
        emit sendStatusMessage("Failed to run the workflow!!!");
        failed = true;
    }

    if(failed)
    {
        qDebug().noquote() << proc->readAllStandardOutput();
        qDebug().noquote() << proc->readAllStandardError();
        emit runComplete();
        return false;
    }

#else

    // check for bashrc or bash profile
    QDir homeDir(QDir::homePath());
    QString sourceBash("");
    if (homeDir.exists(".bash_profile")) {
        sourceBash = QString("source $HOME/.bash_profile; ");
    } else if (homeDir.exists(".bashrc")) {
        sourceBash = QString("source $HOME/.bashrc; ");
    } else if (homeDir.exists(".zprofile")) {
        sourceBash = QString("source $HOME/.zprofile; ");
    } else if (homeDir.exists(".zshrc")) {
        sourceBash = QString("source $HOME/.zshrc; ");
    } else
        emit sendErrorMessage( "No .bash_profile, .bashrc, .zprofile or .zshrc file found. This may not find Dakota or OpenSees");

    // note the above not working under linux because bash_profile not being called so no env variables!!
    QString command;

    if (appName == "R2D"){

        /*
        command = sourceBash + exportPath + "; \"" + python + QString("\" \"" ) + pySCRIPT + QString("\" " )
                + QString(" \"" ) + inputFile + QString("\"");

      */
        command = sourceBash + exportPath + "; \"" + python + QString("\" \"" ) + pySCRIPT + QString("\" " )
                + QString(" \"" ) + inputFile + QString("\" ") +"--registry"
                + QString(" \"") + registryFile + QString("\" ") + "--referenceDir" + QString(" \"")
                + tmpDirectory + QString("/input_data\" ") + "-w" + QString(" \"") + tmpDirectory + QDir::separator() + "Results" + QString("\"");

    } else {

        command = sourceBash + exportPath + "; \"" + python + QString("\" \"" ) +
                pySCRIPT + QString("\" " ) + runType + QString(" \"" ) + inputFile + QString("\" \"") + registryFile + QString("\"");

    }
    qDebug() << "PYTHON COMMAND" << command;

    proc->start("bash", QStringList() << "-c" <<  command);
    proc->waitForStarted();

#endif

    return 0;
}

void
LocalApplication::displayed(void){
    this->onRunButtonPressed();
}

void LocalApplication::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::ExitStatus::CrashExit)
    {
        QString errText("Error, the process running the Python script crashed");

        emit sendErrorMessage(errText);
        emit sendStatusMessage("Analysis complete with errors");
        emit runComplete();

        return;
    }

    if(exitCode != 0)
    {
        QString errText("An error occurred in the Python script, the exit code is " + QString::number(exitCode));

        emit sendErrorMessage(errText);
        emit sendStatusMessage("Analysis complete with errors");
        emit runComplete();

        return;
    }

    QString appName = QCoreApplication::applicationName();

    if (appName != "R2D"){
      
        //
        // copy input file to main directory & process results
        //

        /* 
        QString filenameIN = tempDirectory + QDir::separator() +  QString("dakota.json");
        QFile::copy(inputFilePath, filenameIN);
        QString filenameOUT = tempDirectory + QDir::separator() +  QString("dakota.out");
        QString filenameTAB = tempDirectory + QDir::separator() +  QString("dakotaTab.out");
        emit processResults(filenameOUT, filenameTAB, inputFilePath);
	*/
      
	emit processResults(tempDirectory);
    }
    else
    {
        QString dirOut = tempDirectory + QDir::separator() +  QString("Results");
        // QString name2("");
        // QString name3("");
	//        emit processResults(dirOut, name2, name3);
        emit processResults(dirOut);	
    }

    emit sendStatusMessage("Analysis complete");
    emit runComplete();
}


void LocalApplication::handleProcessStarted(void)
{
    QString msg = "Starting the analysis. This may take awhile!";
    emit sendStatusMessage(msg);
    QApplication::processEvents();
}


void LocalApplication::handleProcessTextOutput(void)
{
    QByteArray output =  proc->readAllStandardOutput();
    QString outputStr = QString(output);
    emit sendStatusMessage(outputStr);
    QApplication::processEvents();
}
