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

#include "SimCenterUQResultsSensitivity.h"
//#include "InputWidgetFEM.h"
#include "qbarset.h"
#include <QBarCategoryAxis>
#include <QBarSeries>

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

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

//#include <QMessageBox>
#include <QVBoxLayout>
#include <QComboBox>
#include <QStackedWidget>
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


#define NUM_DIVISIONS 10



//QLabel *best_fit_label_text;


SimCenterUQResultsSensitivity::SimCenterUQResultsSensitivity(RandomVariablesContainer *theRandomVariables, QWidget *parent)
    : UQ_Results(parent), theRVs(theRandomVariables)
{
    // title & add button
    theDataTable = NULL;
    tabWidget = new QTabWidget(this);
    layout->addWidget(tabWidget,1);
}

SimCenterUQResultsSensitivity::~SimCenterUQResultsSensitivity()
{

}


void SimCenterUQResultsSensitivity::clear(void)
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

int SimCenterUQResultsSensitivity::processResults(QString &dirName)
{
  QString filenameOUT = dirName + QDir::separator() + tr("dakota.out");;
  QString filenameTAB = dirName + QDir::separator() + tr("dakotaTab.out");;
  return this->processResults(filenameOUT, filenameTAB);
}

int SimCenterUQResultsSensitivity::processResults(QString &filenameResults, QString &filenameTab)
{
    statusMessage(tr("Processing Sampling Results"));

    this->clear();

    //
    // check it actually ran with errors
    //

    QFileInfo fileResInfo(filenameResults);
    QString filenameErrorString = fileResInfo.absolutePath() + QDir::separator() + QString("dakota.err");

    QFileInfo filenameErrorInfo(filenameErrorString);
    if (!filenameErrorInfo.exists()) {
        errorMessage("No error file - SimCenterUQ did not run - problem with the application setup or the applications failed with inputs provided");
        return 0;
    }
    QFile fileError(filenameErrorString);
    QString line("");
    if (fileError.open(QIODevice::ReadOnly)) {
        QTextStream in(&fileError);
        QString contents = in.readAll();
//        while (!in.atEnd()) {
//            line = in.readLine();
//        }
        if (!contents.isEmpty()) {
            //errorMessage(QString(QString("Error Running SimCenterUQ: ") + line));
            errorMessage(QString(QString("Error Running SimCenterUQ: ") + contents));
            return 0;
        }
    }


    // If surrogate model is used, display additional info.
//    QDir tempFolder(fileResInfo.absolutePath());
//    QFileInfo surrogateTabInfo(tempFolder.filePath("surrogateTab.out"));
//    if (surrogateTabInfo.exists()) {
//        filenameTab = tempFolder.filePath("surrogateTab.out");
//        isSurrogate = true;
//    } else {
//        isSurrogate = false;
//    }

    //
    // create summary, a QWidget for summary data, the EDP name, mean, stdDev, kurtosis info
    //

    QScrollArea *sa = new QScrollArea;

    //
    // First Read Sobol Indecies
    //

    // open file containing data
    std::ifstream fileResults(filenameResults.toStdString().c_str());
    if (!fileResults.is_open()) {
        errorMessage("No error file - SimCenterUQ did not run - problem with the application setup or the applications failed with inputs provided");
        return 0;
    }

    std::string readline;

    bool usedData = false;
    QString inpPath(""), outPath("");
    getline(fileResults, readline);// header- data generation OR number of input combinations
    if (readline.compare("* data generation")==0){
        getline(fileResults, readline);// header- data generation
        if (readline.compare("Import Data Files")==0){
            usedData= true;
            getline(fileResults, readline);// path_input
            inpPath = QString::fromStdString(readline);
            getline(fileResults, readline);// path_input
            outPath = QString::fromStdString(readline);
        }

        getline(fileResults, readline);// header- number of input combinations
        getline(fileResults, readline);// value- number of input combinations
        ncomb=atoi(readline.c_str());
    } else {
        getline(fileResults, readline);// value- number of input combinations
        ncomb=atoi(readline.c_str());
    }


    getline(fileResults, readline);// header- input names
    for (int nc=0; nc<ncomb; nc++) {
        getline(fileResults, readline);
        combs.push_back(QString::fromStdString(readline));
    }

    nAggQoI=0;
    getline(fileResults, readline);// header- number of aggregated outputs OR number of outputs
    if (readline.compare("* number of aggregated outputs")==0){
        getline(fileResults, readline);// value
        nAggQoI=atoi(readline.c_str());

        getline(fileResults, readline);// header- number of outputs
        getline(fileResults, readline);// value
        nQoI=atoi(readline.c_str());
    } else {
        getline(fileResults, readline);// value
        nQoI=atoi(readline.c_str());
    }

    getline(fileResults, readline);// header- output names
    for (int nq=0; nq<nQoI+nAggQoI; nq++) {
        getline(fileResults, readline);// value
        QoInames.push_back(QString::fromStdString(readline));
    }

    getline(fileResults, readline);// header- Sm(E) Sm(P) Sm(Ao) Sm(Au) St(E) St(P) St(Ao) St(Au)
    for (int nq=0; nq<nQoI+nAggQoI; nq++) {
        QVector<double> sobols_vec;
        getline(fileResults, readline);// value
        std::istringstream stm(readline) ;
        double val ;
        while( stm >> val ) sobols_vec.push_back(val) ;
        sobols.push_back(sobols_vec);
    }

    getline(fileResults, readline);// header- number of samples
    getline(fileResults, readline);// value
    int nSamp=atoi(readline.c_str());

    getline(fileResults, readline);// header- elapsed time
    getline(fileResults, readline);// value
    std::istringstream strm(readline);
    std::string elaps_str;
    strm >> elaps_str; // only read the first number
    elaps=QString::fromStdString(elaps_str);

    // default - older version.
    performedPCA=QString("no");
    PCAncomp = "";
    PCAvarRatio = "";
    // for newer version - sy.
    if (!fileResults.eof()) {
        getline(fileResults, readline);// header- PCA
        getline(fileResults, readline);// value
        performedPCA=QString::fromStdString(readline);
        if (performedPCA==QString("yes")) {
            getline(fileResults, readline);// header- number of PCA components
            getline(fileResults, readline);// value
            PCAncomp=QString::fromStdString(readline);

            getline(fileResults, readline);// header- PCA
            getline(fileResults, readline);// value
            PCAvarRatio=QString::fromStdString(readline);
        }
    }



    gsaGraph(*&sa);


    fileResults.close();

    //
    // create spreadsheet,  a QTableWidget showing RV and results for each run
    //

    if (!usedData) {
            theDataTable = new ResultsDataChart(filenameTab,  theRVs->getNumRandomVariables());
    } else {
        QStringList QoIlist; // w
        for (int i=nAggQoI;i<(nAggQoI+nQoI); i++){
            QoIlist <<QoInames[i];
        }
        theDataTable = new ResultsDataChart(inpPath, outPath, ncomb, nQoI, nSamp, combs.toList(), QoIlist);
    }
    //theDataTable = new ResultsDataChart(filenameTab);

    //
    // add summary, detained info and spreadsheet with chart to the tabed widget
    //

    tabWidget->addTab(sa,tr("Summary"));
    tabWidget->addTab(theDataTable, tr("Data Values"));
    tabWidget->adjustSize();

    statusMessage(tr(""));

    return 0;
}


void SimCenterUQResultsSensitivity::gsaGraph(QScrollArea *&sa)
{
    int nQoIall = nQoI + nAggQoI;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);
    QWidget *summary = new QWidget();
    sa->setWidget(summary);
    QVBoxLayout *summaryLayout = new QVBoxLayout();
    summary->setLayout(summaryLayout);

    if (nQoIall<20) {
        for (int nq=0; nq<nQoIall; nq++){
            QGroupBox *groupBox;
            bool useAnimation = true;
            getGroupBox(groupBox, nq, useAnimation);
            summaryLayout->addWidget(groupBox);
        }
    }
    else if (nQoIall<1000) {
        QComboBox *qoiSelection = new QComboBox();
        qoiSelection->setMinimumWidth(minimumSizeHint().width()*1.2);
        qoiSelection->setMaximumWidth(minimumSizeHint().width()*1.2);
        bool useAnimation = true;

        for (int nq=0; nq<nQoIall; nq++)
        {
            qoiSelection->addItem(QoInames[nq]);
        }
        QGroupBox *groupBox;
        getGroupBox(groupBox, 0, useAnimation);
        summaryLayout->addWidget(qoiSelection);
        summaryLayout->addWidget(groupBox);
        QLabel * warningMsg = new QLabel("The component sobol indices can be saved using \"save results\" button");
        warningMsg->setStyleSheet({"color: blue"});
        summaryLayout->addWidget(warningMsg);

        connect(qoiSelection,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,[=](int selec) {
            QLayoutItem  *w = summaryLayout->takeAt(1);
            summaryLayout->removeItem(w);
            delete w->widget();
            delete w;
            //delete summaryLayout->takeAt(1);
            QGroupBox *groupBox;
            getGroupBox(groupBox, selec, useAnimation);
            summaryLayout->insertWidget(1, groupBox);
        });
    } else {
        // Display only aggregated ones...
        QComboBox *qoiSelection = new QComboBox();
        qoiSelection->setMinimumWidth(minimumSizeHint().width()*1.2);
        qoiSelection->setMaximumWidth(minimumSizeHint().width()*1.2);
        bool useAnimation = true;

        for (int nq=0; nq<nAggQoI; nq++)
        {
            qoiSelection->addItem(QoInames[nq]);
        }
        QGroupBox *groupBox;
        getGroupBox(groupBox, 0, useAnimation);
        summaryLayout->addWidget(qoiSelection);
        summaryLayout->addWidget(groupBox);

        connect(qoiSelection,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,[=](int selec) {
            QLayoutItem  *w = summaryLayout->takeAt(1);
            summaryLayout->removeItem(w);
            delete w->widget();
            delete w;
            //delete summaryLayout->takeAt(1);
            QGroupBox *groupBox;
            getGroupBox(groupBox, selec, useAnimation);
            summaryLayout->insertWidget(1, groupBox);
        });
    }
    summaryLayout->setContentsMargins(0,0,0,0); // adding back
    summaryLayout->setSpacing(10);
    //summaryLayout->setMargin(10);

    QString elapsCutoff = QString::number(elaps.toDouble(), 'f', 1);
    QLabel *elapsLabel = new QLabel(QString("Elapsed time:")+elapsCutoff+QString("s"));
    elapsLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    summaryLayout -> addWidget(elapsLabel);

    //
    // PCA summary
    //
    if (performedPCA==QString("yes")) {
        QString PCAratioPerc = QString::number(100*PCAvarRatio.toDouble());
        QLabel *PCALabel = new QLabel(QString("PCA is performed for QoI. "+PCAratioPerc+"% of the total variance is explained by "+PCAncomp+" principal component(s)"));
        PCALabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        summaryLayout -> addWidget(PCALabel);
    }

    //
    // save button
    //

    QPushButton * saveData = new QPushButton("Save Results");
    saveData->setMaximumWidth(150);
    summaryLayout->addWidget(saveData);
    summaryLayout->setAlignment(saveData,Qt::AlignRight);
    connect(saveData,SIGNAL(clicked()),this,SLOT(onSaveButtonClicked()));

    //
    //
    //


    summaryLayout->addStretch();
}

void
SimCenterUQResultsSensitivity::getGroupBox(QGroupBox *&groupBox, int nq, bool useAnimation) {

    //QStringList theTableHeadings;
    //theTableHeadings << "Random Variable" << "Main" << "Total";

    QGridLayout * trainingDataLayout = NULL;
    QString h(QoInames[nq]+" Sobol' Indicies");
    groupBox = new QGroupBox(h);

    trainingDataLayout = new QGridLayout();
    QLabel *l1 = new QLabel("Random Variable");
    QLabel *l2 = new QLabel("Main");
    QLabel *l3 = new QLabel("Total");
    trainingDataLayout->addWidget(l1, 0,0);
    trainingDataLayout->addWidget(l2, 0,1);
    trainingDataLayout->addWidget(l3, 0,2);



    QFont font;
    font.setBold(true);
    font.setPixelSize(12);

    l1->setMaximumHeight(25);
    l1->setMinimumHeight(25);
    l1->setAlignment(Qt::AlignCenter);
    l1->setFont(font);

    l2->setMaximumHeight(25);
    l2->setMinimumHeight(25);
    l2->setAlignment(Qt::AlignCenter);
    l2->setFont(font);

    l3->setMaximumHeight(25);
    l3->setMinimumHeight(25);
    l3->setAlignment(Qt::AlignCenter);
    l3->setFont(font);

    groupBox->setLayout(trainingDataLayout);

    QBarSet *set0 = new QBarSet("Main");
    QBarSet *set1 = new QBarSet("Total");

    for (int nc=0; nc<ncomb; nc++) {
        QLabel *e1 = new QLabel(combs[nc]); e1->setAlignment(Qt::AlignCenter);
        QLineEdit *e2 = new QLineEdit(QString::number(sobols[nq][nc],'f', 3)); e2->setReadOnly(true);  e2->setAlignment(Qt::AlignRight);
        QLineEdit *e3 = new QLineEdit(QString::number(sobols[nq][nc+ncomb],'f', 3)); e3->setReadOnly(true); e3->setAlignment(Qt::AlignRight);
        e1->setMaximumWidth(100);
        e2->setMaximumWidth(100);
        e3->setMaximumWidth(100);

        e1->setMinimumWidth(100);
        e2->setMinimumWidth(100);
        e3->setMinimumWidth(100);

        trainingDataLayout->addWidget(e1, nc+1,0);
        trainingDataLayout->addWidget(e2, nc+1,1);
        trainingDataLayout->addWidget(e3, nc+1,2);
        trainingDataLayout->setColumnStretch(1,0);
        trainingDataLayout->setColumnStretch(2,0);
        trainingDataLayout->setColumnStretch(3,1);

        *set0 << sobols[nq][nc];
        *set1 << sobols[nq][nc+ncomb];
        //*set0 << 2;
        //*set1 << 3;
     }

    QBarSeries *series = new QBarSeries();
    series->append(set0);
    series->append(set1);

    QChart *chartSobol = new QChart();
    chartSobol->addSeries(series);
    //chart->setTitle("");
    if (useAnimation){
     chartSobol->setAnimationOptions(QChart::SeriesAnimations);
    }
    QStringList categories;
    foreach (const QString str, combs) {
        categories << str;
    }

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);


    chartSobol->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0.0, 1.05);
    axisY->setTickType(QValueAxis::TickType::TicksDynamic);
    axisY->setTickInterval(0.5);
    axisY->setTickAnchor(0.0);
    chartSobol->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chartSobol->setMargins(QMargins(10,10,10,10));
    chartSobol->setBackgroundRoundness(0);
    chartSobol->setMinimumHeight(120);
    if (ncomb<5) {
        //chartSobol->setMaximumWidth(400);
    }


    QChartView *chartView = new QChartView(chartSobol);
    chartView->setRenderHint(QPainter::Antialiasing);

    chartSobol->legend()->setVisible(true);
    chartSobol->legend()->setAlignment(Qt::AlignRight);
    trainingDataLayout->addWidget(chartView, 0,3,ncomb+1,1);
    trainingDataLayout->setSpacing(5);
    //trainingDataLayout->setMargin(10);
    trainingDataLayout->setColumnStretch(0 | 1 | 2, 1);

}

// padhye
// this function is called if you decide to say save the data from UI into a json object
bool
SimCenterUQResultsSensitivity::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    if (theDataTable == NULL)
        return result; // no data

    jsonObject["resultType"]=QString(tr("SimCenterUQResultsSensitivity"));
    //jsonObject["isSurrogate"]=isSurrogate;
    //
    // add summary data
    //


    QJsonArray sobols_vec;
    for (int nq=0; nq<nQoI+nAggQoI; nq++){
        for (int nc=0; nc<ncomb; nc++){
            sobols_vec.append(sobols[nq][nc]);
        }
        for (int nc=0; nc<ncomb; nc++){
            sobols_vec.append(sobols[nq][nc+ncomb]);
        }
    }
    jsonObject["sobols"]=sobols_vec;

    QJsonArray QoIlist;
    for (int nq=0; nq<nQoI+nAggQoI; nq++){
        QoIlist.append(QoInames[nq]);
    }
    jsonObject["QoIlist"]=QoIlist;

    QJsonArray comblist;
    for (int nc=0; nc<ncomb; nc++){
        comblist.append(combs[nc]);
    }
    jsonObject["Comblist"]=comblist;

    jsonObject["numQoI"]=nQoI+nAggQoI;
    jsonObject["numCombs"]=ncomb;

    jsonObject["ElapsedTime"]=elaps;
    jsonObject["PCAncomp"]=PCAncomp;
    jsonObject["PCAvarRatio"]=PCAvarRatio;
    jsonObject["performedPCA"]=performedPCA;
    jsonObject["numAggQoI"]=nAggQoI;


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
SimCenterUQResultsSensitivity::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;

    this->clear();

    QJsonValue spreadsheetValue = jsonObject["spreadsheet"];
    if (spreadsheetValue.isNull())
        return true;

    //
    // create a summary widget in which place basic output (name, mean, stdDev)
    //

    QWidget *summary = new QWidget();
    QVBoxLayout *summaryLayout = new QVBoxLayout();
    summary->setLayout(summaryLayout);


    //
    // sobols
    //

    nQoI=jsonObject["numQoI"].toInt();
    ncomb=jsonObject["numCombs"].toInt();
    nAggQoI=jsonObject["numAggQoI"].toInt();

    QJsonArray sobols_vals=jsonObject["sobols"].toArray();
    int n=0;
    for (int nq=0; nq<nQoI+nAggQoI; nq++){
        QVector<double> sobols_vec;
        for (int nc=0; nc<ncomb; nc++){
            sobols_vec.push_back(sobols_vals[n++].toDouble());
        }
        for (int nc=0; nc<ncomb; nc++){
            sobols_vec.push_back(sobols_vals[n++].toDouble());
        }
        sobols.push_back(sobols_vec);
    }

    QJsonArray QoIlist=jsonObject["QoIlist"].toArray();
    for (int nq=0; nq<nQoI+nAggQoI; nq++) {
        QoInames.push_back(QoIlist[nq].toString());
    }

    QJsonArray comblist=jsonObject["Comblist"].toArray();
    for (int nc=0; nc<ncomb; nc++) {
        combs.push_back(comblist[nc].toString());
    }

    if (jsonObject.contains("ElapsedTime")) { //
        elaps=jsonObject["ElapsedTime"].toString();
    } else {
        elaps="";
    }

    if (jsonObject.contains("performedPCA")) { //
        bool pPCA = jsonObject["performedPCA"].toBool();
        if (pPCA == true)
            performedPCA="yes";
        else
            performedPCA = "no";
    } else {
        performedPCA="no";
    }

    if (jsonObject.contains("PCAncomp")) { //
        PCAncomp=jsonObject["PCAncomp"].toString();
    } else {
        PCAncomp="";
    }

    if (jsonObject.contains("PCAvarRatio")) { //
        PCAvarRatio=jsonObject["PCAvarRatio"].toString();
    } else {
        PCAvarRatio="";
    }

    QScrollArea *sa = new QScrollArea;
    gsaGraph(*&sa);

//    if (jsonObject.contains("isSurrogate")) { // no saving of analysis data
//        isSurrogate=jsonObject["isSurrogate"].toBool();
//    } else {
//        isSurrogate=false;
//    }

    theDataTable = new ResultsDataChart(spreadsheetValue.toObject());


    //
    // add summary, detained info and spreadsheet with chart to the tabed widget
    //

    tabWidget->addTab(sa,tr("Summary"));
    tabWidget->addTab(theDataTable, tr("Data Values"));
    tabWidget->adjustSize();

    return result;
}



void
SimCenterUQResultsSensitivity::onSaveButtonClicked(void) {

//        QString fileName = QFileDialog::getSaveFileName(this,
//                                                        tr("Save Data"), "/sensitivityResults",
//                                                        tr("output (*.out)"));
//        QFile file(fileName);
//        if (file.open(QIODevice::WriteOnly))
//        {
//            QTextStream stream(&file);

//             for (int nq=0; nq< nQoI+nAggQoI; nq++) {

//                 stream << "* " << QoInames[nq] << endl;
//                 stream << "randomVariable   Main   Total" << endl;
//                 for (int nc=0; nc< ncomb; nc++) {
//                     stream << combs[nc] << "   " << QString::number(sobols[nq][nc],'f', 3) << "   " ;
//                     stream << QString::number(sobols[nq][nc+ncomb],'f', 3) << endl;
//                 }
//            stream << endl;
//             }
//        file.close();
//        }

            QString fileName = QFileDialog::getSaveFileName(this,
                                                            tr("Save Data"), "/sensitivityResults",
                                                            tr("output (*.out)"));
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly))
            {
                QTextStream stream(&file);

                stream << "QoIName" << "\t" ;
                for (int nc = 0; nc<ncomb ; nc++) {
                    stream << "\t" << "Sm(" << combs[nc] << ")";
                }
                for (int nc = 0; nc<ncomb ; nc++) {
                    stream << "\t" << "St(" << combs[nc] << ")";
                }
                stream << "\n";

                 for (int nq=0; nq< nQoI+nAggQoI; nq++) {
                     stream << QoInames[nq]<< "\t";
                     for (int nc=0; nc< ncomb; nc++) {
                         stream << QString::number(sobols[nq][nc],'f', 3) << "\t" ;
                     }
                     for (int nc=0; nc<ncomb; nc++) {
                         stream << QString::number(sobols[nq][nc+ncomb],'f', 3) << "\t" ;
                     }
                     stream << "\n";
                 }
            file.close();
            }
            statusMessage("Sobol indices saved at "+ fileName);
}

extern QWidget *addLabeledLineEdit(QString theLabelName, QLineEdit **theLineEdit);
