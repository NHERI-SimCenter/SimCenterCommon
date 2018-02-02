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
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QList>

#include <QLabel>
#include <QLineEdit>

GeneralInformationWidget::GeneralInformationWidget(QWidget *parent) : SimCenterWidget(parent)
{
    QLabel *nameLabel = new QLabel(this);
    nameLabel->setText("Name");
    nameEdit = new QLineEdit;

    QLabel *revLabel = new QLabel(this);
    revLabel->setText("Revision");
    revEdit = new QLineEdit;

    QLabel *typeLabel = new QLabel(this);
    typeLabel->setText("Type");
    typeEdit = new QLineEdit;

    QLabel *yearLabel = new QLabel(this);
    yearLabel->setText("Year");
    yearEdit = new QLineEdit;

    QLabel *storiesLabel = new QLabel(this);
    storiesLabel->setText("Stories");
    storiesEdit = new QLineEdit;

    QLabel *heightLabel = new QLabel(this);
    heightLabel->setText("Height");
    heightEdit = new QLineEdit;

    QLabel *locationLabel = new QLabel(this);
    locationLabel->setText("Location");
    locationLabel->setStyleSheet("font-weight: bold; font-size: 14px ");
    QLabel *emptyLabel = new QLabel(this);
    emptyLabel->setText("");

    QLabel *locationNameLabel = new QLabel(this);
    locationNameLabel->setText("Name");
    locationNameEdit = new QLineEdit;

    QLabel *locationLatLabel = new QLabel(this);
    locationLatLabel->setText("Latitude");
    locationLatEdit = new QLineEdit;

    QLabel *locationLonLabel = new QLabel(this);
    locationLonLabel->setText("Longitude");
    locationLonEdit = new QLineEdit;

    // Units
    QLabel *unitsLabel = new QLabel(this);
    unitsLabel->setStyleSheet("font-weight: bold; font-size: 14px ");
    unitsLabel->setText("Units");

    QLabel *unitsForceLabel = new QLabel(this);
    unitsForceLabel->setText("Force");
    unitsForceEdit = new QLineEdit;

    QLabel *unitsLengthLabel = new QLabel(this);
    unitsLengthLabel->setText("Length");
    unitsLengthEdit = new QLineEdit;

    QLabel *unitsTemperatureLabel = new QLabel(this);
    unitsTemperatureLabel->setText("Temperature");
    unitsTemperatureEdit = new QLineEdit;

    QLabel *unitsTimeLabel = new QLabel(this);
    unitsTimeLabel->setText("Time");
    unitsTimeEdit = new QLineEdit;

    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);

    QGridLayout *leftLayout = new QGridLayout;
    leftLayout->addWidget(nameLabel, 0, 0);
    leftLayout->addWidget(revLabel, 1, 0);
    leftLayout->addWidget(typeLabel, 2, 0);
    leftLayout->addWidget(yearLabel, 3, 0);
    leftLayout->addWidget(storiesLabel, 4, 0);
    leftLayout->addWidget(heightLabel, 5, 0);
    leftLayout->addWidget(locationLabel, 6, 0, 1, 2);
    leftLayout->addWidget(locationNameLabel, 7, 0);
    leftLayout->addWidget(locationLatLabel, 8, 0);
    leftLayout->addWidget(locationLonLabel, 9, 0);
    leftLayout->addWidget(unitsLabel, 10, 0, 1, 2);
    leftLayout->addWidget(unitsForceLabel, 11, 0);
    leftLayout->addWidget(unitsLengthLabel, 12, 0);
    leftLayout->addWidget(unitsTemperatureLabel, 13, 0);
    leftLayout->addWidget(unitsTimeLabel, 14, 0);

    QGridLayout *rightLayout = new QGridLayout;
    rightLayout->addWidget(nameEdit, 0, 1);
    rightLayout->addWidget(revEdit, 1, 1);
    rightLayout->addWidget(typeEdit, 2, 1);
    rightLayout->addWidget(yearEdit, 3, 1);
    rightLayout->addWidget(storiesEdit, 4, 1);
    rightLayout->addWidget(heightEdit, 5, 1);
    rightLayout->addWidget(emptyLabel, 6, 1);
    rightLayout->addWidget(locationNameEdit, 7, 1);
    rightLayout->addWidget(locationLatEdit, 8, 1);
    rightLayout->addWidget(locationLonEdit, 9, 1);
    rightLayout->addWidget(emptyLabel, 10, 1);
    rightLayout->addWidget(unitsForceEdit, 11, 1);
    rightLayout->addWidget(unitsLengthEdit, 12, 1);
    rightLayout->addWidget(unitsTemperatureEdit, 13, 1);
    rightLayout->addWidget(unitsTimeEdit, 14, 1);

    theLayout->addLayout(leftLayout);
    theLayout->addLayout(rightLayout);
    setLayout(theLayout);
    setWindowTitle(tr("General Information"));

    this->setMinimumWidth(500);
}

GeneralInformationWidget::~GeneralInformationWidget()
{

}

void
GeneralInformationWidget::outputToJSON(QJsonObject &jsonObj){

    jsonObj["name"] = nameEdit->text().trimmed();

    QString revVal = revEdit->text();
    jsonObj["revision"] = revVal.toDouble();

    jsonObj["type"] = typeEdit->text().trimmed();

    QString intVal = yearEdit->text();
    jsonObj["year"] = intVal.toInt();

    intVal = storiesEdit->text();
    jsonObj["stories"] = intVal.toInt();

    intVal = heightEdit->text();
    jsonObj["height"] = intVal.toInt();


    QJsonObject location;
    location["name"] = locationNameEdit->text().trimmed();

    QString dblVal = locationLatEdit->text();
    location["latitude"] = dblVal.toDouble();

    dblVal = locationLonEdit->text();
    location["longitude"] = dblVal.toDouble();

    jsonObj["location"] = location;

    QJsonObject units;
    units["force"] = unitsForceEdit->text().trimmed();
    units["length"] = unitsLengthEdit->text().trimmed();
    units["time"] = unitsTimeEdit->text().trimmed();
    units["temperature"] = unitsTemperatureEdit->text().trimmed();

    jsonObj["units"] = units;

}

void
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
    yearEdit->setText( QString::number(yearValue.toInt()) );

    QJsonValue storiesValue = jsonObject["stories"];
    storiesEdit->setText(  QString::number(storiesValue.toInt()));

    QJsonValue heightValue = jsonObject["height"];
    heightEdit->setText(  QString::number(heightValue.toInt()) );

    // Location Object
    QJsonValue locationValue = jsonObject["location"];
    QJsonObject locationObj = locationValue.toObject();

    QJsonValue locationNameValue = locationObj["name"];
    locationNameEdit->setText(locationNameValue.toString());

    QJsonValue locationLatitudeValue = locationObj["latitude"];
    locationLatEdit->setText( QString::number(locationLatitudeValue.toDouble()) );

    QJsonValue locationLongitudeValue = locationObj["longitude"];
    locationLonEdit->setText( QString::number(locationLongitudeValue.toDouble()) );


    // Units Object
    QJsonValue unitsValue = jsonObject["units"];
    QJsonObject unitsObj = unitsValue.toObject();

    QJsonValue unitsForceValue = unitsObj["force"];
    unitsForceEdit->setText(unitsForceValue.toString());

    QJsonValue unitsLengthValue = unitsObj["length"];
    unitsLengthEdit->setText(unitsLengthValue.toString());

    QJsonValue unitsTimeValue = unitsObj["time"];
    unitsTimeEdit->setText(unitsTimeValue.toString());

    QJsonValue unitsTempValue = unitsObj["temperature"];
    unitsTemperatureEdit->setText(unitsTempValue.toString());

}

void
GeneralInformationWidget::clear(void)
{
    nameEdit->clear();
    revEdit->clear();
    typeEdit->clear();
    yearEdit->clear();
    storiesEdit->clear();
    heightEdit->clear();

    locationNameEdit->clear();
    locationLatEdit->clear();
    locationLonEdit->clear();

    unitsForceEdit->clear();
    unitsLengthEdit->clear();
    unitsTemperatureEdit->clear();
    unitsTimeEdit->clear();

}
