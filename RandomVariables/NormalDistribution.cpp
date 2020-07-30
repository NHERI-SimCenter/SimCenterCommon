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
#include <QVBoxLayout>
#include <QHBoxLayout>
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
    // create the main horizontal layout and add the input entries
    //

    QHBoxLayout *mainLayout = new QHBoxLayout();
    QPushButton *showPlotButton = new QPushButton("Show PDF");

    this->inpty=inpType;

    if ((inpty==QString("Parameters"))||(inpty==QString("Moments"))) {

        mean = this->createTextEntry(tr("Mean"), mainLayout);
        standardDev = this->createTextEntry(tr("Standard Dev"), mainLayout);
        mean->setValidator(new QDoubleValidator);
        standardDev->setValidator(new QDoubleValidator);

        mainLayout->addWidget(showPlotButton);

    } else if (inpty==QString("Dataset")) {

        dataDir = this->createTextEntry(tr("Data dir"), mainLayout);
        dataDir->setMinimumWidth(200);
        dataDir->setMaximumWidth(200);
        QPushButton *chooseFileButton = new QPushButton("Choose");
        mainLayout->addWidget(chooseFileButton);

        // Action
        connect(chooseFileButton, &QPushButton::clicked, this, [=](){
                dataDir->setText(QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)"));
        });
    }


    mainLayout->addStretch();

    // set some defaults, and set layout for widget to be the horizontal layout
    mainLayout->setSpacing(10);
    mainLayout->setMargin(0);
    this->setLayout(mainLayout);

    thePlot = new SimCenterGraphPlot(QString("x"),QString("Probability Densisty Function"),500, 500);

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

bool
NormalDistribution::outputToJSON(QJsonObject &rvObject){

    if ((inpty==QString("Parameters")) || (inpty==QString("Moments"))) {
        if (mean->text().isEmpty() || standardDev->text().isEmpty()) {
            emit sendErrorMessage("ERROR: NormalDistribution - data has not been set");
            return false;
        }
        rvObject["mean"]=mean->text().toDouble();
        rvObject["stdDev"]=standardDev->text().toDouble();
        return true;
    }
}

bool
NormalDistribution::inputFromJSON(QJsonObject &rvObject){

    //
    // for all entries, make sure i exists and if it does get it, otherwise return error
    //

    if ((this->inpty==QString("Parameters")) || (this->inpty==QString("Moments"))) {

        if (rvObject.contains("mean")) {
            QJsonValue theMeanValue = rvObject["mean"];
            mean->setText(QString::number(theMeanValue.toDouble()));
        } else {
            emit sendErrorMessage("ERROR: NormalDistribution - no \"mean\" entry");
            return false;
        }

        if (rvObject.contains("stdDev")) {
            QJsonValue theStdDevValue = rvObject["stdDev"];
            standardDev->setText(QString::number(theStdDevValue.toDouble()));
        } else {
            emit sendErrorMessage("ERROR: NormalDistribution - no \"stdDev\" entry");
            return false;
        }

    } else if (this->inpty==QString("Dataset")) {

      if (rvObject.contains("dataDir")) {
          QString theDataDir = rvObject["dataDir"].toString();
          dataDir->setText(theDataDir);
      } else {
          emit sendErrorMessage("ERROR: NormalDistribution - no \"mean\" entry");
          return false;
      }
    }

    this->updateDistributionPlot();
    return true;
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
        if (st > 0.0) {
            double min = me - 5*st;
            double max = me + 5*st;
            QVector<double> x(100);
            QVector<double> y(100);
            for (int i=0; i<100; i++) {
                double xi = min + i*(max-min)/99;
                x[i] = xi;
                y[i] =1.0/(sqrt(2*3.141159)*st)*exp(-(0.5*(xi-me)*(xi-me)/(st*st)));
            }
            thePlot->clear();
            thePlot->addLine(x,y);
        }
    }
}
