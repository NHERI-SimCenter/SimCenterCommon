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

    yearBuilt = new QLineEdit();
    yearBuilt->setText("1990");
    
    yearBuilt->setValidator(new QIntValidator); 

    structType = new QComboBox(this);
    structType->setToolTip(tr("The type of structural system used to resist lateral loads. The available structural systems below are defined as Building Types in the Hazus Earthquake Technical Manual."));
    structType->addItem("W1",0);
    structType->addItem("W2",1);
    structType->addItem("S1",2);
    structType->addItem("S2",3);
    structType->addItem("S3",4);
    structType->addItem("S4",5);
    structType->addItem("S5",6);
    structType->addItem("C1",7);
    structType->addItem("C2",8);
    structType->addItem("C3",9);
    structType->addItem("PC1",10);
    structType->addItem("PC2",11);
    structType->addItem("RM1",12);
    structType->addItem("RM2",13);
    structType->addItem("URM",14);
    structType->addItem("MH",15);
    structType->addItem("N/A",16);

    structType->setItemData( 0,"Wood, Light Frame, less than 5,000 sq. ft.", Qt::ToolTipRole);
    structType->setItemData( 1,"Wood, Commercial & Industrial, larger than 5,000 sq. ft.", Qt::ToolTipRole);
    structType->setItemData( 2,"Steel Moment Frame", Qt::ToolTipRole);
    structType->setItemData( 3,"Steel Braced Frame", Qt::ToolTipRole);
    structType->setItemData( 4,"Steel Light Frame", Qt::ToolTipRole);
    structType->setItemData( 5,"Steel Frame with Cast-in-Place Concrete Shear Walls", Qt::ToolTipRole);
    structType->setItemData( 6,"Steel Frame with Unreinforced Masonry Infill Walls", Qt::ToolTipRole);
    structType->setItemData( 7,"Concrete Moment Frame", Qt::ToolTipRole);
    structType->setItemData( 8,"Concrete Shear Walls", Qt::ToolTipRole);
    structType->setItemData( 9,"Concrete Frame with Unreinforced Masonry Infill Walls", Qt::ToolTipRole);
    structType->setItemData(10,"Precast Concrete Tilt-Up Walls", Qt::ToolTipRole);
    structType->setItemData(11,"Precast Concrete Frames with Concrete Shear Walls", Qt::ToolTipRole);
    structType->setItemData(12,"Reinforced Masonry Bearing Walls with Wood or Metal Deck Diaphragms", Qt::ToolTipRole);
    structType->setItemData(13,"Reinforced Masonry Bearing Walls with Precast Concrete Diaphragms", Qt::ToolTipRole);
    structType->setItemData(14,"Unreinforced Masonry Bearing Walls", Qt::ToolTipRole);
    structType->setItemData(15,"Mobile Homes and Manufactured Housing", Qt::ToolTipRole);
    structType->setItemData(16,"Undefined", Qt::ToolTipRole);

    designLevel = new QComboBox(this);
    designLevel->setToolTip(tr("Identifies the severity of demands considered during design. The available design levels are defined in the Hazus Earthquake Technical Manual."));
    designLevel->addItem("PC",0);
    designLevel->addItem("LC",1);
    designLevel->addItem("MC",2);
    designLevel->addItem("HC",3);
    designLevel->addItem("VC",4);
    designLevel->addItem("SC",5);
    designLevel->addItem("N/A",6);

    designLevel->setItemData(0, "Pre-Code\nApproximate Basis: UBC Seismic Zone 0, NEHRP Map Area 1.; Pre-1941 construction in all other UBC and NEHRP areas.\nPre-Code design represents older buildings that were not designed for earthquake load, regardless of where they are located in the United States.", Qt::ToolTipRole);
    designLevel->setItemData(1, "Low-Code\nApproximate Basis: 1941-1975 construction in UBC Seismic Zone 2B, NEHRP Map Area 5; Post-1941 construction in UBC Seismic Zone 2A, NEHRP Map Area 4; Post-1975 construction in UBC Seismic Zone 1, NEHRP Map Area 2/3", Qt::ToolTipRole);
    designLevel->setItemData(2, "Moderate-Code\nApproximate Basis: Post-1941 construction in UBC Seismic Zone 3, NEHRP Map Area 6; Post-1975 construction in UBC Seismic Zone 2B, NEHRP Map Area 5.", Qt::ToolTipRole);
    designLevel->setItemData(3, "High-Code\nApproximate Basis: Post-1975 construction in UBC Seismic Zone 4, NEHRP Map Area 7.", Qt::ToolTipRole);
    designLevel->setItemData(4, "Very High-Code\nApproximate Basis: Represents shaking and code strengths 1.5 times the High Code design level developed for the traditional Zone 4 hazard.", Qt::ToolTipRole);
    designLevel->setItemData(5, "Severe-Code\nApproximate Basis: Represents shaking and code strengths 2.0 times the High Code design level developed for the traditional Zone 4 hazard.", Qt::ToolTipRole);
    designLevel->setItemData(6,"Undefined", Qt::ToolTipRole);

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
   propertiesFormLayout->addRow(tr("Year Built"), yearBuilt);
   propertiesFormLayout->addRow(tr("# Stories"), storiesEdit);
   propertiesFormLayout->addRow(tr("Struct. Type"), structType);
   propertiesFormLayout->addRow(tr("Design Level"), designLevel);       
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

    connect(unitsLengthCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(unitLengthTextChanged()));
    connect(unitsForceCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(unitForceTextChanged()));

    connect(longitudeEdit, &QLineEdit::editingFinished, this, [this](){
        GeneralInformationWidget::buildingLocationChanged(latitudeEdit->text().toDouble(), longitudeEdit->text().toDouble());
    });

    connect(latitudeEdit, &QLineEdit::editingFinished, this, [this](){
        GeneralInformationWidget::buildingLocationChanged(latitudeEdit->text().toDouble(), longitudeEdit->text().toDouble());
    });
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
    jsonObj["stories"] = storiesEdit->text().toInt(); // keep both for now
    jsonObj["NumberOfStories"] = storiesEdit->text().toInt();
    jsonObj["width"] = widthEdit->text().toDouble();
    jsonObj["depth"] = depthEdit->text().toDouble();
    if (structType->currentText() != "N/A") {
        jsonObj["StructureType"] = structType->currentText();
    }
    if (designLevel->currentText() != "N/A") {
        jsonObj["DesignLevel"] = designLevel->currentText();
    }
    jsonObj["YearBuilt"] = yearBuilt->text().toInt();    
    

    // if the plan area is provided:
    double planAreaValue = planAreaEdit->text().toDouble();
    if (planAreaValue > 0.0) {
        jsonObj["PlanArea"] = planAreaValue;
        jsonObj["planArea"] = planAreaValue;
    }
    else {
        double widthValue = widthEdit->text().toDouble();
        double depthValue = depthEdit->text().toDouble();
        double rectAreaValue = widthValue * depthValue;
        // otherwise, assume a rectangular plan and use width x depth
        if (rectAreaValue > 0.0) {
            jsonObj["planArea"] = rectAreaValue;
            jsonObj["PlanArea"] = rectAreaValue;
        }
        else {
            // if none of the above are provided, use 0.0 for the area
            jsonObj["planArea"] = 0.0;
            jsonObj["PlanArea"] = 0.0;
        }
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

    return true;
}

bool
GeneralInformationWidget::inputFromJSON(QJsonObject &jsonObject){
  
    QJsonValue nameValue = jsonObject["name"];
    nameEdit->setText(nameValue.toString());

    if (!jsonObject["stories"].isUndefined()) {
        QJsonValue storiesValue = jsonObject["stories"];
        storiesEdit->setText(QString::number(storiesValue.toInt()));
    } else if (!jsonObject["NumberOfStories"].isUndefined()) {
        QJsonValue storiesValue = jsonObject["NumberOfStories"];
        storiesEdit->setText(QString::number(storiesValue.toInt()));
    }

    if (jsonObject.contains("StructureType")) {
        structType->setCurrentText(jsonObject["StructureType"].toString());
    } else {
        structType->setCurrentText("N/A");
    }
    if (jsonObject.contains("DesignLevel")) {
        designLevel->setCurrentText(jsonObject["DesignLevel"].toString());
    } else {
        designLevel->setCurrentText("N/A");
    }

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
        QJsonValue planAreaValue = jsonObject["PlanArea"];

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

    emit numStoriesOrHeightChanged(getNumFloors(), getHeight());
    double newW, newD, newA;
    this->getBuildingDimensions(newW, newD, newA);
    emit buildingDimensionsChanged(newW, newD, newA);
    
    return true;
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

void
GeneralInformationWidget::setLengthUnit(QString unitsLengthValue)
{
    LengthUnit lengthUnit = unitStringToEnum<LengthUnit>(unitsLengthValue);
    int lengthUnitIndex = unitsLengthCombo->findData(lengthUnit);
    unitsLengthCombo->setCurrentIndex(lengthUnitIndex);
}

QString
GeneralInformationWidget::getForceUnit()
{
   return unitEnumToString(unitsForceCombo->currentData().value<ForceUnit>());
}

void
GeneralInformationWidget::unitLengthTextChanged(void) {
    emit unitLengthChanged(getLengthUnit());
}

void
GeneralInformationWidget::unitForceTextChanged(void) {
    emit unitForceChanged(getForceUnit());
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
    depthEdit->setText(QString::number(newD));
    
    emit buildingDimensionsChanged(newB, newD, newA);
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

double
GeneralInformationWidget::getWidth(void) {
  return widthEdit->text().toDouble();
}

double
GeneralInformationWidget::getDepth(void) {
  return depthEdit->text().toDouble();
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

