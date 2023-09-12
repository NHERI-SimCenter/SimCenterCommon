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
// added and modified: padhye

#include "SimCenterUQResultsMFSampling.h"
//#include "InputWidgetFEM.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>
#include <QJsonDocument>
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
#include <QHeaderView>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QVXYModelMapper>
//using namespace QtCharts;
#include <math.h>
#include <QValueAxis>

#include <QXYSeries>
#include <RandomVariablesContainer.h>
#include <QFileInfo>
#include <QFile>

//#define NUM_DIVISIONS 10

SimCenterUQResultsMFSampling::SimCenterUQResultsMFSampling(RandomVariablesContainer *theRandomVariables, QWidget *parent)
  : UQ_Results(parent), theRVs(theRandomVariables)
{
    // title & add button
    theDataTable = NULL;
    tabWidget = new QTabWidget(this);
    layout->addWidget(tabWidget,1);
}

SimCenterUQResultsMFSampling::~SimCenterUQResultsMFSampling()
{

}


void SimCenterUQResultsMFSampling::clear(void)
{
    // delete any existing widgets
    int count = tabWidget->count();
    if (count > 0) {
        for (int i=0; i<count; i++) {
            QWidget *theWidget = tabWidget->widget(count);
            delete theWidget;
        }
    }
    //theHeadings.clear();
    //spreadsheet = NULL;
    theMeans.clear();
    theStdDevs.clear();
    theKurtosis.clear();
    theSkewness.clear();

    tabWidget->clear();
    theDataTable = NULL;
}



static void merge_helper(double *input, int left, int right, double *scratch)
{
    // if one element: done  else: recursive call and then merge
    if(right == left + 1) {
        return;
    } else {
        int length = right - left;
        int midpoint_distance = length/2;
        /* l and r are to the positions in the left and right subarrays */
        int l = left, r = left + midpoint_distance;

        // sort each subarray
        merge_helper(input, left, left + midpoint_distance, scratch);
        merge_helper(input, left + midpoint_distance, right, scratch);

        // merge the arrays together using scratch for temporary storage
        for(int i = 0; i < length; i++) {
            /* Check to see if any elements remain in the left array; if so,
            * we check if there are any elements left in the right array; if
            * so, we compare them.  Otherwise, we know that the merge must
            * use take the element from the left array */
            if(l < left + midpoint_distance &&
                    (r == right || fmin(input[l], input[r]) == input[l])) {
                scratch[i] = input[l];
                l++;
            } else {
                scratch[i] = input[r];
                r++;
            }
        }
        // Copy the sorted subarray back to the input
        for(int i = left; i < right; i++) {
            input[i] = scratch[i - left];
        }
    }
}

static int mergesort(double *input, int size)
{
    double *scratch = new double[size];
    if(scratch != NULL) {
        merge_helper(input, 0, size, scratch);
        delete [] scratch;
        return 1;
    } else {
        return 0;
    }
}

// if sobelov indices are selected then we would need to do some processing outselves

int SimCenterUQResultsMFSampling::processResults(QString &dirName)
{
  QString filenameOUT = dirName + QDir::separator() + tr("dakota.out");;
  QString filenameTAB = dirName + QDir::separator() + tr("dakotaTab.out");;
  return this->processResults(filenameOUT, filenameTAB);
}

int SimCenterUQResultsMFSampling::processResults(QString &filenameResults, QString &filenameTab)
{
    Q_UNUSED(filenameResults);
    statusMessage(tr("Processing Sampling Results"));

    this->clear();

    //
    // check it actually ran with no errors
    //

    QFileInfo fileTabInfo(filenameTab);

    QString errMsg("");
    this->extractErrorMsg( fileTabInfo.absolutePath(),"dakota.err", "SimCenterUQ", errMsg);

    if (errMsg.length() != 0) {
        errorMessage(errMsg);
        return 0;
    }

    QFileInfo filenameTabInfo(filenameTab);
    if (!filenameTabInfo.exists()) {
        errorMessage("No SimCenterUQTab.out file - SimCenterUQ failed .. possibly no QoI");
        return 0;
    }

    //
    // Load summary contents
    //


    //
    // open file
    //

    QFile Resfile(filenameResults);

    // place contents of file into json object
    QString val;
    if (Resfile.open(QFile::ReadOnly | QFile::Text)) {
        val=Resfile.readAll();
        Resfile.close();
    }

    val.replace(QString("NaN"),QString("null"));
    val.replace(QString("Infinity"),QString("inf"));

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    resObj = doc.object();



    //
    // create summary, a QWidget for summary data, the EDP name, mean, stdDev, kurtosis info
    //

    // create a scrollable windows, place summary inside it
    QScrollArea *sa = new QScrollArea;
    this->createSummary(sa);

    //
    // create spreadsheet,  a QTableWidget showing RV and results for each run
    //

    theDataTable = new ResultsDataChart(filenameTab,  theRVs->getNumRandomVariables());

    //
    // add summary, detained info and spreadsheet with chart to the tabed widget
    //

    tabWidget->addTab(sa,tr("Summary"));
    tabWidget->addTab(theDataTable, tr("Data Values"));
    tabWidget->adjustSize();

    statusMessage(tr(""));

    return 0;
}

bool
SimCenterUQResultsMFSampling::createSummary(QScrollArea *&sa)
{

    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    QWidget *summary = new QWidget();
    QVBoxLayout *summaryLayout = new QVBoxLayout();
    summaryLayout->setContentsMargins(0,0,0,0); // adding back
    summary->setLayout(summaryLayout);

    sa->setWidget(summary);


    QVector<QVector<double>> statisticsVector;
    QVector<QString> NamesVector;
    QVector<int> flags;
    bool atLeastOneIs1 = false;
    bool atLeastOneIs2 = false;
    getNamesAndSummary(NamesVector,statisticsVector, flags);
    bool did_log_transform = resObj["Log_transform"].toBool();

    for (int nq = 0; nq<NamesVector.size(); nq++) {
        QWidget *theWidget = this->createResultEDPWidget(NamesVector[nq], statisticsVector[nq], flags[nq],did_log_transform);
        summaryLayout->addWidget(theWidget);

        if (flags[nq]==1) {
            atLeastOneIs1 = true;
        } else if (flags[nq]==2) {
            // flag1 is more critical so if the qoi shares both flag1 and flag2, we show only flag1
            atLeastOneIs2 = true;
        }
    }

    if (atLeastOneIs1) {
        QLabel * myWarning1 ;
        if (atLeastOneIs2) {
            myWarning1 = new QLabel(QString("*Warning: Multi-fidelity estimate of the variance is negative. High-fidelity variance is displayed."));
        } else {
            myWarning1 = new  QLabel(QString("**Warning: Multi-fidelity estimate of the variance is negative. High-fidelity variance is displayed. \nCarefully check the <Data Values> tab to see if the analysis results match your intuition."));
        }
        myWarning1->setStyleSheet("QLabel {color:red}");
        summaryLayout->addWidget(myWarning1);
    }
    if (atLeastOneIs2) {
        QLabel * myWarning2 = new  QLabel(QString("**Warning: The results may not be accurate as the model variability is very large. \nCarefully check the <Data Values> tab to see if the analysis results match your intuition."));
        myWarning2->setStyleSheet("QLabel {color:blue}");
        summaryLayout->addWidget(myWarning2);
    }

    /*
    myWarning += QString("Warning: Multi-fidelity estimate of the variance is negative. High-fidelity variance is displayed");
    myWarning += QString("\nWarning: The results may not be accuracy as the coefficient of variance is too large.");
    */

    summaryLayout->setSpacing(0);

    double analysis_time = resObj["AnalysisTime_sec"].toDouble();

    QString infoMsg;
    if (analysis_time>180) {
        QString elapsCutoff = QString::number(analysis_time/60, 'f', 1);
        infoMsg = "Elapsed time: " + elapsCutoff + " mins";
    } else {
        QString elapsCutoff = QString::number(analysis_time, 'f', 1);
        infoMsg = "Elapsed time: " + elapsCutoff + " s";
    }

    QJsonObject infoObj = resObj["Info"].toObject()["models"].toObject();
    int numModels = infoObj.size();
    for (int nm =0; nm<numModels; nm++) {
        //
        // Computing correlation coefficient
        //

        QJsonObject modelObj = infoObj["model" + QString::number(nm+1)].toObject();
        int neval = modelObj["nPilot"].toInt();
        neval += modelObj["nAdd"].toInt();

        double cost = modelObj["cost_sec_per_sim"].toDouble();
        QString costCutoff;
        if (cost>600) {
            costCutoff = QString::number(cost/60, 'f', 1)+ " mins";
         } else if (cost>0.1) {
            costCutoff = QString::number(cost, 'f', 1)+ " s";
        } else  if (cost>0.001)  {
            costCutoff = QString::number(cost, 'f', 4)+ " s";
        } else {
            costCutoff = QString::number(cost)+ " s";
        }
        infoMsg += "\n Model " + QString::number(nm+1) + " is evaluated " + QString::number(neval) + " times. The computation time per evaluation was " + costCutoff + ".";
    }

    summaryLayout->addWidget(new QLabel(infoMsg));

    summaryLayout->addStretch();

    return true;
}

bool
SimCenterUQResultsMFSampling::getNamesAndSummary(QVector<QString> & qoiNames, QVector<QVector<double>> & statistics, QVector<int> & flags) {

    QJsonObject qoiObj = resObj["QoI"].toObject();
    QJsonObject hfObj = resObj["Info"].toObject()["models"].toObject()["model1"].toObject();
    QJsonObject lfObj = resObj["Info"].toObject()["models"].toObject()["model2"].toObject(); // first low fidelity model

    int nqoi = qoiObj["qoiNames"].toArray().size();

    QJsonArray name_array = qoiObj["qoiNames"].toArray();
    QJsonArray mean_array = qoiObj["mean"].toArray();
    QJsonArray std_array = qoiObj["standardDeviation"].toArray();
    QJsonArray var_HF_array = hfObj["modelVar"].toArray();

    QJsonArray speedUp_array = qoiObj["speedUp"].toArray();

    for (int nq =0; nq<nqoi; nq++) {
        double myStd =  std_array[nq].toDouble();
        double myVariabilityScore = lfObj["mean_diff"].toArray()[nq].toDouble()/std::sqrt(lfObj["modelVar"].toArray()[nq].toDouble());

        int flag = 0;
        //
        // Check if MF estimate of variance is negative
        //

        if (myStd==0){
            myStd =std::sqrt(var_HF_array[nq].toDouble());
            flag = 1;
           // myWarning += QString("Warning: Multi-fidelity estimate of the variance is negative. High-fidelity variance is displayed");
        }

        if (std::abs(myVariabilityScore)>0.3){
            flag = 2;
          //  myWarning += QString("\nWarning: The results may not be accuracy as the coefficient of variance is too large.");
        }

        qoiNames.push_back(name_array[nq].toString());
        statistics.push_back( {mean_array[nq].toDouble(), myStd, speedUp_array[nq].toDouble() , speedUp_array[nq+nqoi].toDouble()});
        flags.push_back(flag);
    }
    return 0;
};

// padhye
// this function is called if you decide to say save the data from UI into a json object
bool
SimCenterUQResultsMFSampling::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;


    jsonObject["resultType"]=QString(tr("SimCenterUQResultsMFSampling"));

    //
    // add summary data
    //

    QJsonArray resultsData;
    int numEDP = theNames.count();
    for (int i=0; i<numEDP; i++) {
        QJsonObject edpData;
        edpData["name"]=theNames.at(i);
        edpData["mean"]=theMeans.at(i);
        edpData["stdDev"]=theStdDevs.at(i);
        //edpData["kurtosis"]=theKurtosis.at(i);
        //edpData["skewness"]=theSkewness.at(i);
        resultsData.append(edpData);
    } // not going to be used

    jsonObject["summary"]=resultsData;

    jsonObject["MFres"] = resObj;

    //
    // add spreadsheet data
    //

    if(theDataTable != NULL) {
        theDataTable->outputToJSON(jsonObject);
    }
    return result;
}



// if you already have a json data file then you can populate the UI with the entries from json.

bool
SimCenterUQResultsMFSampling::inputFromJSON(QJsonObject &jsonObject)
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
    if (spreadsheetValue.isNull()) { // ok .. if saved files but did not run a simulation
        return true;
    }

    //
    // create a summary widget in which place basic output (name, mean, stdDev)
    //

    resObj = jsonObject["MFres"].toObject();
    QScrollArea *sa = new QScrollArea;
    this->createSummary(sa);
    //    sa->setWidgetResizable(true);
    //    sa->setLineWidth(0);
    //    sa->setFrameShape(QFrame::NoFrame);

    //    QWidget *summary = new QWidget();
    //    QVBoxLayout *summaryLayout = new QVBoxLayout();
    //    summaryLayout->setContentsMargins(0,0,0,0); // adding back
    //    summary->setLayout(summaryLayout);

    //    sa->setWidget(summary);

    theDataTable = new ResultsDataChart(spreadsheetValue.toObject());


    //
    // determine summary statistics for each edp
    //

//    QVector<QVector<double>> statisticsVector;
//    QVector<QString> NamesVector;
//    getNamesAndSummary(filepath, NamesVector,statisticsVector);

//    for (int col = theRVs->getNumRandomVariables()+1; col<NamesVector.size(); ++col) { // +
//        QWidget *theWidget = this->createResultEDPWidget(NamesVector[col], statisticsVector[col]);
//        summaryLayout->addWidget(theWidget);
//    }
//    summaryLayout->addStretch();

    //
    // add summary, detained info and spreadsheet with chart to the tabed widget
    //

    tabWidget->addTab(sa,tr("Summary"));
    tabWidget->addTab(theDataTable, tr("Data Values"));
    tabWidget->adjustSize();

    return result;
}


extern QWidget *addLabeledLineEdit(QString theLabelName, QLineEdit **theLineEdit);

QWidget *
SimCenterUQResultsMFSampling::createResultEDPWidget(QString &name, QVector<double> statistics, int flag, bool log_transform) {

    bool do_ske_kur = false;

    double mean = statistics[0];
    double stdDev = statistics[1];
    double speedUp_m = statistics[2];
    double speedUp_v = statistics[3];
    double skewness, kurtosis;

    if (do_ske_kur) {
        skewness = statistics[2];
        kurtosis = statistics[3];
    }

    QWidget *edp = new QWidget;
    QHBoxLayout *edpLayout = new QHBoxLayout();

    edp->setLayout(edpLayout);

    QLineEdit *nameLineEdit;
    QWidget *nameWidget = addLabeledLineEdit(QString("Name"), &nameLineEdit);
    nameLineEdit->setText(name);
    nameLineEdit->setReadOnly(true);
    theNames.append(name);
    edpLayout->addWidget(nameWidget);


    QString meanStr = QString("Mean");
    QString stdStr = QString("StdDev");
    if (log_transform) {
        meanStr = "Log " + meanStr;
        stdStr = "Log " + stdStr;
    }

    QLineEdit *meanLineEdit;
    QWidget *meanWidget = addLabeledLineEdit(meanStr, &meanLineEdit);
    meanLineEdit->setText(QString::number(mean));
    meanLineEdit->setReadOnly(true);
    theMeans.append(mean);
    edpLayout->addWidget(meanWidget);

    QLineEdit *stdDevLineEdit;
    QWidget *stdDevWidget;
    if (flag == 1 ) { // HF variance
        stdStr += "*";
        stdDevWidget = addLabeledLineEdit(stdStr, &stdDevLineEdit);
        stdDevWidget->setStyleSheet("QLabel {color:red}");
    } else if (flag == 2) {// large c.o.v. warning
        stdStr += "**";
        stdDevWidget = addLabeledLineEdit(stdStr, &stdDevLineEdit);
        stdDevWidget->setStyleSheet("QLabel {color:blue}");
    } else {
        stdDevWidget = addLabeledLineEdit(stdStr, &stdDevLineEdit);
    }
    stdDevLineEdit->setText(QString::number(stdDev));
    stdDevLineEdit->setReadOnly(true);
    theStdDevs.append(stdDev);
    edpLayout->addWidget(stdDevWidget);

    QLineEdit *speedUpLineEdit;
    QWidget *speedUpWidget = addLabeledLineEdit(QString("Speed Up (1st order moment)"), &speedUpLineEdit);
    speedUpLineEdit->setText(QString::number(speedUp_m));
    speedUpLineEdit->setReadOnly(true);
    theSpeedUps.append(speedUp_m);
    edpLayout->addWidget(speedUpWidget);

    QLineEdit *speedUpLineEdit2;
    QWidget *speedUpWidget2 = addLabeledLineEdit(QString("Speed Up (2nd order moment)"), &speedUpLineEdit2);
    speedUpLineEdit2->setText(QString::number(speedUp_v));
    speedUpLineEdit2->setReadOnly(true);
    theSpeedUps.append(speedUp_v);
    edpLayout->addWidget(speedUpWidget2);


    if (do_ske_kur) {
        QLineEdit *skewnessLineEdit;
        QWidget *skewnessWidget = addLabeledLineEdit(QString("Skewness"), &skewnessLineEdit);
        skewnessLineEdit->setText(QString::number(skewness));
        skewnessLineEdit->setReadOnly(true);
        theSkewness.append(skewness);
        edpLayout->addWidget(skewnessWidget);

        QLineEdit *kurtosisLineEdit;
        QWidget *kurtosisWidget = addLabeledLineEdit(QString("Kurtosis"), &kurtosisLineEdit);
        kurtosisLineEdit->setText(QString::number(kurtosis));
        kurtosisLineEdit->setReadOnly(true);
        theKurtosis.append(kurtosis);
        edpLayout->addWidget(kurtosisWidget);
    }

    edpLayout->addStretch();

    return edp;
}
