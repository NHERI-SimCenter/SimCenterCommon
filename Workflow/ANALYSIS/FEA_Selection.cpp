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

#include "FEA_Selection.h"
#include <InputWidgetOpenSeesAnalysis.h>
#include <SimCenterAppMulti.h>
#include <CustomPySimulation.h>
#include <QCoreApplication>
#include <SurrogateSimulation.h>
#include <QCoreApplication>

FEA_Selection::FEA_Selection(bool inclMulti, QWidget *parent)
  : SimCenterAppSelection(QString("FE Application"), QString("Simulation"), parent)
{

  SimCenterAppWidget *opensees= new InputWidgetOpenSeesAnalysis();
  this->addComponent(QString("OpenSees"), QString("OpenSees-Simulation"), opensees);
  if (inclMulti == true) {
    SimCenterAppWidget *multi = new SimCenterAppMulti(QString("Simulation"), QString("MultiModel-Simulation"),this, this);
    this->addComponent(QString("Multiple Models"), QString("MultiModel"), multi);
  }  

  SimCenterAppWidget *custom_py_simulation= new CustomPySimulation();
  this->addComponent(QString("CustomPy-Simulation"), QString("CustomPy-Simulation"), custom_py_simulation);

  QString appName = QCoreApplication::applicationName();
  if ((appName == "EE-UQ" || appName =="WE-UQ")|| appName =="PBE") {
     SimCenterAppWidget *surrogate = new SurrogateSimulation();
      this->addComponent(QString("None (only for surrogate)"), QString("SurrogateSimulation"), surrogate);
  }

}

FEA_Selection::~FEA_Selection()
{

}


SimCenterAppWidget *
FEA_Selection::getClone()
{
  FEA_Selection *newSelection = new FEA_Selection(false);
  return newSelection;
}
