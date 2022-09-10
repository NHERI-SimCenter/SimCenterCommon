#include "DakotaResultsOptimization.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>

#include <QTabWidget>
#include <QTextEdit>
#include <MyTableWidget.h>
#include <QDebug>
#include <QHBoxLayout>
#include <QColor>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <QMessageBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QVXYModelMapper>
using namespace QtCharts;
#include <math.h>
#include <QValueAxis>
#include <QPushButton>
#include <QFileDialog>

#include <QXYSeries>
#include "ResultsDataChart.h"


DakotaResultsOptimization::DakotaResultsOptimization(RandomVariablesContainer *theRandomVariables, QWidget *parent)
    : UQ_Results(parent), theRVs(theRandomVariables)
{
    // title & add button
    theDataTable = NULL;
    tabWidget = new QTabWidget(this);
    layout->addWidget(tabWidget,1);
}

DakotaResultsOptimization::~DakotaResultsOptimization()
{

}

void DakotaResultsOptimization::clear(void)
{
    tabWidget->clear();
    theDataTable = NULL;
}

bool
DakotaResultsOptimization::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    int numEDP = theNames.count();

    // quick return .. noEDP -> no analysis done -> no results out
    if (numEDP == 0)
      return true;

    jsonObject["resultType"]=QString(tr("DakotaResultsOptimization"));
    jsonObject["isSurrogate"]=isSurrogate;

    //
    // add summary data
    //

    QJsonArray resultsData;
    for (int i=0; i<numEDP; i++) {
        QJsonObject edpData;
        edpData["name"]=theNames.at(i);
        edpData["best_value"]=theBestValues.at(i);
        resultsData.append(edpData);
    }
    jsonObject["summary"]=resultsData;

    // add general data
    jsonObject["general"]=dakotaText->toPlainText();

    //
    // add spreadsheet data
    //
    if(theDataTable != NULL) {
        theDataTable->outputToJSON(jsonObject);
    }
    return result;

}


bool
DakotaResultsOptimization::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;
    this->clear();

    if (!jsonObject.contains("summary")) { // no saving of analysis data
       return true;
    }


    //
    // create a summary widget
    //

    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    QWidget *summary = new QWidget();
    QVBoxLayout *summaryLayout = new QVBoxLayout();
    summaryLayout->setContentsMargins(0,0,0,0); // adding back
    summary->setLayout(summaryLayout);

    sa->setWidget(summary);

    QJsonArray edpArray = jsonObject["summary"].toArray();
    foreach (const QJsonValue &edpValue, edpArray) {
        QString name;
        double bestValue;
        QJsonObject edpObject = edpValue.toObject();
        QJsonValue theNameValue = edpObject["name"];
        name = theNameValue.toString();

        QJsonValue theBestValue = edpObject["best_value"];
        bestValue = theBestValue.toDouble();

        QWidget *theWidget = this->createResultParameterWidget(name, bestValue);
        summaryLayout->addWidget(theWidget);
    }
    summaryLayout->addStretch();

    //
    // into dakotaText place more detailed Dakota text
    //

    dakotaText = new QTextEdit();
    dakotaText->setReadOnly(true); // make it so user cannot edit the contents
    QJsonValue theValue = jsonObject["general"];
    dakotaText->setText(theValue.toString());

    //
    // into a spreadsheet place all the data returned
    //

    QJsonValue spreadsheetValue = jsonObject["spreadsheet"];
    if (spreadsheetValue.isNull()) { // ok .. if saved files but did not run a simulation
        return true;
    }

    if (jsonObject.contains("isSurrogate")) { // no saving of analysis data
        isSurrogate=jsonObject["isSurrogate"].toBool();
    } else {
        isSurrogate=false;
    }

    theDataTable = new ResultsDataChart(spreadsheetValue.toObject(), isSurrogate, theRVs->getNumRandomVariables());

    tabWidget->addTab(sa,tr("Summary"));
    tabWidget->addTab(dakotaText,tr("General"));
    tabWidget->addTab(theDataTable, tr("Data Values"));
    tabWidget->adjustSize();
    return result;

}


//static void merge_helper(double *input, int left, int right, double *scratch)
//{
//    // if one element: done  else: recursive call and then merge
//    if(right == left + 1) {
//        return;
//    } else {
//        int length = right - left;
//        int midpoint_distance = length/2;
//        /* l and r are to the positions in the left and right subarrays */
//        int l = left, r = left + midpoint_distance;

//        // sort each subarray
//        merge_helper(input, left, left + midpoint_distance, scratch);
//        merge_helper(input, left + midpoint_distance, right, scratch);

//        // merge the arrays together using scratch for temporary storage
//        for(int i = 0; i < length; i++) {
//            /* Check to see if any elements remain in the left array; if so,
//            * we check if there are any elements left in the right array; if
//            * so, we compare them.  Otherwise, we know that the merge must
//            * use take the element from the left array */
//            if(l < left + midpoint_distance &&
//                    (r == right || fmin(input[l], input[r]) == input[l])) {
//                scratch[i] = input[l];
//                l++;
//            } else {
//                scratch[i] = input[r];
//                r++;
//            }
//        }
//        // Copy the sorted subarray back to the input
//        for(int i = left; i < right; i++) {
//            input[i] = scratch[i - left];
//        }
//    }
//}

//static int mergesort(double *input, int size)
//{
//    double *scratch = new double[size];
//    if(scratch != NULL) {
//        merge_helper(input, 0, size, scratch);
//        delete [] scratch;
//        return 1;
//    } else {
//        return 0;
//    }
//}

int DakotaResultsOptimization::processResults(QString &dirName)
{
  qDebug() << "DakotaResultsCalibration::processResults dir" << dirName;
  QString filenameOut = dirName + QDir::separator() + tr("dakota.out");
  QString filenameTAB = dirName + QDir::separator() + tr("dakotaTab.out");
  return this->processResults(filenameOut, filenameTAB);
}

int DakotaResultsOptimization::processResults(QString &filenameResults, QString &filenameTab) {

    statusMessage(tr("Dakota Optimization Processing Results"));

    this->clear();

    //
    // check it actually ran with no errors
    //

    QFileInfo fileTabInfo(filenameTab);
    QString filenameErrorString = fileTabInfo.absolutePath() + QDir::separator() + QString("dakota.err");

    QFileInfo filenameErrorInfo(filenameErrorString);
    if (!filenameErrorInfo.exists()) {
        errorMessage("No dakota.err file - dakota did not run - problem with dakota setup or the applications failed with inputs provided");
    return 0;
    }


    QFileInfo filenameTabInfo(filenameTab);
    if (!filenameTabInfo.exists()) {
        errorMessage("No dakotaTab.out file - dakota failed .. possibly no QoI");
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

    if (line.length() != 0) {
        errorMessage(QString(QString("Error Running Dakota: ") + line));
        return 0;
    }

    //
    // open Dakota output file
    //

    std::ifstream fileResults(filenameResults.toStdString().c_str());
    if (!fileResults.is_open()) {
        qDebug() << "Could not open file: " << filenameResults;
        return -1;
    }


    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    summary = new QWidget();
    summaryLayout = new QVBoxLayout();
//    summaryLayout->setContentsMargins(0,0,0,0);
    summary->setLayout(summaryLayout);
    sa->setWidget(summary);

    dakotaText = new QTextEdit();
    dakotaText->setReadOnly(true); // make it so user cannot edit the contents


    // now ignore every line until Best Parameters

    const std::string needle = "Best parameters          =";
    const std::string needle2 = "Best objective function  =";
    const std::string needle3 = "Original";
    std::string haystack;

    while (std::getline(fileResults, haystack)) {
        if (haystack.find(needle) != std::string::npos) {
            break;
        }
    }

    //
    // now copy line and every subsequent line into text editor
    //  - also if still collecting summary data, add each EDP's summary info
    //

    dakotaText->setText("\n");
    dakotaText->append(haystack.c_str());

    bool isSummaryDone = false;

    while (std::getline(fileResults, haystack)) {
        std::cerr << haystack << "\n";
        dakotaText->append(haystack.c_str());
        if (isSummaryDone == false) {

            if (haystack.find(needle2) != std::string::npos || haystack.find(needle3) != std::string::npos) {
                isSummaryDone = true;
            } else {
                //
                // add sumary info
                //

                std::istringstream iss(haystack);
                std::string subs;

                iss >> subs;
                QString bestText(QString::fromStdString(subs));
                double best = bestText.toDouble();

                iss >> subs;
                QString  nameString(QString::fromStdString(subs));

                QWidget *theWidget = this->createResultParameterWidget(nameString, best);
                summaryLayout->addWidget(theWidget);
            }
        }
    }
    summaryLayout->addStretch();

    // close input file
    fileResults.close();

    //
    // now into a QTableWidget copy the random variable and edp's of each black box run
    //

    //spreadsheet = new MyTableWidget();

//    QFileInfo filenameTabInfo(filenameTab);
//    if (!filenameTabInfo.exists()) {
//        errorMessage("No dakotaTab.out file - dakota failed .. possibly no QoI");
//        return 0;
//    }

    // If surrogate model is used, display additional info.
    QDir tempFolder(filenameTabInfo.absolutePath());
    QFileInfo surrogateTabInfo(tempFolder.filePath("surrogateTab.out"));
    if (surrogateTabInfo.exists()) {
        filenameTab = tempFolder.filePath("surrogateTab.out");
        isSurrogate = true;
    } else {
        isSurrogate = false;
    }



    //
    // create spreadsheet,  a QTableWidget showing RV and results for each run
    //

    theDataTable = new ResultsDataChart(filenameTab, isSurrogate, theRVs->getNumRandomVariables());


    tabWidget->addTab(sa,tr("Summary"));
    tabWidget->addTab(dakotaText, tr("General"));
    tabWidget->addTab(theDataTable, tr("Data Values"));
    tabWidget->adjustSize();

    statusMessage(tr(""));

    return 0;
}

extern QWidget *addLabeledLineEdit(QString theLabelName, QLineEdit **theLineEdit);


QWidget *
DakotaResultsOptimization::createResultParameterWidget(QString &name, double bestValue) {
    QWidget *edp = new QWidget;
    QHBoxLayout *edpLayout = new QHBoxLayout();
    edpLayout->setContentsMargins(0,0,0,0);
    edpLayout->setSpacing(3);

    edp->setLayout(edpLayout);

    QLineEdit *nameLineEdit;
    QWidget *nameWidget = addLabeledLineEdit(QString("Name"), &nameLineEdit);
    nameLineEdit->setText(name);
    nameLineEdit->setDisabled(true);
    theNames.append(name);
    edpLayout->addWidget(nameWidget);

    QLineEdit *meanLineEdit;
    QWidget *meanWidget = addLabeledLineEdit(QString("Best Parameter"), &meanLineEdit);
    meanLineEdit->setText(QString::number(bestValue));
    meanLineEdit->setDisabled(true);
    theBestValues.append(bestValue);
    edpLayout->addWidget(meanWidget);

    edpLayout->addStretch();

    return edp;
}
