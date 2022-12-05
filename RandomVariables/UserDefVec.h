#ifndef UserDefVec_H
#define UserDefVec_H

// added by padhye
// https://stackoverflow.com/questions/30168845/qt-error-lnk1120-1-unresolved-externals-main-obj-1-error-lnk2019-run-qmake

#include "RandomVariableDistribution.h"
class QLineEdit;
class QLabel;

class UserDefVec : public RandomVariableDistribution
{
    Q_OBJECT
public:
    explicit  UserDefVec(QWidget *parent = 0);
    ~UserDefVec();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);
    bool copyFiles(QString fileDir);

    QString getAbbreviatedName(void);

signals:

public slots:

private:
   QLineEdit *scriptDir;
   QLineEdit *length;
   // QLineEdit *betaparam;
};



#endif
