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

#include "UniformDistribution.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <SimCenterGraphPlot.h>
#include <math.h>
#include <QPushButton>


UniformDistribution::UniformDistribution(QWidget *parent) :RandomVariableDistribution(parent)
{
    //
    // create the main horizontal layout and add the input entries
    //

    QHBoxLayout *mainLayout = new QHBoxLayout();

    min = this->createTextEntry(tr("Min."), mainLayout);
    max = this->createTextEntry(tr("Max."), mainLayout);

    QPushButton *showPlotButton = new QPushButton("Show PDF");
    mainLayout->addWidget(showPlotButton);

    // initialPoint = this->createTextEntry(tr("Initial Point"), mainLayout);

    mainLayout->addStretch();

    // set some defaults, and set layout for widget to be the horizontal layout
    mainLayout->setSpacing(10);
    mainLayout->setMargin(0);
    this->setLayout(mainLayout);

    thePlot = new SimCenterGraphPlot(QString("x"),QString("Probability Densisty Function"), 500, 500);


    connect(min,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
    connect(max,SIGNAL(textEdited(QString)), this, SLOT(updateDistributionPlot()));
    connect(showPlotButton, &QPushButton::clicked, this, [=](){ thePlot->hide(); thePlot->show();});
}
UniformDistribution::~UniformDistribution()
{
    delete thePlot;
}

bool
UniformDistribution::outputToJSON(QJsonObject &rvObject){
    // check for error condition, an entry had no value
    if (min->text().isEmpty() || max->text().isEmpty()) {
        emit sendErrorMessage("ERROR: Uniform Distribution - data has not been set");
        return false;
    }

    rvObject["lowerbound"]=min->text().toDouble();
    rvObject["upperbound"]=max->text().toDouble();

    return true;
}

bool
UniformDistribution::inputFromJSON(QJsonObject &rvObject){
    //
    // for all entries, make sure i exists and if it does get it, otherwise return error
    //
    if (rvObject.contains("lowerbound")) {
        QJsonValue theValue = rvObject["lowerbound"];
        min->setText(QString::number(theValue.toDouble()));

        qDebug()<<QString::number(theValue.toDouble());


    } else {
        emit sendErrorMessage("ERROR: Uniform Distribution - no \"lowerbound\" entry");
        return false;
    }

    if (rvObject.contains("upperbound")) {
        QJsonValue theValue = rvObject["upperbound"];
        max->setText(QString::number(theValue.toDouble()));
        qDebug()<<QString::number(theValue.toDouble());
    } else {
        emit sendErrorMessage("ERROR: Uniform Distribution - no \"upperbound\" entry");
        return false;
    }

    this->updateDistributionPlot();
    return true;
}

QString 
UniformDistribution::getAbbreviatedName(void) {
  return QString("Uniform");
}

void
UniformDistribution::updateDistributionPlot() {
    double minV = min->text().toDouble();
    double maxV = max->text().toDouble();

    if (maxV > minV) {

        QVector<double> x(103);
        QVector<double> y(103);
        double delta = (maxV-minV)/10;
        x[0]=minV-delta; x[101]=maxV;
        x[1]=minV; x[102]=maxV+delta;
        y[0]=y[100]=y[101]=y[102]=0.;
        for (int i=1; i<100; i++) {
            double xi = minV + (i-1)*(maxV-minV)/98;
            x[i+1] = xi;
            y[i+1] =1.0/(maxV-minV);
        }
        thePlot->clear();
        thePlot->addLine(x,y);
    }
}
