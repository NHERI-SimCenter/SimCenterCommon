#ifndef UQPYRESULTSSUBSETSIM_H
#define UQPYRESULTSSUBSETSIM_H

#include <QJsonObject>
#include <QString>

class UQpyResultsSubsetSim
{
public:
    UQpyResultsSubsetSim();

    ~UQpyResultsSubsetSim();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);

    int processResults(QString &dirName);
};

#endif // UQPYRESULTSSUBSETSIM_H
