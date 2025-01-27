/* *****************************************************************************
Copyright (c) 2016-2021, The Regents of the University of California (Regents).
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

// Written by: fmk

#include "SC_MultipleLineChart.h"
#include <QGraphicsSimpleTextItem>
#include <QStackedWidget>
#include <QComboBox>
#include <QLabel>


// following two functions modified a bit from code provided by chatGPT
double roundToNice(double stepSize) {
    double scale = std::pow(10, std::floor(std::log10(stepSize)));
    double normalized = stepSize / scale;

    if (normalized <= 1) return 1 * scale;
    else if (normalized <= 2) return 2 * scale;
    else if (normalized <= 3) return 3 * scale;
    else if (normalized <= 4) return 4 * scale;
    else if (normalized <= 5) return 5 * scale;
    else if (normalized <= 6) return 6 * scale;
    else if (normalized <= 7) return 7 * scale;
    else if (normalized <= 8) return 8 * scale;
    else if (normalized <= 9) return 9 * scale;
    else return 10 * scale;
}

double niceMaxY(double maxY, int numTicks) {
  double paddedY = 1.1 * maxY;
  double rawStepSize = paddedY / numTicks;
  double stepSize = roundToNice(rawStepSize);
  return std::ceil(paddedY / stepSize) * stepSize;
}
	    


SC_MLC_ChartData::SC_MLC_ChartData()
{
  title = "";
  xLabel = "x-axis";
  yLabel = "y-axis";
}

SC_MLC_ChartData::~SC_MLC_ChartData()
{
  qDebug() << "SC_MLC_ChartData Destructor";
}


SC_MLC_Chart::SC_MLC_Chart(QWidget *parent)
  :QWidget(parent), chartData(0), chart(0)
{
  chart = new QChart();
  QChartView *chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(chartView);
  this->setLayout(layout);  
}

SC_MLC_Chart::~SC_MLC_Chart()
{
  qDebug() << "SC_MLC_Chart Destructor";
}

int
SC_MLC_Chart::setData(SC_MLC_ChartData *theNewData)
{
  chartData = theNewData;

  //
  // delete old
  //

  chart->removeAllSeries();

  //
  // reset
  //
  
  chart->setTitle(theNewData->title);
  chart->legend()->setVisible(false);

  // add the Lines
  //   - note get maxY for setting Y axis limits

  // Label to display point coordinates
  QLabel *pointInfoLabel = new QLabel(this);
  pointInfoLabel->setAlignment(Qt::AlignCenter);
  pointInfoLabel->setText("");

  double maxY = 0;
  
  for (int i = 0; i < theNewData->theLines.size(); ++i) {

    QLineSeries *theSeries = theNewData->theLines[i];
    if (!theSeries->points().isEmpty()) {

      qDebug() << "Points in series:";
      for (const QPointF &point : theSeries->points()) {
        qDebug() << "Point:" << point.x() << " " << point.y();
      }
    
      // add QLineSeries to chart
      chart->addSeries(theSeries);
    
      // connect hovering over a point on to displaying the point values
      connect(theSeries, &QLineSeries::hovered, this, &SC_MLC_Chart::pointInfo);

      // check pionts against maxY
      for (const QPointF &point : theSeries->points()) {
        if (point.y() > maxY) {
	  maxY = point.y();
        }
      }
    }
  }

  
  QValueAxis *axisX = new QValueAxis();
  axisX->setTitleText(theNewData->xLabel);
  axisX->setLabelFormat("%.0f");
  axisX->setTickCount(11);  
  chart->addAxis(axisX, Qt::AlignBottom);

  // compute a nice value for maxY for display purposes
  double niceY = niceMaxY(maxY, 10);
  
  QValueAxis *axisY = new QValueAxis();
  axisY->setTitleText(theNewData->yLabel);
  axisY->setLabelFormat("%.0f");
  axisY->setRange(0, niceY);
  axisY->setTickCount(11);  
  chart->addAxis(axisY, Qt::AlignLeft);
 
  QPen axisPen(Qt::black); // Black axis line
  axisPen.setWidth(3);     // Thicker width
  axisX->setLinePen(axisPen);
  axisY->setLinePen(axisPen);

  for (int i = 0; i < theNewData->theLines.size(); ++i) {
    QLineSeries *theSeries = theNewData->theLines[i];
    if (!theSeries->points().isEmpty()) {    
      theSeries->attachAxis(axisX);
      theSeries->attachAxis(axisY);
    }
  }
}

// code from MC_Chart class developed by Sina
void SC_MLC_Chart::pointInfo(QPointF point, bool state)
{
  if (state) {
    if (!pointLocationText){
      pointLocationText = new QGraphicsSimpleTextItem("", chart);
    }
    
    QString m_text = QString("X: %1 \nY: %2 ").arg(point.x()).arg(point.y());
    
    pointLocationText->setText(m_text);
    pointLocationText->setPos((chart->mapToPosition(point))+ QPoint(15, -30));
    pointLocationText->show();
  } else {
    if (pointLocationText){
      pointLocationText->hide();
    }
  }
}


SC_MultipleLineChart::SC_MultipleLineChart(QWidget *parent)
{
  theSelection = new QComboBox();
  theStackedWidget = new QStackedWidget();
  QGridLayout *theLayout = new QGridLayout();
  theLayout->addWidget(new QLabel("Select Plot to Show"),0,0);
  theLayout->addWidget(theSelection,0,1);
  theLayout->addWidget(theStackedWidget,1,0,1,3);
  this->setLayout(theLayout);

 connect(theSelection, SIGNAL(currentIndexChanged(int)),
	  theStackedWidget, SLOT(setCurrentIndex(int)));

}

SC_MultipleLineChart::~SC_MultipleLineChart()
{
  theSelection->clear();
  while (theStackedWidget->count() > 0) {
    QWidget *widget = theStackedWidget->widget(0); // Get the first widget
    theStackedWidget->removeWidget(widget);        // Remove it from QStackedWidget
    delete widget;                                 // Delete the widget to free it's memory
  }
}

int
SC_MultipleLineChart::setData(QMap<QString, SC_MLC_ChartData *> *newData)
{
  //
  // clear old data
  //
  
  theSelection->clear();
  while (theStackedWidget->count() > 0) { // as in destructor, remove each and invoke destructor
    QWidget *widget = theStackedWidget->widget(0); 
    theStackedWidget->removeWidget(widget);        
    delete widget;                                 
  }

  //
  // for each map entry create a new widget and add widget to StackedWidget and QString to Combo box
  //  

  for (auto it = newData->begin(); it != newData->end(); ++it) {
    const QString &key = it.key();
    SC_MLC_ChartData *dataChart = newData->value(key); // don't use it.value as returns const T &
    SC_MLC_Chart *newChart = new SC_MLC_Chart();
    newChart->setData(dataChart);
    theStackedWidget->addWidget(newChart);
    theSelection->addItem(key);
  }
}
