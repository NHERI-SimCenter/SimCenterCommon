// Written: fmckenna

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

#include "EDP_EarthquakeSelection.h"
#include "SurrogateEDP.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QStackedWidget>
#include <QComboBox>


#include <QPushButton>
#include <QJsonObject>
#include <QJsonArray>

#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <SectionTitle.h>

#include <StandardEarthquakeEDP.h>
#include <UserDefinedEDP.h>

EDP_EarthquakeSelection::EDP_EarthquakeSelection(QWidget *parent)
: SimCenterAppWidget(parent), theCurrentEDP(0)
{
  QVBoxLayout *layout = new QVBoxLayout();

  //
  // the selection part
  //

  QHBoxLayout *theSelectionLayout = new QHBoxLayout();
  //QLabel *label = new QLabel();
  SectionTitle *label = new SectionTitle();
  label->setMinimumWidth(250);
  label->setText(QString("Engineering Demand Paramater Generator"));
  edpSelection = new QComboBox();
  edpSelection->addItem(tr("Standard Earthquake"));
  edpSelection->addItem(tr("User Defined"));
  edpSelection->addItem(tr("None (only for surrogate)"));
  edpSelection->setObjectName("EDPSelectionComboBox");

  edpSelection->setItemData(1, "A Seismic event using Seismic Hazard Analysis and Record Selection/Scaling", Qt::ToolTipRole);

  theSelectionLayout->addWidget(label);
  QSpacerItem *spacer = new QSpacerItem(50,10);
  theSelectionLayout->addItem(spacer);
  theSelectionLayout->addWidget(edpSelection,1);
  theSelectionLayout->addStretch(1);
  layout->addLayout(theSelectionLayout);

  //
  // create the stacked widget
  //
  theStackedWidget = new QStackedWidget();

  //
  // create the individual widgets add to stacked widget
  //

  theStandardEarthquakeEDPs = new StandardEarthquakeEDP();
  theStackedWidget->addWidget(theStandardEarthquakeEDPs);

  theUserDefinedEDPs = new UserDefinedEDP();
  theStackedWidget->addWidget(theUserDefinedEDPs);


  SurrogateEDP * theSurrogateEDPs_tmp = SurrogateEDP::getInstance();
  theSurrogateEDPs = theSurrogateEDPs_tmp;

  connect(theSurrogateEDPs_tmp, &SurrogateEDP::surrogateSelected, [=](){
     edpSelection->setCurrentIndex(2);
  });
  theStackedWidget->addWidget(theSurrogateEDPs);


  layout->addWidget(theStackedWidget);
  this->setLayout(layout);
  theCurrentEDP=theStandardEarthquakeEDPs;
  //layout->setMargin(0);

  connect(edpSelection, SIGNAL(currentIndexChanged(int)), this,
	  SLOT(edpSelectionChanged(int)));
}

EDP_EarthquakeSelection::~EDP_EarthquakeSelection()
{

}


bool
EDP_EarthquakeSelection::outputToJSON(QJsonObject &jsonObject)
{
  theCurrentEDP->outputToJSON(jsonObject);
  return true;
}


bool
EDP_EarthquakeSelection::inputFromJSON(QJsonObject &jsonObject) {

  if (theCurrentEDP != 0) {
    return theCurrentEDP->inputFromJSON(jsonObject);
  } else {
      errorMessage("EDP_SElection no current EDP selected");
  }

  return false;
}

void EDP_EarthquakeSelection::edpSelectionChanged(int slot)
{
  //
  // switch stacked widgets depending on text
  // note type output in json and name in pull down are not the same and hence the ||
  //

    //
    // switch stacked widgets depending on text
    // note type output in json and name in pull down are not the same and hence the ||
    //

    if (slot == 0) {
        theStackedWidget->setCurrentIndex(0);
        theCurrentEDP = theStandardEarthquakeEDPs;
    }

    else if (slot == 1) {
        theStackedWidget->setCurrentIndex(1);
        theCurrentEDP = theUserDefinedEDPs;
    }
    else if (slot == 2) {
        theStackedWidget->setCurrentIndex(2);
        theCurrentEDP = theSurrogateEDPs;
    qDebug() << "EDP_Selection::Changed tp Auto Defined";
    }

    else {
        qDebug() << "ERROR .. EDP_Selection selection .. unknown slot used: " << slot;
    }
}

bool
EDP_EarthquakeSelection::outputAppDataToJSON(QJsonObject &jsonObject)
{
  theCurrentEDP->outputAppDataToJSON(jsonObject);
  return true;
}


bool
EDP_EarthquakeSelection::inputAppDataFromJSON(QJsonObject &jsonObject)
{

  // get name from "Application" key

  QString type;
  if (jsonObject.contains("Application")) {
    QJsonValue theName = jsonObject["Application"];
    type = theName.toString();
  } else {
    errorMessage("EDP_EarthquakeSelection - no Application key found");
    return false;
  }

  // based on application name value set edp type
  int index = 0;
  if ((type == QString("Standard Earthquake EDPs")) ||
      (type == QString("StandardEarthquakeEDP"))) {
    index = 0;
  } else if ((type == QString("UserDefinedEDP")) ||
	     (type == QString("User Defined EDPs"))) {
    index = 1;
  } else if ((type == QString("None (only for surrogate)")) ||
             (type == QString("SurrogateSimulation"))) {
    index = 2;
  } else {
    errorMessage("EDP_EarthquakeSelection - no valid type found");
    return false;
  }

  edpSelection->setCurrentIndex(index);

  // invoke inputAppDataFromJSON on new type

  if (theCurrentEDP != 0) {
    return theCurrentEDP->inputAppDataFromJSON(jsonObject);
  }

  return true;
}

bool
EDP_EarthquakeSelection::copyFiles(QString &destDir) {

  if (theCurrentEDP != 0) {
    return  theCurrentEDP->copyFiles(destDir);
  }

  return false;
}

void
EDP_EarthquakeSelection::clear(void) {
  if (theCurrentEDP != 0) {
    theCurrentEDP->clear();
  }
}
