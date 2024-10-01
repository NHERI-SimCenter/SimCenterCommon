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
#include <QLegendMarker>
#include <QAbstractAxis>

#include <SC_Chart.h>

//#include <qgsattributes.h>
//#include <qgsmapcanvas.h>


SC_Chart::SC_Chart() : QtCharts::QChart()
{
    qDebug() << "Constructor of SC_Chart";
}

SC_Chart::SC_Chart(QMap<QString, QtCharts::QLineSeries *> &seriesMap, QString title, QString hAxis, QString vAxis, QMap<QString, QColor*> colorMap, QMap<QString, Qt::PenStyle> penStyleMap, QMap<QString, QList<QString> *> *options) : QtCharts::QChart()
{
    toolTipBox = nullptr;

    readQSeriesMap(seriesMap, hAxis, vAxis, colorMap, penStyleMap);
    setToolTip(seriesMap);
    addAxisTitle(seriesMap, hAxis, vAxis);
    insertTitle(title);
    setCollorAndPenStyle(seriesMap, colorMap, penStyleMap);
    setOptions(seriesMap, options);
    groupChartLegend(seriesMap);

}


SC_Chart::SC_Chart(QMap<QString, QtCharts::QLineSeries *> &seriesMap, QString title, QString hAxis, QString vAxis, QMap<QString, QString> color, QMap<QString, QString> lineStyle, QMap<QString, QList<QString> *> *options) : QtCharts::QChart()
{
    toolTipBox = nullptr;

    QMap<QString, QColor*> colorMap = convertStringMapToColorMap(color, seriesMap);
    QMap<QString, Qt::PenStyle> penStyleMap = convertStringlineStyleMapToPenStyleMap(lineStyle, seriesMap);
    
    readQSeriesMap(seriesMap, hAxis, vAxis, colorMap, penStyleMap);
    addAxisTitle(seriesMap, hAxis, vAxis);
    insertTitle(title);
    setCollorAndPenStyle(seriesMap, colorMap, penStyleMap);
    setOptions(seriesMap, options);
    groupChartLegend(seriesMap);
}


SC_Chart::~SC_Chart()
{
    // qDebug() << "Destructor of SC_Chart";
}


void SC_Chart::tooltip(QPointF point, bool state)
{
    //Create tooltip box

    if (state) {
        if (!toolTipBox){
            toolTipBox = new QGraphicsSimpleTextItem("", this);
        }

        QString m_text = QString("X: %1 \nY: %2 ").arg(point.x()).arg(point.y());
        
        toolTipBox->setText(m_text);
        toolTipBox->setPos(( mapToPosition(point))+ QPoint(15, -30));
        toolTipBox->show();
    } else {
            if (toolTipBox){
                toolTipBox->hide();
            }
    }
}


void SC_Chart::readQSeriesMap(const QMap<QString, QtCharts::QLineSeries *> &seriesMap, QString hAxis, QString vAxis, QMap<QString, QColor*> colorMap, QMap<QString, Qt::PenStyle> penStyleMap){
    // Read the each Series and set axis titke for it
    for(auto seriesIT = seriesMap.begin(); seriesIT != seriesMap.end(); ++seriesIT){

        QtCharts::QLineSeries *series = seriesIT.value();
        series->setName(QString(seriesIT.key()));
        addSeries(series);
        seriesNames.append(QString(seriesIT.key()));

    }

}

void SC_Chart::setToolTip(const QMap<QString, QtCharts::QLineSeries *> &seriesMap){
    // Set tooltip for each series
    for(auto seriesIT = seriesMap.begin(); seriesIT != seriesMap.end(); ++seriesIT){
        QtCharts::QLineSeries *series = seriesIT.value();

        connect(series, &QtCharts::QLineSeries::hovered, this, [&](const QPointF &point, bool state) {
        SC_Chart::tooltip(point, state);});
    }
}


void SC_Chart::addAxisTitle(QMap<QString, QtCharts::QLineSeries *> &seriesMap, QString hAxis, QString vAxis){

    if (!hAxis.isEmpty()) {

        QtCharts::QValueAxis *axisX = new QtCharts::QValueAxis();
        axisX->setTitleText(hAxis);
        addAxis(axisX, Qt::AlignBottom);

        for (auto seriesIT = seriesMap.begin(); seriesIT != seriesMap.end(); ++seriesIT){
            QtCharts::QLineSeries *series = seriesIT.value();
            series->attachAxis(axisX);
        }
    }

    if (!vAxis.isEmpty()) {
        QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis();
        axisY->setTitleText(vAxis);
        addAxis(axisY, Qt::AlignLeft);

        for (auto seriesIT = seriesMap.begin(); seriesIT != seriesMap.end(); ++seriesIT){
            QtCharts::QLineSeries *series = seriesIT.value();
            series->attachAxis(axisY);
        }
        
    }
}


void SC_Chart::insertTitle(QString title){

    if (!title.isEmpty()) {
        title = title;
        setTitle(title);
    }
}


void SC_Chart::setCollorAndPenStyle(QMap<QString, QtCharts::QLineSeries *> &seriesMap, QMap<QString, QColor*> colorMap, QMap<QString, Qt::PenStyle> penStyleMap){

    if (colorMap.isEmpty()){

        int i = 0;
        for (auto seriesIT = seriesMap.begin(); seriesIT != seriesMap.end(); ++seriesIT){
            
            QtCharts::QLineSeries *series = seriesIT.value();

            QColor *color = new QColor();
            color->setHsv(i*360/seriesMap.size(), 255, 255);
            series->setColor(*color);
            i++;
        }
    }
    else{
        for (auto seriesIT = seriesMap.begin(); seriesIT != seriesMap.end(); ++seriesIT){
            int i = 0;
            QtCharts::QLineSeries *series = seriesIT.value();

            if (!colorMap.contains(seriesIT.key())){
                QColor *color = new QColor();
                color->setHsv(i*360/seriesMap.size(), 255, 255);
                colorMap[seriesIT.key()] = color;
            }
            else{
                qDebug() << "Color found for " << QString(seriesIT.key());
            }
            i++;
            series->setColor(*colorMap[QString(seriesIT.key())]);
        }
    }

    if (penStyleMap.isEmpty()){
        qDebug() << "Penstyle map is empty";
    }
    else{
        for (auto seriesIT = seriesMap.begin(); seriesIT != seriesMap.end(); ++seriesIT){
            int i = 0;
            QtCharts::QLineSeries *series = seriesIT.value();

            if (!penStyleMap.contains(seriesIT.key())){
                penStyleMap[seriesIT.key()] = Qt::SolidLine;
            }
            else{
                qDebug() << "PenStyle found for " << QString(seriesIT.key());

            }
            i++;
            Qt::PenStyle penStyle = penStyleMap[QString(seriesIT.key())];
            QPen pen = series->pen();
            pen.setStyle(penStyle);
            series->setPen(pen);
        }
    }
}

QMap<QString, QColor*> SC_Chart::convertStringMapToColorMap(QMap<QString, QString> color, QMap<QString, QtCharts::QLineSeries *> seriesMap){
    
    QMap<QString, QColor*> colorMap;
    for (auto seriesIT = seriesMap.begin(); seriesIT != seriesMap.end(); ++seriesIT){
        QString key = QString(seriesIT.key());
        if (color.contains(key)){
            QString colorString = color[key];
            colorMap[key] = getColorFromString(colorString);            
        }
    }
    return colorMap;
}

QColor* SC_Chart::getColorFromString(QString colorString){
    QColor *color;
    colorString = colorString.toUpper();

    if (colorString == "RED" || colorString == "DARKRED" || colorString == "R" ||
        colorString == "GREEN" || colorString == "DARKGREEN" || colorString == "G" ||
        colorString == "BLUE" || colorString == "DARKBLUE" || colorString == "B" ||
        colorString == "YELLOW" || colorString == "DARKYELLOW" || colorString == "Y" ||
        colorString == "CYAN" || colorString == "DARKCYAN" || colorString == "C" ||
        colorString == "MAGENTA" || colorString == "DARKMAGENTA" || colorString == "M"||
        colorString == "GRAY" || colorString == "DARKGRAY" || colorString == "LIGHTGRAY" || colorString == "GY" ||
        colorString == "BLACK" || colorString == "BL" ||
        colorString == "WHITE" || colorString == "W"){

        if (colorString ==  "RED" || colorString == "R")
            color = new QColor(Qt::red);
        else if (colorString == "GREEN" || colorString == "G")
            color = new QColor(Qt::green);
        else if (colorString == "BLUE" || colorString == "B")
            color = new QColor(Qt::blue);
        else if (colorString == "YELLOW" || colorString == "Y")
            color = new QColor(Qt::yellow);
        else if (colorString == "CYAN" || colorString == "C")
                color = new QColor(Qt::cyan);
        else if (colorString == "MAGENTA" || colorString == "M")
                color = new QColor(Qt::magenta);
        else if (colorString == "GRAY" || colorString == "GY")
                color = new QColor(Qt::gray);
        else if (colorString == "BLACK" || colorString == "BL")
                color = new QColor(Qt::black);
        else if (colorString == "WHITE" || colorString == "W")           
                color = new QColor(Qt::white);
        else if (colorString ==  "DARKRED")
            color = new QColor(Qt::darkRed);
        else if (colorString == "DARKGREEN")
            color = new QColor(Qt::darkGreen);
        else if (colorString == "DARKBLUE")
            color = new QColor(Qt::darkBlue);
        else if (colorString == "DARKYELLOW")
            color = new QColor(Qt::darkYellow);
        else if (colorString == "DARKCYAN")
                color = new QColor(Qt::darkCyan);
        else if (colorString == "DARKMAGENTA")
                color = new QColor(Qt::darkMagenta);
        else if (colorString == "DARKGRAY")
                color = new QColor(Qt::darkGray);
        else if (colorString == "LIGHTGRAY")
                color = new QColor(Qt::lightGray);
    }
    else if(colorString.startsWith("(") && colorString.endsWith(")") && colorString.count(",") == 2){
        QStringList colorList = colorString.split(",");
        bool conversionStatus;
        int r = colorList[0].toInt(&conversionStatus);
        int g = colorList[1].toInt(&conversionStatus);
        int b = colorList[2].toInt(&conversionStatus);

        if (conversionStatus){
            color = new QColor(r, g, b);
        }
        else{
            qDebug() << "Color not found for " << colorString;
            color = new QColor(Qt::white);
        }
    }
    else if(colorString.startsWith("#") && colorString.count() == 7){
        bool conversionStatus;
        int r = colorString.mid(1, 2).toInt(&conversionStatus, 16);
        int g = colorString.mid(3, 2).toInt(&conversionStatus, 16);
        int b = colorString.mid(5, 2).toInt(&conversionStatus, 16);

        if (conversionStatus){
            color = new QColor(r, g, b);
        }
        else{
            qDebug() << "Color not found. String Skipped.";
            color = new QColor(Qt::white);
        }
    }
    else{
        qDebug() << "Color not found. String Skipped.";
    }
    return color;

}

QMap<QString, Qt::PenStyle> SC_Chart::convertStringlineStyleMapToPenStyleMap(QMap<QString, QString> lineStyle, QMap<QString, QtCharts::QLineSeries *> seriesMap){
    QMap<QString, Qt::PenStyle> penStyleMap;
    for (auto seriesIT = seriesMap.begin(); seriesIT != seriesMap.end(); ++seriesIT){
        QString key = QString(seriesIT.key());
        if (lineStyle.contains(key)){

            QString lineStyleString = lineStyle[key];
            lineStyleString = lineStyleString.toUpper();

            if (lineStyleString == "SOLID"     || lineStyleString == "DASH"     || lineStyleString == "DOT"     || lineStyleString == "DASHDOT"     || lineStyleString == "DASHDOTDOT" ||
                lineStyleString == "-"         || lineStyleString == "--"       || lineStyleString == ":"       || lineStyleString == "-."          || lineStyleString == "-.." ||
                lineStyleString == "SOLIDLINE" || lineStyleString == "DASHLINE" || lineStyleString == "DOTLINE" || lineStyleString == "DASHDOTLINE" || lineStyleString == "DASHDOTDOTLINE"){
                
                if (lineStyleString == "SOLID" || lineStyleString == "-" || lineStyleString == "SOLIDLINE")
                        penStyleMap[key] = Qt::SolidLine;
                else if (lineStyleString == "DASH" || lineStyleString == "--" || lineStyleString == "DASHLINE")
                    penStyleMap[key] = Qt::DashLine;
                else if (lineStyleString == "DOT" || lineStyleString == ":" || lineStyleString == "DOTLINE")
                    penStyleMap[key] = Qt::DotLine;
                else if (lineStyleString == "DASHDOT" || lineStyleString == "-." || lineStyleString == "DASHDOTLINE")
                    penStyleMap[key] = Qt::DashDotLine;
                else if (lineStyleString == "DASHDOTDOT" || lineStyleString == "-.." || lineStyleString == "DASHDOTDOTLINE")
                    penStyleMap[key] = Qt::DashDotDotLine;
            }
            else{
                qDebug() << "PenStyle not found for " << seriesIT.key();
            }
        }
    }
    return penStyleMap;
}

void SC_Chart::setOptions(QMap<QString, QtCharts::QLineSeries *> &seriesMap, QMap<QString, QList<QString> *> *options){

    if (!options){
        return;
    }

    QList<QString> *toGroupLegends = new QList<QString>();

    if (options->contains("Group") && options->value("Group")){
        QList<QString> *grouped = options->value("Group");
        //QList<QString> *groupedLegend = options->value("GroupLegend");

        for (int i = 0; i < grouped->count(); i++){
            QString groupName = grouped->at(i);
            QColor *color = new QColor("gray");
            if (options->contains("GroupColor")){
                QList<QString> *colorGroupList = options->value("GroupColor");
                QString colorString = colorGroupList->first();
                color = getColorFromString(colorString);
            }
            if (seriesMap.contains(groupName)){
                toGroupLegends->append(groupName);
                QtCharts::QLineSeries *series = seriesMap[groupName];
                series->setColor(*color);
            }
        }
        
        legendGroup["Realizations"] = grouped;
    }
}


void SC_Chart::groupChartLegend(QMap<QString, QtCharts::QLineSeries *> &seriesMap){
    
    // Hide all legend markers initially
    int i = 0;

    if (legend()) {}
    else {
        qDebug() << "Legend null";
        return;
    }

    for (QtCharts::QLegendMarker* marker : legend()->markers()) {
        if (marker)
            marker->setVisible(false);
        else
            qDebug() << "Marker is null";
    }

    // Show only one legend item with the desired label

    for (auto listIT = legendGroup.begin(); listIT != legendGroup.end(); ++listIT){
        QString groupName = listIT.key();
        QList<QString> *groupList = listIT.value();
        QtCharts::QLegendMarker* marker = legend()->markers(seriesMap[groupList->first()]).first();
        marker->setVisible(true);
        marker->setLabel(groupName);
    }
        
}
