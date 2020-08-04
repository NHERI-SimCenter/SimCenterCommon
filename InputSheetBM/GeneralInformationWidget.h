#ifndef GENERALINFORMATIONWIDGET_H
#define GENERALINFORMATIONWIDGET_H



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

#include <QStringList>
#include <QHBoxLayout>
#include <QComboBox>
#include <SimCenterWidget.h>

class QLineEdit;

class GeneralInformationWidget : public SimCenterWidget
{
    Q_OBJECT

private:
    explicit GeneralInformationWidget(QWidget *parent = 0);
    ~GeneralInformationWidget();
    static GeneralInformationWidget *theInstance;

public:
    static GeneralInformationWidget *getInstance(void);

    void setDefaultProperties(int numStory,
			      double height,
			      double width,
			      double depth,
			      double latit,
			      double longit);

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);
    bool outputToJSON(QJsonArray &arrayObject);
    bool inputFromJSON(QJsonArray &arrayObject);

    void clear(void);

    enum LengthUnit{m, cm, mm, in, ft};
    Q_ENUM(LengthUnit)
    enum ForceUnit{N, kN, lb, kips};
    Q_ENUM(ForceUnit)
    enum TimeUnit{sec, min, hr};
    Q_ENUM(TimeUnit)
    enum TemperatureUnit{C, F, K};
    Q_ENUM(TemperatureUnit)

    int getNumFloors(void);
    double getHeight(void);
    void getBuildingDimensions(double &newWidth, double &newDepth, double &planArea);
    void getBuildingLocation(double &latitude, double &longitude);
    QString getLengthUnit();
    QString getForceUnit();

public slots:
    void numStoriesEditingFinished(void);
    void heightEditingFinished(void);
    void buildingDimensionsEditingFinished(void);
    // void setNumFloors(int newNumFloors);
    // void setHeight(double newHeight);
    void setNumStoriesAndHeight(int numFloors, double height);
    void setBuildingDimensions(double newWidth, double newDepth, double planArea);
    void setBuildingLocation(double latitude, double longitude);

signals:
    //    void numFloorsChanged(int newNumFloors);
    //    void buildingHeightChanged(double newHeight);
    void numStoriesOrHeightChanged(int numFloors, double height);
    void buildingDimensionsChanged(double newWidth, double newDepth, double planArea);
    void buildingLocationChanged(double latitude, double longitude);

    void unitsChanged(QString lengthUnit, QString ForceUnit);

private:
    QStringList   tableHeader;

    QLineEdit *nameEdit;
    QLineEdit *storiesEdit;
    QLineEdit *heightEdit;
    QLineEdit *widthEdit;
    QLineEdit *depthEdit;
    QLineEdit *planAreaEdit;

    QLineEdit *latitudeEdit;
    QLineEdit *longitudeEdit;

    QComboBox *unitsForceCombo;
    QComboBox *unitsLengthCombo;
    QComboBox *unitsTemperatureCombo;
    QComboBox *unitsTimeCombo;

    template<typename UnitEnum> QString unitEnumToString(UnitEnum enumValue);
    template<typename UnitEnum> UnitEnum unitStringToEnum(QString unitString);

};


#endif // GENERALINFORMATIONWIDGET_H
