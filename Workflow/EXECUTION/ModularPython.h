#ifndef MODULAR_PYTHON_H
#define MODULAR_PYTHON_H

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

/**
 *  @author  sangri
 *  @date    2/2023
 *  @version 1.0
 *
 *  @section DESCRIPTION
 *
 *  running Python
 */

#include <QWidget>
#include <QProcess>
#include "Utils/PythonProcessHandler.h"
#include <Application.h>

class QJsonObject;
class ProgramOutputDialog;

class ModularPython  : public Application
{
    Q_OBJECT
public:
    explicit ModularPython(QString pythonWorkDir, QWidget *parent = nullptr);
    virtual ~ModularPython();
    void run();

    QString pythonExe;
    QStringList pythonArgs;
    QString pythonScriptPath;
    QString pythonWorkDir;
    void run(QString pythonScriptPath, QStringList pythonArgs);

signals:
    void processResults(QString &resultsDir);
    void runComplete();
    void sendErrorMessage(QString);
    void sendStatusMessage(QString);

public slots:
   void handleProcessFinished(int exitCode);

private:
    std::unique_ptr<PythonProcessHandler> theProcessHandler = nullptr;
    QString python;
    QString exportPath;
};

#endif // MODULAR_PYTHON_H
