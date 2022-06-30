#ifndef PLOM_INPUT_WIDGET_H
#define PLOM_INPUT_WIDGET_H

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

#include <UQ_Method.h>
class QLineEdit;
class QCheckBox;
class QPushButton;
class QComboBox;
class QLabel;
class QFrame;
class QButtonGroup;
class QStackedWidget;
//class InputWidgetParameters;
//class InputWidgetEDP;
//class InputWidgetFEM;
class QComboBox;
class QTabWidget;

class PLoMInputWidget : public UQ_Method
{
    Q_OBJECT
public:
    explicit PLoMInputWidget(QWidget *parent = 0);
    ~PLoMInputWidget();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);
    void clear(void);

    int getNumberTasks(void);
    int parseInputDataForRV(QString name1);
    int parseOutputDataForQoI(QString name1);
    int parsePretrainedModelForRVQoI(QString name1);
    int numSamples;
    bool copyFiles(QString &fileDir);
public slots:
    void setOutputDir(bool tog);
    void setConstraints(bool tog);
    void doAdvancedSetup(bool tog);
    void setDiffMaps(bool tog);
private:
    QLineEdit *ratioNewSamples;
    QLineEdit *epsilonPCA;
    QLineEdit *smootherKDE;
    QLineEdit *tolKDE;
    QLineEdit *randomSeed;
    QLineEdit *inpFileDir;
    QLineEdit *outFileDir;
    QLineEdit *inpFileDir2;
    QPushButton *chooseOutFile;
    QLineEdit *initialDoE;
    QCheckBox *theAdvancedCheckBox;
    QCheckBox *theLogtCheckBox;
    QCheckBox *theDMCheckBox;
    QCheckBox *theConstraintsButton;
    QLineEdit *constraintsPath;
    QPushButton *chooseConstraints;
    QLineEdit *numIter;
    QLineEdit *tolIter;

    QLabel * theAdvancedTitle;
    QLabel * theLogtLabel;
    QLabel * theLogtLabel2;
    QLabel * theInitialLabel;
    QLabel * errMSG;
    QLabel * newEpsilonPCA;
    QLabel * newSmootherKDE;
    QLabel * newTolKDE;
    QLabel * newRandomSeed;
    QLabel * theDMLabel;
    QLabel * theConstraintsLabel1;
    QLabel * theConstraintsLabel2;
    QLabel * numIterLabel;
    QLabel * tolIterLabel;

    //InputWidgetParameters *theParameters;
    //InputWidgetEDP *theEdpWidget;
    //InputWidgetFEM *theFemWidget;

    QFrame * lineA;

    QButtonGroup* m_typeButtonsGroup;
    QStackedWidget* m_stackedWidgets;
    QWidget* rawDataGroup;
    QWidget* preTrainGroup;
    QWidget* newSampleRatioWidget;

    //QComboBox *theAdvancedComboBox;
    QWidget* advGeneralWidget;
    QWidget* advKDEWidget;
    QWidget* advConstraintsWidget;
    //QStackedWidget* adv_stackedWidgets;

    int countColumn(QString name1);
    bool preTrained;

    QTabWidget *advComboWidget;
};

#endif // PLOM_INPUT_WIDGET_H
