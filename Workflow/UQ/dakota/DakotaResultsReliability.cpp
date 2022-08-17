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
    // clear current
  chart = new QChart();
  QChartView *chartView = new QChartView(chart);
  chart->setAnimationOptions(QChart::AllAnimations);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->chart()->legend()->hide();

  //layout = new QVBoxLayout();
  spreadsheet = new MyTableWidget();
  QScrollArea *sa = new QScrollArea;
  sa->setWidgetResizable(true);
  sa->setLineWidth(0);
  sa->setFrameShape(QFrame::NoFrame);
  sa->setWidget(spreadsheet);
  layout->addWidget(chartView);
  layout->addWidget(sa);

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

  //clearLayout(layout);

}

int DakotaResultsReliability::processResults(QString &dirName)
{
  QString filenameOut = dirName + QDir::separator() + tr("dakota.out");
  QString filenameTAB = dirName + QDir::separator() + tr("dakotaTab.dat");
  return this->processResults(filenameOut, filenameTAB);
}


int DakotaResultsReliability::processResults(QString &filenameResults, QString &filenameTab)
{

  statusMessage(tr("Processing Reliability Results"));



  //
  // check it actually ran with no errors
  //

  QFileInfo fileResultsInfo(filenameResults);
  QString filenameErrorString = fileResultsInfo.absolutePath() + QDir::separator() + QString("dakota.err");

  QFileInfo filenameErrorInfo(filenameErrorString);
  if (!filenameErrorInfo.exists()) {
      errorMessage("No dakota.err file - dakota did not run - problem with dakota setup or the applications failed with inputs provided");
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

  /*
  if ((line.length() != 0) && (!line.contains("Warning: unit probability", Qt::CaseInsensitive))
                               && !line.contains("Warning: maximum back-tracking", Qt::CaseInsensitive)
                               && !line.contains("Warning: maximum Newton iterations ", Qt::CaseInsensitive)){
      qDebug() << line.length() << " " << line;
      errorMessage(QString(QString("Error Running Dakota: ") + line));
      return 0;
  }
  */
  if ((line.length() != 0)) {



          if (line.contains("-- Expected 1 function value(s) but found ", Qt::CaseInsensitive)) {
              if (line.at(42)=='0') {
                  // Multiple output by user
                  qDebug() << line.length() << " " << line;
                  errorMessage(QString("Dakota Error: No results.out. Check dakota.err file"));
                  return 0;
              } else {
                  // Multiple output by user
                  qDebug() << line.length() << " " << line;
                  errorMessage(QString("Dakota Error: Current reliability option supports only single output. Please try with Mean Value option or with a single output. "));
                  return 0;
              }
          }
          if (line.contains("Warning:", Qt::CaseInsensitive)) {
              // Warning
                qDebug() << line.length() << " " << line;
                  errorMessage(QString(QString("Dakota ") + line));
          } else {
              // Other errors
                qDebug() << line.length() << " " << line;
                errorMessage(QString(QString("Dakota ") + line));
                return 0;
          }


  }

  QFileInfo filenameResultsInfo(filenameResults);
  if (!filenameResultsInfo.exists()) {
      errorMessage("No dakota.out file - dakota failed .. possibly no QoI");
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
  
    errorMessage((QString("")));

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

  numSpreadsheetCols = 3;
  numSpreadsheetRows = 0;

  spreadsheet->setColumnCount(numSpreadsheetCols);

  const std::string needleStart = "Cumulative Distribution Function (CDF)";
  const std::string needleEnd = "---------------------------------------";
  const std::string needleEnd2 = "<<<<< Iterator importance_sampling completed.";

  std::string haystack;
  bool isStartOrEnd = true;
  int numCols = 3;

  while(fileResults.eof() != true) {

      std::vector<double>col1;
      std::vector<double>col2;
      std::vector<double>col3;

    if (isStartOrEnd) {
        // to remove all the text before and after the results we are interested in
          while (std::getline(fileResults, haystack)) {
              if (haystack.find(needleStart) != std::string::npos) {
                  break;
              }
              else {
                  if (numSpreadsheetRows > 0);
              }
          }

          if (fileResults.eof()) {
              break;
          }
     }

      // get descriptor name
      std::istringstream iss(haystack);
      std::string subs;

      do {
          iss >> subs;
      } while (iss);

      //theHeadings << QString("Pr(")+QString(subs.c_str()).remove(QChar(':'))+QString(")");
      theHeadings << QString(subs.c_str()).remove(QChar(':'));
      theHeadings << QString("Probability level: "+QString(subs.c_str()).remove(QChar(':')));
      theHeadings << QString("Reliability index: "+QString(subs.c_str()).remove(QChar(':')));

      // read next 2 lines of drivel
      std::getline(fileResults, haystack);
      std::getline(fileResults, haystack);

      // for some reason
      // std::getline(fileResults, haystack);


       qDebug() << "numCOL" << numSpreadsheetCols;
       QMap<float,float> data;
       // now read the data till end of data encountered
       int numRows = 0;
       while (std::getline(fileResults, haystack)) {
           if ((haystack.find(needleEnd) != std::string::npos)) {
               // wrap up
               qDebug() << "FOUND END";
               isStartOrEnd = true;
               break;
           } else if ((haystack.find(needleEnd2) != std::string::npos)) {
               // wrap up
               qDebug() << "FOUND END";
               isStartOrEnd = true;
               break;
           } else if (haystack.find(needleStart) != std::string::npos) {
               // move on to the next EDP
               isStartOrEnd = false;
               spreadsheet->insertColumn(numCols);
               spreadsheet->insertColumn(numCols);
               spreadsheet->insertColumn(numCols);
               numCols = numCols+3;
               break;
           } else {
               // read column entries
               std::string data1, data2, data3, data4;
               std::istringstream is(haystack);

               is >> data1 >> data2 >> data3 >> data4;

               //if (!data1.empty() && (((data1.find("e-"))!= std::string::npos)||((data1.find("e+"))!= std::string::npos))) {
               if (data1.empty()) {

               }
               else if (data1.find("Warning:") != std::string::npos) {
                   errorMessage(QString(QString("Dakota ") + QString::fromStdString(haystack)));
               } else {
                   if (numSpreadsheetCols == 3)
                       spreadsheet->insertRow(numRows);
                   data.insert(std::stod(data1),std::stod(data2));
                   QModelIndex index = spreadsheet->model()->index(numRows, numCols-3);
                   spreadsheet->model()->setData(index, data1.c_str());
                   QModelIndex index2 = spreadsheet->model()->index(numRows, numCols-2);
                   spreadsheet->model()->setData(index2, data2.c_str());
                   QModelIndex index3 = spreadsheet->model()->index(numRows, numCols-1);
                   spreadsheet->model()->setData(index3, data3.c_str());
                   numRows++;
               }
               //}
           }
       }
       numSpreadsheetRows = numRows;
       numSpreadsheetCols = numCols;
  }

  spreadsheet->setHorizontalHeaderLabels(theHeadings);
  spreadsheet->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  for (int i=0; i<numSpreadsheetCols; i++) {
      spreadsheet->horizontalHeaderItem(i)->setToolTip(theHeadings[i]);
  }
  fileResults.close();

  this->onSpreadsheetCellClicked(0,1);
  if (numSpreadsheetRows == 0)
      statusMessage(tr("No Result Data Found .. dakota failed .. possibly no QoI provided"));

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
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        for (int j=0; j<columnCount; j++)
        {
	  if (j == columnCount -1)
            stream <<theHeadings.at(j);	    
	  else
            stream <<theHeadings.at(j)<<", ";
        }
        stream <<endl;
        for (int i=0; i<rowCount; i++)
        {
            for (int j=0; j<columnCount; j++)
            {
                QTableWidgetItem *item_value = spreadsheet->item(i,j);
                double value = item_value->text().toDouble();
		if (j == columnCount-1)		
		  stream << value ;
		else
		  stream << value << ", ";		  
            }
            stream<<endl;
        }
	file.close();
    }
}

void DakotaResultsReliability::onSpreadsheetCellClicked(int row, int col)
{
    if (numSpreadsheetRows == 0)
        return;

    qDebug() << "row,col " << row << " " << col;
    mLeft = spreadsheet->wasLeftKeyPressed();

    // leave alone if pressed i first column
    if (col == 0)
        return;
    else
        //col2 = col;
        if (col%3 == 1) {
            col2 = col-1;
            col1 = col;
        } else if (col%3 == 2) {
            col2 = col-2;
            col1 = col;
        } else {
            col2 = col;
            col1 = col+1;
        }
    //
    // remove old
    //

    chart->removeAllSeries();

    QAbstractAxis *oldAxisX=chart->axisX();
    if (oldAxisX != 0)
        chart->removeAxis(oldAxisX);
    QAbstractAxis *oldAxisY=chart->axisY();
    if (oldAxisY != 0)
        chart->removeAxis(oldAxisY);

    //
    // create new line series & plot
    //

    int rowCount = spreadsheet->rowCount();


    double minX=std::numeric_limits<double>::infinity();
    double maxX=-std::numeric_limits<double>::infinity();
    double minY=std::numeric_limits<double>::infinity();
    double maxY=-std::numeric_limits<double>::infinity();

    if (rowCount>1)     {
        QLineSeries *series= new QLineSeries;
        for (int i=0; i<rowCount; i++) {
            QTableWidgetItem *itemY = spreadsheet->item(i,col1);
            QTableWidgetItem *itemX = spreadsheet->item(i,col2);

            double value1 = itemX->text().toDouble();
            double value2 = itemY->text().toDouble();
            if (value1 > maxX) maxX = value1;
            if (value1 < minX) minX = value1;
            if (value2 > maxY) maxY = value2;
            if (value2 < minY) minY = value2;
            series->append(value1, value2);
        }
        chart->addSeries(series);
       // series->setName("CDF");


        // if value is constant, adjust axes
        double axisMargin;
        if (minX==maxX) {
            axisMargin=abs(maxX-minX)*0.1;
            minX=minX-axisMargin;
            maxX=maxX+axisMargin;
        }
        if (minY==maxY) {
            axisMargin=abs(maxY-minY)*0.1;
            minY=minY-axisMargin;
            maxY=maxY+axisMargin;
        }

        QValueAxis *axisX = new QValueAxis();
        QValueAxis *axisY = new QValueAxis();

        axisX->setRange(minX, maxX);
        axisY->setRange(minY, maxY);


        axisX->setTitleText(theHeadings.at(col2));
        axisY->setTitleText(theHeadings.at(col1));

        axisY->setTickCount(5);
        axisX->setTickCount(NUM_DIVISIONS+1);

        chart->setAxisX(axisX, series);
        chart->setAxisY(axisY, series);

    } else {
        QScatterSeries *series= new QScatterSeries;
        QTableWidgetItem *itemY = spreadsheet->item(0,col1);
        QTableWidgetItem *itemX = spreadsheet->item(0,col2);
        double value1 = itemX->text().toDouble();
        double value2 = itemY->text().toDouble();
        series->append(value1, value2);
        chart->addSeries(series);
        series->setName("CDF");

        QValueAxis *axisX = new QValueAxis();
        QValueAxis *axisY = new QValueAxis();
        axisX->setRange(value1-abs(value1)*0.2, value1+abs(value1)*0.2);
        axisY->setRange(0., 1.);
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

    int numCol = spreadsheet->columnCount();
    int numRow = spreadsheet->rowCount();

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
    bool result = true;


    this->clear();

    //
    // check any data exists
    //

    QJsonObject &theObject = jsonObject;

    QJsonValue uqValue;
    if (jsonObject.contains("uqResults")) {
        uqValue = jsonObject["uqResults"];
        jsonObject = uqValue.toObject();
    } else
        theObject = jsonObject;


    QJsonValue spreadsheetValue = theObject["spreadsheet"];
    QJsonArray dataValue = spreadsheetValue["data"].toArray();

    if (dataValue.isEmpty()) { // ok .. if saved files but did not run a simulation
        return true;
    }

    //
    // create a summary widget in which place basic output (name, mean, stdDev)
    //
    /*
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
    */

    //
    // into a spreadsheet place all the data returned
    //


    //spreadsheet = new MyTableWidget();
    QJsonObject spreadsheetData = jsonObject["spreadsheet"].toObject();
    int numRow = spreadsheetData["numRow"].toInt();
    int numCol = spreadsheetData["numCol"].toInt();
    spreadsheet->setColumnCount(numCol);
    spreadsheet->setRowCount(numRow);
    numSpreadsheetRows = numRow;
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
    //spreadsheet->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(spreadsheet,SIGNAL(cellPressed(int,int)),this,SLOT(onSpreadsheetCellClicked(int,int)));

    spreadsheet->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->onSpreadsheetCellClicked(0,1);

    //QWidget *widget = new QWidget();
    //QVBoxLayout *layout = new QVBoxLayout(widget);
    //layout->addWidget(chartView);
    //layout->addWidget(spreadsheet);

    //qDebug()<<"\n debugging the values: result is  \n"<<result<<"\n";

    return result;
}


