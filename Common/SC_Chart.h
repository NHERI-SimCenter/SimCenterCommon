#ifndef SC_CHART_H
#define SC_CHART_H

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
//#include <QMainWindow>
#include <QJsonArray>
//#include <RewetResults.h>
#include <QJsonObject>
#include <QChart>

class QVBoxLayout;
class QGISVisualizationWidget;
class QPointF;
class QGraphicsSimpleTextItem;
class QColor;
class QPen;

namespace QtCharts
{
    class QChartView;
    class QBarSet;
    class QChart;
    class QLineSeries;
    class QLegendMarker;
}

class SC_Chart : public QtCharts::QChart
{
    Q_OBJECT
  
public:

  SC_Chart();
  SC_Chart(QMap<QString, QtCharts::QLineSeries *> &seriesMap, QString title=QString(), QString hAxis=QString(), QString vAxis=QString(), QMap<QString, QColor*> colorMap=QMap<QString, QColor*>(), QMap<QString, Qt::PenStyle> penStyleMap=QMap<QString, Qt::PenStyle>(), QMap<QString, QList<QString> *> *options=nullptr);
  SC_Chart(QMap<QString, QtCharts::QLineSeries *> &seriesMap, QString title          , QString hAxis          , QString vAxis          , QMap<QString, QString> color                            , QMap<QString, QString> lineStyle                                     , QMap<QString, QList<QString> *> *options=nullptr);
  ~SC_Chart();
  
signals:

public slots:
    void tooltip(QPointF point, bool state);

private:
  QString title;
  QList<QString> seriesNames;
  QMap<QString, QList<QString> *> legendGroup;
  QGraphicsSimpleTextItem *toolTipBox;
  
  void addAxisTitle(QMap<QString, QtCharts::QLineSeries *> &seriesMap, QString hAxis, QString vAxis);
  void insertTitle(QString title);
  void readQSeriesMap(const QMap<QString, QtCharts::QLineSeries *> &seriesMap, QString hAxis, QString vAxis, QMap<QString, QColor*> colorMap, QMap<QString, Qt::PenStyle> penStyleMap);
  void setToolTip(const QMap<QString, QtCharts::QLineSeries *> &seriesMap);
  void addChartTitle(QString title);
  void setCollorAndPenStyle(QMap<QString, QtCharts::QLineSeries *> &seriesMap, QMap<QString, QColor*> colorMap, QMap<QString, Qt::PenStyle> penStyleMap);
  QMap<QString, QColor*> convertStringMapToColorMap(QMap<QString, QString> color, QMap<QString, QtCharts::QLineSeries *> seriesMap);
  QMap<QString, Qt::PenStyle> convertStringlineStyleMapToPenStyleMap(QMap<QString, QString> lineStyle, QMap<QString, QtCharts::QLineSeries *> seriesMap);
  QColor* getColorFromString(QString colorString);
  void setOptions(QMap<QString, QtCharts::QLineSeries *> &seriesMap, QMap<QString, QList<QString> *> *options);
  void groupChartLegend(QMap<QString, QtCharts::QLineSeries *> &seriesMap);

};

#endif // SC_CHART_H
