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

    if (resultWidget != 0)
        return resultWidget->processResults(dirName);
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
    // First check dakota.err

    QString filenameErrorString = workDir + QDir::separator() + errFileName;
    QFileInfo filenameErrorInfo(filenameErrorString);
    if (!filenameErrorInfo.exists()) {
        errMsg = "No " + errFileName + " file - " + uqEngineName + " did not run - problem with the setup or the applications failed with inputs provided";
        return;
    }
    QFile fileError(filenameErrorString);
    QString line("");

    if (fileError.open(QIODevice::ReadOnly)) {
        if (uqEngineName==QString("Dakota")) {
               QTextStream in(&fileError);
               while (!in.atEnd()) {
                  line += in.readLine();
               }

        } else {
                QTextStream in(&fileError);
                // QString contents = in.readAll(); -- not reading newline char
                bool errorWritten = false;
                for (QString myLine = in.readLine(); !myLine.isNull(); myLine = in.readLine())
                     line +=  myLine + "<br>";
                //line += "Please check logFileSimUQ.txt in the Local Jobs Directory";
        }
        fileError.close();
    }

    if (line.length()!= 0)
        errMsg = QString("Error Running " + uqEngineName + ": " + line);


    // Overwrite with surrogate if sur.err is found
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

    // Overwrite with workflow if workflow.err is found and not empty
    QString filenameWorkErrString = workDir + QDir::separator() + QString("workflow.err");
    QFileInfo workflowErrorInfo(filenameWorkErrString);
    if (workflowErrorInfo.exists()) {
        QFile workflowError(filenameWorkErrString);
        if (workflowError.open(QIODevice::ReadOnly)) {
           QTextStream in(&workflowError);
           line = in.readLine();
           workflowError.close();
        }
        if (line.length()!= 0) {
            errMsg = QString(QString("Error in Creating Workflow: ") + line);
            errMsg = errMsg + ".... see more in " + filenameWorkErrString;
        }
    }
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
