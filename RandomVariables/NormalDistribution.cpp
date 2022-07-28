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

#include "NormalDistribution.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QDoubleValidator>
#include <SimCenterGraphPlot.h>
#include <math.h>
#include <QPushButton>
#include <QFileDialog>

NormalDistribution::NormalDistribution(QString inpType, QWidget *parent) :RandomVariableDistribution(parent)
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

    if ((inpty==QString("Parameters"))||(inpty==QString("Moments"))) {

        mean = this->createTextEntry(tr("Mean"), mainLayout,0);
        standardDev = this->createTextEntry(tr("Standard Dev"), mainLayout,1);
        //standardDev->setValidator(new QDoubleValidator);
        showPlotButton = new QPushButton("Show PDF");
        mainLayout->addWidget(showPlotButton,1,2);
        mainLayout->setColumnStretch(3,1);

    } else if (inpty==QString("Dataset")) {

        dataDir = this->createTextEntry(tr("Data File"), mainLayout,0);
        dataDir->setMinimumWidth(200);
        dataDir->setMaximumWidth(200);
        QPushButton *chooseFileButton = new QPushButton("Choose");
        mainLayout->addWidget(chooseFileButton,1,1);
        mainLayout->setColumnStretch(2,1);

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

    thePlot = new SimCenterGraphPlot(QString("x"),QString("Probability Density Function"),500, 500);

    if ((inpty==QString("Parameters"))||(inpty==QString("Moments"))) {
        connect(mean,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(standardDev,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(showPlotButton, &QPushButton::clicked, this, [=](){ thePlot->hide(); thePlot->show();});
    }
}

NormalDistribution::~NormalDistribution()
{
    delete thePlot;
}

NormalDistribution::NormalDistribution(double initValue, QWidget *parent) :RandomVariableDistribution(parent)
{
    //
    // create the main layout and add the input entries
    //
    QGridLayout *mainLayout = new QGridLayout(this);

    // set some defaults, and set layout for widget to be the horizontal layout
    mainLayout->setHorizontalSpacing(10);
    mainLayout->setVerticalSpacing(0);
    mainLayout->setMargin(0);

    QPushButton *showPlotButton = new QPushButton("Show PDF");

    //Parameters
    this->inpty = "Parameters";
    mean = this->createTextEntry(tr("Mean"), mainLayout,0);
    standardDev = this->createTextEntry(tr("Standard Dev"), mainLayout,1);

    mainLayout->addWidget(showPlotButton,1,2);
    mainLayout->setColumnStretch(3,1);

    thePlot = new SimCenterGraphPlot(QString("x"),QString("Probability Density Function"),500, 500);

    connect(mean,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
    connect(standardDev,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
    connect(showPlotButton, &QPushButton::clicked, this, [=](){ thePlot->hide(); thePlot->show();});


    // set initial or Disabled
    mean->setText(QString::number(initValue));
    standardDev->setText(QString::number(fabs(initValue)*0.1)); // 0.1 c.o.v
    this->updateDistributionPlot();
}

bool
NormalDistribution::outputToJSON(QJsonObject &rvObject){

    if ((inpty==QString("Parameters")) || (inpty==QString("Moments"))) {
        if (mean->text().isEmpty() || standardDev->text().isEmpty()) {
            this->errorMessage("ERROR: NormalDistribution - data has not been set");
            return false;
        }
        rvObject["mean"]=mean->text().toDouble();
        rvObject["stdDev"]=standardDev->text().toDouble();
        return true;

    } else if (inpty==QString("Dataset")) {
        if (dataDir->text().isEmpty()) {
            this->errorMessage("ERROR: LognormalDistribution - data has not been set");
            return false;
        }
        rvObject["dataDir"]=QString(dataDir->text());
        return true;
    }
}

bool
NormalDistribution::inputFromJSON(QJsonObject &rvObject){

    //
    // for all entries, make sure i exists and if it does get it, otherwise return error
    //

    if (rvObject.contains("inputType")) {
        inpty=rvObject["inputType"].toString();
    } else {
        inpty = "Parameters";
    }
    if ((inpty==QString("Parameters")) || (inpty==QString("Moments"))) {

        if (rvObject.contains("mean")) {
            QJsonValue theMeanValue = rvObject["mean"];
            mean->setText(QString::number(theMeanValue.toDouble()));
        } else {
            this->errorMessage("ERROR: NormalDistribution - no \"mean\" entry");
            return false;
        }

        if (rvObject.contains("stdDev")) {
            QJsonValue theStdDevValue = rvObject["stdDev"];
            standardDev->setText(QString::number(theStdDevValue.toDouble()));
        } else {
            this->errorMessage("ERROR: NormalDistribution - no \"stdDev\" entry");
            return false;
        }

    } else if (inpty==QString("Dataset")) {

      if (rvObject.contains("dataDir")) {
          QString theDataDir = rvObject["dataDir"].toString();
          dataDir->setText(theDataDir);
      } else {
          this->errorMessage("ERROR: NormalDistribution - no \"mean\" entry");
          return false;
      }
    }

    this->updateDistributionPlot();
    return true;
}

bool
NormalDistribution::copyFiles(QString fileDir) {
    if (inpty==QString("Dataset")) {
        return QFile::copy(dataDir->text(), fileDir);
    } else {
        return true;
    }
}

QString
NormalDistribution::getAbbreviatedName(void) {
  return QString("Normal");
}

void
NormalDistribution::updateDistributionPlot() {
    if ((this->inpty)==QString("Parameters") || (this->inpty)==QString("Moments")) {
        double me = mean->text().toDouble();
        double st =standardDev->text().toDouble();
        if (st < 0.0) {
            thePlot->clear();
            return;
        }
        double min = me - 5*st;
        double max = me + 5*st;
        QVector<double> x(100);
        QVector<double> y(100);
        for (int i=0; i<100; i++) {
            double xi = min + i*(max-min)/99;
            x[i] = xi;
            y[i] =1.0/(sqrt(2*3.1415926535)*st)*exp(-(0.5*(xi-me)*(xi-me)/(st*st)));
        }
        thePlot->clear();
        thePlot->drawPDF(x,y);
    }

}
