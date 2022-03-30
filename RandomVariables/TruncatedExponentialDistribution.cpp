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

#include "TruncatedExponentialDistribution.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QDoubleValidator>
#include <SimCenterGraphPlot.h>
#include <math.h>
#include <QPushButton>
#include <QFileDialog>

TruncatedExponentialDistribution::TruncatedExponentialDistribution(QString inpType, QWidget *parent) :RandomVariableDistribution(parent)
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
    errorMsgLabel = new QLabel();
    errorMsgLabel -> setStyleSheet("QLabel { color : red; }");

    this->inpty=inpType;

    if (inpty==QString("Parameters"))
    {
        lambda = this->createTextEntry(tr("lambda"), mainLayout, 0);
         a  = this->createTextEntry(tr("Min."), mainLayout, 1);
         b  = this->createTextEntry(tr("Max."), mainLayout, 2);
        showPlotButton = new QPushButton("Show PDF");
        mainLayout->addWidget(showPlotButton, 1,3);


    } else if (inpty==QString("Moments")) {

        mean = this->createTextEntry(tr("Mean"), mainLayout, 0);
        a  = this->createTextEntry(tr("Min."), mainLayout, 1);
         b  = this->createTextEntry(tr("Max."), mainLayout, 2);
        showPlotButton = new QPushButton("Show PDF");
        mainLayout->addWidget(showPlotButton, 1,3);

    } else if (inpty==QString("Dataset")) {

        a  = this->createTextEntry(tr("Min."), mainLayout, 0);
        b  = this->createTextEntry(tr("Max."), mainLayout, 1);
        dataDir = this->createTextEntry(tr("Data File"), mainLayout, 2);
        dataDir->setMinimumWidth(200);
        dataDir->setMinimumWidth(200);

        QPushButton *chooseFileButton = new QPushButton("Choose");
        mainLayout->addWidget(chooseFileButton, 1,3);

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



    //mainLayout->addWidget(errorMsgLabel, 2, 0, 1, 4);
    mainLayout->addWidget(errorMsgLabel, 1, 4, 1, 4);

    mainLayout->setColumnStretch(4,1);


    thePlot = new SimCenterGraphPlot(QString("x"),QString("Probability Density Function"),500, 500);

    if (inpty==QString("Parameters")) {
        connect(lambda,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(a,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(b,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(showPlotButton, &QPushButton::clicked, this, [=](){ thePlot->hide(); thePlot->show();});
    } else if (inpty==QString("Moments")) {
        connect(mean,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(a,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(b,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
        connect(showPlotButton, &QPushButton::clicked, this, [=](){ thePlot->hide(); thePlot->show();});
    }
}

TruncatedExponentialDistribution::~TruncatedExponentialDistribution()
{
    delete thePlot;
}

bool
TruncatedExponentialDistribution::outputToJSON(QJsonObject &rvObject){

    if (validIdx==0)
    {
        this->errorMessage("ERROR: TruncatedExponentialDistribution - input parameters are not valid");
        return false;
    }

    if (inpty==QString("Parameters")) {
        // check for error condition, an entry had no value
        if ((lambda->text().isEmpty())||(a->text().isEmpty())||(b->text().isEmpty())) {
            this->errorMessage("ERROR: TruncatedExponentialDistribution - data has not been set");
            return false;
        }
        rvObject["lambda"]=lambda->text().toDouble();
        rvObject["a"]=a->text().toDouble();
        rvObject["b"]=b->text().toDouble();
    } else if (inpty==QString("Moments")) {
        if ((mean->text().isEmpty())||(a->text().isEmpty())||(b->text().isEmpty())) {
            this->errorMessage("ERROR: TruncatedExponentialDistribution - data has not been set");
            return false;
        }
        rvObject["mean"]=mean->text().toDouble();
        rvObject["a"]=a->text().toDouble();
        rvObject["b"]=b->text().toDouble();
    } else if (inpty==QString("Dataset")) {
        if (dataDir->text().isEmpty()||(a->text().isEmpty())||(b->text().isEmpty())) {
            this->errorMessage("ERROR: TruncatedExponentialDistribution - data has not been set");
            return false;
        }
        rvObject["a"]=a->text().toDouble();
        rvObject["b"]=b->text().toDouble();
        rvObject["dataDir"]=QString(dataDir->text());
    }
    return true;
}

bool
TruncatedExponentialDistribution::inputFromJSON(QJsonObject &rvObject){

    //
    // for all entries, make sure i exists and if it does get it, otherwise return error
    //

    if (rvObject.contains("inputType")) {
        inpty=rvObject["inputType"].toString();
    } else {
        inpty = "Parameters";
    }

    if (inpty==QString("Parameters")) {
        if (rvObject.contains("lambda")) {
            double thelambdaValue = rvObject["lambda"].toDouble();
            lambda->setText(QString::number(thelambdaValue));
        } else {
            this->errorMessage("ERROR: TruncatedExponentialDistribution - no \"a\" entry");
            return false;
        }
        if (rvObject.contains("a")) {
            double thezetaValue = rvObject["a"].toDouble();
            a->setText(QString::number(thezetaValue));
        } else {
            this->errorMessage("ERROR: TruncatedExponentialDistribution - no \"a\" entry");
            return false;
        }
        if (rvObject.contains("b")) {
            double thezetaValue = rvObject["b"].toDouble();
            b->setText(QString::number(thezetaValue));
        } else {
            this->errorMessage("ERROR: TruncatedExponentialDistribution - no \"a\" entry");
            return false;
        }


      } else if (inpty==QString("Moments")) {

        if (rvObject.contains("mean")) {
            double theMeanValue = rvObject["mean"].toDouble();
            mean->setText(QString::number(theMeanValue));
        } else {
            this->errorMessage("ERROR: TruncatedExponentialDistribution - no \"mean\" entry");
            return false;
        }
        if (rvObject.contains("a")) {
            double thezetaValue = rvObject["a"].toDouble();
            a->setText(QString::number(thezetaValue));
        } else {
            this->errorMessage("ERROR: TruncatedExponentialDistribution - no \"a\" entry");
            return false;
        }
        if (rvObject.contains("b")) {
            double thezetaValue = rvObject["b"].toDouble();
            b->setText(QString::number(thezetaValue));
        } else {
            this->errorMessage("ERROR: TruncatedExponentialDistribution - no \"a\" entry");
            return false;
        }

    } else if (inpty==QString("Dataset")) {

        if (rvObject.contains("a")) {
            double thezetaValue = rvObject["a"].toDouble();
            a->setText(QString::number(thezetaValue));
        } else {
            this->errorMessage("ERROR: TruncatedExponentialDistribution - no \"a\" entry");
            return false;
        }
        if (rvObject.contains("b")) {
            double thezetaValue = rvObject["b"].toDouble();
            b->setText(QString::number(thezetaValue));
        } else {
            this->errorMessage("ERROR: TruncatedExponentialDistribution - no \"a\" entry");
            return false;
        }

      if (rvObject.contains("dataDir")) {
          QString theDataDir = rvObject["dataDir"].toString();
          dataDir->setText(theDataDir);
      } else {
          this->errorMessage("ERROR: TruncatedExponentialDistribution - no \"mean\" entry");
          return false;
      }
    }

    this->updateDistributionPlot();
    return true;
}

bool
TruncatedExponentialDistribution::copyFiles(QString fileDir) {
    if (inpty==QString("Dataset")) {
        QFile::copy(dataDir->text(), fileDir);
        return QFile::copy(dataDir->text(), fileDir);
    } else {
        return true;
    }
}

QString 
TruncatedExponentialDistribution::getAbbreviatedName(void) {
  return QString("TruncatedExponential");
}

void
TruncatedExponentialDistribution::updateDistributionPlot() {
    double la=0, aa=0,bb=0, me=0;
    validIdx=0;
    QString errorMsg;

    if ((this->inpty)==QString("Parameters")) {
        la=lambda->text().toDouble();
        aa=a->text().toDouble();
        bb=b->text().toDouble();

        if ((bb-aa)<=0.0) {
            errorMsg = tr("* Max. should be greater than Min.");
            //this->errorMessage("ERROR: Max. should be greater than Min.");
        } else {
            errorMsg = tr("");
            //this->errorMessage(" ");
            validIdx=1;
        }

     } else if ((this->inpty)==QString("Moments")) {
        me = mean->text().toDouble();
        aa=a->text().toDouble();
        bb=b->text().toDouble();

        double lam1, lam0=0,mean1,mean0=0;
        double lamMin=0.0, lamMax=1/(me-aa);
        int nDiscrete=500;
        bool exitflag=0;
        for (int i=0; i<nDiscrete+1; i++) {
            lam1= lamMin + i*(lamMax-lamMin)/(nDiscrete-1);
            mean1 = integrateMean(lam1, aa, bb);
            if ((mean1-me)*(mean0-me)<0){
                exitflag=1;
                break;
            }
            mean0  = mean1;
            lam0    = lam1;
        }

        // only for very small lambdas
        if (exitflag==0){
            for (int i=0; i<nDiscrete; i++) {
                lam1= lamMin + i*(lamMax/nDiscrete-lamMin)/(nDiscrete-1);
                mean1 = integrateMean(lam1, aa, bb);
                if ((mean1-me)*(mean0-me)<0){
                    exitflag=1;
                    break;
                }
                mean0  = mean1;
                lam0    = lam1;
            }
        }
        la = lam0 + (me - mean0)*(lam1-lam0)/(mean1 - mean0);

        if ((bb-aa)<0.0) {
            errorMsg = tr("* Max. should be greater than Min.");
        } else if ((me <= aa) || (me >= aa+(bb-aa)/2)) {
            errorMsg = tr("* Mean must lie in the valid range: (Min, Min+(Max-Min)/2)");
        } else {
            errorMsg = tr("");
            validIdx=1;
        }
    }

    //
    // CHECK user-input ERRORS
    //

    errorMsgLabel -> setText(errorMsg);


    if (validIdx==1) {
        double prob = integrateProb(la, aa, bb);
        double min = aa-(bb-aa)*0.1; // defined in x>0
        double max = bb+(bb-aa)*0.1;;
        QVector<double> x(500);
        QVector<double> y(500);
        for (int i=0; i<500; i++) {
            double xi = min + i*(max-min)/499;
            x[i] = xi;
            if ((xi>aa) && (xi<bb) && (xi>0.0)){
                y[i] = la*exp(-la*xi)/prob;
            } else {
                y[i] = 0;
            }
        }
        thePlot->clear();
        thePlot->drawPDF(x,y);
    } else {
        thePlot->clear();
    }
}

double
TruncatedExponentialDistribution::integrateMean(double lam1, double lower, double upper) {
  double increment = (upper-lower)*0.001;
  double integralMean = 0.0, integralProb = 0.0;
  for(double x = lower; x < upper; x+=increment) {
    integralMean += ( x*lam1*exp(-lam1*x)  )  * increment;
    integralProb += ( lam1*exp(-lam1*x)  )  * increment;
  }
  return integralMean/integralProb;
}

double
TruncatedExponentialDistribution::integrateProb(double lam1, double lower, double upper) {
  double increment = (upper-lower)*0.001;

  double integralProb = 0.0;
  for(double x = lower; x < upper; x+=increment) {
    integralProb += ( lam1*exp(-lam1*x)  )  * increment;
  }
  return integralProb;
}
