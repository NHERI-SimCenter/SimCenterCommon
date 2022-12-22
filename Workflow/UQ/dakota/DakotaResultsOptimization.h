#ifndef DAKOTARESULTSOPTIMIZATION_H
#define DAKOTARESULTSOPTIMIZATION_H


#include <UQ_Results.h>
#include <QtCharts/QChart>

//using namespace QtCharts;

class QTextEdit;
class QTabWidget;
class MyTableWidget;
class ResultsDataChart;
class RandomVariablesContainer;

//class QChart;

class DakotaResultsOptimization : public UQ_Results
{

public:
    explicit DakotaResultsOptimization(RandomVariablesContainer * theRVs, QWidget *parent = 0);
    ~DakotaResultsOptimization();
//    Q_OBJECT

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);

    int processResults(QString &dirName);
    QWidget *createResultParameterWidget(QString &name, double value);

//signals:

public slots:
   void clear(void);

//private:
   int processResults(QString &filenameResults, QString &filenameTab);
   RandomVariablesContainer *theRVs;

   QTabWidget *tabWidget;
   QTextEdit  *dakotaText;
//   MyTableWidget *spreadsheet;
//   QChart *chart;
   ResultsDataChart* theDataTable;

//   int col1, col2;
//   bool mLeft;
//   QStringList theHeadings;

   QVector<QString>theNames;
   QVector<double>theBestValues;

   QWidget *summary;
   QVBoxLayout *summaryLayout;

   bool isSurrogate;
};

#endif // DAKOTARESULTSOPTIMIZATION_H
