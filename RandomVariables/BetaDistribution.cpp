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

#include "BetaDistribution.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QDoubleValidator>
#include <SimCenterGraphPlot.h>
#include <math.h>
#include <QPushButton>
#include <QFileDialog>

BetaDistribution::BetaDistribution(QString inpType, QWidget *parent) :RandomVariableDistribution(parent)
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
        alpha = this->createTextEntry(tr("alpha"), mainLayout, 0);
        beta  = this->createTextEntry(tr("beta"), mainLayout, 1);
        a = this->createTextEntry(tr("Min."), mainLayout, 2);
        b  = this->createTextEntry(tr("Max."), mainLayout, 3);
        showPlotButton = new QPushButton("Show PDF");
        mainLayout->addWidget(showPlotButton, 1,4);

        mainLayout->setColumnStretch(5,1);

    } else if (inpty==QString("Moments")) {

        mean = this->createTextEntry(tr("Mean"), mainLayout, 0);
        standardDev = this->createTextEntry(tr("Standard Dev"), mainLayout, 1);
        a = this->createTextEntry(tr("Min."), mainLayout, 2);
        b  = this->createTextEntry(tr("Max."), mainLayout, 3);
        showPlotButton = new QPushButton("Show PDF");
        mainLayout->addWidget(showPlotButton, 1,4);

        mainLayout->setColumnStretch(5,1);

    } else if (inpty==QString("Dataset")) {

        a = this->createTextEntry(tr("Min."), mainLayout, 0);
        b  = this->createTextEntry(tr("Max."), mainLayout, 1);
        dataDir = this->createTextEntry(tr("Data File"), mainLayout, 2);
        dataDir->setMinimumWidth(200);
        dataDir->setMaximumWidth(200);

        QPushButton *chooseFileButton = new QPushButton("Choose");
        mainLayout->addWidget(chooseFileButton, 1, 3);

        mainLayout->setColumnStretch(4,1);

        connect(chooseFileButton, &QPushButton::clicked, this, [=](){
                  QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),"", "All files (*)");
                  if (!fileName.isEmpty()) {
                      dataDir->setText(fileName);
                  }
              });
    }

    thePlot = new SimCenterGraphPlot(QString("x"),QString("Probability Density Function"),500, 500);

    if (inpty==QString("Parameters")) {
        connect(alpha,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(beta,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(a,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(b,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(showPlotButton, &QPushButton::clicked, this, [=](){ thePlot->hide(); thePlot->show();});
    } else if (inpty==QString("Moments")) {
        connect(mean,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(standardDev,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(a,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(b,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(showPlotButton, &QPushButton::clicked, this, [=](){ thePlot->hide(); thePlot->show();});
    }
}

BetaDistribution::~BetaDistribution()
{
    delete thePlot;
}

bool
BetaDistribution::outputToJSON(QJsonObject &rvObject){

    if (inpty==QString("Parameters")) {
        // check for error condition, an entry had no value
        if ((alpha->text().isEmpty())||(beta->text().isEmpty())||(a->text().isEmpty())||(b->text().isEmpty())) {
            this->errorMessage("ERROR: BetaDistribution - data has not been set");
            return false;
        }
        rvObject["alphas"]=alpha->text().toDouble();
        rvObject["betas"]=beta->text().toDouble();
        rvObject["lowerbound"]=a->text().toDouble();
        rvObject["upperbound"]=b->text().toDouble();
    } else if (inpty==QString("Moments")) {
        if ((mean->text().isEmpty())||(standardDev->text().isEmpty())||(a->text().isEmpty())||(b->text().isEmpty())) {
            this->errorMessage("ERROR: BetaDistribution - data has not been set");
            return false;
        }
        rvObject["mean"]=mean->text().toDouble();
        rvObject["standardDev"]=standardDev->text().toDouble();
        rvObject["lowerbound"]=a->text().toDouble();
        rvObject["upperbound"]=b->text().toDouble();
    } else if (inpty==QString("Dataset")) {
        if (dataDir->text().isEmpty()) {
            this->errorMessage("ERROR: BetaDistribution - data has not been set");
            return false;
        }
        rvObject["lowerbound"]=a->text().toDouble();
        rvObject["upperbound"]=b->text().toDouble();
        rvObject["dataDir"]=QString(dataDir->text());
    }
    return true;
}

bool
BetaDistribution::inputFromJSON(QJsonObject &rvObject){

    //
    // for all entries, make sure i exists and if it does get it, otherwise return error
    //

    if (rvObject.contains("inputType")) {
        inpty=rvObject["inputType"].toString();
    } else {
        inpty = "Parameters";
    }

    if (inpty==QString("Parameters")) {
        if (rvObject.contains("alphas")) {
            double theAlphaValue = rvObject["alphas"].toDouble();
            alpha->setText(QString::number(theAlphaValue));
        } else {
            this->errorMessage("ERROR: BetaDistribution - no \"alpha\" entry");
            return false;
        }
        if (rvObject.contains("betas")) {
            double theBetaValue = rvObject["betas"].toDouble();
            beta->setText(QString::number(theBetaValue));
        } else {
            this->errorMessage("ERROR: BetaDistribution - no \"beta\" entry");
            return false;
        }
        if (rvObject.contains("lowerbound")) {
            double theAValue = rvObject["lowerbound"].toDouble();
            a->setText(QString::number(theAValue));
        } else {
            this->errorMessage("ERROR: BetaDistribution - no \"a\" entry");
            return false;
        }
        if (rvObject.contains("upperbound")) {
            double theBValue = rvObject["upperbound"].toDouble();
            b->setText(QString::number(theBValue));
        } else {
            this->errorMessage("ERROR: BetaDistribution - no \"b\" entry");
            return false;
        }

      } else if (inpty==QString("Moments")) {

        if (rvObject.contains("mean")) {
            double theMeanValue = rvObject["mean"].toDouble();
            mean->setText(QString::number(theMeanValue));
        } else {
            this->errorMessage("ERROR: BetaDistribution - no \"mean\" entry");
            return false;
        }
        if (rvObject.contains("standardDev")) {
            double theStdValue = rvObject["standardDev"].toDouble();
            standardDev->setText(QString::number(theStdValue));
        } else {
            this->errorMessage("ERROR: BetaDistribution - no \"mean\" entry");
            return false;
        }
        if (rvObject.contains("lowerbound")) {
            double theAValue = rvObject["lowerbound"].toDouble();
            a->setText(QString::number(theAValue));
        } else {
            this->errorMessage("ERROR: BetaDistribution - no \"a\" entry");
            return false;
        }
        if (rvObject.contains("upperbound")) {
            double theBValue = rvObject["upperbound"].toDouble();
            b->setText(QString::number(theBValue));
        } else {
            this->errorMessage("ERROR: BetaDistribution - no \"b\" entry");
            return false;
        }

    } else if (inpty==QString("Dataset")) {

      if (rvObject.contains("dataDir")) {
          QString theDataDir = rvObject["dataDir"].toString();
          dataDir->setText(theDataDir);
      } else {
          this->errorMessage("ERROR: BetaDistribution - no \"mean\" entry");
          return false;
      }
    }

    this->updateDistributionPlot();
    return true;
}

bool BetaDistribution::copyFiles(QString fileDir) {
    if (inpty==QString("Dataset")) {
        return QFile::copy(dataDir->text(), fileDir);
    } else {
        return true;
    }
}


QString
BetaDistribution::getAbbreviatedName(void) {
  return QString("Beta");
}

void
BetaDistribution::updateDistributionPlot() {
    double alp=0, bet=0, aa=0, bb=0, me=0, st=0;
    if ((this->inpty)==QString("Parameters")) {
        alp=alpha->text().toDouble();
        bet=beta->text().toDouble();
        aa=a->text().toDouble();
        bb=b->text().toDouble();
        me = (aa*bet+bb*alp)/(alp+bet);
        st = sqrt( alp*bet*(bb-aa)/pow(alp+bet,2)/(alp+bet+1)  );
     } else if ((this->inpty)==QString("Moments")) {
        me = mean->text().toDouble();
        st = standardDev->text().toDouble();
        aa=a->text().toDouble();
        bb=b->text().toDouble();
        alp = ((bb-me)*(me-aa)/pow(st,2)-1)*(me-aa)/(bb-aa);
        bet = alp*(bb-me)/(me-aa);
    }
    if (aa>bb) {
        thePlot->clear();
        return;
    }

        if (alp >= 0.0 && bet > 0.0 && me != aa) {
            double min = aa; // defined in x>0
            double max = bb;
            QVector<double> x(100);
            QVector<double> y(100);
            for (int i=0; i<100; i++) {
                double xi = min + i*(max-min)/99;
                x[i] = xi;
                double betai=tgamma(alp)*tgamma(bet)/tgamma(alp+bet);
                y[i] = pow(xi-aa,alp-1)*pow(bb-xi,bet-1)/betai/pow(bb-aa,alp+bet-1);
            }
            thePlot->clear();
            thePlot->drawPDF(x,y);
        } else {
            thePlot->clear();
        }

//        else {
//            QVector<double> x(100);
//            QVector<double> y(100);
//            QVector<double> x1(100);
//            QVector<double> y1(100);
//            for (int i=0; i<100; i++) {
//                x[i] =  aa+1;
//                y[i] =  bb+10;
//                x1[i] =  me+1;
//                y1[i] =  st+10;
//            }
//            thePlot->clear();
//            thePlot->drawPDF(x,y);
//            thePlot->drawPDF(x1,y1);
//        }
}
