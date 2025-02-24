#ifndef SC_MULTIPLE_LINE_CHART_H
#define SC_MULTIPLE_LINE_CHART_H

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

#include <QWidget>
#include <QList>
#include <QString>
#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using namespace QtCharts;
#endif

class QStackedWidget;
class QComboBox;
class QGraphicsSimpleTextItem;

// SC_MultipleLineChart is a QWidget that displays multiple line plots. There is a QComboBox that
// selects which line plot is to be displayed. Each line plot is an SC_MLC_Chart object. The data for each
// line plot is self contained in an SC_MLC_ChartData object. There is one method in the interface for both
// the SC_MLC_Chart and SC_MultipleLineChart classes, namely setData. setData() will invoke destructors on
// any previos data passed (including QLineSeries*).

// NOTE: I am not nesting SC_MLC_Chart in the SC_MultipleLineChart class in case it is useful elsewhere,
// but I am not putting it in it's own file as there are already too many of these Chart classes!

//
// SC_MLC_ChartData : the data structure used to keep info on what is to be plotted
//

class SC_MLC_ChartData {

public:
  SC_MLC_ChartData();  
  ~SC_MLC_ChartData();
  QList<QLineSeries *> theLines;   // one or more Lines
  QStringList  theLineNames;       // unique name of SC_MLC_ChartData, needed for selection of
  QString title;                   // plot title
  QString xLabel;                  // xaxis label
  QString yLabel;                  // y axis label
  bool showLegend;
};


//
// SC_MLC_ChartData : the class that will plot one SC_MLC_ChartData object
//
  
class SC_MLC_Chart : public QWidget {

  Q_OBJECT
  
public:
  
  SC_MLC_Chart(QWidget *parent = 0);
  ~SC_MLC_Chart();  
  int setData(SC_MLC_ChartData *);

public slots:
  
private:
  SC_MLC_ChartData *chartData;
  QChart *chart;
  QGraphicsSimpleTextItem *chartPointText;
}; 


// utility class to spit out series name and coords when hovered over

class ChartHandler : public QObject {

  Q_OBJECT
  
public:
  ChartHandler(QLineSeries *series, QGraphicsSimpleTextItem *output, QChart *chart, QObject *parent = nullptr);

public slots:
  void onPointHovered(const QPointF &point, bool state);

private:
  QLineSeries *lineSeries;
  QChart      *chart;
  QGraphicsSimpleTextItem *chartTextItem;    
};


//
// SC_MLC_ChartData : the data structure used to keep info on what is to be plotted
//
  
class SC_MultipleLineChart : public QWidget {

  Q_OBJECT
  
public:
  SC_MultipleLineChart(QWidget *parent = 0);
  ~SC_MultipleLineChart();  
  int setData(QMap<QString, SC_MLC_ChartData *> *newData);
									   
public slots:
  
private:
  QComboBox *theSelection;           // combox box to switch between charts based on QString passed in QMap
  QStackedWidget *theStackedWidget;  // stacked widgets to hold all the Charts
};

#endif // SC_MULTIPLE_LINE_CHART_H
