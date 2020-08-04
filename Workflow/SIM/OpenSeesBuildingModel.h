#ifndef OPENSEES_BUILDING_MODEL_H
#define OPENSEES_BUILDING_MODEL_H

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

#include <QGroupBox>
#include <QVector>
#include <QGridLayout>
#include <QComboBox>

class InputWidgetParameters;
class RandomVariablesContainer;

class OpenSeesBuildingModel : public SimCenterAppWidget
{
    Q_OBJECT
public:
    explicit OpenSeesBuildingModel(RandomVariablesContainer *theRandomVariableIW, 
				   bool includeCentroid = false,
				   QWidget *parent = 0);
    ~OpenSeesBuildingModel();

    bool outputToJSON(QJsonObject &rvObject) override;
    bool inputFromJSON(QJsonObject &rvObject) override;
    bool outputAppDataToJSON(QJsonObject &rvObject) override;
    bool inputAppDataFromJSON(QJsonObject &rvObject) override;
    bool copyFiles(QString &dirName) override;

    QString getMainInput();

     // copy main file to new filename ONLY if varNamesAndValues not empy
    void specialCopyMainInput(QString fileName, QStringList varNamesAndValues);
    void setFilename1(QString filnema1);

signals:

public slots:
   void clear(void);
   void chooseFileName1(void);

private:

    QGridLayout *layout;
    QWidget     *femSpecific;

    QString fileName1;
    QLineEdit *file1;
    QLineEdit *centroidNodes;
    QLineEdit *responseNodes;
    QLineEdit *elements;
    QLineEdit *ndm;
    QLineEdit *ndf;
    bool includeCentroid;
    RandomVariablesContainer *theRandomVariablesContainer;
    QStringList varNamesAndValues;
};

#endif // OPENSEES_BUILDING_MODEL_H
