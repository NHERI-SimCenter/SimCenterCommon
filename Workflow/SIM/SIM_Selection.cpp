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

#include "SIM_Selection.h"

#include <OpenSeesBuildingModel.h>
#include <MDOF_BuildingModel.h>
#include <SteelBuildingModel.h>
#include <SimCenterAppMulti.h>
#include <RandomVariablesContainer.h>
#include <ConcreteBuildingModel.h>
#include <MDOF_LU.h>
#include <CustomPy.h>
#include <SurrogateGP.h>


#include <QApplication>

SIM_Selection::SIM_Selection(bool includeC,
			     bool doMulti,
                             QWidget *parent)
  : SimCenterAppSelection(QString("Building Model Generator"), QString("Modeling"), QString("StructuralInformation"), QString(), parent),
    includeCentroid(includeC)
{
  
  RandomVariablesContainer *theRVs = RandomVariablesContainer::getInstance();
  SimCenterAppWidget *opensees = new OpenSeesBuildingModel(includeCentroid);
  SimCenterAppWidget *mdof = new MDOF_BuildingModel();
  this->addComponent(QString("MDOF"), QString("MDOF_BuildingModel"), mdof);    
  this->addComponent(QString("OpenSees"), QString("OpenSeesInput"), opensees);
  QString appName = QCoreApplication::applicationName();

  if (appName == "PBE" || appName == "EE-UQ") {
    SimCenterAppWidget *autosda = new SteelBuildingModel(theRVs);
    SimCenterAppWidget *concrete = new ConcreteBuildingModel(theRVs);    
    this->addComponent(QString("Steel Building Model"), QString("SteelBuildingModel"), autosda);
    this->addComponent(QString("Concrete Building Model"), QString("ConcreteBuildingModel"), concrete);
  }

  if ((appName == "EE-UQ" || appName =="WE-UQ") || (appName == "PBE"))  {
    SimCenterAppWidget *sur = new surrogateGP(theRVs);
    this->addComponent(QString("Surrogate (GP)"), QString("SurrogateGPBuildingModel"), sur);
  }


  if (appName == "HydroUQ" || appName == "Hydro-UQ") {
    SimCenterAppWidget *autosda = new SteelBuildingModel(theRVs);
    SimCenterAppWidget *concrete = new ConcreteBuildingModel(theRVs);    
    SimCenterAppWidget *sur = new surrogateGP(theRVs);
    this->addComponent(QString("Steel Building Model"), QString("SteelBuildingModel"), autosda);
    this->addComponent(QString("Concrete Building Model"), QString("ConcreteBuildingModel"), concrete);
    this->addComponent(QString("Surrogate (GP)"), QString("SurrogateGPBuildingModel"), sur);
  }



  SimCenterAppWidget *mdof_lu = new MDOF_LU();
  this->addComponent(QString("MDOF-LU"), QString("MDOF-LU"), mdof_lu);
    
  if (doMulti == true) {
    SimCenterAppWidget *multi = new SimCenterAppMulti(QString("Modeling"), QString("MultiModel-Modeling"),this, this);
    this->addComponent(QString("Multiple Models"), QString("MultiModel"), multi);
  }    

  // KZ: adding CustomPy
  SimCenterAppWidget *custom_py = new CustomPy();
  this->addComponent(QString("CustomPy"), QString("CustomPyInput"), custom_py);
}

SIM_Selection::~SIM_Selection()
{

}


SimCenterAppWidget *
SIM_Selection::getClone()
{
  SIM_Selection *newSelection = new SIM_Selection(includeCentroid, false);
  return newSelection;
}

QString
SIM_Selection::getCurrentSIM()
{
    return this->getCurrentSelectionName();
}



bool
SIM_Selection::outputCitation(QJsonObject &jsonObject) {
  QJsonObject appSpecificCitation;
  auto currentApp = this->getCurrentSelection();
  currentApp->outputCitation(appSpecificCitation);
  if (!appSpecificCitation.isEmpty()) {
    jsonObject.insert(this->getCurrentSelectionName(), appSpecificCitation);
  }
  return true;
}
