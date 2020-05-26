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
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

#include "DakotaResultsReliability.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>

#include <QFileDialog>
#include <QTabWidget>
#include <QTextEdit>
#include <MyTableWidget.h>
#include <QDebug>
#include <QHBoxLayout>
#include <QColor>
#include <QMenuBar>
#include <QAction>
#include <QMenu>
#include <QPushButton>
#include <QProcess>
#include <QScrollArea>
#include <QHeaderView>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <QMessageBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QVXYModelMapper>
using namespace QtCharts;
#include <math.h>
#include <QValueAxis>
#include <QLabel>

#include <QXYSeries>
#include <RandomVariablesContainer.h>
#include <QMap>

#define NUM_DIVISIONS 10
#include <iostream>

DakotaResultsReliability::DakotaResultsReliability(RandomVariablesContainer *theRandomVariables, QWidget *parent)
  : UQ_Results(parent), theRVs(theRandomVariables), numSpreadsheetRows(0), numSpreadsheetCols(0)
{
  chart = new QChart();
  chart->setAnimationOptions(QChart::AllAnimations);
  QChartView *chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->chart()->legend()->hide();

  //layout = new QVBoxLayout();
  spreadsheet = new MyTableWidget();  
  layout->addWidget(chartView);
  layout->addWidget(spreadsheet);

  mLeft = true;
  col1 = 0;
  col2 = 0;

  connect(spreadsheet,SIGNAL(cellPressed(int,int)),this,SLOT(onSpreadsheetCellClicked(int,int)));
}

DakotaResultsReliability::~DakotaResultsReliability()
{

}


void DakotaResultsReliability::clear(void)
{
  spreadsheet->clear();
  theHeadings.clear();

  numSpreadsheetCols = 0;
  numSpreadsheetRows = 0;

  mLeft = true;
  col1 = 0;
  col2 = 0;
}



int DakotaResultsReliability::processResults(QString &filenameResults, QString &filenameTab)
{
  Q_UNUSED(filenameTab);
  emit sendStatusMessage(tr("Processing Reliability Results"));

  // clear current
  this->clear();

  numSpreadsheetCols = 1;
  numSpreadsheetRows = 0;

  theHeadings << "%";
  spreadsheet->setColumnCount(1);


  //
  // check it actually ran with n errors
  //

  QFileInfo fileResultsInfo(filenameResults);
  QString filenameErrorString = fileResultsInfo.absolutePath() + QDir::separator() + QString("dakota.err");

  QFileInfo filenameErrorInfo(filenameErrorString);
  if (!filenameErrorInfo.exists()) {
      emit sendErrorMessage("No dakota.err file - dakota did not run - problem with dakota setup or the applicatins failed with inputs provied");
      return 0;
  }
  QFile fileError(filenameErrorString);
  QString line("");
  if (fileError.open(QIODevice::ReadOnly)) {
     QTextStream in(&fileError);
     while (!in.atEnd()) {
        line = in.readLine();
     }
     fileError.close();
  }

  if ((line.length() != 0) && (!line.contains("Warning: unit probability", Qt::CaseInsensitive)
                               && !line.contains("We set the probability to 1.0 in this case", Qt::CaseInsensitive))){
      qDebug() << line.length() << " " << line;
      emit sendErrorMessage(QString(QString("Error Running Dakota: ") + line));
      return 0;
  }

  QFileInfo filenameResultsInfo(filenameResults);
  if (!filenameResultsInfo.exists()) {
      emit sendErrorMessage("No dakota.out file - dakota failed .. possibly no QoI");
      return 0;
  }


  // 
  // read data from file filename
  //  
  
  // open file containing data
  std::ifstream fileResults(filenameResults.toStdString().c_str());
  if (!fileResults.is_open()) {
    std::cerr << "DakotaResultsReliability: Could not open file";
    return -1;
  }
  

  
  /* **************************************** LOOKING FOR THE FOLLOWING
     -----------------------------------------------------------------
     Cumulative Distribution Function (CDF) for response_fn_1:
     Response Level  Probability Level  Reliability Index  General Rel Index
     --------------  -----------------  -----------------  -----------------
     2.5359850842e-01   1.9999879291e-02   2.0537514037e+00   2.0537514037e+00
     ........           ........           .......           .........
     ........           ........           .......           .........
     -----------------------------------------------------------------
     
  *************************************************************************** */

  const std::string needleStart = "Cumulative Distribution Function (CDF)";
  std::string haystack;

  bool foundResults = false;
  bool endResponses = false;
  while(foundResults == false && fileResults.eof() != true) {

      std::vector<double>col1;
      std::vector<double>col2;

      while (std::getline(fileResults, haystack)) {
          if (haystack.find(needleStart) != std::string::npos) {
              foundResults = true;
              break;
          }
      }

      if (fileResults.eof()) {
          endResponses = true;
          break;
      }
  }

  if (foundResults == false || endResponses == true) {
      emit sendErrorMessage("ProcessingResults: No Results found in output file & no error .. Dakota crashed");
      return -1;
  }

  /*
   * for each response quantity enter prob levels into spreadsheet
   */

  while(endResponses == false && fileResults.eof() != true) {

      std::istringstream iss(haystack);
      std::string subs;

      do {
          iss >> subs;
      } while (iss);

      theHeadings << subs.c_str();
      qDebug() << "HEADINGS" << theHeadings << " numCols: " << numSpreadsheetCols;


      // read next 2 lines of drivel
      std::getline(fileResults, haystack);
      std::getline(fileResults, haystack);

      // for some reason
      // std::getline(fileResults, haystack);

      const std::string needleEnd = "---------------------------------------";

       spreadsheet->insertColumn(numSpreadsheetCols);
       numSpreadsheetCols++;

       QMap<float,float> data;
       // now read the data till end of data encountered
       int numRows = 0;

       while (std::getline(fileResults, haystack)) {
           if (haystack.find(needleStart) != std::string::npos) {
               break;
           } else if (haystack.find(needleEnd) != std::string::npos) {
               qDebug() << "FOUND END";
               endResponses = true;
               break;
           } else {
               // read column entries
               if (numSpreadsheetCols == 2)
                   spreadsheet->insertRow(numRows);

               std::string data1, data2, data3, data4;
               std::istringstream is(haystack);

               is >> data1 >> data2 >> data3 >> data4;

               if (numSpreadsheetCols == 2) {
                   data.insert(std::stod(data2),std::stod(data1));
                   QModelIndex index = spreadsheet->model()->index(numRows, numSpreadsheetCols-2);
                   spreadsheet->model()->setData(index, data2.c_str());
               }
               QModelIndex index = spreadsheet->model()->index(numRows, numSpreadsheetCols-1);
               spreadsheet->model()->setData(index, data1.c_str());

               numRows++;
           }
       }

       numSpreadsheetRows = numRows;
  }

  spreadsheet->setHorizontalHeaderLabels(theHeadings);
  spreadsheet->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  fileResults.close();

  this->onSpreadsheetCellClicked(0,numSpreadsheetCols-1);
  if (numSpreadsheetRows == 0)
      emit sendStatusMessage(tr("No Result Data Found .. dakota failed .. possibly no QoI provided"));

  return 0;
}


void
DakotaResultsReliability::onSaveSpreadsheetClicked()
{

  int rowCount = spreadsheet->rowCount();
  int columnCount = spreadsheet->columnCount();
  
  QString fileName = QFileDialog::getSaveFileName(this,
						  tr("Save Data"), "",
						  tr("All Files (*)"));
  
  QFile file(fileName);
  if (file.open(QIODevice::ReadWrite))
  {
      QTextStream stream(&file);
      for (int j=0; j<columnCount; j++)
      {
          stream <<theHeadings.at(j)<<",\t";
      }
      stream <<endl;
      for (int i=0; i<rowCount; i++)
      {
          for (int j=0; j<columnCount; j++)
          {
              QTableWidgetItem *item_value = spreadsheet->item(i,j);
              double value = item_value->text().toDouble();
              stream << value << ",\t";
              //     qDebug()<<value;
          }
          stream<<endl;
      }
  }
}


void DakotaResultsReliability::onSpreadsheetCellClicked(int row, int col)
{
    Q_UNUSED(row);
    col2 = 0;
    int oldCol = col1;
    if (col == 0)
        return;
    else
        col1 = col;

    mLeft = spreadsheet->wasLeftKeyPressed();

    // create a new series
    chart->removeAllSeries();

    //Remove all axes
    for(auto axis: chart->axes())
        chart->removeAxis(axis);

    int rowCount = spreadsheet->rowCount();

    if (col1 != col2) {
        QScatterSeries *series = new QScatterSeries;
        QLineSeries *lineSeries= new QLineSeries;

        double minX = 0, maxX = 0;

        for (int i=0; i<rowCount; i++) {
            QTableWidgetItem *itemX = spreadsheet->item(i,col1);    //col1 goes in x-axis, col2 on y-axis
            //col1=0;
            QTableWidgetItem *itemY = spreadsheet->item(i,col2);
            QTableWidgetItem *itemOld = spreadsheet->item(i,oldCol);
            itemOld->setData(Qt::BackgroundRole, QColor(Qt::white));
            itemX->setData(Qt::BackgroundRole, QColor(Qt::lightGray));
            itemY->setData(Qt::BackgroundRole, QColor(Qt::lightGray));
            double xVal = itemX->text().toDouble();
            double yVal = itemY->text().toDouble();

            series->append(xVal,yVal);
            lineSeries->append(xVal,yVal);

            if (i == 0) {
                maxX = xVal;
                minX = xVal;
            } else {
                if(xVal<minX){minX=xVal;}
                if(xVal>maxX){maxX=xVal;}
            }
        }

        chart->addSeries(series);
        chart->addSeries(lineSeries);
        series->setName("Samples");

        QValueAxis *axisX = new QValueAxis();
        QValueAxis *axisY = new QValueAxis();

        axisX->setTitleText(theHeadings.at(col1));
        axisY->setTitleText("Probability");

        if (maxX == minX) {
            maxX = maxX*1.1;
            minX = minX*0.9;
        }

        double xRange=maxX-minX;

        //axisX->setRange(minX - 0.01*xRange, maxX + 0.1*xRange);
        axisX->setRange(minX, maxX);
        axisY->setRange(0, 1.0);

        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
        lineSeries->attachAxis(axisX);
        chart->addAxis(axisY, Qt::AlignLeft);
        lineSeries->attachAxis(axisY);
        series->attachAxis(axisY);
   }
}



bool
DakotaResultsReliability::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    jsonObject["resultType"]=QString(tr("DakotaResultsReliability"));

    //
    // add summary data
    //

    /*
    QJsonArray resultsData;
    int numEDP = theNames.count();
    for (int i=0; i<numEDP; i++) {
        QJsonObject edpData;
        edpData["name"]=theNames.at(i);
        edpData["mean"]=theMeans.at(i);
        edpData["stdDev"]=theStdDevs.at(i);
        edpData["kurtosis"]=theKurtosis.at(i);
        resultsData.append(edpData);
    }
    jsonObject["summary"]=resultsData;
    */



    //
    // add spreadsheet data
    //

    QJsonObject spreadsheetData;


    spreadsheetData["numRow"]=numSpreadsheetRows;
    spreadsheetData["numCol"]=numSpreadsheetCols;

    QJsonArray headingsArray;
    for (int i = 0; i <theHeadings.size(); ++i) {
        headingsArray.append(QJsonValue(theHeadings.at(i)));
    }

    spreadsheetData["headings"]=headingsArray;

    QJsonArray dataArray;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (int row = 0; row < numSpreadsheetRows; ++row) {
        for (int column = 0; column < numSpreadsheetCols; ++column) {
            QTableWidgetItem *theItem = spreadsheet->item(row,column);
            QString textData = theItem->text();
            dataArray.append(textData.toDouble());
        }
    }
    QApplication::restoreOverrideCursor();
    spreadsheetData["data"]=dataArray;

    jsonObject["spreadsheet"] = spreadsheetData;
    return result;
}



// if you already have a json data file then you can populate the UI with the entries from json.

bool
DakotaResultsReliability::inputFromJSON(QJsonObject &jsonObject)
{
    Q_UNUSED(jsonObject);
    bool result = true;

    /*
    this->clear();

    //
    // create a summary widget in which place basic output (name, mean, stdDev)
    //

    QWidget *summary = new QWidget();
    QVBoxLayout *summaryLayout = new QVBoxLayout();
    summary->setLayout(summaryLayout);

    QJsonArray edpArray = jsonObject["summary"].toArray();
    foreach (const QJsonValue &edpValue, edpArray) {
        QString name;
        double mean, stdDev;
        QJsonObject edpObject = edpValue.toObject();
        QJsonValue theNameValue = edpObject["name"];
        name = theNameValue.toString();

        QJsonValue theMeanValue = edpObject["mean"];
        mean = theMeanValue.toDouble();

        QJsonValue theStdDevValue = edpObject["stdDev"];
        stdDev = theStdDevValue.toDouble();

        QJsonValue theKurtosis = edpObject["kurtosis"];
        double kurtosis = theKurtosis.toDouble();

        QWidget *theWidget = this->createResultEDPWidget(name, mean, stdDev, kurtosis);
        summaryLayout->addWidget(theWidget);
    }
    summaryLayout->addStretch();


    //
    // into a spreadsheet place all the data returned
    //

    spreadsheet = new MyTableWidget();
    QJsonObject spreadsheetData = jsonObject["spreadsheet"].toObject();
    int numRow = spreadsheetData["numRow"].toInt();
    int numCol = spreadsheetData["numCol"].toInt();
    spreadsheet->setColumnCount(numCol);
    spreadsheet->setRowCount(numRow);

    QJsonArray headingData= spreadsheetData["headings"].toArray();
    for (int i=0; i<numCol; i++) {
        theHeadings << headingData.at(i).toString();
    }

    spreadsheet->setHorizontalHeaderLabels(theHeadings);

    QJsonArray dataData= spreadsheetData["data"].toArray();
    int dataCount =0;
    for (int row =0; row<numRow; row++) {
        for (int col=0; col<numCol; col++) {
            QModelIndex index = spreadsheet->model()->index(row, col);
            spreadsheet->model()->setData(index, dataData.at(dataCount).toDouble());
            dataCount++;
        }
    }
    spreadsheet->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(spreadsheet,SIGNAL(cellPressed(int,int)),this,SLOT(onSpreadsheetCellClicked(int,int)));

    //
    // create a chart, setting data points from first and last col of spreadsheet
    //

    chart = new QChart();
    chart->setAnimationOptions(QChart::AllAnimations);
    QScatterSeries *series = new QScatterSeries;
    col1 = 0;           // col1 is initialied as the first column in spread sheet
    col2 = numCol-1;    // col2 is initialized as the second column in spread sheet
    mLeft = true;       // left click

    this->onSpreadsheetCellClicked(0,numCol-1);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->chart()->legend()->hide();


    //
    // create a widget into which we place the chart and the spreadsheet
    //

    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->addWidget(chartView, 1);
    layout->addWidget(spreadsheet, 1);

    //qDebug()<<"\n debugging the values: result is  \n"<<result<<"\n";
    */
    return result;
}


