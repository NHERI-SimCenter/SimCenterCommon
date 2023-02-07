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
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

#include <ModularPython.h>
#include <Utils/ProgramOutputDialog.h>
#include <Utils/PythonProcessHandler.h>
#include <SimCenterPreferences.h>

#include "LocalApplication.h"
#include <QFile>
#include <QPixmap>
#include <QLabel>
#include <QGridLayout>
#include <QFileInfo>
#include <QProcess>
#include <QDebug>
#include <QStandardPaths>

ModularPython::ModularPython(QString workDir, QWidget *parent): Application(parent) {

    //
    // Create handler
    //

    theProcessHandler = std::make_unique<PythonProcessHandler>();
    theProcessHandler->setWorkingDir(workDir);
    connect(theProcessHandler.get(),&PythonProcessHandler::processFinished, this, &ModularPython::handleProcessFinished);


    //
    // set Pathes
    //

    auto procEnv = QProcessEnvironment::systemEnvironment();
    QString pathEnv = procEnv.value("PATH");
    QString pythonPathEnv = procEnv.value("PYTHONPATH");

    python = QString("python");
    exportPath = QString("export PATH=");
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
        this->errorMessage("Python exe does not exist at" + python);
    }

    exportPath += "$PATH";
    procEnv.insert("PATH", pathEnv);
    procEnv.insert("PYTHONPATH", pythonPathEnv);
    theProcessHandler->setProcessEnv(procEnv);

    pythonWorkDir = workDir;
}


ModularPython::~ModularPython()
{

}

void ModularPython::run(QString pythonScriptPath, QStringList pythonArgs){

    QString processName = "python module";
    //
    // RUN
    //

#ifdef Q_OS_WIN

    QStringList args;
    args << pythonScriptPath << pythonArgs;

    python = QString("\"") + python + QString("\"");

    qDebug() << python;
    qDebug() << args;

    //proc->start(python,args);


    theProcessHandler->startProcess(python,args,processName, nullptr);

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
        this->errorMessage( "No .bash_profile, .bashrc, .zprofile or .zshrc file found. This may not find Dakota or OpenSees");

    // note the above not working under linux because bash_profile not being called so no env variables!!
    QString command;

    command = sourceBash + exportPath + "; \"" + python + QString("\" \"" ) +
            pythonScriptPath + QString("\" " ) + pythonArgs.join(" \"");

    qDebug() << "PYTHON COMMAND" << command;


    QStringList cmdList = {"-c",command};
    theMainProcessHandler->startProcess("bash", cmdList, processName, nullptr);

#endif


//#ifdef Q_OS_WIN

//    QString python = QString("\"") + pythonExe + QString("\"");
//    QStringList args;
//    args << pythonScriptPath << pythonArgs;

//    qDebug() << python;
//    //qDebug() << args;

//    proc->setWorkingDirectory(pythonWorkDir);
//    proc->start(python,pythonArgs);

//    bool failed = false;
//    if (!proc->waitForStarted(-1))
//    {
//        qDebug() << "Failed to start the workflow!!! exit code returned: " << proc->exitCode();
//        //qDebug() << proc->errorString().split('\n');
//        //this->statusMessage("Failed to start the workflow!!!");
//        failed = true;
//    }

//    if(!proc->waitForFinished(-1))
//    {
//        qDebug() << "Failed to finish running the workflow!!! exit code returned: " << proc->exitCode();
//        //qDebug() << proc->errorString();
//        //this->statusMessage("Failed to finish running the workflow!!!");
//        failed = true;
//    }


//    if((0 != proc->exitCode())&&(-1 != proc->exitCode())) // latter is caputred error
//    {
//        qDebug() << "Failed to run the workflow!!! exit code returned: " << proc->exitCode();
//        //qDebug() << proc->errorString();
//        //this->statusMessage("Failed to run the workflow!!!");
//        failed = true;
//    }

//    if(failed)
//    {
//        qDebug().noquote() << proc->readAllStandardOutput();
//        qDebug().noquote() << proc->readAllStandardError();
//        //error
//        return;
//    }

//#else
//    //
//    // TODO: FIX MAC BEFORE RELEASE
//    //
//    // check for bashrc or bash profile
//    QDir homeDir(QDir::homePath());
//    QString sourceBash("");
//    if (homeDir.exists(".bash_profile")) {
//        sourceBash = QString("source $HOME/.bash_profile; ");
//    } else if (homeDir.exists(".bashrc")) {
//        sourceBash = QString("source $HOME/.bashrc; ");
//    } else if (homeDir.exists(".zprofile")) {
//        sourceBash = QString("source $HOME/.zprofile; ");
//    } else if (homeDir.exists(".zshrc")) {
//        sourceBash = QString("source $HOME/.zshrc; ");
//    } else


//      qDebug() << QString("No .bash_profile, .bashrc, .zprofile or .zshrc file found. This may not find Dakota or OpenSees");

//    // note the above not working under linux because bash_profile not being called so no env variables!!
//    QString command;
//        command = "\"" + myPython + QString("\" \"" ) +
//                pythonScriptPath + QString("\" \"") + pythonArgs + QString("\"");

//        qDebug() << "PYTHON COMMAND" << command;

//    proc->start("bash", QStringList() << "-c" <<  command);
//    proc->waitForStarted();

//    bool failed = false;
//    if(!proc->waitForFinished(-1))
//    {
//        qDebug() << "Failed to finish running the workflow!!! exit code returned: " << proc->exitCode();
//        qDebug() << proc->errorString();
//    //        this->statusMessage("Failed to finish running the workflow!!!");
//        failed = true;
//    }


//    if(0 != proc->exitCode())
//    {
//        qDebug() << "Failed to run the workflow!!! exit code returned: " << proc->exitCode();
//        qDebug() << proc->errorString();
//        // this->statusMessage("Failed to run the workflow!!!");
//        failed = true;
//    }

//    if(failed)
//    {
//        qDebug().noquote() << proc->readAllStandardOutput();
//        qDebug().noquote() << proc->readAllStandardError();
//        emit runComplete(false, configJSON.value("runDir").toString(), "gridIM_output.json");
//        return;
//    }

//#endif

    theProcessHandler->waitForFinished(-1);

}

void ModularPython::handleProcessFinished(int exitCode)
{

    if(exitCode == 0)
    {
        this->statusMessage("Analysis complete");
    } else {
//        QString errText("An error occurred in the Python script, the exit code is " + QString::number(exitCode));

//        this->errorMessage(errText);
//        this->statusMessage("Analysis complete with errors");
//        emit runComplete();

//        return;
    }


    emit runComplete();
}
