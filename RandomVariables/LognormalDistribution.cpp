/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code lambdast retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form lambdast reproduce the above copyright notice,
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

#include "LognormalDistribution.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QDoubleValidator>
#include <SimCenterGraphPlot.h>
#include <math.h>
#include <QPushButton>
#include <QFileDialog>

LognormalDistribution::LognormalDistribution(QString inpType, QWidget *parent) :RandomVariableDistribution(parent)
{
    //
    // create the main layout and add the input entries
    //
    QGridLayout *mainLayout = new QGridLayout(this);

    // set some defaults, and set layout for widget to be the horizontal layout
    mainLayout->setHorizontalSpacing(10);
    mainLayout->setVerticalSpacing(0);
    mainLayout->setMargin(0);

    QPushButton *showPlotButton = NULL;
    this->inpty=inpType;

    if (inpty==QString("Parameters"))
    {
        lambda = this->createTextEntry(tr("lambda"), mainLayout, 0);
        zeta  = this->createTextEntry(tr("zeta"), mainLayout, 1);
        showPlotButton = new QPushButton("Show PDF");
        mainLayout->addWidget(showPlotButton, 1,2);

    } else if (inpty==QString("Moments")) {

        mean = this->createTextEntry(tr("Mean"), mainLayout, 0);
        standardDev = this->createTextEntry(tr("Standard Dev"), mainLayout, 1);
        showPlotButton = new QPushButton("Show PDF");
        mainLayout->addWidget(showPlotButton, 1,2);

    } else if (inpty==QString("Dataset")) {


        dataDir = this->createTextEntry(tr("Data File"), mainLayout, 0);
        dataDir->setMinimumWidth(200);
        dataDir->setMinimumWidth(200);

        QPushButton *chooseFileButton = new QPushButton("Choose");
        mainLayout->addWidget(chooseFileButton, 1,1);

        // Action
//        connect(chooseFileButton, &QPushButton::clicked, this, [=](){
//                dataDir->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*.*)"));
//        });

        connect(chooseFileButton, &QPushButton::clicked, this, [=](){
                  QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*)");
                  if (!fileName.isEmpty()) {
                      dataDir->setText(fileName);
                  }
              });
    }

    mainLayout->setColumnStretch(3,1);

    thePlot = new SimCenterGraphPlot(QString("x"),QString("Probability Density Function"),500, 500);

    if (inpty==QString("Parameters")) {
        connect(lambda,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(zeta,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(showPlotButton, &QPushButton::clicked, this, [=](){ thePlot->hide(); thePlot->show();});
    } else if (inpty==QString("Moments")) {
        connect(mean,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(standardDev,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(showPlotButton, &QPushButton::clicked, this, [=](){ thePlot->hide(); thePlot->show();});
    }
}

LognormalDistribution::~LognormalDistribution()
{
    delete thePlot;
}

bool
LognormalDistribution::outputToJSON(QJsonObject &rvObject){

    if (inpty==QString("Parameters")) {
        // check for error condition, an entry had no value
        if ((lambda->text().isEmpty())||(zeta->text().isEmpty())) {
            this->errorMessage("ERROR: LognormalDistribution - data has not been set");
            return false;
        }
        rvObject["lambda"]=lambda->text().toDouble();
        rvObject["zeta"]=zeta->text().toDouble();
    } else if (inpty==QString("Moments")) {
        if ((mean->text().isEmpty())||(standardDev->text().isEmpty())) {
            this->errorMessage("ERROR: LognormalDistribution - data has not been set");
            return false;
        }
        rvObject["mean"]=mean->text().toDouble();
        rvObject["stdDev"]=standardDev->text().toDouble();
    } else if (inpty==QString("Dataset")) {
        if (dataDir->text().isEmpty()) {
            this->errorMessage("ERROR: LognormalDistribution - data has not been set");
            return false;
        }
        rvObject["dataDir"]=QString(dataDir->text());
    }
    return true;
}

bool
LognormalDistribution::copyFiles(QString fileDir) {
    if (inpty==QString("Dataset")) {
        return QFile::copy(dataDir->text(), fileDir);
    } else {
        return true;
    }
}

bool
LognormalDistribution::inputFromJSON(QJsonObject &rvObject){

    //
    // for all entries, make sure i exists and if it does get it, otherwise return error
    //

    if (rvObject.contains("inputType")) {
        inpty=rvObject["inputType"].toString();
    } else {
        inpty = "Moments";
    }

    if (inpty==QString("Parameters")) {
        if (rvObject.contains("lambda")) {
            double thelambdaValue = rvObject["lambda"].toDouble();
            lambda->setText(QString::number(thelambdaValue));
        } else {
            this->errorMessage("ERROR: LognormalDistribution - no \"lambda\" entry");
            return false;
        }
        if (rvObject.contains("zeta")) {
            double thezetaValue = rvObject["zeta"].toDouble();
            zeta->setText(QString::number(thezetaValue));
        } else {
            this->errorMessage("ERROR: LognormalDistribution - no \"zeta\" entry");
            return false;
        }
      } else if (inpty==QString("Moments")) {

        if (rvObject.contains("mean")) {
            double theMeanValue = rvObject["mean"].toDouble();
            mean->setText(QString::number(theMeanValue));
        } else {
            this->errorMessage("ERROR: LognormalDistribution - no \"mean\" entry");
            return false;
        }
        if (rvObject.contains("stdDev")) {
            double theStdValue = rvObject["stdDev"].toDouble();
            standardDev->setText(QString::number(theStdValue));
        } else {
            this->errorMessage("ERROR: LognormalDistribution - no \"stdDev\" entry");
            return false;
        }
    } else if (inpty==QString("Dataset")) {

      if (rvObject.contains("dataDir")) {
          QString theDataDir = rvObject["dataDir"].toString();
          dataDir->setText(theDataDir);
      } else {
          this->errorMessage("ERROR: LognormalDistribution - no \"dataDir\" entry");
          return false;
      }
    }

    this->updateDistributionPlot();
    return true;
}

QString 
LognormalDistribution::getAbbreviatedName(void) {
  return QString("Normal");
}

void
LognormalDistribution::updateDistributionPlot() {
    double la=0, ze=0, me=0, st=0;
    if ((this->inpty)==QString("Parameters")) {
        la=lambda->text().toDouble();
        ze=zeta->text().toDouble();
        me = exp(la+(pow(ze,2)/2));
        st = sqrt(exp(2*la+pow(ze,2))*(exp(pow(ze,2))-1));
     } else if ((this->inpty)==QString("Moments")) {
        me = mean->text().toDouble();
        st = standardDev->text().toDouble();
        la = log(pow(me,2)/sqrt(pow(st,2)+pow(me,2)));
        ze = sqrt(log(pow(st/me,2)+1));
    }

        if (st > 0.0) {
            double min = 0; // defined in x>0
            double max = me + 5*st;
            QVector<double> x(500);
            QVector<double> y(500);
            for (int i=0; i<500; i++) {
                double xi = min + i*(max-min)/499;
                x[i] = xi;
                y[i] = 1.0/(xi*ze*sqrt(2*3.141592))*exp( - pow((log(xi)-la)/ze,2)/2  );
            }
            thePlot->clear();
            thePlot->drawPDF(x,y);
        }

       if ((st <= 0.0)|| (me <= 0)) {
            thePlot->clear();
        }
}
