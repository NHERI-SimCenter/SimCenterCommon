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


//#include "RewetResults.h"
//#include "VisualizationWidget.h"
//#include "QGISVisualizationWidget.h"
//#include "SimCenterMapcanvasWidget.h"

//#include "CSVReaderWriter.h"
//#include "ComponentDatabaseManager.h"
//#include "GeneralInformationWidgetR2D.h"
//#include "MainWindowWorkflowApp.h"
//#include "REmpiricalProbabilityDistribution.h"
//#include "TablePrinter.h"
//#include "TableNumberItem.h"
//#include "VisualizationWidget.h"
//#include "WorkflowAppR2D.h"
#include "Utils/ProgramOutputDialog.h"

#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QChartView>
#include <QComboBox>
#include <QDir>
#include <QDockWidget>
#include <QFileInfo>
#include <QFontMetrics>
#include <QGraphicsLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineSeries>
#include <QMenuBar>
#include <QPixmap>
#include <QPrinter>
#include <QStackedBarSeries>
#include <QStringList>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextCursor>
#include <QTextTable>
#include <QValueAxis>
#include <QJsonObject>
#include <QtCharts/QChart>
#include <QToolTip>
#include <QColor>
#include <QPen>
#include <QWidget>
#include <QSplineSeries>
#include <QList>

#include <SC_Chart.h>
#include <SC_TimeSeriesResultChart.h> 

using namespace QtCharts;

/*
SC_TimeSeriesResutlChart is meant to facilitate the creation of time series charts.
It is a widget that contains a combobox to select the metric and a chart to display the time series data.
The chart is an instance of SC_Chart. The widget also provides the functionality to add mean and
percentile lines to the chart. The mean and percentile lines are added when the chart is drawn.
The widget also provides the functionality to update the chart based on the selected metric.
The widget is originally desigend to be used in RewetResults to display the time series data.

PLAN FOR THE FUTURE DEVELOPMENT: An option to change teh color of the group, grouping, the mean,
and the percentile lines MAY BE needed. An option to change the name of the chart is also needed.
The possibility of reading horizontal and vertical axis names from the JSON file is also needed.

*/


SC_TimeSeriesResultChart::SC_TimeSeriesResultChart(QMap<QString, QMap<QString, QLineSeries *>> *allSeries, QWidget *parent = nullptr) : QWidget(parent) {
    /*
        The constructor of the SC_TimeSeriesResultChart class.
        allSeries: A pointer to a map of series maps. The key of the outer map is the metric name and the value is a map of series.
        The key to the inner map is the realziation name and the value is a QLineSeries.
        parent: The parent widget. Default is nullptr.
    */

    chartOptions = nullptr;

    QString metricName;
    metricComboBox = new QComboBox();
    chart = new SC_Chart();
    chartView = new QChartView(chart, this);
    this->allSeries = allSeries;

    // Get the first value of the map

    if (!allSeries->isEmpty()) {
        curSeriesMap = &(allSeries->first());
        metricName = allSeries->firstKey();
        drawChart(metricName);
        populatemetricComboBox();
        metricComboBox->setCurrentText(metricName);
    }

    QLabel *metricLabel = new QLabel("Metric:");
    
    
    
    layout = new QHBoxLayout;
    QVBoxLayout *vLayout = new QVBoxLayout;
    setLayout(vLayout);
    layout->addWidget(metricLabel);
    layout->addWidget(metricComboBox);
    vLayout->addLayout(layout);
    vLayout->addWidget(chartView);

    connect(metricComboBox, &QComboBox::currentTextChanged, this, &SC_TimeSeriesResultChart::updateChart);

}

void SC_TimeSeriesResultChart::updateChart(const QString &metricName) {
    // Update the chart based on the selected metric
    if (metricName.isEmpty()) {
        return;
    }
    
    if (allSeries->contains(metricName)) {
        drawChart(metricName);
    }

}

void SC_TimeSeriesResultChart::drawChart(const QString &metricName) {
    
    // Check if the series map is empty
    if (!allSeries->isEmpty() && !metricName.isEmpty()) {

        QMap<QString, Qt::PenStyle> penStyleMap;
        QMap<QString, QColor*> colorMap;
        chartOptions = new QMap<QString, QList<QString> *>();

        QList<QString> *grouped = new QList<QString>();
        QList<QString> *groupedColor = new QList<QString>();
        QList<QString> *groupedLegend = new QList<QString>();

        for (auto seriesIT = curSeriesMap->begin(); seriesIT != curSeriesMap->end(); ++seriesIT) {
            penStyleMap[seriesIT.key()] = Qt::SolidLine;
            colorMap[seriesIT.key()] = new QColor("gray");
            grouped->append(QString(seriesIT.key()));
            groupedLegend->append(QString(seriesIT.key()));
        }

        groupedColor->append("lightGray");
        chartOptions->insert("Group", grouped);
        chartOptions->insert("GroupColor", groupedColor);
        chartOptions->insert("GroupLegend", groupedLegend);        

        SC_Chart *old_chart = chart;
        auto slist=chart->series();
        for (auto curSeries : slist) {
            QList<QAbstractAxis *> axes = curSeries->attachedAxes();
            for (auto axis : axes) {
                curSeries->detachAxis(axis);
                chart->removeAxis(axis);
            }

            chart->removeSeries(curSeries);
        }

        chart = new SC_Chart(*curSeriesMap, QString("Network Performace"), QString("time"), QString("Percent"), colorMap, penStyleMap, chartOptions);
        chartView->setChart(chart);
        chartView->update();

        if (old_chart!=nullptr) {
            delete old_chart;
            old_chart = nullptr;
        }
        
        if (addMeanFlag) {
            addMean(mean_name, mean_color, mean_style, mean_width);
        }

        if (addPercentileFlag) {
            addPercentile(percentile_name, percentile_percentile, percentile_color, percentile_style, percentile_width);
        }
    }
}

void SC_TimeSeriesResultChart::populatemetricComboBox() {
    if (metricComboBox == nullptr) {
        return;
    }

    metricComboBox->clear();
    if (!curSeriesMap->isEmpty()) {
        for (auto seriesIT = allSeries->begin(); seriesIT != allSeries->end(); ++seriesIT) {
            metricComboBox->addItem(seriesIT.key());
        }
    }
}

void SC_TimeSeriesResultChart::addMean(QString name, QColor color, Qt::PenStyle style, float width){
    /*
        the name of the mean line to be added to the chart. default is Mean.
        The color of the mean line. default is red.
        The style of the mean line. default is Qt::SolidLine.
        The width of the line. default is 1.5 of the original width (realizations width).
    */


    // set the flag true so that the mean will be added when the chart is drawn
    addMeanFlag = true;

    // set mean name, color, style, and width
    mean_name = name;
    mean_color = color;
    mean_style = style;
    mean_width = width;
    
    if(!curSeriesMap || curSeriesMap->isEmpty()){
        return;
    }

    QLineSeries *meanSeries = new QLineSeries();
    meanSeries->setName(name);

    qreal minX = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::lowest();

    QMap<QString, QSplineSeries*> splineMap;
    qreal step = 1;
    qreal size = 0;

    bool flag = false;
    for (auto it = curSeriesMap->begin(); it != curSeriesMap->end(); ++it) {
        size++;

        QLineSeries *lineSeries = it.value();
        QSplineSeries *splineSeries = new QSplineSeries();
        splineMap.insert(it.key(), splineSeries);
        splineSeries->setName(lineSeries->name() + " Spline");

        QList<QPointF> points = lineSeries->points();

        for (int i = 0; i < points.size(); ++i) {
            splineSeries->append(points[i]);
        }

        if (splineSeries->points().first().x() < minX) {
            minX = splineSeries->points().first().x();
        }
        
        if (splineSeries->points().last().x() > maxX) {
            maxX = splineSeries->points().last().x();
        }
        flag = true;
    }

    // This here to make sure that for series that have different x maximum values, work properly
    for (auto it = splineMap.begin(); it != splineMap.end(); ++it) {
        QSplineSeries *splineSeries = splineMap[it.key()];
        if (splineSeries->points().last().x() < maxX) {
            auto curLastY = splineSeries->points().last().y();
            splineSeries->append(maxX, curLastY);
        }
    }
    
    if (!flag) {
        qDebug() << "ADD LINE - RETURNED! Must not happen!";
        return;
    }

    for (qreal x = minX; x <= maxX; x += step){
        qreal sum_y = 0;

        for (auto it = curSeriesMap->begin(); it != curSeriesMap->end(); ++it) {

            if (!splineMap.contains(it.key())) {
                qDebug() << "Error in splineMap";
                return;
            }
            QSplineSeries *splineSeries = splineMap[it.key()];

            qreal y = splineSeries->at(x).y();
            sum_y += y;   
        }
        sum_y = sum_y / size;
        meanSeries->append(x, sum_y);
    }

    // clean up
    for (auto it = curSeriesMap->begin(); it != curSeriesMap->end(); ++it) {
        delete splineMap[it.key()];
    }
    
    meanSeries->setColor(color);
    QPen pen = meanSeries->pen();
    pen.setStyle(style);
    pen.setWidth(pen.widthF()*width);
    meanSeries->setPen(pen);
    chart->addSeries(meanSeries);
}

void SC_TimeSeriesResultChart::addPercentile(QString name, qreal percentile, QColor color, Qt::PenStyle style, float width){
    /*
        The name of the percentile line to be added to the chart. default is 90th Percentile.
        The name of the percentile line to be added to the chart. default is 90th Percentile.
        if theStyle if not givven or teh value for it is Qt::CustomDashLine, then a hard coded dash pattern will be used.
        The width of the line. default is 1.5 of the original width (realizations width).
    */

    // set the flag true so that the percentile will be added when the chart is drawn
    addPercentileFlag = true;

    // set percentile name, color, style, and width
    percentile_name = name;
    percentile_percentile = percentile;
    percentile_color = color;
    percentile_style = style;
    percentile_width = width;

    // check if the series map exist or is empty
    if(!curSeriesMap || curSeriesMap->isEmpty()){
        return;
    }


    QLineSeries *percentileSeries_pos = new QLineSeries();
    QLineSeries *percentileSeries_neg = new QLineSeries();

    qreal minX = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::lowest();

    QMap<QString, QSplineSeries*> splineMap;
    qreal step = 1;
    qreal size = 0;

    bool flag = false;
    
    for (auto it = curSeriesMap->begin(); it != curSeriesMap->end(); ++it) {
        size++;

        QLineSeries *lineSeries = it.value();
        QSplineSeries *splineSeries = new QSplineSeries();
        splineMap.insert(it.key(), splineSeries);
        splineSeries->setName(lineSeries->name() + " Spline");

        QList<QPointF> points = lineSeries->points();

        for (int i = 0; i < points.size(); ++i) {
            splineSeries->append(points[i]);
        }

        if (splineSeries->points().first().x() < minX) {
            minX = splineSeries->points().first().x();
        }
        
        if (splineSeries->points().last().x() > maxX) {
            maxX = splineSeries->points().last().x();
        }
        flag = true;
    }

    QMap<qreal, QVector<qreal>> xToYMap;

    for (qreal x = minX; x <= maxX; x += step) {
        for (auto it = curSeriesMap->begin(); it != curSeriesMap->end(); ++it){
            // QString realizationName = it.key();
            QSplineSeries *splineSeries = splineMap[it.key()];
            QLineSeries *realizationSeries = it.value();
            xToYMap[x].append(splineSeries->at(x).y());
        }
    }

    for (auto xIt = xToYMap.begin(); xIt != xToYMap.end(); ++xIt) {
        qreal x = xIt.key();
        QVector<qreal> &yValues = xIt.value();
        qreal percentile90_pos = calculatePercentile(yValues, percentile);
        qreal percentile90_neg = calculatePercentile(yValues, 1-percentile);
        percentileSeries_pos->append(x, percentile90_pos);
        percentileSeries_neg->append(x, percentile90_neg);
    }

    QVector<double> dashes;
    double space = 8;

    dashes << 25 << space << 1 << space << 1 << space;

    percentileSeries_pos->setColor(color);
    percentileSeries_pos->setName(name);
    QPen pen = percentileSeries_pos->pen();
    pen.setStyle(style);
    if (style == Qt::CustomDashLine)
        pen.setDashPattern(dashes);
    pen.setWidth(pen.widthF()*width);
    percentileSeries_pos->setPen(pen);
    chart->addSeries(percentileSeries_pos);

    percentileSeries_neg->setColor(color);
    percentileSeries_neg->setName(name);
    pen = percentileSeries_neg->pen();
    pen.setStyle(style);
    if (style == Qt::CustomDashLine)
        pen.setDashPattern(dashes);
    pen.setWidth(pen.widthF()*width);
    percentileSeries_neg->setPen(pen);
    chart->addSeries(percentileSeries_neg);
}

qreal SC_TimeSeriesResultChart::calculatePercentile(QVector<qreal> &values, qreal percentile) {
    if (values.isEmpty()) {
        return 0;
    }
    qSort(values.begin(), values.end());  // Sorting the QVector
    int index = static_cast<int>(percentile * (values.size() - 1));
    return values[index];
}
