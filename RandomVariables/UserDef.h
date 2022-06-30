#ifndef USERDEF_H
#define USERDEF_H

// added by padhye
// https://stackoverflow.com/questions/30168845/qt-error-lnk1120-1-unresolved-externals-main-obj-1-error-lnk2019-run-qmake

#include "RandomVariableDistribution.h"
class QLineEdit;
class QLabel;

class UserDef : public RandomVariableDistribution
{
    Q_OBJECT
public:
    explicit  UserDef(QWidget *parent = 0);
    ~UserDef();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);
    bool copyFiles(QString fileDir);

    QString getAbbreviatedName(void);

signals:

public slots:

private:
   QLineEdit *scriptDir;
   // QLineEdit *betaparam;
};



#endif
