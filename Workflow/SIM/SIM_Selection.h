#ifndef 	SIM_SELECTION_H
#define 	SIM_SELECTION_H


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

#include <SimCenterAppWidget.h>
//#include "EDP.h"
#include <QGroupBox>
#include <QVector>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>

class RandomVariablesContainer;

class SIM_Selection : public SimCenterAppWidget
{
    Q_OBJECT
public:
  explicit SIM_Selection(RandomVariablesContainer *, bool includeCentroid = false, QWidget *parent = 0);
    ~SIM_Selection();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);
    bool outputAppDataToJSON(QJsonObject &rvObject);
    bool inputAppDataFromJSON(QJsonObject &rvObject);
    bool copyFiles(QString &destDir);

signals:

public slots:
   void clear(void);
   void bimSelectionChanged(const QString &arg1);
   void errorMessage(QString message);

signals:
    void bimWidgetChanged(void);
 //  void uqMethodChanged(const QString &arg1);


private:
    QVBoxLayout *layout;
    QWidget     *methodSpecific;
    //    QComboBox   *samplingMethod;
    //    QLineEdit   *numSamples;
    //    QLineEdit   *randomSeed;
    //    QPushButton *run;

    QComboBox   *bimSelection;

    //    SimCenterWidget *uqType;
    SimCenterAppWidget *bimInput;
    bool selectionChangeOK;
    bool includeCentroid;
    RandomVariablesContainer *theRandomVariablesContainer;

};

#endif // 	SIM_SELECTION_H
