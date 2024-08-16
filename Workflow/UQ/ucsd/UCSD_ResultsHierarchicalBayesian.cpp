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

// Written: bsaakash

#include "UCSD_ResultsHierarchicalBayesian.h"


UCSD_ResultsHierarchicalBayesian::UCSD_ResultsHierarchicalBayesian(RandomVariablesContainer *theRandomVariables, QWidget *parent)
    : UQ_Results(parent), theRVs(theRandomVariables)
{
    theDataTable = NULL;
    tabWidget = new QTabWidget(this);
    layout->addWidget(tabWidget,1);
}

UCSD_ResultsHierarchicalBayesian::~UCSD_ResultsHierarchicalBayesian()
{

}

void UCSD_ResultsHierarchicalBayesian::clear(void)
{
    // delete any existing widgets
    int count = tabWidget->count();
    if (count > 0) {
        for (int i=0; i<count; i++) {
            QWidget *theWidget = tabWidget->widget(count);
            delete theWidget;
        }
    }
    theHeadings.clear();
    theMeans.clear();
    theStdDevs.clear();

    tabWidget->clear();
    theDataTable = NULL;
}


bool UCSD_ResultsHierarchicalBayesian::outputToJSON(QJsonObject &jsonObject)
{
    int numEDP = theNames.count();

    // quick return .. noEDP -> no analysis done -> no results out
    if (numEDP == 0)
        return true;

    jsonObject["resultType"]=QString(tr("UCSD_ResultsHierarchicalBayesian"));

    //
    // add summary data
    //
    QJsonArray resultsData;
    for (int i=0; i<numEDP; i++) {
        QJsonObject edpData;
        edpData["name"]=theNames.at(i);
        edpData["mean"]=theMeans.at(i);
        edpData["stdDev"]=theStdDevs.at(i);
        resultsData.append(edpData);
    }
    jsonObject["summary"]=resultsData;

    //
    // add spreadsheet data
    //
    if(theDataTable != NULL) {
        theDataTable->outputToJSON(jsonObject);
    }

    return true;
}

bool UCSD_ResultsHierarchicalBayesian::inputFromJSON(QJsonObject &jsonObject)
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

    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    QWidget *summary = new QWidget();
    QVBoxLayout *summaryLayout = new QVBoxLayout();
    summaryLayout->setContentsMargins(0,0,0,0); // adding back
    summary->setLayout(summaryLayout);

    sa->setWidget(summary);

    theDataTable = new ResultsDataChart(spreadsheetValue.toObject());

    //
    // determine summary statistics for each edp
    //

    QVector<QVector<double>> statisticsVector = theDataTable->getStatistics();
    QVector<QString> NamesVector = theDataTable->getNames();
    for (int col = 1; col<NamesVector.size(); ++col) { // +
        QWidget *theWidget = this->createResultEDPWidget(NamesVector[col], statisticsVector[col]);
        summaryLayout->addWidget(theWidget);
    }
    summaryLayout->addStretch();

    //
    // add summary, detained info and spreadsheet with chart to the tab widget
    //

    tabWidget->addTab(sa,tr("Summary"));
    tabWidget->addTab(theDataTable, tr("Data Values"));
    tabWidget->adjustSize();

    return result;
}

int UCSD_ResultsHierarchicalBayesian::processResults(QString &dirName)
{
    QString posteriorPredictiveSamplesTabFileName = dirName + "/" + tr("posterior_samples_table.out");
    QFileInfo posteriorPredictiveSamplesTabFileInfo(posteriorPredictiveSamplesTabFileName);
    QDir dir(dirName);
    QStringList filter;
    filter << "posterior_samples_table_dataset_*.out";
    dir.setNameFilters(filter);
    QFileInfoList posteriorSamplesTabFileInfoPerDataset = dir.entryInfoList();
    return this->postprocessResults(posteriorPredictiveSamplesTabFileInfo, posteriorSamplesTabFileInfoPerDataset);
}

int UCSD_ResultsHierarchicalBayesian::postprocessResults(QFileInfo &posteriorPredictiveSamplesTabFileInfo,
                                                         QFileInfoList &posteriorSamplesTabFileInfoPerDataset) {
    statusMessage(tr("Processing Hierarchical Bayesian Results"));

    this->clear();

    //
    // check it actually ran with no errors
    //
    QDir fileDirTab = posteriorPredictiveSamplesTabFileInfo.absoluteDir();
    QString errMsg("");
    this->extractErrorMsg( fileDirTab.absolutePath(),"UCSD_UQ.err", "UCSD_UQ", errMsg);
    if (errMsg.length() != 0) {
        errorMessage(errMsg);
        return 0;
    }

    //
    // check if posterior samples table file exists
    //
    if (!posteriorPredictiveSamplesTabFileInfo.exists()) {
        errorMessage("ERROR: No 'posterior_samples_table.out' file found");
        return 0;
    }

    //
    // create summary, a QWidget for summary data: the name, mean, stdDev, C.O.V.(%) of the posterior predictive RV sample values
    //
    // create a scrollable window, place summary inside it
    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    QWidget *summary = new QWidget();
    QVBoxLayout *summaryLayout = new QVBoxLayout();
    summaryLayout->setContentsMargins(0,0,0,0); // adding back
    summary->setLayout(summaryLayout);

    sa->setWidget(summary);

    theDataTable = new ResultsDataChart(posteriorPredictiveSamplesTabFileInfo.canonicalFilePath(), 0, false);

    //
    // create spreadsheet,  a QTableWidget showing RV and results for each run
    //

    QVector<QVector<double>> statisticsVector = theDataTable->getStatistics();
    QVector<QString> NamesVector = theDataTable->getNames();
    for (int col = 1; col<NamesVector.size(); ++col) { // +
        QWidget *theWidget = this->createResultEDPWidget(NamesVector[col], statisticsVector[col]);
        summaryLayout->addWidget(theWidget);
    }
    summaryLayout->addStretch();

//    foreach (QFileInfo fileInfo, posteriorSamplesTabFileInfoPerDataset) {
//        QString fileName = fileInfo.canonicalFilePath();
//        statusMessage("Looping over: " + fileName);
//    }

    tabWidget->addTab(sa,tr("Summary"));
    tabWidget->addTab(theDataTable, tr("Data Values"));
    tabWidget->adjustSize();

    return 0;
}

extern QWidget *addLabeledLineEdit(QString theLabelName, QLineEdit **theLineEdit);

QWidget *
UCSD_ResultsHierarchicalBayesian::createResultEDPWidget(QString &name, QVector<double> statistics) {

    double mean = statistics[0];
    double stdDev = statistics[1];

    QWidget *edp = new QWidget;
    QHBoxLayout *edpLayout = new QHBoxLayout();

    edp->setLayout(edpLayout);

    QLineEdit *nameLineEdit;
    QWidget *nameWidget = addLabeledLineEdit(QString("Name"), &nameLineEdit);
    nameLineEdit->setText(name);
    nameLineEdit->setReadOnly(true);
    theNames.append(name);
    edpLayout->addWidget(nameWidget);

    QLineEdit *meanLineEdit;
    QWidget *meanWidget = addLabeledLineEdit(QString("Mean"), &meanLineEdit);
    meanLineEdit->setText(QString::number(mean));
    meanLineEdit->setReadOnly(true);
    theMeans.append(mean);
    edpLayout->addWidget(meanWidget);

    QLineEdit *stdDevLineEdit;
    QWidget *stdDevWidget = addLabeledLineEdit(QString("StdDev"), &stdDevLineEdit);
    stdDevLineEdit->setText(QString::number(stdDev));
    stdDevLineEdit->setReadOnly(true);
    theStdDevs.append(stdDev);
    edpLayout->addWidget(stdDevWidget);

    QLineEdit *coefficientOfVariationLineEdit;
    QWidget *coefficientOfVariationWidget = addLabeledLineEdit(QString("C.O.V.(%)"), &coefficientOfVariationLineEdit);
    double percentCoefficientOfVariation = qAbs(stdDev/mean*100);
    coefficientOfVariationLineEdit->setText(QString::number(percentCoefficientOfVariation));
    coefficientOfVariationLineEdit->setReadOnly(true);
    thePercentCoVs.append(percentCoefficientOfVariation);
    edpLayout->addWidget(coefficientOfVariationWidget);

    edpLayout->addStretch();

    return edp;
}
