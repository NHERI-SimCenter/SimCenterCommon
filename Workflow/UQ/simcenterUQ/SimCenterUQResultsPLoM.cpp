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

#include "SimCenterUQResultsPLoM.h"
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
#include <QJsonDocument>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

//#include <QMessageBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFrame>
//#include <InputWidgetFEM.h>
//#include <InputWidgetUQ.h>
//#include <MainWindow.h>
#include <QHeaderView>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QVXYModelMapper>
#include <QtCharts/QLegendMarker>
//using namespace QtCharts;
#include <math.h>
#include <QValueAxis>
#include <QLogValueAxis>

// for CI
#include <QXYSeries>
#include <RandomVariablesContainer.h>
#include <QPen>

//#include "qcustomplot.h"
//#include <QBoxSet>
#include <QAreaSeries>
#define NUM_DIVISIONS 10


//QLabel *best_fit_label_text;


SimCenterUQResultsPLoM::SimCenterUQResultsPLoM(RandomVariablesContainer *theRandomVariables, QWidget *parent)
    : UQ_Results(parent), theRVs(theRandomVariables), saveModelButton(NULL), saveResultButton(NULL), saveXButton(NULL), saveYButton(NULL)
{
    // title & add button
    theDataTable = NULL;
    tabWidget = new QTabWidget(this);
    layout->addWidget(tabWidget,1);
}

SimCenterUQResultsPLoM::~SimCenterUQResultsPLoM()
{

}


void SimCenterUQResultsPLoM::clear(void)
{
    // delete any existing widgets
    int count = tabWidget->count();
    if (count > 0) {
        for (int i=0; i<count; i++) {
            QWidget *theWidget = tabWidget->widget(count);
            delete theWidget;
        }
    }
    tabWidget->clear();
    theDataTable = NULL;

}


// not needed
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

// Not used

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

int SimCenterUQResultsPLoM::processResults(QString &dirName)
{
  QString filenameOUT = dirName + QDir::separator() + tr("dakota.out");;
  QString filenameTAB = dirName + QDir::separator() + tr("dakotaTab.out");;
  return this->processResults(filenameOUT, filenameTAB);
}

int SimCenterUQResultsPLoM::processResults(QString &filenameResults, QString &filenameTab)
{
    statusMessage(tr("Processing Results ... "));

    this->clear();
    lastPath = "";

    //
    // check if it actually ran without errors
    //

    QFileInfo fileTabInfo(filenameTab);


    QString errMsg("");
    this->extractErrorMsg( fileTabInfo.absolutePath(),"dakota.err", "SimCenterUQ", errMsg);

    if (errMsg.length() != 0) {
        errorMessage(errMsg);
        return 0;
    }

//    QString filenameErrorString = fileTabInfo.absolutePath() + QDir::separator() + QString("dakota.err");
//    workingDir=fileTabInfo.absolutePath()+ QDir::separator();
//    qDebug() << "filenameErrorString: " << filenameErrorString;

//    QFileInfo filenameErrorInfo(filenameErrorString);
//    if (!filenameErrorInfo.exists()) {
//        errorMessage("No error file - SimCenterUQ did not run - problem with the application setup or the applications failed with inputs provided");
//        return 0;
//    }
//    QFile fileError(filenameErrorString);
//    QString line("");
//    if (fileError.open(QIODevice::ReadOnly)) {
//        QTextStream in(&fileError);
//        while (!in.atEnd()) {
//            line = in.readLine();
//        }
//        fileError.close();
//    }

//    if (line.length() != 0) {
//        qDebug() << line.length() << " " << line;
//        errorMessage(QString(QString("Error Running SimCenterUQ: ") + line));
//        return 0;
//    }

    QFileInfo filenameTabInfo(filenameTab);
    if (!filenameTabInfo.exists()) {
        errorMessage("No Tab file - PLoM failed .. possibly no QoI or a permission issue. Check out Jobs Directory");
        return 0;
    }

//    QDir tempFolder(filenameTabInfo.absolutePath());
//    QFileInfo surrogateTabInfo(tempFolder.filePath("surrogateTab.out"));
//    if (surrogateTabInfo.exists()) {
//        filenameTab = tempFolder.filePath("surrogateTab.out");
//        isSurrogate = true;
//    } else {
//        isSurrogate = false;
//    }


    // create a scrollable windows, place summary inside it

    QScrollArea *sa = new QScrollArea;

    //
    // read data from file filename
    //

    //
    // open file
    //

    qDebug() << "filenameResults: " << filenameResults;
    QFile file(filenameResults);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QString message = QString("Error: could not open file") + filenameResults;
        errorMessage(message);
        return 0;
    }

    // place contents of file into json object
    QString val;
    val=file.readAll();
    file.close();
    val.replace(QString("NaN"),QString("null"));
    val.replace(QString("Infinity"),QString("inf"));

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    //QJsonParseError parseErr;
    //QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseErr );
    jsonObj = doc.object();

    // close file

    // check file contains valid object
    if (jsonObj.isEmpty()) {
        QString message = QString("ERROR: file either empty or malformed JSON");
        errorMessage(message);
        return 0;
    }

    qDebug() << "creating ResultDataChart.";
    qDebug() << filenameTab;
    //qDebug() << isSurrogate;
    qDebug() << theRVs->getNumRandomVariables();
    //theDataTable = new ResultsDataChart(filenameTab);
    theDataTable = new ResultsDataChart(filenameTab,  theRVs->getNumRandomVariables());

    qDebug() << "ResultDataChart created.";

    //
    // create spreadsheet,  a QTableWidget showing RV and results for each run
    //

    summarySurrogate(*&sa);
    //
    // add summary, detained info and spreadsheet with chart to the tabed widget
    //

    tabWidget->addTab(sa,tr("Summary"));
    tabWidget->addTab(theDataTable, tr("Data Values"));
    tabWidget->adjustSize();

    statusMessage(tr(""));

    return 0;
}


void
SimCenterUQResultsPLoM::onSaveModelClicked()
{


    QString fileName = QFileDialog::getSaveFileName(this,
                                                   tr("Save Data"), lastPath+"/SurrogatePLoM",
                                                   tr("H5 File (*.h5)"));
    QString fileName2 = fileName;

    fileName2.replace(".h5",".json");

    QFileInfo fileInfo(fileName);
    QString path = fileInfo.absolutePath();

    QFile::copy(workingDir+QString("SurrogatePLoM.h5"), fileName);
    QFile::copy(workingDir+QString("dakota.out"), fileName2);
    //QFile::copy(workingDir+QString("templatedir"), path+"templatedir_SIM");

    QString workflowDir1 = workingDir+QString("templatedir");
    QString workflowDir2 = path+QString("/templatedir_SIM");
    bool directoryCopied = copyPath(workflowDir1, workflowDir2, true);
    if (directoryCopied == false) {
        QString err = QString("SimCenterUQResultsPLoM::onSaveModelClicked() - files not copied");
        errorMessage(err);
    }

    lastPath =  QFileInfo(fileName).path();

    // saving training data
    QFile::copy(workingDir+QString("inputTab.out"), path+QDir::separator()+"inpFile.in");
    QFile::copy(workingDir+QString("outputTab.out"), path+QDir::separator()+"outFile.in");

    this->statusMessage("PLoM model and data saved in "+path);

}

void
SimCenterUQResultsPLoM::onSaveInfoClicked()
{

    QString fileName = QFileDialog::getSaveFileName(this,
                                                   tr("Save Data"), lastPath+"/ResultsPLoM",
                                                   tr("Text File (*.out)"));
    QFile::copy(workingDir+QString("X_new.csv"), fileName);
    lastPath =  QFileInfo(fileName).path();
}

void
SimCenterUQResultsPLoM::onSaveXClicked()
{

    QString fileName = QFileDialog::getSaveFileName(this,
                                                   tr("Save Data"), lastPath+"/X",
                                                   tr("Text File (*.txt)"));
    QFile::copy(workingDir+QString("inputTab.out"), fileName);
    lastPath =  QFileInfo(fileName).path();
}

void
SimCenterUQResultsPLoM::onSaveYClicked()
{

    QString fileName = QFileDialog::getSaveFileName(this,
                                                   tr("Save Data"), lastPath+"/Y",
                                                   tr("Output File (*.txt)"));
    QFile::copy(workingDir+QString("outputTab.out"), fileName);
    lastPath =  QFileInfo(fileName).path();
}




// padhye
// this function is called if you decide to say save the data from UI into a json object
bool
SimCenterUQResultsPLoM::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    jsonObject["resultType"]=QString(tr("SimCenterUQResultsPLoM"));
    //jsonObject["isSurrogate"]=isSurrogate;

    //
    // add summary data
    //

    jsonObject["summary"] = jsonObj;


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
SimCenterUQResultsPLoM::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;

    this->clear();

    QJsonValue spreadsheetValue = jsonObject["spreadsheet"];
    if (spreadsheetValue.isNull())
        return true;

    //
    // create a summary widget in which place basic output (name, mean, stdDev)
    //

    jsonObj = jsonObject["summary"].toObject();

    //
    // into a spreadsheet place all the data returned
    //

    //isSurrogate=jsonObject["isSurrogate"].toBool();

//    if (jsonObject.contains("isSurrogate")) { // no saving of analysis data
//        isSurrogate=jsonObject["isSurrogate"].toBool();
//    } else {
//        isSurrogate=false;
//    }

    theDataTable = new ResultsDataChart(spreadsheetValue.toObject());

    QScrollArea *sa = new QScrollArea;
    summarySurrogate(*&sa);
    if (saveModelButton!=NULL)  {
        saveModelButton ->setDisabled(true);
        saveResultButton ->setDisabled(true);
        saveXButton->setDisabled(true);
        saveYButton->setDisabled(true);
        saveModelButton ->setStyleSheet({ "background-color: lightgrey; border: none;" });
        saveResultButton ->setStyleSheet({ "background-color: lightgrey; border: none;" });
        saveXButton ->setStyleSheet({ "background-color: lightgrey; border: none;" });
        saveYButton ->setStyleSheet({ "background-color: lightgrey; border: none;" });
    }



    //
    // add summary, detained info and spreadsheet with chart to the tabed widget
    //

    tabWidget->addTab(sa,tr("Summary"));
    tabWidget->addTab(theDataTable, tr("Data Values"));
    tabWidget->adjustSize();
    //qDebug()<<"\n debugging the values: result is  \n"<<result<<"\n";

    return result;
}


extern QWidget *addLabeledLineEdit(QString theLabelName, QLineEdit **theLineEdit);


bool SimCenterUQResultsPLoM::copyPath(QString sourceDir, QString destinationDir, bool overWriteDirectory)
{
    QDir originDirectory(sourceDir);

    if (! originDirectory.exists())
    {
        return false;
    }

    QDir destinationDirectory(destinationDir);

    if(destinationDirectory.exists() && !overWriteDirectory)
    {
        return false;
    }
    else if(destinationDirectory.exists() && overWriteDirectory)
    {
        destinationDirectory.removeRecursively();
    }

    originDirectory.mkpath(destinationDir);

    foreach (QString directoryName, originDirectory.entryList(QDir::Dirs | \
                                                              QDir::NoDotAndDotDot))
    {
        QString destinationPath = destinationDir + "/" + directoryName;
        originDirectory.mkpath(destinationPath);
        copyPath(sourceDir + "/" + directoryName, destinationPath, overWriteDirectory);
    }

    foreach (QString fileName, originDirectory.entryList(QDir::Files))
    {
        QFile::copy(sourceDir + "/" + fileName, destinationDir + "/" + fileName);
    }

    QDir finalDestination(destinationDir);
    finalDestination.refresh();

    if(finalDestination.exists())
    {
        return true;
    }

    return false;
}

void SimCenterUQResultsPLoM::summarySurrogate(QScrollArea *&sa)
{

    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    QWidget *summary = new QWidget();
    QGridLayout *summaryLayout = new QGridLayout();
    summaryLayout->setContentsMargins(0,0,0,0); // adding back
    summary->setLayout(summaryLayout);
    sa->setWidget(summary);

    //QJsonObject uqObject = jsonObj["UQ_Method"].toObject();
    int nQoI = jsonObj["ydim"].toInt();

    if (nQoI==0) {
        return;
    }
    QJsonArray QoI_tmp = jsonObj["ylabels"].toArray();
    int nSamp = jsonObj["valSamp"].toInt();
    int nSim  = jsonObj["valSim"].toInt();
    double nTime = jsonObj["valTime"].toDouble();
    double NRMSEthr =jsonObj["thrNRMSE"].toDouble();
    QString termCode =jsonObj["terminationCode"].toString();
    QJsonObject valNugget = jsonObj["valNugget"].toObject();
    QJsonObject valNRMSE = jsonObj["valNRMSE"].toObject();
    QJsonObject valR2 = jsonObj["valR2"].toObject();
    QJsonObject valCorrCoeff = jsonObj["valCorrCoeff"].toObject();
    QJsonObject yExact = jsonObj["yExact"].toObject();
    QJsonObject yPredi = jsonObj["yPredict"].toObject();
    QJsonObject yConfidenceLb = jsonObj["yPredict_CI_lb"].toObject();
    QJsonObject yConfidenceUb = jsonObj["yPredict_CI_ub"].toObject();
    bool didLogtransform = jsonObj["logTransform"].toBool();
    bool isMultiFidelity = jsonObj["doMultiFidelity"].toBool();
    QJsonArray pcaEigen = jsonObj["pcaEigen"].toArray();
    QJsonArray pcaError = jsonObj["pcaError"].toArray();
    int pcaComp = jsonObj["pcaComp"].toInt();
    QJsonArray kdeEigen = jsonObj["kdeEigen"].toArray();
    int kdeComp = jsonObj["kdeComp"].toInt();
    int newSampleRatio = jsonObj["newSampleRatio"].toInt();
    QJsonArray Errors = jsonObj["Errors"].toArray();

    QStringList QoInames;
    foreach (QJsonValue str, QoI_tmp) {
        QoInames << str.toString();
    }

    QGridLayout *resultsLayout = new QGridLayout();

    QString termMsg="\nSurrogate Modeling Completed! ";
    if (termCode=="count"){
        termMsg = termMsg+"- Process ended as the maximum allowable number of simulations is reached";
    } else if (termCode=="time"){
        termMsg = termMsg+"- Process ended as the time limit is exceeded";
    } else if (termCode=="accuracy"){
        termMsg = termMsg+"- Model Converged";
    } else {
        termMsg = termMsg + "termination code unidentified";
    }

    QLabel *surrogateStatusLabel =new QLabel(termMsg);
    surrogateStatusLabel-> setStyleSheet({"font-weight: bold"});
    QFrame *lineA = new QFrame;
    lineA->setFrameShape(QFrame::HLine);
    lineA->setFrameShadow(QFrame::Sunken);

    summaryLayout->addWidget(surrogateStatusLabel, 0, 0,1,2);
    summaryLayout->addWidget(lineA, 1, 0,1,2);
    int idSum=0;
    if (isMultiFidelity) {
        int nSamp_HF = jsonObj["valSamp_HF"].toInt();
        resultsLayout->addWidget(new QLabel("# high-fidelity (HF) samples"), idSum, 0);
        resultsLayout->addWidget(new QLabel(QString::number(nSamp_HF)), idSum++, 1);
        resultsLayout->addWidget(new QLabel("# low-fidelity (LF) samples"), idSum, 0);
        resultsLayout->addWidget(new QLabel(QString::number(nSamp)), idSum++, 1);
        resultsLayout->addWidget(new QLabel("# low-fidelity (LF) model simulations"), idSum, 0);
        resultsLayout->addWidget(new QLabel(QString::number(nSim)), idSum++, 1);
    } else {
        resultsLayout->addWidget(new QLabel("# training samples"), idSum, 0);
        resultsLayout->addWidget(new QLabel(QString::number(nSamp)), idSum++, 1);
        resultsLayout->addWidget(new QLabel("# model simulations"), idSum, 0);
        resultsLayout->addWidget(new QLabel(QString::number(nSim)), idSum++, 1);
    }
    resultsLayout->addWidget(new QLabel("Analysis time"), idSum, 0);
    resultsLayout->addWidget(new QLabel(QString::number(nTime/60, 'f', 1).append(" min.")), idSum++, 1);
    resultsLayout->addWidget(new QLabel(" "), idSum++, 0);

    // blank space

    //QLabel *accuMeasureLabel =new QLabel("Goodness-of-Fit");
    //accuMeasureLabel-> setStyleSheet({"font-weight: bold"});
    //resultsLayout->addWidget(accuMeasureLabel, idSum++, 0);
    //resultsLayout->addWidget(new QLabel("Normalized error (NRMSE)"), idSum++, 0);
    //resultsLayout->addWidget(new QLabel("R2"), idSum++, 0);
    //resultsLayout->addWidget(new QLabel("Correlation coeff"), idSum++, 0);

    //QLineEdit *NRMSE;
    //QLineEdit *R2;
    //QLineEdit *Corr;

    bool warningIdx=false;

    /***
    for (int nq=0; nq<nQoI; nq++){
        NRMSE = new QLineEdit();
        R2 = new QLineEdit();
        Corr = new QLineEdit();

        double NRMSEvalue= valNRMSE[QoInames[nq]].toDouble();


        NRMSE -> setText(QString::number(valNRMSE[QoInames[nq]].toDouble(), 'f', 3));
        R2 -> setText(QString::number(valR2[QoInames[nq]].toDouble(), 'f', 3));
        Corr -> setText(QString::number(valCorrCoeff[QoInames[nq]].toDouble(), 'f', 3));

        auto aa = valNRMSE[QoInames[nq]].isNull();
        if (valNRMSE[QoInames[nq]].isNull()) {
            NRMSE -> setText("NaN");
        }
        if (valR2[QoInames[nq]].isNull()) {
            R2 -> setText("NaN");
        }
        if (valCorrCoeff[QoInames[nq]].isNull()) {
            Corr -> setText("NaN");
        }

        NRMSE->setAlignment(Qt::AlignRight);
        R2->setAlignment(Qt::AlignRight);
        Corr->setAlignment(Qt::AlignRight);

        NRMSE->setReadOnly(true);
        R2->setReadOnly(true);
        Corr->setReadOnly(true);

        NRMSE ->setMaximumWidth(100);
        R2 ->setMaximumWidth(100);
        Corr ->setMaximumWidth(100);

        if (NRMSEvalue>NRMSEthr) {
            NRMSE -> setStyleSheet({"color: red"});
            R2 -> setStyleSheet({"color: red"});
            Corr -> setStyleSheet({"color: red"});
            warningIdx=true;
        }

        resultsLayout->addWidget(new QLabel(QoInames[nq]), idSum-4, nq+1);
        resultsLayout->addWidget(NRMSE, idSum-3, nq+1);
        resultsLayout->addWidget(R2, idSum-2, nq+1);
        resultsLayout->addWidget(Corr, idSum-1, nq+1);
    }
    ***/

    idSum += 1;
    if (warningIdx) {
        //surrogateStatusLabel->setText("\nSurrogate analysis finished. - The model may not be accurate");
        QLabel *waringMsgLabel = new QLabel("* Note: Some or all of the QoIs did not converge to the target accuracy (NRMSE="+QString::number(NRMSEthr)+")");
        resultsLayout->addWidget(waringMsgLabel, idSum++, 0,1,-1);
        waringMsgLabel -> setStyleSheet({"color: red"});

    }

    resultsLayout->setRowStretch(idSum, 1);
    resultsLayout->setColumnStretch(nQoI+2, 1);
    summaryLayout->addLayout(resultsLayout,2,0);




    //
    // QScatter plot
    //

    QTabWidget *tabWidgetScatter = new QTabWidget();
    tabWidgetScatter -> setTabPosition(QTabWidget::East);

    int nCVsamps = yExact[QoInames[0]].toArray().size();

    if (newSampleRatio > 0) {
        for (int nq=0; nq<nQoI; nq++)
        {
        //int nq=0;
            QScatterSeries *series_CV = new QScatterSeries;
            int alpha;
            // adjust marker size and opacity based on the number of samples
            if (nCVsamps < 10) {
                alpha = 200;
                series_CV->setMarkerSize(15.0);
            } else if (nCVsamps < 100) {
                alpha = 160;
                series_CV->setMarkerSize(11.0);
            } else if (nCVsamps < 1000) {
                alpha = 100;
                series_CV->setMarkerSize(8.0);
            } else if (nCVsamps < 10000) {
                alpha = 70;
                series_CV->setMarkerSize(6.0);
            } else if (nCVsamps < 100000) {
                alpha = 50;
                series_CV->setMarkerSize(5.0);
            } else {
                alpha = 30;
                series_CV->setMarkerSize(4.5);
            }
            series_CV->setColor(QColor(0, 114, 178, alpha));
            series_CV->setBorderColor(QColor(255,255,255,0));

            QWidget *container = new QWidget();
            QGridLayout *chartAndNugget = new QGridLayout(container);
            QChart *chart_CV = new QChart;
            QChartView *chartView_CV = new QChartView(chart_CV);

            chart_CV->setAnimationOptions(QChart::AllAnimations);
            chartView_CV->setRenderHint(QPainter::Antialiasing);

            QJsonArray yEx= yExact[QoInames[nq]].toArray();
            QJsonArray yPr= yPredi[QoInames[nq]].toArray();
            QJsonArray yLb= yConfidenceLb[QoInames[nq]].toArray();
            QJsonArray yUb= yConfidenceUb[QoInames[nq]].toArray();
            double maxy=-INFINITY;
            double miny=INFINITY;
            for (int i=0; i<nCVsamps; i++) {
                if (didLogtransform) {
                    series_CV->append(exp(yEx[i].toDouble()), exp(yPr[i].toDouble()));
                    maxy = std::max(maxy,std::max(exp(yEx[i].toDouble()),exp(yPr[i].toDouble())));
                    miny = std::min(miny,std::min(exp(yEx[i].toDouble()),exp(yPr[i].toDouble())));
                } else {
                    series_CV->append(yEx[i].toDouble(), yPr[i].toDouble());
                    maxy = std::max(maxy,std::max(yEx[i].toDouble(),yPr[i].toDouble()));
                    miny = std::min(miny,std::min(yEx[i].toDouble(),yPr[i].toDouble()));
                }
            }

            // set axis
            double inteval = maxy - miny;
            miny = miny - inteval*0.1;
            maxy = maxy + inteval*0.1;
            QValueAxis *axisX = new QValueAxis();
            QValueAxis *axisY = new QValueAxis();
            axisX->setTitleText(QString("Exact response"));
            axisY->setTitleText(QString("Predicted response (LOOCV)"));
            axisX->setRange(miny, maxy);
            axisY->setRange(miny, maxy);

            // draw nugget scale
            QLineSeries *series_nugget_ub = new QLineSeries;
            QLineSeries *series_nugget_lb = new QLineSeries;
            int nd=100;
            bool nuggetLabel = true;
            double nuggetvar = valNugget[QoInames[nq]].toDouble();
            double nuggetstd = std::sqrt(valNugget[QoInames[nq]].toDouble());
            if (nuggetstd/inteval < 1.e-5) {
                nuggetLabel = false;
            }
            if (nuggetstd/inteval < 1.e-2) {
                nuggetstd = inteval*1.e-2;
            }
            for (int i=0; i<nd+1; i++) {
                series_nugget_ub->append(miny+i*(maxy-miny)/nd, miny+i*(maxy-miny)/nd + nuggetstd);
                series_nugget_lb->append(miny+i*(maxy-miny)/nd, miny+i*(maxy-miny)/nd - nuggetstd);
                if (didLogtransform) {
                    double log_mean = std::log(miny+i*(maxy-miny)/nd);
                    double log_var = nuggetvar;
                    nuggetstd = std::sqrt(std::exp(2.0*log_mean+log_var)*(std::exp(log_var)-1.0));
                    if (nuggetstd/inteval < 1.e-2) {
                        nuggetstd = inteval*1.e-2;
                    }
                    series_nugget_ub->append(miny+i*(maxy-miny)/nd, miny+i*(maxy-miny)/nd + nuggetstd);
                    series_nugget_lb->append(miny+i*(maxy-miny)/nd, miny+i*(maxy-miny)/nd - nuggetstd);
                }
            }

            QAreaSeries *series_nugget = new QAreaSeries(series_nugget_ub,series_nugget_lb);
            series_nugget->setName("± Nugget Std.");

            chart_CV->addSeries(series_nugget);
            series_nugget->setColor(QColor(180,180,180,alpha/2));
            chart_CV->setAxisX(axisX, series_nugget);// share the X-axis
            chart_CV->setAxisY(axisY, series_nugget);
            if (nuggetLabel == false) {
                //hide label if nugget is zero
                chart_CV->legend()->markers(series_nugget)[0]->setVisible(false);
            }
            series_nugget->setBorderColor(QColor(255,255,255,0));

            // draw bounds first

            QPen pen;
            pen.setWidth(series_CV->markerSize()/10);
            for (int i=0; i<nCVsamps; i++) {
                QLineSeries *series_err = new QLineSeries;
                series_err->append(yEx[i].toDouble(), yLb[i].toDouble());
                series_err->append(yEx[i].toDouble()*(1+1.e-10), yUb[i].toDouble());
                series_err->setPen(pen);
                chart_CV->addSeries(series_err);
                //series_nugget->setColor(QColor(180,180,180,150));

                series_err->setColor(QColor(0, 114, 178, alpha/2));
                //series_err->setColor(QColor(255,255,255,0.5));
                //series_err->setOpacity(series_CV->opacity());
                chart_CV->setAxisX(axisX, series_err);//cos share the X-axis of the sin curve
                chart_CV->setAxisY(axisY, series_err);
                chart_CV->legend()->markers(series_err)[0]->setVisible(false);
            }

            // draw values

            chart_CV->addSeries(series_CV);
            series_CV->setName("Sample Predictions");
            chart_CV->setAxisX(axisX, series_CV);
            chart_CV->setAxisY(axisY, series_CV);

            // legend of quantiles

            QLineSeries *dummy_series_err = new QLineSeries;
            dummy_series_err->setColor(QColor(0, 114, 178, 50));
            //dummy_series_err->setOpacity(series_CV->opacity());
            chart_CV->addSeries(dummy_series_err);
            dummy_series_err->setName("Inter-quartile Range");

            // set fontsize
            QFont chartFont;
            chartFont.setPixelSize(12);
            chart_CV->setFont(chartFont);
            axisX->setLabelsFont(chartFont);
            axisY->setLabelsFont(chartFont);
            axisX->setTitleFont(chartFont);
            axisY->setTitleFont(chartFont);
            chart_CV->legend()->setFont(chartFont);

            // to get mean value

            double nugget = valNugget[QoInames[nq]].toDouble();
            QVector<QVector<double>> statisticsVector = theDataTable->getStatistics();
            QString nuggetStr;
            chartAndNugget->addWidget(chartView_CV,0,0);
            /***
            if (!didLogtransform) {
                nuggetStr = "nugget variance: ";
            } else {
                nuggetStr = "nugget variance (log-transformed space): ";
            }
            if (nugget/statisticsVector[jsonObj["xdim"].toInt()+1+nq][0]<1.e-12) {
                chartAndNugget->addWidget(new QLabel(nuggetStr+"0.000"));
            } else {
                chartAndNugget->addWidget(new QLabel(nuggetStr + QString::number(nugget,'g',4)),1,0);
            }
            ***/


            tabWidgetScatter->addTab(container,QoInames[nq]);
        }
    }

    // adding extra figure for PCA error
    QScatterSeries *series_PCA = new QScatterSeries;
    QLineSeries *l_PCA = new QLineSeries;
    QLineSeries *l_cutoff = new QLineSeries;
    int alpha = 200;
    series_PCA->setMarkerSize(10.0);
    series_PCA->setColor(QColor(0, 114, 178, alpha));
    series_PCA->setBorderColor(QColor(255,255,255,0));
    l_PCA->setColor(QColor(0, 114, 178, alpha));
    l_cutoff->setColor(QColor(255, 0, 0, alpha));
    QWidget *container = new QWidget();
    QGridLayout *chartAndNugget = new QGridLayout(container);
    QChart *chart_PCA = new QChart;
    QChartView *chartView_CV = new QChartView(chart_PCA);
    chart_PCA->setAnimationOptions(QChart::AllAnimations);
    chartView_CV->setRenderHint(QPainter::Antialiasing);
    QJsonArray yEx= pcaEigen;
    QJsonArray yPr= pcaError;
    double maxy=-INFINITY;
    double miny=INFINITY;
    double maxx=-INFINITY;
    double minx=INFINITY;
    for (int i=0; i<yEx.size(); i++) {
        if (didLogtransform) {
            series_PCA->append(std::exp(yEx[i].toDouble()), std::exp(yPr[i].toDouble()));
            maxx = std::max(maxx,std::exp(yEx[i].toDouble()));
            minx = std::min(minx,std::exp(yEx[i].toDouble()));
            maxy = std::max(maxy,std::exp(yPr[i].toDouble()));
            miny = std::min(miny,std::exp(yPr[i].toDouble()));
            l_PCA->append(std::exp(yEx[i].toDouble()), std::exp(yPr[i].toDouble()));
        } else {
            series_PCA->append(yEx[i].toDouble(), yPr[i].toDouble());
            maxx = std::max(maxx,yEx[i].toDouble());
            minx = std::min(minx,yEx[i].toDouble());
            maxy = std::max(maxy,yPr[i].toDouble());
            miny = std::min(miny,yPr[i].toDouble());
            l_PCA->append(yEx[i].toDouble(), yPr[i].toDouble());
        }
        if (i == (pcaComp-1)) {
            l_cutoff->append(yEx[i].toDouble(), 0);
            l_cutoff->append(yEx[i].toDouble(), yPr[i].toDouble());
        }
    }
    // set axis
    double inteval = maxy - miny;
    miny = miny - inteval*0.1;
    maxy = maxy + inteval*0.1;
    l_cutoff->append(yEx[pcaComp-1].toDouble(), maxy);
    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();
    axisX->setLabelFormat("%.4f");
    axisY->setLabelFormat("%.4f");
    axisX->setTitleText(QString("PCA Eigenvalues"));
    axisY->setTitleText(QString("PCA Error"));
    axisX->setRange(minx, maxx);
    axisY->setRange(0, maxy);
    axisX->setReverse();
    chart_PCA->addSeries(series_PCA);
    chart_PCA->addSeries(l_PCA);
    series_PCA->setName("PCA Error");
    chart_PCA->addSeries(l_cutoff);
    l_cutoff->setName("Mininum eigen taken in PCA");
    chart_PCA->setAxisX(axisX, series_PCA);
    chart_PCA->setAxisY(axisY, series_PCA);
    chart_PCA->setAxisX(axisX, l_PCA);
    chart_PCA->setAxisY(axisY, l_PCA);
    chart_PCA->setAxisX(axisX, l_cutoff);
    chart_PCA->setAxisY(axisY, l_cutoff);
    // set fontsize
    QFont chartFont;
    chartFont.setPixelSize(12);
    chart_PCA->setFont(chartFont);
    axisX->setLabelsFont(chartFont);
    axisY->setLabelsFont(chartFont);
    axisX->setTitleFont(chartFont);
    axisY->setTitleFont(chartFont);
    chart_PCA->legend()->setFont(chartFont);
    chartAndNugget->addWidget(chartView_CV,0,0);
    tabWidgetScatter->addTab(container,QString("PCA"));
    auto markerlist = chart_PCA->legend()->markers();
    markerlist[1]->setVisible(false);

    // adding extra figure for KDE eigen values
    if (kdeEigen.size() > 0) {
        QScatterSeries *series_KDE = new QScatterSeries;
        QLineSeries *l_KDE = new QLineSeries;
        QLineSeries *l_cutoff_kde = new QLineSeries;
        //int alpha_kde = 200;
        series_KDE->setMarkerSize(10.0);
        series_KDE->setColor(QColor(0, 114, 178, alpha));
        series_KDE->setBorderColor(QColor(255,255,255,0));
        l_KDE->setColor(QColor(0, 114, 178, alpha));
        l_cutoff_kde->setColor(QColor(255, 0, 0, alpha));
        QWidget *container_kde = new QWidget();
        QGridLayout *chartAndNugget_kde = new QGridLayout(container_kde);
        QChart *chart_KDE = new QChart;
        QChartView *chartView_KDE = new QChartView(chart_KDE);
        chart_KDE->setAnimationOptions(QChart::AllAnimations);
        chartView_CV->setRenderHint(QPainter::Antialiasing);
        QJsonArray yKDE= kdeEigen;
        maxy=-INFINITY;
        miny=INFINITY;
        maxx=-INFINITY;
        minx=INFINITY;
        for (int i=0; i<yKDE.size(); i++) {
            if (didLogtransform) {
                series_KDE->append(i+1, yKDE[i].toDouble());
                maxx = std::max(maxx,double(i+1));
                minx = std::min(minx,double(i+1));
                maxy = std::max(maxy,std::exp(yKDE[i].toDouble()));
                miny = std::min(miny,std::exp(yKDE[i].toDouble()));
                l_KDE->append(i+1, std::exp(yKDE[i].toDouble()));
            } else {
                series_PCA->append(i+1, yKDE[i].toDouble());
                maxx = std::max(maxx,double(i+1));
                minx = std::min(minx,double(i+1));
                maxy = std::max(maxy,yKDE[i].toDouble());
                miny = std::min(miny,yKDE[i].toDouble());
                l_KDE->append(i+1, yKDE[i].toDouble());
            }
            if (i == (kdeComp-2)) {
                if (didLogtransform) {
                    l_cutoff_kde->append(0, std::exp(yKDE[i].toDouble()));
                    l_cutoff_kde->append(i+1, std::exp(yKDE[i].toDouble()));
                } else {
                    l_cutoff_kde->append(0, yKDE[i].toDouble());
                    l_cutoff_kde->append(i+1, yKDE[i].toDouble());
                }
            }
            if (didLogtransform) {
                if (yKDE[i].toDouble() < 0.01*yKDE[kdeComp-2].toDouble()) {
                    break;
                }
            } else {
                if (std::exp(yKDE[i].toDouble()) < 0.01*std::exp(yKDE[kdeComp-2].toDouble())) {
                    break;
                }
            }
        }
        // set axis
        inteval = maxy - miny;
        maxy = maxy + inteval*0.1;
        if (didLogtransform)
            l_cutoff_kde->append(maxx, std::exp(yKDE[kdeComp-2].toDouble()));
        else
            l_cutoff_kde->append(maxx, yKDE[kdeComp-2].toDouble());
        QValueAxis *axisX_kde = new QValueAxis();
        QLogValueAxis *axisY_kde = new QLogValueAxis();
        axisX_kde->setLabelFormat("%.0f");
        axisY_kde->setLabelFormat("%.6f");
        axisX_kde->setTitleText(QString("Number of Component"));
        axisY_kde->setTitleText(QString("Diff. Maps Eigenvalue"));
        axisX_kde->setRange(minx, maxx);
        axisY_kde->setRange(miny, maxy);
        chart_KDE->addSeries(series_KDE);
        chart_KDE->addSeries(l_KDE);
        series_KDE->setName("Diff. Maps Eigenvalue");
        chart_KDE->addSeries(l_cutoff_kde);
        l_cutoff_kde->setName("Mininum eigen considered");
        chart_KDE->setAxisX(axisX_kde, series_KDE);
        chart_KDE->setAxisY(axisY_kde, series_KDE);
        chart_KDE->setAxisX(axisX_kde, l_KDE);
        chart_KDE->setAxisY(axisY_kde, l_KDE);
        chart_KDE->setAxisX(axisX_kde, l_cutoff_kde);
        chart_KDE->setAxisY(axisY_kde, l_cutoff_kde);
        // set fontsize
        QFont chartFont_kde;
        chartFont_kde.setPixelSize(12);
        chart_PCA->setFont(chartFont_kde);
        axisX->setLabelsFont(chartFont_kde);
        axisY->setLabelsFont(chartFont_kde);
        axisX->setTitleFont(chartFont_kde);
        axisY->setTitleFont(chartFont_kde);
        chart_KDE->legend()->setFont(chartFont_kde);
        chartAndNugget_kde->addWidget(chartView_KDE,0,0);
        tabWidgetScatter->addTab(container_kde,QString("KDE"));
        auto markerlist_kde = chart_KDE->legend()->markers();
        markerlist_kde[1]->setVisible(false);
    }

    tabWidgetScatter->setMinimumWidth(500);
    tabWidgetScatter->setMinimumHeight(500);
    tabWidgetScatter->setMaximumHeight(500);
    tabWidgetScatter->setMaximumWidth(500);
    QString CVmsg;
    CVmsg = "\nPLoM Training Results";
    QLabel *CVresultsLabel =new QLabel(CVmsg);
    CVresultsLabel-> setStyleSheet({"font-weight: bold"});
    summaryLayout->addWidget(CVresultsLabel, 3, 0);
    summaryLayout->addWidget(tabWidgetScatter,4,0);

    QLabel *buttonsLabel =new QLabel("\nSaving Options");
    buttonsLabel-> setStyleSheet({"font-weight: bold"});

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    saveModelButton =  new QPushButton("Save PLoM Model");
    connect(saveModelButton,SIGNAL(clicked()),this,SLOT(onSaveModelClicked()));
    saveModelButton->setMinimumWidth(130);
    saveModelButton->setMaximumWidth(130);
    buttonsLayout->addWidget(saveModelButton,0,Qt::AlignLeft);

    if (newSampleRatio > 0) {
        saveResultButton =  new QPushButton("Save New Sample");
        connect(saveResultButton,SIGNAL(clicked()),this,SLOT(onSaveInfoClicked()));
        saveResultButton->setMaximumWidth(130);
        saveResultButton->setMinimumWidth(130);
        buttonsLayout->addWidget(saveResultButton,1);
    }

    buttonsLayout->addStretch(true);
    //buttonsLayout->setStretch(1,1);

    summaryLayout->addWidget(buttonsLabel, 5, 0);
    summaryLayout->addLayout(buttonsLayout, 6, 0,Qt::AlignTop);
    summaryLayout->setRowStretch(7, 1);
    summaryLayout->setColumnStretch(3, 1);

}


