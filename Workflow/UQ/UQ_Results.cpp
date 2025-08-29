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

// essentially an abstract class, if no results are available this is the widget that shows up

#include "UQ_Results.h"
#include <QVBoxLayout>
#include <QJsonObject>
#include <UQ_Results.h>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <RandomVariablesContainer.h>
#include <QFileInfo>
#include <QCoreApplication>
//#include <filesystem>

UQ_Results::UQ_Results(QWidget *parent)
: SimCenterWidget(parent), resultWidget(0)
{
    layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    this->setLayout(layout);
    resultWidget = 0;
}

UQ_Results::~UQ_Results()
{

}


bool
UQ_Results::outputToJSON(QJsonObject &jsonObject)
{
    QJsonObject uq;
    bool result = true;

    if (resultWidget != 0) {
        result = resultWidget->outputToJSON(uq);
    } else {
        uq["resultType"]=QString(tr("NONE"));
    }
    jsonObject["uqResults"]=uq;
    return result;
}


bool
UQ_Results::inputFromJSON(QJsonObject &jsonObject)
{   
    bool result = false;
    if (jsonObject.contains("uqResults")) {
        QJsonValue uqValue = jsonObject["uqResults"];

        QJsonObject uq = uqValue.toObject();
        QString resultType = uq["resultType"].toString();
        //UQ_Results *newResultWidget = 0;

        if (resultType == "NONE") {
           // resultWidget=0;
            return true; // no results saved
        }

        if (resultWidget != 0) {
          result = resultWidget->inputFromJSON(uq);
        } else {
          errorMessage("ERROR: reading Dakota Results - no result widget set!");
        }

    } else {
        errorMessage("ERROR: Dakota Results - no \"uqResults\" entry");
        return false;
    }


    // no error if no results .. maybe none actually in file
    return result;
}

int 
UQ_Results::processResults(QString &dirName) {

    if (resultWidget != 0) {
        // check if the directory exists
        QFileInfo dirInfo(dirName);
        if (!dirInfo.exists()) {
            errorMessage(QString("Directory does not exist: ") + dirName);
            // Replace Results with results, vice versa, and try again
            if (dirName.contains("Results")) {
                dirName.replace("Results", "results");
            } else if (dirName.contains("results")) {
                dirName.replace("results", "Results");
            }
            if (!dirInfo.exists()) {
                errorMessage(QString("Directory does not exist: ") + dirName);
                return 0;
            }
        }
        qDebug() << "Processing results in directory: " << dirName;
    
        return resultWidget->processResults(dirName);
    }
    else {
      QString message = QString("ERROR: Processing results - No resultsWidget set, directory: " ) +  dirName;
      qDebug() << message;
      errorMessage(message);
//      QMessageBox::warning(this, tr("Application"),tr("BUG - No ResultsWidget Set!"));
      return 0;
    }
}


void
UQ_Results::extractErrorMsg(QString workDir, QString errFileName, QString uqEngineName, QString &errMsg) {

    //
    // 1. First check if "dakota.err" is created.
    //
    QString appName = QCoreApplication::applicationName();

    QString filenameErrorString = workDir + QDir::separator() + errFileName;
    QFileInfo filenameErrorInfo(filenameErrorString);
    if (!filenameErrorInfo.exists()) {
        // Replace Results with results and try again
        if (workDir.contains("Results")) {
            workDir.replace("Results", "results");
        } else if (workDir.contains("results")) {
            workDir.replace("results", "Results");
        }
        filenameErrorString = workDir + QDir::separator() + errFileName;
        filenameErrorInfo.setFile(filenameErrorString);
        if (!filenameErrorInfo.exists()) {
        //        errMsg = "No " + errFileName + " file - " + uqEngineName + " did not run - problem with the setup or the applications failed with inputs provided";
            errMsg = "No " + filenameErrorString + " file - " + uqEngineName + " did not run - problem with the setup or the applications failed with inputs provided";      
            return;
        }
    }

    //
    // When "dakota.err" is found - error from UQ is written here.
    //

    QFile fileError(filenameErrorString);
    QString line_UQ("");

    if (fileError.open(QIODevice::ReadOnly)) {
        if (uqEngineName==QString("Dakota")) {
               QTextStream in(&fileError);
               while (!in.atEnd()) {
                  line_UQ += in.readLine();
               }

        } else {
                QTextStream in(&fileError);
                // QString contents = in.readAll(); -- not reading newline char, so adding <br> - sy
                for (QString myLine = in.readLine(); !myLine.isNull(); myLine = in.readLine())
                     line_UQ +=  myLine + "<br>";
                //line += "Please check logFileSimUQ.txt in the Local Jobs Directory";
        }
        fileError.close();
    }
    if ((line_UQ.length()!= 0))
        errMsg = QString("Analysis terminated with error<br>") +
                 QString("-----------------------------------------------------------------------<br>") +
                 QString("Error from ") + uqEngineName + QString(": ") + line_UQ + QString("<br>") +
                 QString("-----------------------------------------------------------------------<br>");
    //
    // Parse workdir number
    //
    line_UQ.replace("/","\\");
    if (line_UQ.contains(QString("workdir."))) {

        QString tmp_string = line_UQ.right(line_UQ.length()  - line_UQ.indexOf("workdir."));
        QString workdirName;
        if (uqEngineName=="Dakota") {
            workdirName = tmp_string.left(tmp_string.indexOf("\\"));
        } else {
            if (tmp_string.lastIndexOf(".") == tmp_string.indexOf(".")) {
                //python(surrogate): "results.out missing at C:/Users/SimCenter/Documents/quoFEM/LocalWorkDir/tmp.SimCenter/workdir.1<br>"
                workdirName = tmp_string.left(tmp_string.lastIndexOf("<"));
            } else {
                // cpp: "results.out not found at workdir.1."
                workdirName = tmp_string.left(tmp_string.lastIndexOf("."));
            }

        }


        QDir myWorkDir(workDir);
        QString filenameWorkErrString = workDir + QDir::separator() + workdirName + QDir::separator() + "workflow.err"; // display the first file

        QString openseesMsg = QString("OpenSees -- Open System For Earthquake Engineering Simulation Pacific Earthquake Engineering Research Center Version 3.3.0 64-Bit (c) Copyright 1999-2016 The Regents of the University of California All Rights Reserved (Copyright and Disclaimer @ http://www.berkeley.edu/OpenSees/copyright.html)").replace(" ","");
        if (QDir(workDir + QDir::separator() + workdirName).exists("workflow.err")) {

            QString line_WF("");

            QFileInfo workflowErrorInfo(filenameWorkErrString);
            if (workflowErrorInfo.exists()) {
                QFile workflowError(filenameWorkErrString);
                if (workflowError.open(QIODevice::ReadOnly)) {
                   //QTextStream in(&workflowError);
                   //line = in.readLine();
                    QTextStream in2(&workflowError);
                    int lineCount = 0;
                    while ((!in2.atEnd()) && lineCount<10) {
                       QString lineTmp = in2.readLine();
                       QString lineTmp2 = lineTmp;
                       if (!openseesMsg.contains(lineTmp2.replace(" ",""))) {
                            line_WF += lineTmp + QString("<br>");
                            lineCount ++;
                       }
                    }
                    workflowError.close();
                }
                if (line_WF.length()!= 0) {
                    errMsg += QString("Error running the workflow in ") + workdirName + QString(": <br>") +
                            line_WF + ".... see more in " + filenameWorkErrString.replace("/","\\") + QString("<br>") +
                            QString("-----------------------------------------------------------------------<br>");
                }
            }
        }
    }

    //
    // Overwrite with surrogate if sur.err is found
    //
    /*
    if (errMsg.length()!=0) {
        QString filenameSurErrString = workDir + QDir::separator() + QString("surrogate.err");
        QFileInfo surrogateErrorInfo(filenameSurErrString);
        if (surrogateErrorInfo.exists()) {
            QFile surrogateError(filenameSurErrString);
            if (surrogateError.open(QIODevice::ReadOnly)) {
               QTextStream in(&surrogateError);
               line = in.readLine();
               surrogateError.close();
            }
            if (line.length()!= 0) {
                errMsg = QString(QString("Error Running Surrogate Simulation: ") + line);
                errMsg = errMsg + ".... see more in " + filenameSurErrString;
            }
        }
    }

    //
    // Overwrite with workflow err if "workflow.err*" file is found and not empty - error from other workflow pieces should be written here - sy
    //
    QDir myWorkDir(workDir);
    QStringList errFiles = myWorkDir.entryList(QStringList() << "workflow.err*",QDir::Files); // find all files that starts with workflow.err

    if (errFiles.size()>0) {
        QString filenameWorkErrString = workDir + QDir::separator() + errFiles[0]; // display the first file
        //QString filenameWorkErrString = workDir + QDir::separator() + QString("workflow.err");
        QFileInfo workflowErrorInfo(filenameWorkErrString);
        if (workflowErrorInfo.exists()) {
            QFile workflowError(filenameWorkErrString);
            if (workflowError.open(QIODevice::ReadOnly)) {
               QTextStream in(&workflowError);
               line = in.readLine();
               workflowError.close();
            }
            if (line.length()!= 0) {
                errMsg = QString(QString("Error in Workflow: ") + line);
                errMsg = errMsg + ".... see more in " + filenameWorkErrString;
            }
        }
    }
    */
}

void
UQ_Results::setResultWidget(UQ_Results *result) {
  
//    if (resultWidget != NULL) {
//        layout->removeWidget(resultWidget);
//        delete resultWidget;
//        resultWidget = 0;
//    }

    for (int i =0; i<layout->count();i++) {
        layout->removeItem(layout->itemAt(i));
    }

    if (result != 0) {
        layout->addWidget(result);
        resultWidget = result;
    } else {
        errorMessage(QString("ResultWidget::set result widget new:  NULL!"));
    }
}

void
UQ_Results::clear(void) {

//    if (resultWidget != NULL) {
//        layout->removeWidget(resultWidget);
//        delete resultWidget;
//        resultWidget = 0;
//    }

    for (int i =0; i<layout->count();i++) {
        layout->removeItem(layout->itemAt(i));
    }
}
