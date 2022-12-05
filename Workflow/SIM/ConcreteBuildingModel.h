#ifndef CONCRETE_BUILDING_MODEL_H
#define CONCRETE_BUILDING_MODEL_H
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

// Written: Dr. Stevan Gavrilovic

/* macro added by pmh on 12/31/2020 for compiling with MSVC2019 */
#ifndef __FUNCSIG__
#define __FUNCSIG__  __PRETTY_FUNCTION__
#endif

#include "SimCenterAppWidget.h"

#include <QVector>
#include <QMap>

class InputWidgetParameters;
class RandomVariablesContainer;

class LineEditRV;
class QButtonGroup;
class QLineEdit;
class QRadioButton;
class QTableWidget;
class QFile;

class ConcreteBuildingModel : public SimCenterAppWidget
{
    Q_OBJECT
public:
    explicit ConcreteBuildingModel(RandomVariablesContainer *theRandomVariableIW, QWidget *parent = 0);
    ~ConcreteBuildingModel();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);
    bool outputAppDataToJSON(QJsonObject &rvObject);
    bool inputAppDataFromJSON(QJsonObject &rvObject);
    bool copyFiles(QString &dirName);

public slots:

    void onLineEditTextChanged();

signals:
    void numStoriesOrHeightChanged(int numFloors, double height);
    void buildingDimensionsChanged(double newWidth, double newDepth, double planArea);

private slots:

    void onStoryLEChange(void);
    void onSpanLEChange(void);

private:

    // Checks if the incoming text is a random variable and adds it to the container if it is
    void addRandomVariable(QString &text, int numReferences = 1);

    // Removes a random variable
    void removeRandomVariable(QString &text, int numReferences=1);

    LineEditRV *dampingRatio;
    QLineEdit* extWidthLE = nullptr;
    QLineEdit* extDepthLE = nullptr;
    QLineEdit* intWidthLE = nullptr;
    QLineEdit* intDepthLE = nullptr;
    QLineEdit* beamWidthLE = nullptr;
    QLineEdit* beamDepthLE = nullptr;
    QLineEdit* vecStoryLE = nullptr;
    QLineEdit* vecSpansLE = nullptr;
    QLineEdit* RLE = nullptr;
    QLineEdit* CdLE = nullptr;
    QLineEdit* OmegaLE = nullptr;
    QLineEdit* SDSLE = nullptr;
    QLineEdit* SD1LE = nullptr;
    QLineEdit* TLLE = nullptr;
    QLineEdit* colIgLE = nullptr;
    QLineEdit* beamIgLE = nullptr;
    QLineEdit* fyLE = nullptr;
    QLineEdit* beamfpcLE = nullptr;
    QLineEdit* colsfpcLE = nullptr;
    QLineEdit* deadLaodLE = nullptr;
    QLineEdit* liveLoadLE = nullptr;
    QLineEdit* tribLengthGravityLE = nullptr;
    QLineEdit* tribLengthSeismicLE = nullptr;
    QRadioButton* hingOpt1Radio = nullptr;
    QRadioButton* hingOpt2Radio = nullptr;
    QRadioButton* hingOpt3Radio = nullptr;
    QRadioButton* regOpt1Radio = nullptr;
    QRadioButton* regOpt2Radio = nullptr;

    QButtonGroup* hingeButtonGroup = nullptr;
    QButtonGroup* regButtonGroup = nullptr;

    // Sets the values of the default example
    void setDefaultValues(void);

    // Returns true if the input string is a random variable and false if not
    bool checkRV(const QString& value);

    RandomVariablesContainer *theRandomVariablesContainer;
    QStringList varNamesAndValues;

    QMap<QString, int>randomVariables;

    // Pop-up for any error messages
    void errorMessageDialog(const QString& errorString);

};

#endif // CONCRETE_BUILDING_MODEL_H
