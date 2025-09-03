#ifndef RANDOM_VARIABLES_CONTAINER_H
#define RANDOM_VARIABLES_CONTAINER_H

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
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

#include <SimCenterWidget.h>

#include "RandomVariable.h"
#include <QGroupBox>
#include <QVector>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QDebug>
#include <SectionTitle.h>
#include <QLineEdit>
#include <QCheckBox>

class QDialog;
enum  RV_Type {Normal, Constant, Uniform, ContinuousDesign};

class RandomVariablesContainer : public SimCenterWidget
{
    Q_OBJECT


private:
    static RandomVariablesContainer *theInstance;  
  
    explicit RandomVariablesContainer(QWidget *parent = 0);
    explicit RandomVariablesContainer(QString &randomVariableClass, QString uqengin="Dakota", QWidget *parent = 0);
    ~RandomVariablesContainer();
  
public:
  
    static RandomVariablesContainer *getInstance();

    void addRandomVariable(RandomVariable *theRV);
    bool inputFromJSON(QJsonObject &rvObject);
    bool outputToJSON(QJsonObject &rvObject);

    //void setInitialConstantRVs(QStringList &varNamesAndValues);

    void addRandomVariable(QString &rvName);
    void addRVs(QStringList &varNames);
    void addRVsWithValues(QStringList &varNames);  
    void addConstantRVs(QStringList &varNamesAndValues);
    void addUniformRVs(QStringList &varNamesAndValues);
    void addNormalRVs(QStringList &varNamesAndValues);
    void addContinuousDesignRVs(QStringList &varNamesAndValues);

    void setCorrelationDisabled(bool on);

    void removeRandomVariable(QString &varName);
    void removeRandomVariables(QStringList &varNames);

    QStringList getRandomVariableNames(void);
    int getNumRandomVariables(void);
    QVector<RandomVariable *> getRVdists(void);
    QTableWidget * getRVcorr();
    //void copyRVs(RandomVariablesContainer *oldRVcontainers);
    bool copyFiles(QString fileName);
    void setDefaults (QString &theUQ_Engine,
		      QString &theVariableClass,
		      RV_Type Normal);

    QString getRVStringDatasetDiscrete(void);
    QString getAllRVString(void);

public slots:
   void addRandomVariable(void);
   void variableNameChanged(const QString &newValue);
   void removeRandomVariable(void);
   void removeThisRandomVariable(RandomVariable *);
   void addCorrelationMatrix(void); // added by padhye for correlation matrix
   //   void addSobolevIndices(bool);// added by padhye for sobolev indices
   void refreshRandomVariables(void);
   void loadRVsFromJson(void);
   void saveRVsToJson(void);

   void clear(void);
   void makeCorrSymmetric(int i, int j);
   void checkCorrValidity(void);

private:
    RV_Type defaultRVsType;
    void makeRV(void);
    QVBoxLayout *verticalLayout;
    QVBoxLayout *rvLayout;
    QWidget *rv;

    QString randomVariableClass, uqEngineName;
    QVector<RandomVariable *>theRandomVariables;
    QDialog *correlationDialog;
    QTableWidget *correlationMatrix;
    QCheckBox *checkbox;
    QLabel *correlationError;
    QPushButton *addCorrelation;

    SectionTitle *correlationtabletitle;
    int flag_for_correlationMatrix;
    QStringList randomVariableNames;

    bool x_button_clicked_before;
};

#endif // RANDOM_VARIABLES_CONTAINER_H
