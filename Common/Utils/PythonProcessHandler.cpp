/* *****************************************************************************
Copyright (c) 2016-2021, The Regents of the University of California (Regents).
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

// Written by: Dr. Stevan Gavrilovic, UC Berkeley

#include "PythonProcessHandler.h"
#include "Utils/ProgramOutputDialog.h"

#include <QApplication>
#include <QDir>
#include <QPushButton>

PythonProcessHandler::PythonProcessHandler()
{
    // Create the process where python script will execute
    process = new QProcess(this);

    // Connect signals and slots for multithreaded execution
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &PythonProcessHandler::handleProcessFinished);
    connect(process, &QProcess::readyReadStandardOutput, this, &PythonProcessHandler::handleProcessTextOutput);
    connect(process, &QProcess::readyReadStandardError, this, &PythonProcessHandler::handleProcessErrorOutput);
    connect(process, &QProcess::started, this, &PythonProcessHandler::handleProcessStarted);

    connect(this,&PythonProcessHandler::emitStatusMsg,this,&PythonProcessHandler::handleStatusMsg);
    connect(this,&PythonProcessHandler::emitErrorMsg,this,&PythonProcessHandler::handleErrorMsg);
    connect(this,&PythonProcessHandler::emitInfoMsg,this,&PythonProcessHandler::handleInfoMsg);

    //forwards the output of the running process onto the main process.
    // Anything the child process writes to its standard output and standard error will be written to the standard output and standard error of the main process.
    process->setProcessChannelMode(QProcess::MergedChannels);
//    process->setProcessChannelMode(QProcess::SeparateChannels);
}


void PythonProcessHandler::startProcess(const QString& pythonPath, const QStringList& args, QString name, QPushButton* button)
{
    this->startButton = button;
    this->processName = name;

    this->getProgressDialog()->showProgressBar();

    process->start(pythonPath, args);
    //process->waitForStarted(); // do not use this, use the function call below
    //process->waitForFinished();
    //process->waitForFinished(-1)
}


void PythonProcessHandler::handleProcessTextOutput(void)
{
//    QByteArray output = process->readAllStandardOutput();

    QByteArray output = process->readAll();

    this->statusMessage(QString(output));

    QApplication::processEvents();
}


void PythonProcessHandler::handleProcessErrorOutput(void)
{
    QByteArray output = process->readAllStandardError();

    this->statusMessage(QString(output));

    QApplication::processEvents();
}


void PythonProcessHandler::handleProcessStarted(void)
{
    this->statusMessage("Running python script " + processName + " in the background. This may take a while.");
    QApplication::processEvents();

    if(startButton != nullptr)
        this->startButton->setEnabled(false);
}


void PythonProcessHandler::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(startButton != nullptr)
    {
        this->startButton->setEnabled(true);
        this->startButton = nullptr;
    }

    if(exitStatus == QProcess::ExitStatus::CrashExit)
    {
        QString errText("Error, the python process " + processName + " crashed");
        this->errorMessage(errText);
        this->getProgressDialog()->hideProgressBar();

        emit processFinished(-1);

        return;
    }

    if(exitCode != 0)
    {
        QString errText("There was an error in the python process " + processName + ", the exit code is " + QString::number(exitCode));
        this->errorMessage(errText);
        this->getProgressDialog()->hideProgressBar();

        emit processFinished(-1);

        return;
    }


    this->getProgressDialog()->hideProgressBar();

    QApplication::processEvents();

    emit processFinished(0);
}


void PythonProcessHandler::setProcessEnv(QProcessEnvironment& env)
{
    process->setProcessEnvironment(env);
}


void PythonProcessHandler::handleErrorMsg(const QString msg)
{
    this->errorMessage(msg);
}


void PythonProcessHandler::handleStatusMsg(const QString msg)
{
    this->statusMessage(msg);
}


void PythonProcessHandler::handleInfoMsg(const QString msg)
{
    this->infoMessage(msg);
}

