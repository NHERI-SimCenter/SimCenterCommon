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
#include <sectiontitle.h>
#include <QLineEdit>
#include <QCheckBox>

class QDialog;

class RandomVariablesContainer : public SimCenterWidget
{
    Q_OBJECT
public:
    explicit RandomVariablesContainer(QWidget *parent = 0);
    explicit RandomVariablesContainer(QString &randomVariableClass, QWidget *parent = 0);

    ~RandomVariablesContainer();

    void addRandomVariable(RandomVariable *theRV);
    bool inputFromJSON(QJsonObject &rvObject);
    bool outputToJSON(QJsonObject &rvObject);

    //void setInitialConstantRVs(QStringList &varNamesAndValues);

    void addRandomVariable(QString &rvName);
    void addRVs(QStringList &varNames);
    void addConstantRVs(QStringList &varNamesAndValues);

    void removeRandomVariable(QString &varName);
    void removeRandomVariables(QStringList &varNames);

    QStringList getRandomVariableNames(void);
    int getNumRandomVariables(void);

public slots:
   void errorMessage(QString message);
   void addRandomVariable(void);
   void variableNameChanged(const QString &newValue);
   void removeRandomVariable(void);
   void addCorrelationMatrix(void); // added by padhye for correlation matrix
   //   void addSobolevIndices(bool);// added by padhye for sobolev indices
   void clear(void);

private:
    void makeRV(void);
    QVBoxLayout *verticalLayout;
    QVBoxLayout *rvLayout;
    QWidget *rv;

    QString randomVariableClass;
    QVector<RandomVariable *>theRandomVariables;
    QDialog *correlationDialog;
    QTableWidget *correlationMatrix;
    QCheckBox *checkbox;

    SectionTitle *correlationtabletitle;
    int flag_for_correlationMatrix;
    QStringList randomVariableNames;
    // int flag_for_sobolev_indices;
};

#endif // RANDOM_VARIABLES_CONTAINER_H
