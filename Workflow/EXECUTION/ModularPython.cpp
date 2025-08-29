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
#include <QDir>
#include <QPixmap>
#include <QLabel>
#include <QGridLayout>
#include <QFileInfo>
#include <QProcess>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

ModularPython::ModularPython(QString workDir, QWidget *parent): Application(parent) {
  
  QDir workDirectory(workDir);
  if (!workDirectory.exists()) {
      this->errorMessage(QString("ModularPython::ModularPython no existant workdir: ")+workDir);
      qDebug() <<"ModularPython::ModularPython no existant workdir: " << workDir;

    } else {
      
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
}


ModularPython::~ModularPython()
{

}

void ModularPython::run(QString pythonScriptPath, QStringList pythonArgs){

  if (theProcessHandler == nullptr) {
    this->errorMessage(QString("ModularPython::run no processHandler was created before run .. workdir!!"));    
    return;
  }

  QFile pythonScriptFile(pythonScriptPath);
  if (!pythonScriptFile.exists()) {
    this->errorMessage(QString("ModularPython::run no pythonscript: ") + pythonScriptPath + QString("  exists to run run"));
    qDebug() << "ModularPython::run no pythonscript: " <<  pythonScriptPath << "  exists to run run";;    
    return;
  }

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
      pythonScriptPath + QString("\" "); //  + pythonArgs.join(" \"");

    const int listSize = pythonArgs.size();
    for (int i = 0; i < listSize; ++i) {
      QString arg = pythonArgs.at(i);
        if (arg.contains(QDir::separator()) || arg.contains("\\")) // adding back space if dir path involved
	command += QString(" \"") + arg + QString("\"");
      else
	command += QString(" ") + arg;	
    }

    qDebug() << "PYTHON COMMAND" << command;


    QStringList cmdList = {"-c",command};
    theProcessHandler->startProcess("bash", cmdList, processName, nullptr);

#endif

    theProcessHandler->waitForFinished(-1);

}

void ModularPython::handleProcessFinished(int exitCode)
{

    if(exitCode == 0)
    {
        this->statusMessage("Python script completed");
    } else {
//        QString errText("An error occurred in the Python script, the exit code is " + QString::number(exitCode));

//        this->errorMessage(errText);
//        this->statusMessage("Analysis complete with errors");
//        emit runComplete();

//        return;
    }


    emit runComplete();
}
