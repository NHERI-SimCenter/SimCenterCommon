#ifndef SC_TIMESERIESRESULTCHART_H
#define SC_TIMESERIESRESULTCHART_H

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

// Written by: Sina Naeimi


//#include "ComponentDatabase.h"
//#include "SC_ResultsWidget.h"
//#include "SimCenterMapcanvasWidget.h"

#include <QString>
#include <QMainWindow>
#include <QJsonArray>
#include <QJsonObject>
#include <QChart>
#include <QWidget>
#include <QComboBox>

class QVBoxLayout;
class QGISVisualizationWidget;
class QPointF;
class QGraphicsSimpleTextItem;
class QColor;
class QPen;
class QHBoxLayout;
class SC_Chart;

namespace QtCharts
{
    class QChartView;
    class QBarSet;
    class QChart;
    class QLineSeries;
    class PenStyle;
}

class SC_TimeSeriesResultChart : public QWidget
{
    Q_OBJECT
  
public:
    SC_TimeSeriesResultChart(QMap<QString, QMap<QString, QtCharts::QLineSeries *>> *allSeiries, QWidget *parent);
    SC_TimeSeriesResultChart(QWidget *parent);
    void addMean(QString name=QString("Mean"), QColor color=Qt::red, Qt::PenStyle style=Qt::SolidLine, float width=1.5);
    void addPercentile(QString name, qreal percentile, QColor color=Qt::red, Qt::PenStyle style=Qt::CustomDashLine, float width=1.5);

private:
    SC_Chart *chart;
    QComboBox *metricComboBox;
    QtCharts::QChartView *chartView;
    QHBoxLayout *layout;
    QString metricName;
    QMap<QString, QMap<QString, QtCharts::QLineSeries *>> *allSeries;
    QMap<QString, QList<QString> *> *chartOptions;
    // mean memory
    bool addMeanFlag=false;
    QString mean_name;
    QColor mean_color;
    Qt::PenStyle mean_style;
    float mean_width=1.5;
    // percentile memory
    bool addPercentileFlag=false;
    QString percentile_name;
    qreal percentile_percentile;
    QColor percentile_color;
    Qt::PenStyle percentile_style;
    float percentile_width=1.5;
    

    void drawChart(const QString &metricName);
    void populatemetricComboBox();
    QMap<QString, QtCharts::QLineSeries *> *curSeriesMap;
    qreal calculatePercentile(QVector<qreal> &values, qreal percentile);
    //QMap<QString, QMap<QString, QtCharts::QLineSeries *>> *seriesMap;

public slots:
    void updateChart(const QString &metricName);

};

#endif // SC_TIMESERIESRESULTCHART_H
