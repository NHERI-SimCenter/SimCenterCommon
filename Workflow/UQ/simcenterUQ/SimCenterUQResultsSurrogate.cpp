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
// added and modified: padhye,sangri

#include "SimCenterUQResultsSurrogate.h"
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

#include <QVBoxLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFrame>
#include <QHeaderView>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QVXYModelMapper>
#include <QtCharts/QLegendMarker>
#include <math.h>
#include <QValueAxis>

// for CI
#include <QXYSeries>
#include <RandomVariablesContainer.h>
#include <QPen>

#include <QAreaSeries>

SimCenterUQResultsSurrogate::SimCenterUQResultsSurrogate(RandomVariablesContainer *theRandomVariables, QWidget *parent)
    : UQ_Results(parent), theRVs(theRandomVariables), saveModelButton(NULL), saveResultButton(NULL), saveXButton(NULL), saveYButton(NULL)
{
    // title & add button
    theDataTable = NULL;
    tabWidget = new QTabWidget(this);
    layout->addWidget(tabWidget,1);
}

SimCenterUQResultsSurrogate::~SimCenterUQResultsSurrogate()
{

}


void SimCenterUQResultsSurrogate::clear(void)
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


int SimCenterUQResultsSurrogate::processResults(QString &dirName)
{
  QString filenameOUT = dirName + QDir::separator() + tr("dakota.out");;
  QString filenameTAB = dirName + QDir::separator() + tr("dakotaTab.out");;
  return this->processResults(filenameOUT, filenameTAB);
}

int SimCenterUQResultsSurrogate::processResults(QString &filenameResults, QString &filenameTab)
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
    workingDir=fileTabInfo.absolutePath()+ QDir::separator();

//    QString filenameErrorString = fileTabInfo.absolutePath() + QDir::separator() + QString("dakota.err");

//    QFileInfo filenameErrorInfo(filenameErrorString);
//    if (!filenameErrorInfo.exists()) {
//        errorMessage("No error file - SimCenterUQ did not run - problem with the application setup or the applications failed with inputs provided");
//        return -1;
//    }

//    QFile fileError(filenameErrorString);
//    if (fileError.open(QIODevice::ReadOnly)) {
//        QTextStream in(&fileError);
//        // QString contents = in.readAll(); -- not reading newline char
//        bool errorWritten = false;
//        QString errmsgs;
//        for (QString line = in.readLine();
//             !line.isNull();
//             line = in.readLine()) {
//             errmsgs +=  line + "<br>";
//             errorWritten = true;
//        };
//        if (errorWritten) {
//            errorMessage(errmsgs);
//            return -1;
//        }
//    }


    QFileInfo filenameTabInfo(filenameTab);
    if (!filenameTabInfo.exists()) {
        QString filenameLogString = fileTabInfo.absolutePath() + QDir::separator() + QString("logFileSimUQ.txt");
        QFile fileLog(filenameLogString);
        if (fileLog.open(QIODevice::ReadOnly)) {
            QTextStream in(&fileLog);
            bool errorWritten = false;
            QString errmsgs;
            for (QString line = in.readLine(); !line.isNull(); line = in.readLine()) {
                if (line.toLower().contains(QString("error"))) {
                    errmsgs +=  line + "<br>";
                    errorWritten = true;
                }
            };
            if (errorWritten) {
                errorMessage(errmsgs);
                errorMessage("No Tab file - Read logFileSimUQ.txt at the working directory for details");
                return -1;
            }        }
        errorMessage("No Tab file - surrogate modeling failed - possibly no QoI or a permission issue.");
        return -1;
    }

    //
    // For surrogate results
    //

//    QDir tempoFolder(filenameTabInfo.absolutePath()); //
//    //QDir templFolder(filenameTabInfo.absolutePath()+QDir::separator() +"templatedir");
//    QFileInfo surrogateTabInfo(tempoFolder.filePath("surrogateTab.out"));
//    //QFileInfo scInputInfo(templFolder.filePath("scInput.json"));
//    //if (surrogateTabInfo.exists() && scInputInfo.exists()) {
//    if (surrogateTabInfo.exists()) {
//        filenameTab = tempoFolder.filePath("surrogateTab.out");
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

    if (!jsonObj.contains("xdim")) {
      qDebug() << "no xdim";
      return -1;
    }
    int numRandomVar = theRVs->getNumRandomVariables();
    QJsonValue theValue = jsonObj["xdim"];
    if (theValue.isDouble())
      numRandomVar = theValue.toInt();
      
    
    //theDataTable = new ResultsDataChart(filenameTab);
    theDataTable = new ResultsDataChart(filenameTab, numRandomVar );

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

    return 0;
}


void
SimCenterUQResultsSurrogate::onSaveModelClicked()
{

    QString fileName = QFileDialog::getSaveFileName(this,
                                                   tr("Save Data"), lastPath+"/SimGpModel",
                                                   tr("Json File (*.json)"));
    QString fileName2 = fileName;

    fileName2.replace(".pkl",".json");

    QFileInfo fileInfo(fileName);
    QString path = fileInfo.absolutePath();
    QString pkldir = workingDir+QString("SimGpModel.pkl");
    QString jsondir = workingDir+QString("dakota.out");
    QString workflowDir1 = workingDir+QString("templatedir");
    QString workflowDir2 = path+QString("/templatedir_SIM");

    if (QFile::exists(fileName))
        QFile::remove(fileName);

    if (QFile::exists(fileName2))
        QFile::remove(fileName2);

    QDir dir(workflowDir2);
    dir.removeRecursively();

    QFile::copy(jsondir, fileName2);
    QFile::copy(pkldir, fileName);
    //QFile::copy(workingDir+QString("templatedir"), path+"templatedir_SIM");


    bool directoryCopied = copyPath(workflowDir1, workflowDir2, true);
    if (directoryCopied ==  false) {
        //check the remote directory
        QDir dir(workflowDir1);// remote/results/templatedir
        dir.cdUp();// Results
        dir.cdUp();// Remote
        dir.cd("templatedir");// templatedir
        directoryCopied = copyPath(dir.path(), workflowDir2, true);
        if (directoryCopied ==  false) {
              QString err = QString("SimCenterUQResultsSurrogate::onSaveModelClicked - copyPath failed from : ") + workflowDir1 + QString(" to ") + workflowDir2;
              errorMessage(err);
        }
    }

    lastPath =  QFileInfo(fileName).path();
}

void
SimCenterUQResultsSurrogate::onSaveInfoClicked()
{

    QString fileName = QFileDialog::getSaveFileName(this,
                                                   tr("Save Data"), lastPath+"/GPresults",
                                                   tr("Text File (*.out)"));
    QFile::copy(workingDir+QString("GPresults.out"), fileName);
    lastPath =  QFileInfo(fileName).path();
}

void
SimCenterUQResultsSurrogate::onSaveXClicked()
{

    QString fileName = QFileDialog::getSaveFileName(this,
                                                   tr("Save Data"), lastPath+"/X",
                                                   tr("Text File (*.txt)"));
    QFile::copy(workingDir+QString("inputTab.out"), fileName);
    lastPath =  QFileInfo(fileName).path();
}

void
SimCenterUQResultsSurrogate::onSaveYClicked()
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
SimCenterUQResultsSurrogate::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    jsonObject["resultType"]=QString(tr("SimCenterUQResultsSurrogate"));
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
SimCenterUQResultsSurrogate::inputFromJSON(QJsonObject &jsonObject)
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


//    int numRandomVar;
//    if (jsonObj.contains("xdim")) { // no saving of analysis data
//        QJsonValue theValue = jsonObj["xdim"];
//        if (theValue.isDouble())
//          numRandomVar = theValue.toInt();
//        else
//          numRandomVar = theRVs->getNumRandomVariables(); // better than nothing
//    } else {
//        numRandomVar = theRVs->getNumRandomVariables(); // better than nothing
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


bool SimCenterUQResultsSurrogate::copyPath(QString sourceDir, QString destinationDir, bool overWriteDirectory)
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

void SimCenterUQResultsSurrogate::summarySurrogate(QScrollArea *&sa)
{

    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    QWidget *summary = new QWidget();
    QGridLayout *summaryLayout = new QGridLayout();
//    summaryLayout->setContentsMargins(0,0,0,0); // adding back
    summary->setLayout(summaryLayout);
    sa->setWidget(summary);

    QJsonObject jsonObjHF = jsonObj["highFidelityInfo"].toObject();

    //QJsonObject uqObject = jsonObj["UQ_Method"].toObject();
    int nQoI = jsonObj["ydim"].toInt();

    if (nQoI==0) {
        return;
    }

    QJsonArray QoI_tmp = jsonObj["ylabels"].toArray();
    double nTime = jsonObj["valTime"].toDouble();
    QJsonObject valNRMSE = jsonObj["valNRMSE"].toObject();
    QString termCode =jsonObj["terminationCode"].toString();
    QJsonObject valNugget = jsonObj["valNugget"].toObject();
    QJsonObject valR2 = jsonObj["valR2"].toObject();
    QJsonObject valCorrCoeff = jsonObj["valCorrCoeff"].toObject();
    QJsonObject valIQratio = jsonObj["valIQratio"].toObject();
    QJsonObject valPval = jsonObj["valPval"].toObject();
    QJsonObject yExact = jsonObj["yExact"].toObject();
    QJsonObject yPredi = jsonObj["yPredict"].toObject();
    QJsonObject yConfidenceLb = jsonObj["yPredict_PI_lb"].toObject();
    QJsonObject yConfidenceUb = jsonObj["yPredict_PI_ub"].toObject();

    bool didLogtransform = jsonObj["doLogtransform"].toBool();
    bool isMultiFidelity = jsonObj["doMultiFidelity"].toBool();
    QJsonArray didStochastic = jsonObj["doStochastic"].toArray();



    int nSamp = jsonObjHF["valSamp"].toInt();
    int nSim  = jsonObjHF["valSim"].toInt();
    double NRMSEthr =jsonObjHF["thrNRMSE"].toDouble();

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
    } else if (termCode=="data"){
        //pass
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
        QJsonObject jsonObjLF = jsonObj["lowFidelityInfo"].toObject();
        int nSamp_LF = jsonObjLF["valSamp"].toInt();
        int nSim_LF = jsonObjLF["valSim"].toInt();
        resultsLayout->addWidget(new QLabel("# high-fidelity (HF) samples"), idSum, 0);
        resultsLayout->addWidget(new QLabel(QString::number(nSamp)), idSum++, 1);
        resultsLayout->addWidget(new QLabel("# high-fidelity (HF) model simulations"), idSum, 0);
        resultsLayout->addWidget(new QLabel(QString::number(nSim)), idSum++, 1);
        resultsLayout->addWidget(new QLabel("# low-fidelity (LF) samples"), idSum, 0);
        resultsLayout->addWidget(new QLabel(QString::number(nSamp_LF)), idSum++, 1);
        resultsLayout->addWidget(new QLabel("# low-fidelity (LF) model simulations"), idSum, 0);
        resultsLayout->addWidget(new QLabel(QString::number(nSim_LF)), idSum++, 1);
    } else {
        resultsLayout->addWidget(new QLabel("# training samples"), idSum, 0);
        resultsLayout->addWidget(new QLabel(QString::number(nSamp)), idSum++, 1);
        resultsLayout->addWidget(new QLabel("# model simulations"), idSum, 0);
        resultsLayout->addWidget(new QLabel(QString::number(nSim)), idSum++, 1);
    }
    resultsLayout->addWidget(new QLabel("Analysis time"), idSum, 0);
    if (nTime<60) {
        resultsLayout->addWidget(new QLabel(QString::number(nTime, 'f', 1).append(" sec.")), idSum++, 1);
    } else {
        resultsLayout->addWidget(new QLabel(QString::number(nTime/60, 'f', 1).append(" min.")), idSum++, 1);
    }
    resultsLayout->addWidget(new QLabel(" "), idSum++, 0);

    // blank space

    QLabel *accuMeasureLabel =new QLabel("Goodness-of-Fit");
    accuMeasureLabel-> setStyleSheet({"font-weight: bold"});
    resultsLayout->addWidget(accuMeasureLabel, idSum++, 0);
    QLineEdit *NRMSE;
    QLineEdit *R2;
    QLineEdit *Corr;
    QLineEdit *IQratio;
    QLineEdit *Pval;

    QVector<QVector<double>> statisticsVector = theDataTable->getStatistics();


    QVector<bool> nugget_idx(nQoI);
    int numnugget_vars = 0;
    for (int nq=0; nq<nQoI; nq++){
        double nugget = valNugget[QoInames[nq]].toDouble();
        if (nugget/abs(statisticsVector[jsonObj["xdim"].toInt()+1+nq][0])<1.e-5) {
            nugget_idx[nq] = false;
        } else {
            nugget_idx[nq] = true;
            numnugget_vars++;
        }
//        nugget_idx[nq] = false;
    }

    if (numnugget_vars < nQoI) { // there are some nugget = 0 variables
        resultsLayout->addWidget(new QLabel("Normalized error (NRMSE)"), idSum, 0);
        resultsLayout->addWidget(new QLabel("R2"), idSum+1, 0);
        resultsLayout->addWidget(new QLabel("Correlation coeff"), idSum+2, 0);
    }
    if (numnugget_vars > 0) { // there are some nugget > 0 variables
        resultsLayout->addWidget(new QLabel("Inter-quartile ratio"), idSum+3, 0);
        resultsLayout->addWidget(new QLabel("Normality (Cramér-von Mises) test"), idSum+4, 0);
    }

    bool warningIdx1=false;
    bool warningIdx2=false;
    for (int nq=0; nq<nQoI; nq++){

        bool is_nugget = nugget_idx[nq];

        NRMSE = new QLineEdit();
        R2 = new QLineEdit();
        Corr = new QLineEdit();
        IQratio = new QLineEdit();
        Pval = new QLineEdit();

        double NRMSEvalue= valNRMSE[QoInames[nq]].toDouble();

        NRMSE -> setText(QString::number(valNRMSE[QoInames[nq]].toDouble(), 'f', 3));
        R2 -> setText(QString::number(valR2[QoInames[nq]].toDouble(), 'f', 3));
        Corr -> setText(QString::number(valCorrCoeff[QoInames[nq]].toDouble(), 'f', 3));
        IQratio -> setText(QString::number(valIQratio[QoInames[nq]].toDouble(), 'f', 3));
        Pval -> setText(QString::number(valPval[QoInames[nq]].toDouble(), 'f', 3));

        if (valNRMSE[QoInames[nq]].isNull()) {
            NRMSE -> setText("NaN");
        }
        if (valR2[QoInames[nq]].isNull()) {
            R2 -> setText("NaN");
        }
        if (valCorrCoeff[QoInames[nq]].isNull()) {
            Corr -> setText("NaN");
        }
        if (valIQratio[QoInames[nq]].isNull()) {
            IQratio -> setText("NaN");
        }
        if (valPval[QoInames[nq]].isNull()) {
            Pval -> setText("NaN");
        }

        NRMSE->setAlignment(Qt::AlignRight);
        R2->setAlignment(Qt::AlignRight);
        Corr->setAlignment(Qt::AlignRight);
        IQratio->setAlignment(Qt::AlignRight);
        Pval->setAlignment(Qt::AlignRight);

        NRMSE->setReadOnly(true);
        R2->setReadOnly(true);
        Corr->setReadOnly(true);
        IQratio->setReadOnly(true);
        Pval->setReadOnly(true);

        NRMSE ->setMaximumWidth(100);
        R2 ->setMaximumWidth(100);
        Corr ->setMaximumWidth(100);
        IQratio ->setMaximumWidth(100);
        Pval ->setMaximumWidth(100);

        if (is_nugget) {
            if ((IQratio->text().toDouble()>=0.6)||(IQratio->text().toDouble()<=0.4)) {
                IQratio -> setStyleSheet({"color: red"});
                Pval -> setStyleSheet({"color: red"});
                warningIdx2=true;
            }
            NRMSE->setStyleSheet("background-color: lightgray;""color: lightgray;");
            R2->setStyleSheet("background-color: lightgray;" "color: lightgray;");
            Corr->setStyleSheet("background-color: lightgray;" "color: lightgray;");
        } else {
            if (NRMSEvalue>NRMSEthr)  {
                NRMSE -> setStyleSheet({"color: red"});
                R2 -> setStyleSheet({"color: red"});
                Corr -> setStyleSheet({"color: red"});
                warningIdx1=true;
            }
            IQratio->setStyleSheet("background-color: lightgray;""color: lightgray;");
            Pval->setStyleSheet("background-color: lightgray;" "color: lightgray;");
        }


        resultsLayout->addWidget(new QLabel(QoInames[nq]), idSum-1, nq+1);
        if (numnugget_vars < nQoI) { // there are some nugget = 0 variables
            resultsLayout->addWidget(NRMSE, idSum, nq+1);
            resultsLayout->addWidget(R2, idSum+1, nq+1);
            resultsLayout->addWidget(Corr, idSum+2, nq+1);
        }
        if (numnugget_vars > 0) { // there are some nugget > 0 variables
            resultsLayout->addWidget(IQratio, idSum+3, nq+1);
            resultsLayout->addWidget(Pval, idSum+4, nq+1);
        }
    }

    idSum += 5;

    if (warningIdx1) {
        //surrogateStatusLabel->setText("\nSurrogate analysis finished. - The model may not be accurate");
        QLabel *waringMsgLabel = new QLabel("* Note: Some or all of the QoIs did not converge to the target accuracy (NRMSE="+QString::number(NRMSEthr)+")");
        resultsLayout->addWidget(waringMsgLabel, idSum++, 0,1,-1);
        waringMsgLabel -> setStyleSheet({"color: red"});

    }
    if (warningIdx2) {
        //surrogateStatusLabel->setText("\nSurrogate analysis finished. - The model may not be accurate");
        QLabel *waringMsgLabel2 = new QLabel("* Note: Some or all of the QoIs have inter-quartile ratio far from the target value (IQratio=0.5)");
        resultsLayout->addWidget(waringMsgLabel2, idSum++, 0,1,-1);
        waringMsgLabel2 -> setStyleSheet({"color: red"});
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

    for (int nq=0; nq<nQoI; nq++)
    {
    //int nq=0;
        double nugget = valNugget[QoInames[nq]].toDouble();
        QScatterSeries *series_CV = new QScatterSeries;
        QScatterSeries *series_EX = new QScatterSeries;
        int alpha;
        // adjust marker size and opacity based on the number of samples
        if (nCVsamps < 10) {
            alpha = 200;
            series_EX->setMarkerSize(15.0);
            series_CV->setMarkerSize(15.0/2);
        } else if (nCVsamps < 100) {
            alpha = 160;
            series_EX->setMarkerSize(11.0);
            series_CV->setMarkerSize(11.0/2);
        } else if (nCVsamps < 1000) {            
            alpha = 100;
            series_EX->setMarkerSize(8.0);
            series_CV->setMarkerSize(8.0/2);
        } else if (nCVsamps < 10000) {
            alpha = 70;
            series_EX->setMarkerSize(6.0);
            series_CV->setMarkerSize(6.0/2);
        } else if (nCVsamps < 100000) {
            alpha = 50;
            series_EX->setMarkerSize(5.0);
            series_CV->setMarkerSize(5.0/2);
        } else {
            alpha = 30;
            series_EX->setMarkerSize(4.5);
            series_CV->setMarkerSize(4.5/2);
        }
        series_EX->setColor(QColor(0, 114, 178, alpha));
        series_EX->setBorderColor(QColor(255,255,255,0));

        series_CV->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
        series_CV->setColor(QColor(180, 180, 180, alpha));// grey
        series_CV->setBorderColor(QColor(180, 180, 180, alpha));

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
        double maxx=-INFINITY;
        double minx=INFINITY;
        for (int i=0; i<nCVsamps; i++) {
            series_CV->append(yPr[i].toDouble(), yPr[i].toDouble());
            series_EX->append(yPr[i].toDouble(), yEx[i].toDouble());
            maxy = std::max(maxy,std::max(yEx[i].toDouble(),yPr[i].toDouble()));
            miny = std::min(miny,std::min(yEx[i].toDouble(),yPr[i].toDouble()));
            maxx = std::max(maxx,yPr[i].toDouble());
            minx = std::min(minx,yPr[i].toDouble());
        }

        // set axis
        double inteval = maxy - miny;
        miny = miny - inteval*0.1;
        maxy = maxy + inteval*0.1;
        double delta = (miny+maxy)/2 - (minx+maxx)/2; // sy - to bring the plot center
        QValueAxis *axisX = new QValueAxis();
        QValueAxis *axisY = new QValueAxis();
        axisX->setRange(miny-delta, maxy-delta);
        axisY->setRange(miny, maxy);

        if (abs(nugget) < 1.e-10) {
            axisX->setTitleText(QString("Surrogate prediction (LOOCV)"));
            axisY->setTitleText(QString("Exact"));
        } else {
            axisX->setTitleText(QString("Predicted mean (LOOCV)"));
            axisY->setTitleText(QString("Training sample"));
        }

            // draw values

            chart_CV->addSeries(series_CV);
            chart_CV->addSeries(series_EX);
            series_CV->setName("Predicted mean");
            series_EX->setName("Samples");
            chart_CV->setAxisX(axisX, series_CV);
            chart_CV->setAxisY(axisY, series_CV);
            chart_CV->setAxisX(axisX, series_EX);
            chart_CV->setAxisY(axisY, series_EX);

            // legend of quantiles



            if (yLb.size()>0) {
                QPen pen;
                pen.setWidth(series_CV->markerSize()/10);
                for (int i=0; i<yLb.size(); i++) {
                    QLineSeries *series_err = new QLineSeries;
                    series_err->append(yPr[i].toDouble(), yLb[i].toDouble());
                    series_err->append(yPr[i].toDouble()*(1+1.e-10), yUb[i].toDouble());
                    series_err->setPen(pen);
                    chart_CV->addSeries(series_err);
                    //series_nugget->setColor(QColor(180,180,180,150));

                    series_err->setColor(QColor(0, 114, 178, alpha/2));
                    chart_CV->setAxisX(axisX, series_err);//cos share the X-axis of the sin curve
                    chart_CV->setAxisY(axisY, series_err);
                    chart_CV->legend()->markers(series_err)[0]->setVisible(false);
                }
                QLineSeries *dummy_series_err = new QLineSeries;
                dummy_series_err->setColor(QColor(0, 114, 178, 50));
                chart_CV->addSeries(dummy_series_err);
                dummy_series_err->setName("50% Prediction Interval");
            }

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

        QString nuggetStr;
        chartAndNugget->addWidget(chartView_CV,0,0);
        if (!didLogtransform) {
            nuggetStr = "nugget variance: ";
        } else {
            nuggetStr = "nugget variance (log-transformed space): ";
        }
        if (didStochastic[nq].toBool()){
            chartAndNugget->addWidget(new QLabel("Heteroscedastic nugget variance"));
        } else if (nugget/statisticsVector[jsonObj["xdim"].toInt()+1+nq][0]<1.e-12) {
            chartAndNugget->addWidget(new QLabel(nuggetStr+"0.000"));
        } else {
            chartAndNugget->addWidget(new QLabel(nuggetStr + QString::number(nugget,'g',4)),1,0);
        }

        tabWidgetScatter->addTab(container,QoInames[nq]);
    }
    tabWidgetScatter->setMinimumWidth(500);
    tabWidgetScatter->setMinimumHeight(500);
    tabWidgetScatter->setMaximumHeight(500);
    tabWidgetScatter->setMaximumWidth(500);
    QString CVmsg;
    if (isMultiFidelity) {
        CVmsg="\nLeave-One-Out Cross-Validation (LOOCV) Prediction (for the HF sample points)";
    } else {
        CVmsg="\nLeave-One-Out Cross-Validation (LOOCV) Prediction";
    }
    QLabel *CVresultsLabel =new QLabel(CVmsg);
    CVresultsLabel-> setStyleSheet({"font-weight: bold"});
    summaryLayout->addWidget(CVresultsLabel, 3, 0);
    summaryLayout->addWidget(tabWidgetScatter,4,0);

    QLabel *buttonsLabel =new QLabel("\nSaving Options");
    buttonsLabel-> setStyleSheet({"font-weight: bold"});

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    saveModelButton =  new QPushButton("Save GP Model");
    saveResultButton =  new QPushButton("Save GP Info");
    saveXButton =  new QPushButton("RV Data");
    saveYButton =  new QPushButton("QoI Data");

    if (isMultiFidelity) {
        saveXButton->setText("RV Data (LF)");
        saveYButton->setText("QoI Data (LF)");
    }

    connect(saveModelButton,SIGNAL(clicked()),this,SLOT(onSaveModelClicked()));
    connect(saveResultButton,SIGNAL(clicked()),this,SLOT(onSaveInfoClicked()));
    connect(saveXButton,SIGNAL(clicked()),this,SLOT(onSaveXClicked()));
    connect(saveYButton,SIGNAL(clicked()),this,SLOT(onSaveYClicked()));

    saveModelButton->setMinimumWidth(130);
    saveModelButton->setMaximumWidth(130);
    saveResultButton->setMaximumWidth(130);
    saveResultButton->setMinimumWidth(130);
    saveXButton->setMaximumWidth(110);
    saveXButton->setMinimumWidth(110);
    saveYButton->setMaximumWidth(110);
    saveYButton->setMinimumWidth(110);

    buttonsLayout->addWidget(saveModelButton,0,Qt::AlignLeft);
    buttonsLayout->addWidget(saveResultButton,1);
    buttonsLayout->addWidget(saveXButton,2);
    buttonsLayout->addWidget(saveYButton,3);
    buttonsLayout->addStretch(true);
    //buttonsLayout->setStretch(1,1);

    summaryLayout->addWidget(buttonsLabel, 5, 0);
    summaryLayout->addLayout(buttonsLayout, 6, 0,Qt::AlignTop);
    summaryLayout->setRowStretch(7, 1);
    summaryLayout->setColumnStretch(3, 1);

}


