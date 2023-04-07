#ifndef SIMCENTERINTENSITYMEASUREWIDGET_H
#define SIMCENTERINTENSITYMEASUREWIDGET_H
/* *****************************************************************************
Copyright (c) 2016-2022, The Regents of the University of California (Regents).
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

#include <SimCenterWidget.h>
#include <SimCenterAppWidget.h>

class SimCenterIntensityMeasureCombo;
class SimCenterUnitsCombo;
class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;
class QRadioButton;
class QRegularExpressionValidator;
class QLineEdit;

#include <QGroupBox>
#include <QLabel>
//#include "JsonSerializable.h"

class SimCenterIM : public SimCenterWidget
{
    Q_OBJECT
public:
    explicit SimCenterIM(SimCenterIntensityMeasureCombo *theIM, SimCenterUnitsCombo *theUnit, QWidget *parent = 0);
    ~SimCenterIM();

    QRadioButton *button;
    QGridLayout *imUnitLayout;
    SimCenterIntensityMeasureCombo *myIM;
    SimCenterUnitsCombo *myUnit;
    QWidget *myPeriods;
    QLineEdit *periodLine;
    QRegularExpressionValidator *LEValidator;
    QLabel *myUnitLabel; // this will be hidden - sy

    void setLabelVisible(bool tog);
    void addGridField(void);  // additionally for GM selection -sy
    void setCurrentIMtoPSA(void);
    QLineEdit *minVal, *maxVal, *numBins;  // additionally for GM selection
    int nCol;

public slots:
    void handleIMChanged(const QString& newIM);
    QString checkPeriodsValid(const QString& input) const;
signals:
    void numBinsChanged(void);
private:
};

class SimCenterIntensityMeasureWidget : public SimCenterWidget
{
    Q_OBJECT
public:
    SimCenterIntensityMeasureWidget(QWidget* parent = nullptr);

    bool outputToJSON(QJsonObject &jsonObject);

    bool inputFromJSON(QJsonObject &jsonObject);

    void reset(void);

    void clear(void);

    int getNumberOfIM(void);

    void addGridField(void);  // additionally for GM selection

    int setIM(const QString& parameterName, const QString& im);

    int setUnit(const QString& parameterName, const QString& unit);

    QList<QString> getParameterNames();

    bool addGrid = false;

public slots:
    void addIMItem();
    void removeIMItem();
    void removeAll();
    void getNumBins(void);

signals:
    void numBinsChanged(int);

private:
    QVBoxLayout *vLayout;

    QVBoxLayout *imLayout = nullptr;

    SimCenterIntensityMeasureCombo* imFindChild(const QString& name);

    SimCenterUnitsCombo* unitFindChild(const QString& name);

    int sumGridField(void);



};
#endif // SIMCENTERINTENSITYMEASUREWIDGET_H
