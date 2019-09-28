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
#include <QComboBox>
#include <QDebug>
#include <QList>

#include <QLabel>
#include <QLineEdit>

#include <QMetaEnum>

GeneralInformationWidget *
GeneralInformationWidget::getInstance() {
  if (theInstance == 0)
    theInstance = new GeneralInformationWidget();

  return theInstance;
 }

GeneralInformationWidget *GeneralInformationWidget::theInstance = 0;

GeneralInformationWidget::GeneralInformationWidget(QWidget *parent) 
  : SimCenterWidget(parent)
{
    nameEdit = new QLineEdit(this);
    storiesEdit = new QLineEdit(tr("1"));
    storiesEdit->setValidator(new QIntValidator);  

    heightEdit = new QLineEdit(this);
    widthEdit = new QLineEdit();
    depthEdit = new QLineEdit();
    planAreaEdit = new QLineEdit();
    heightEdit->setValidator(new QDoubleValidator);  
    widthEdit->setValidator(new QDoubleValidator);  
    depthEdit->setValidator(new QDoubleValidator);  
    planAreaEdit->setValidator(new QDoubleValidator);  

    latitudeEdit = new QLineEdit(this);
    latitudeEdit->setValidator(new QDoubleValidator);  

    longitudeEdit = new QLineEdit(this);
    longitudeEdit->setValidator(new QDoubleValidator);  

    unitsForceCombo = new QComboBox(this);
    unitsForceCombo->addItem("Newtons", ForceUnit::N);
    unitsForceCombo->addItem("Kilonewtons", ForceUnit::kN);
    unitsForceCombo->addItem("Pounds", ForceUnit::lb);
    unitsForceCombo->addItem("Kips", ForceUnit::kips);
    unitsForceCombo->setCurrentIndex(3);

    unitsLengthCombo = new QComboBox(this);
    unitsLengthCombo->addItem("Meters", LengthUnit::m);
    unitsLengthCombo->addItem("Centimeters", LengthUnit::cm);
    unitsLengthCombo->addItem("Millimeters", LengthUnit::mm);
    unitsLengthCombo->addItem("Inches", LengthUnit::in);
    unitsLengthCombo->addItem("Feet", LengthUnit::ft);
    unitsLengthCombo->setCurrentIndex(3);

    unitsTemperatureCombo = new QComboBox(this);
    unitsTemperatureCombo->addItem("Celsius", TemperatureUnit::C);
    unitsTemperatureCombo->addItem("Fahrenheit", TemperatureUnit::F);
    unitsTemperatureCombo->addItem("Kelvin", TemperatureUnit::K);


    unitsTimeCombo = new QComboBox();
    unitsTimeCombo->addItem("Seconds", TimeUnit::sec);
    unitsTimeCombo->addItem("Minutes", TimeUnit::min);
    unitsTimeCombo->addItem("Hours", TimeUnit::hr);

    QVBoxLayout* layout = new QVBoxLayout(this);

    //Building information
    QGroupBox* infoGroupEdit = new QGroupBox("Building Information", this);
    QFormLayout* infoFormLayout = new QFormLayout(infoGroupEdit);
    infoFormLayout->addRow(tr("Name"), nameEdit);
    //    infoFormLayout->addRow(tr("Revision"), revEdit);
    //    infoFormLayout->addRow(tr("Type"), typeEdit);
    // infoFormLayout->addRow(tr("Year"), yearEdit);
    infoFormLayout->setAlignment(Qt::AlignLeft);
    infoFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    infoFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    // Properties
    QGroupBox* propertiesGroupBox = new QGroupBox("Properties", this);
    QFormLayout *propertiesFormLayout = new QFormLayout(propertiesGroupBox);

   propertiesFormLayout->addRow(tr("# Stories"), storiesEdit);
   propertiesFormLayout->addRow(tr("Height"), heightEdit);
   propertiesFormLayout->addRow(tr("Width"), widthEdit);
   propertiesFormLayout->addRow(tr("Depth"), depthEdit);
   propertiesFormLayout->addRow(tr("Plan Area"), planAreaEdit);

   //   propertiesFormLayout->addRow(tr("Weight"), weightEdit);
   propertiesFormLayout->setAlignment(Qt::AlignLeft);
   propertiesFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
   propertiesFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
   //Setting Style

    //Location
    QGroupBox* locationGroupEdit = new QGroupBox("Location", this);
    QFormLayout* locationFormLayout = new QFormLayout(locationGroupEdit);
    //   locationFormLayout->addRow(tr("Address"), locationNameEdit);
    locationFormLayout->addRow(tr("Latitude"), latitudeEdit);
    locationFormLayout->addRow(tr("Longitude"), longitudeEdit);
    //Setting Style
    locationFormLayout->setAlignment(Qt::AlignLeft);
    locationFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    locationFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);


    //Units
    QGroupBox* unitsGroupEdit = new QGroupBox("Units", this);
    QFormLayout* unitsFormLayout = new QFormLayout(unitsGroupEdit);
    unitsFormLayout->addRow(tr("Force"), unitsForceCombo);
    unitsFormLayout->addRow(tr("Length"), unitsLengthCombo);
    unitsFormLayout->addRow(tr("Temperature"), unitsTemperatureCombo);
   // unitsFormLayout->addRow(tr("Time"), unitsTimeCombo);
    //Setting Style
    unitsFormLayout->setAlignment(Qt::AlignLeft);
    unitsFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    unitsFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    setWindowTitle(tr("General Information"));

    layout->addWidget(infoGroupEdit);
    layout->addWidget(propertiesGroupBox);
    layout->addWidget(locationGroupEdit);
    layout->addWidget(unitsGroupEdit);
    layout->addStretch(1);

    this->setMinimumWidth(200);
    this->setMaximumWidth(400);

    connect(storiesEdit,SIGNAL(editingFinished()),this,SLOT(numStoriesEditingFinished()));
    connect(heightEdit,SIGNAL(editingFinished()),this,SLOT(heightEditingFinished()));
    connect(widthEdit,SIGNAL(editingFinished()),this,SLOT(buildingDimensionsEditingFinished()));
    connect(depthEdit,SIGNAL(editingFinished()),this,SLOT(buildingDimensionsEditingFinished()));
    connect(planAreaEdit,SIGNAL(editingFinished()), this, SLOT(buildingDimensionsEditingFinished()));
}

GeneralInformationWidget::~GeneralInformationWidget()
{

}

void
GeneralInformationWidget::setDefaultProperties(int numStory,
					 double height,
					 double width,
					 double depth,
					 double latit,
					 double longit)
{
  //    this->setNumFloors(numStory);
  //    this->setHeight(height);

    this->setBuildingDimensions(width, depth, width*depth);
    this->setBuildingLocation(latit, longit);
    this->setNumStoriesAndHeight(numStory, height);
}

bool
GeneralInformationWidget::outputToJSON(QJsonObject &jsonObj){

    jsonObj["name"] = nameEdit->text().trimmed();
    //    jsonObj["revision"] = revEdit->text().toDouble();
    //    jsonObj["type"] = typeEdit->text().trimmed();
    //    jsonObj["year"] = yearEdit->text().toInt();
    jsonObj["stories"] = storiesEdit->text().toInt();
    jsonObj["width"] = widthEdit->text().toDouble();
    jsonObj["depth"] = depthEdit->text().toDouble();

    // if the plan area is provided:
    double planAreaValue = planAreaEdit->text().toDouble();
    if (planAreaValue > 0.0)
        jsonObj["planArea"] = planAreaValue;
    else {
        double widthValue = widthEdit->text().toDouble();
        double depthValue = depthEdit->text().toDouble();
        double rectAreaValue = widthValue * depthValue;
        // otherwise, assume a rectangular plan and use width x depth
        if (rectAreaValue > 0.0)
            jsonObj["planArea"] = rectAreaValue;
        else
            // if none of the above are provided, use 0.0 for the area
            jsonObj["planArea"] = 0.0;
    }

    jsonObj["height"] = heightEdit->text().toDouble();
    //    jsonObj["weight"] = weightEdit->text().toDouble();

    QJsonObject location;

    /*
    location["name"] = locationNameEdit->text().trimmed();
    */

    QString dblVal = latitudeEdit->text();
    location["latitude"] = dblVal.toDouble();

    dblVal = longitudeEdit->text();
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
    qDebug() << "General Information";
    double rev;

    QJsonValue nameValue = jsonObject["name"];
    nameEdit->setText(nameValue.toString());

    QJsonValue storiesValue = jsonObject["stories"];
    storiesEdit->setText(QString::number(storiesValue.toInt()));

    QJsonValue heightValue = jsonObject["height"];
    if (!heightValue.isNull())
        heightEdit->setText(  QString::number(heightValue.toDouble()));
    else
        heightEdit->setText("0.0");

    QJsonValue widthValue = jsonObject["width"];
    if (!widthValue.isNull())
        widthEdit->setText(  QString::number(widthValue.toDouble()));
    else
        widthEdit->setText("0.0");

    QJsonValue depthValue = jsonObject["depth"];
    if (!depthValue.isNull())
        depthEdit->setText(  QString::number(depthValue.toDouble()));
    else
        depthEdit->setText("0.0");

    QJsonValue planAreaValue = jsonObject["planArea"];
    if(planAreaValue.isUndefined() || planAreaValue == QJsonValue::Null || !planAreaValue.isDouble())
        planAreaEdit->setText("0.0");
    else
        planAreaEdit->setText(QString::number(planAreaValue.toDouble()));

    // Location Object
    QJsonValue locationValue = jsonObject["location"];
    if (!locationValue.isNull()) {
        QJsonObject locationObj = locationValue.toObject();

        QJsonValue locationLatitudeValue = locationObj["latitude"];
        latitudeEdit->setText(QString::number(locationLatitudeValue.toDouble()));

        QJsonValue locationLongitudeValue = locationObj["longitude"];
        longitudeEdit->setText(QString::number(locationLongitudeValue.toDouble()));
    } else {
        longitudeEdit->setText("0.0");
        latitudeEdit->setText("0.0");
    }

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
    storiesEdit->clear();
    heightEdit->clear();
    widthEdit->clear();
    depthEdit->clear();
    planAreaEdit->clear();

    latitudeEdit->clear();
    longitudeEdit->clear();

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

QString
GeneralInformationWidget::getLengthUnit()
{
    return unitEnumToString(unitsLengthCombo->currentData().value<LengthUnit>());
}

QString
GeneralInformationWidget::getForceUnit()
{
   return unitEnumToString(unitsForceCombo->currentData().value<ForceUnit>());
}

void
GeneralInformationWidget::numStoriesEditingFinished(void) {
  emit numStoriesOrHeightChanged(storiesEdit->text().toInt(), heightEdit->text().toDouble());
}

void
GeneralInformationWidget::heightEditingFinished(void) {
 emit numStoriesOrHeightChanged(storiesEdit->text().toInt(), heightEdit->text().toDouble());
}

void
GeneralInformationWidget::buildingDimensionsEditingFinished(void) {
 emit buildingDimensionsChanged(widthEdit->text().toDouble(), depthEdit->text().toDouble(), planAreaEdit->text().toDouble());
}
/*
void
GeneralInformationWidget::setNumFloors(int newNumFloors) {
  if (storiesEdit->text().toInt() != newNumFloors) {
    storiesEdit->setValue(newNumFloors);
    qDebug() << "GeneralInformation::setNumFloors()";
    emit numFloorsChanged(newNumFloors);
  }
}

void
GeneralInformationWidget::setHeight(double newHeight) {
   qDebug() << "GEI:setHeight " << newHeight;

  if (heightEdit->text().toDouble() != newHeight) {
    heightEdit->setText(QString::number(newHeight)); 
    emit buildingHeightChanged(newHeight);
  }
}
*/


void
GeneralInformationWidget::setNumStoriesAndHeight(int newNumFloors, double newHeight) {
  if ((storiesEdit->text().toInt() != newNumFloors) ||
       (heightEdit->text().toDouble() != newHeight)) {
    storiesEdit->setText(QString::number(newNumFloors));
    heightEdit->setText(QString::number(newHeight)); 
    emit numStoriesOrHeightChanged(newNumFloors, newHeight);
  }
}


void
GeneralInformationWidget::setBuildingLocation(double newLat, double newLong) {
  if (latitudeEdit->text().toDouble() != newLat || 
      longitudeEdit->text().toDouble() != newLong) {

    latitudeEdit->setText(QString::number(newLat));
    longitudeEdit->setText(QString::number(newLong));
    emit buildingLocationChanged(newLat, newLong);
  }
}

void
GeneralInformationWidget::setBuildingDimensions(double newB, double newD, double newA) {
  if (widthEdit->text().toDouble() != newB || 
      depthEdit->text().toDouble() != newD ||
      planAreaEdit->text().toDouble() != newA) {

    widthEdit->setText(QString::number(newB)); 

    planAreaEdit->setText(QString::number(newA)); 

    emit buildingDimensionsChanged(newB, newD, newA);
    depthEdit->setText(QString::number(newD));
  }
}

int
GeneralInformationWidget::getNumFloors(void) {
  return storiesEdit->text().toInt();
}

double
GeneralInformationWidget::getHeight(void) {
  return heightEdit->text().toDouble();
}

void
GeneralInformationWidget::getBuildingDimensions(double &width, double &depth, double &area) {
  width = widthEdit->text().toDouble();
  depth = depthEdit->text().toDouble();
  area = planAreaEdit->text().toDouble();
}

void
GeneralInformationWidget::getBuildingLocation(double &latitude, double &longitude) {
  latitude = latitudeEdit->text().toDouble();
  longitude = longitudeEdit->text().toDouble();
}

