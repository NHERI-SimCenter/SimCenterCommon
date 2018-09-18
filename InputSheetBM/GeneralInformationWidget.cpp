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



#include "GeneralInformationWidget.h"
#include <QFormLayout>
#include <QGroupBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QList>

#include <QLabel>
#include <QLineEdit>

#include <QMetaEnum>


GeneralInformationWidget::GeneralInformationWidget(QWidget *parent) : SimCenterWidget(parent)
{
    nameEdit = new QLineEdit(this);
    revEdit = new QLineEdit(this);
    typeEdit = new QLineEdit(this);
    yearBox = new QSpinBox(this);
    yearBox->setRange(1800, INT_MAX);
    yearBox->setValue(2018);
    storiesBox = new QSpinBox(this);
    storiesBox->setRange(1, INT_MAX);

    heightEdit = new QLineEdit(this);
    planAreaEdit = new QLineEdit(this);

    locationNameEdit = new QLineEdit(this);
    locationLatBox = new QDoubleSpinBox(this);
    locationLatBox->setRange(-90.0, 90.0);
    locationLatBox->setDecimals(4);
    locationLatBox->setSingleStep(0.0001);

    locationLonBox = new QDoubleSpinBox(this);
    locationLonBox->setRange(-180.0, 180.0);
    locationLonBox->setDecimals(4);
    locationLonBox->setSingleStep(0.0001);

    unitsForceCombo = new QComboBox(this);
    unitsForceCombo->addItem("Newtons", ForceUnit::N);
    unitsForceCombo->addItem("Kilonewtons", ForceUnit::kN);
    unitsForceCombo->addItem("Pounds", ForceUnit::lb);
    unitsForceCombo->addItem("Kips", ForceUnit::kips);

    unitsLengthCombo = new QComboBox(this);
    unitsLengthCombo->addItem("Meters", LengthUnit::m);
    unitsLengthCombo->addItem("Centimeters", LengthUnit::cm);
    unitsLengthCombo->addItem("Millimeters", LengthUnit::mm);
    unitsLengthCombo->addItem("Inches", LengthUnit::in);
    unitsLengthCombo->addItem("Feet", LengthUnit::ft);

    unitsTemperatureCombo = new QComboBox(this);
    unitsTemperatureCombo->addItem("Celsius", TemperatureUnit::C);
    unitsTemperatureCombo->addItem("Fahrenheit", TemperatureUnit::F);
    unitsTemperatureCombo->addItem("Kelvin", TemperatureUnit::K);

    unitsTimeCombo = new QComboBox(this);
    unitsTimeCombo->addItem("Seconds", TimeUnit::sec);
    unitsTimeCombo->addItem("Minutes", TimeUnit::min);
    unitsTimeCombo->addItem("Hours", TimeUnit::hr);

    QVBoxLayout* layout = new QVBoxLayout(this);

    //Building information
    QGroupBox* infoGroupBox = new QGroupBox("Building Information", this);
    QFormLayout* infoFormLayout = new QFormLayout(infoGroupBox);
    infoFormLayout->addRow(tr("Name"), nameEdit);
    infoFormLayout->addRow(tr("Revision"), revEdit);
    infoFormLayout->addRow(tr("Type"), typeEdit);
    infoFormLayout->addRow(tr("Year"), yearBox);
    infoFormLayout->addRow(tr("Stories"), storiesBox);
    infoFormLayout->addRow(tr("Height"), heightEdit);
    infoFormLayout->addRow(tr("Plan Area"), planAreaEdit);
    //Setting Style
    infoFormLayout->setAlignment(Qt::AlignLeft);
    infoFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    infoFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);


    //Location
    QGroupBox* locationGroupBox = new QGroupBox("Location", this);
    QFormLayout* locationFormLayout = new QFormLayout(locationGroupBox);
    locationFormLayout->addRow(tr("Name"), locationNameEdit);
    locationFormLayout->addRow(tr("Latitude"), locationLatBox);
    locationFormLayout->addRow(tr("Longitude"), locationLonBox);
    //Setting Style
    locationFormLayout->setAlignment(Qt::AlignLeft);
    locationFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    locationFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);


    //Units
    QGroupBox* unitsGroupBox = new QGroupBox("Units", this);
    QFormLayout* unitsFormLayout = new QFormLayout(unitsGroupBox);
    unitsFormLayout->addRow(tr("Force"), unitsForceCombo);
    unitsFormLayout->addRow(tr("Length"), unitsLengthCombo);
    unitsFormLayout->addRow(tr("Temperature"), unitsTemperatureCombo);
    unitsFormLayout->addRow(tr("Time"), unitsTimeCombo);
    //Setting Style
    unitsFormLayout->setAlignment(Qt::AlignLeft);
    unitsFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    unitsFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);


    setWindowTitle(tr("General Information"));

    layout->addWidget(infoGroupBox);
    layout->addWidget(locationGroupBox);
    layout->addWidget(unitsGroupBox);
    layout->addStretch(1);

    this->setMinimumWidth(200);
    this->setMaximumWidth(400);
}

GeneralInformationWidget::~GeneralInformationWidget()
{

}

bool
GeneralInformationWidget::outputToJSON(QJsonObject &jsonObj){

    jsonObj["name"] = nameEdit->text().trimmed();
    jsonObj["revision"] = revEdit->text().toDouble();
    jsonObj["type"] = typeEdit->text().trimmed();
    jsonObj["year"] = yearBox->text().toInt();
    jsonObj["stories"] = storiesBox->text().toInt();
    jsonObj["height"] = heightEdit->text().toDouble();
    jsonObj["planArea"] = planAreaEdit->text().toDouble();

    QJsonObject location;
    location["name"] = locationNameEdit->text().trimmed();

    QString dblVal = locationLatBox->text();
    location["latitude"] = dblVal.toDouble();

    dblVal = locationLonBox->text();
    location["longitude"] = dblVal.toDouble();

    jsonObj["location"] = location;

    QJsonObject units;
    units["force"] = unitEnumToString(unitsForceCombo->currentData().value<ForceUnit>());
    units["length"] = unitEnumToString(unitsLengthCombo->currentData().value<LengthUnit>());
    units["time"] = unitEnumToString(unitsTimeCombo->currentData().value<TimeUnit>());
    units["temperature"] = unitEnumToString(unitsTemperatureCombo->currentData().value<TemperatureUnit>());

    jsonObj["units"] = units;

    return(true);

}

bool
GeneralInformationWidget::inputFromJSON(QJsonObject &jsonObject){

    double rev;

    QJsonValue nameValue = jsonObject["name"];
    nameEdit->setText(nameValue.toString());

    QJsonValue revValue = jsonObject["revision"];
    rev = revValue.toDouble();
    revEdit->setText( QString::number(rev) );

    QJsonValue typeValue = jsonObject["type"];
    typeEdit->setText(typeValue.toString());

    QJsonValue yearValue = jsonObject["year"];
    yearBox->setValue(yearValue.toInt());

    QJsonValue storiesValue = jsonObject["stories"];
    storiesBox->setValue(storiesValue.toInt());

    QJsonValue heightValue = jsonObject["height"];
    heightEdit->setText(  QString::number(heightValue.toDouble()) );

    QJsonValue planAreaValue = jsonObject["planArea"];
    if(planAreaValue.isUndefined() || planAreaValue == QJsonValue::Null || !planAreaValue.isDouble())
        planAreaEdit->setText("0.0");
    else
        planAreaEdit->setText(QString::number(planAreaValue.toDouble()));

    // Location Object
    QJsonValue locationValue = jsonObject["location"];
    QJsonObject locationObj = locationValue.toObject();

    QJsonValue locationNameValue = locationObj["name"];
    locationNameEdit->setText(locationNameValue.toString());

    QJsonValue locationLatitudeValue = locationObj["latitude"];
    locationLatBox->setValue(locationLatitudeValue.toDouble());

    QJsonValue locationLongitudeValue = locationObj["longitude"];
    locationLonBox->setValue(locationLongitudeValue.toDouble());


    // Units Object
    QJsonValue unitsValue = jsonObject["units"];
    QJsonObject unitsObj = unitsValue.toObject();

    QJsonValue unitsForceValue = unitsObj["force"];
    ForceUnit forceUnit = unitStringToEnum<ForceUnit>(unitsForceValue.toString());
    int forceUnitIndex = unitsForceCombo->findData(forceUnit);
    unitsForceCombo->setCurrentIndex(forceUnitIndex);

    QJsonValue unitsLengthValue = unitsObj["length"];
    LengthUnit lengthUnit = unitStringToEnum<LengthUnit>(unitsLengthValue.toString());
    int lengthUnitIndex = unitsLengthCombo->findData(lengthUnit);
    unitsLengthCombo->setCurrentIndex(lengthUnitIndex);

    QJsonValue unitsTimeValue = unitsObj["time"];
    TimeUnit timeUnit = unitStringToEnum<TimeUnit>(unitsTimeValue.toString());
    int timeUnitIndex = unitsTimeCombo->findData(timeUnit);
    unitsTimeCombo->setCurrentIndex(timeUnitIndex);

    QJsonValue unitsTempValue = unitsObj["temperature"];
    TemperatureUnit tempUnit = unitStringToEnum<TemperatureUnit>(unitsTempValue.toString());
    int tempUnitIndex = unitsTemperatureCombo->findData(tempUnit);
    unitsTemperatureCombo->setCurrentIndex(tempUnitIndex);

    return(true);
}

void
GeneralInformationWidget::clear(void)
{
    nameEdit->clear();
    revEdit->clear();
    typeEdit->clear();
    yearBox->clear();
    storiesBox->clear();
    heightEdit->clear();

    locationNameEdit->clear();
    locationLatBox->clear();
    locationLonBox->clear();

    unitsForceCombo->clear();
    unitsTemperatureCombo->clear();
    unitsTimeCombo->clear();

}

template<typename UnitEnum>
QString GeneralInformationWidget::unitEnumToString(UnitEnum enumValue)
{
    return QString(QMetaEnum::fromType<UnitEnum>().valueToKey(enumValue));
}

template<typename UnitEnum>
UnitEnum GeneralInformationWidget::unitStringToEnum(QString unitString)
{
    return (UnitEnum)QMetaEnum::fromType<UnitEnum>().keyToValue(unitString.toStdString().c_str());
}
