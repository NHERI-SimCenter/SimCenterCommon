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

#include "WeibullDistribution.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QDoubleValidator>
#include <SimCenterGraphPlot.h>
#include <math.h>
#include <QPushButton>
#include <QFileDialog>

WeibullDistribution::WeibullDistribution(QString inpType, QWidget *parent) :RandomVariableDistribution(parent)
{
    //
    // create the main layout and add the input entries
    //
    QGridLayout *mainLayout = new QGridLayout(this);

    // set some defaults, and set layout for widget to be the horizontal layout
    mainLayout->setHorizontalSpacing(10);
    mainLayout->setVerticalSpacing(0);
    mainLayout->setMargin(0);

    QPushButton *showPlotButton = NULL; // new QPushButton("Show PDF");


    this->inpty=inpType;

    if (inpty==QString("Parameters"))
    {
        an = this->createTextEntry(tr("an (scale)"), mainLayout, 0);
        k  = this->createTextEntry(tr("k (shape)"), mainLayout, 1);
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
        dataDir->setMaximumWidth(200);

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
        connect(an,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(k,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(showPlotButton, &QPushButton::clicked, this, [=](){ thePlot->hide(); thePlot->show();});
    } else if (inpty==QString("Moments")) {
        connect(mean,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(standardDev,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(showPlotButton, &QPushButton::clicked, this, [=](){ thePlot->hide(); thePlot->show();});
    }
}

WeibullDistribution::~WeibullDistribution()
{
    delete thePlot;
}

bool
WeibullDistribution::outputToJSON(QJsonObject &rvObject){

    if (inpty==QString("Parameters")) {
        // check for error condition, an entry had no value
        if ((an->text().isEmpty())||(k->text().isEmpty())) {
            this->errorMessage("ERROR: WeibullDistribution - data has not been set");
            return false;
        }
        rvObject["scaleparam"]=an->text().toDouble();
        rvObject["shapeparam"]=k->text().toDouble();
    } else if (inpty==QString("Moments")) {
        if ((mean->text().isEmpty())||(standardDev->text().isEmpty())) {
            this->errorMessage("ERROR: WeibullDistribution - data has not been set");
            return false;
        }
        rvObject["mean"]=mean->text().toDouble();
        rvObject["standardDev"]=standardDev->text().toDouble();
    } else if (inpty==QString("Dataset")) {
        if (dataDir->text().isEmpty()) {
            this->errorMessage("ERROR: WeibullDistribution - data has not been set");
            return false;
        }
        rvObject["dataDir"]=QString(dataDir->text());
    }
    return true;
}

bool
WeibullDistribution::inputFromJSON(QJsonObject &rvObject){

    //
    // for all entries, make sure i exists and if it does get it, otherwise return error
    //


    if (rvObject.contains("inputType")) {
        inpty=rvObject["inputType"].toString();
    } else {
        inpty = "Parameters";
    }

    if (inpty==QString("Parameters")) {
        if (rvObject.contains("scaleparam")) {
            double theMuValue = rvObject["scaleparam"].toDouble();
            an->setText(QString::number(theMuValue));
        } else {
            this->errorMessage("ERROR: WeibullDistribution - no \"a\" entry");
            return false;
        }
        if (rvObject.contains("shapeparam")) {
            double theSigValue = rvObject["shapeparam"].toDouble();
            k->setText(QString::number(theSigValue));
        } else {
            this->errorMessage("ERROR: WeibullDistribution - no \"a\" entry");
            return false;
        }
      } else if (inpty==QString("Moments")) {

        if (rvObject.contains("mean")) {
            double theMeanValue = rvObject["mean"].toDouble();
            mean->setText(QString::number(theMeanValue));
        } else {
            this->errorMessage("ERROR: WeibullDistribution - no \"mean\" entry");
            return false;
        }
        if (rvObject.contains("standardDev")) {
            double theStdValue = rvObject["standardDev"].toDouble();
            standardDev->setText(QString::number(theStdValue));
        } else {
            this->errorMessage("ERROR: WeibullDistribution - no \"mean\" entry");
            return false;
        }
    } else if (inpty==QString("Dataset")) {

      if (rvObject.contains("dataDir")) {
          QString theDataDir = rvObject["dataDir"].toString();
          dataDir->setText(theDataDir);
      } else {
          this->errorMessage("ERROR: WeibullDistribution - no \"mean\" entry");
          return false;
      }
    }

    this->updateDistributionPlot();
    return true;
}

bool
WeibullDistribution::copyFiles(QString fileDir) {
    if (inpty==QString("Dataset")) {
        return QFile::copy(dataDir->text(), fileDir);
    } else {
        return true;
    }
}

QString 
WeibullDistribution::getAbbreviatedName(void) {
  return QString("Weibull");
}

void
WeibullDistribution::updateDistributionPlot() {
    double aa=0, kk=0, me=0, st=0;
    if ((this->inpty)==QString("Parameters")) {
        aa=an->text().toDouble();
        kk=k->text().toDouble();
        me =aa*tgamma(1+1/kk);
        st =aa*sqrt(tgamma(1+2/kk)-tgamma(1+1/kk)*tgamma(1+1/kk));

     } else if ((this->inpty)==QString("Moments")) {
        me = mean->text().toDouble();
        st = standardDev->text().toDouble();
        double coeffVar = st/me;
        // APPROXIMATED parameters ONLY for visualization
        // interpolations for method of moments
        if ((coeffVar>0.02) && (coeffVar<100)){
            double k1,k0,cov1,cov0;
            int nDiscrete=500;
            for (int i=0; i<nDiscrete; i++) {
                // 0.012 came from max cov=100, 100 came from min cov=0.02
                double xi = 0.012 + i*(100-0.012)/(nDiscrete-1);
                k1 = xi;
                cov1 =(tgamma(1+2/k1)-tgamma(1+1/k1)*tgamma(1+1/k1))/tgamma(1+1/k1);
                if (cov1 < st/me){
                    break;
                }
                cov0  = cov1;
                k0    = k1;
            }
            kk = k0 + (st/me - cov0)*(k1-k0)/(cov1 - cov0);
            aa = me/tgamma(1+1/kk);
        }
    }

    if (kk > 0.0 && aa > 0.0) {
        double min = me - 5*st;
        if (min < 0.0) min = 1.0e-3;
        double max = me + 5*st;
        QVector<double> x(500);
        QVector<double> y(500);
        for (int i=0; i<500; i++) {
            double xi = min + i*(max-min)/499.0;
            x[i] = xi;
            y[i] =(kk/aa)*(pow((xi/aa),(kk-1))*exp(-(pow((xi/aa),kk))));
        }
        thePlot->clear();
        thePlot->drawPDF(x,y);
    } else {
        thePlot->clear();
    }

}
