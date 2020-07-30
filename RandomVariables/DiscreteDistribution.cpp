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

#include "DiscreteDistribution.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QDoubleValidator>
#include <SimCenterGraphPlot.h>
#include <math.h>
#include <QPushButton>
#include <QtCharts>
#include <QJsonArray>

DiscreteDistribution::DiscreteDistribution(QWidget *parent) :RandomVariableDistribution(parent)
{
    //
    // create the main horizontal layout and add the input entries
    //

    QHBoxLayout *mainLayout = new QHBoxLayout();

    values = this->createTextEntry(tr("Values"), mainLayout);
    weights = this->createTextEntry(tr("Weights"), mainLayout);
    values->setValidator(new myDoubleArrayValidator);
    weights->setValidator(new myDoubleArrayValidator);

    QPushButton *showPlotButton = new QPushButton("Show PMF");
    mainLayout->addWidget(showPlotButton);

    mainLayout->addStretch();

    // set some defaults, and set layout for widget to be the horizontal layout
    mainLayout->setSpacing(10);
    mainLayout->setMargin(0);
    this->setLayout(mainLayout);

    thePlot = new SimCenterGraphPlot(QString("x"),QString("Probability Mass Function"),500, 500);

    connect(values,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
    connect(weights,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
    connect(showPlotButton, &QPushButton::clicked, this, [=](){ thePlot->hide(); thePlot->show();});
}

DiscreteDistribution::~DiscreteDistribution()
{
    delete thePlot;
}

bool
DiscreteDistribution::outputToJSON(QJsonObject &rvObject){

    // check for error condition, an entry had no value
    if (values->text().isEmpty()) {
        emit sendErrorMessage("ERROR: DiscreteDistribution - data (Values) has not been set");
        return false;
    }
    if (weights->text().isEmpty()) {
        emit sendErrorMessage("ERROR: DiscreteDistribution - data (Weights) has not been set");
        return false;
    }

    QStringList listValues = values-> text().split(",");
    QStringList listWeights = weights->text().split(",");

    if (!(listValues.length() == listWeights.length())) {
        emit sendErrorMessage("ERROR: DiscreteDistribution - lengths of 'Values' and 'Weights' should be the same");
        return false;
    }

    QJsonArray x, w;
    for (int i=0; i<std::min(listValues.size(),listWeights.size()); i++) {
        x.push_back(listValues[i].toDouble());
        w.push_back(listWeights[i].toDouble());
    }

    rvObject["Values"]=x;
    rvObject["Weights"]=w;
    return true;
}

bool
DiscreteDistribution::inputFromJSON(QJsonObject &rvObject){

    //
    // for all entries, make sure i exists and if it does get it, otherwise return error
    //

    if (rvObject.contains("Weights")) {
        QJsonValue weightVals = rvObject["Weights"];
        QJsonArray weightArrs = weightVals.toArray();
        QString weightString;
        weightString.push_back(QString::number(weightVals[0].toDouble()));
        for (int i=1; i < weightArrs.size() ; i++ ) {
                weightString.push_back(",");
                weightString.push_back(QString::number(weightVals[i].toDouble()));
        }
        weights->setText(weightString);

    } else {
        emit sendErrorMessage("ERROR: DiscreteDistribution - no \"weights\" entry");
        return false;
    }

    if (rvObject.contains("Values")) {
        QJsonValue valueVals = rvObject["Values"];
        QJsonArray valueArrs = valueVals.toArray();
        QString valueString;
        valueString.push_back(QString::number(valueVals[0].toDouble()));
        for (int i=1; i < valueArrs.size() ; i++ ) {
                valueString.push_back(",");
                valueString.push_back(QString::number(valueVals[i].toDouble()));
        }
        values->setText(valueString);
    } else {
        emit sendErrorMessage("ERROR: DiscreteDistribution - no \"values\" entry");
        return false;
    }

    this->updateDistributionPlot();
    return true;
}



QString 
DiscreteDistribution::getAbbreviatedName(void) {
  return QString("Discrete");
}

void
DiscreteDistribution::updateDistributionPlot() {

    QStringList listValues = values-> text().split(",");
    QStringList listWeights = weights->text().split(",");

    //
    // find vector points (x,w) to draw a barchart
    //

    QVector<double> x, w_tmp, w;
    double wsum=0;
    for (int i=0; i<std::min(listValues.size(),listWeights.size()); i++) {
        x.push_back(listValues[i].toDouble());
        w_tmp.push_back(listWeights[i].toDouble());
        wsum +=listWeights[i].toDouble();
    }
    for (int i=0; i<std::min(listValues.size(),listWeights.size()); i++) {
        w.push_back(w_tmp[i]/wsum);
    }

    //
    // for barchart
    //

    if (x.size()>0) {
        // Will not draw plot after comma
         if (!(listValues.back().size()==0) && !(listWeights.back().size()==0)) {
            QVector<double> barx,bary,x_tmp=x, x_diff(x.size());

            std::sort(x_tmp.begin(), x_tmp.end());
            std::adjacent_difference(x_tmp.begin(), x_tmp.end(), x_diff.begin());

            double xmin = *std::min_element(x.constBegin(), x.constEnd());
            double xmax = *std::max_element(x.constBegin(), x.constEnd());
            double xgapmin = *std::min_element(x_diff.constBegin()+1, x_diff.constEnd());

            while ( (xgapmin<1.e-10) && (x_diff.size()>1) ) {
                x_diff.erase(x_diff.begin());
                xgapmin = *std::min_element(x_diff.constBegin()+1, x_diff.constEnd());
            }
            if (x_diff.size()==1){
                xgapmin=2.5;
            }
            //double dx = (xmax-xmin)/x.size()*0.4;
            double dx = xgapmin*0.4;
            barx.push_back(xmin-dx*2);
            bary.push_back(0.0);
            for (int i=0; i<std::min(listValues.size(),listWeights.size()); i++) {
                barx.insert(barx.end(),2, x[i]-dx);
                barx.insert(barx.end(),2, x[i]+dx);
                bary.push_back(0.0);
                bary.push_back(w[i]);
                bary.push_back(w[i]);
                bary.push_back(0.0);
            }
            barx.push_back(xmax+dx*2);
            bary.push_back(0.0);

          thePlot->clear();
          thePlot->addLine(barx,bary);
        }
    }
}

myDoubleArrayValidator::myDoubleArrayValidator(QObject *parent) :
    QValidator(parent)
{
}

QValidator::State myDoubleArrayValidator::validate(QString &input, int &pos) const
{
    qDebug() << input<< pos;
    QDoubleValidator val;
    val.setLocale(QLocale(QLocale::German,QLocale::Germany));
    input.remove(" ");
    //input.replace(","," ");
    QStringList list = input.split(",");
    //QStringList list = input.split("//s+");
        foreach ( QString var, list) {
        int i = 0;
        if(val.validate(var,i) == QValidator::Invalid)
            return QValidator::Invalid;
    }
    return QValidator::Acceptable;
}
