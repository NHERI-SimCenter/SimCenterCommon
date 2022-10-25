#ifndef UQ_ENGINE_SELECTION_H
#define UQ_ENGINE_SELECTION_H

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

#include <SimCenterAppSelection.h>
#include "UQ_Engine.h"
class InputWidgetEDP;

class UQ_EngineSelection : public  SimCenterAppSelection
{
  Q_OBJECT

  public:

  UQ_EngineSelection(UQ_EngineType = All,
		     QWidget *parent = 0);
  
  UQ_EngineSelection(bool includeNone,
		     QString assetType,
		     UQ_EngineType = All,
		     QWidget *parent = 0);    
  
  UQ_Results  *getResults();
  int getNumParallelTasks(void);
  UQ_Engine *getCurrentEngine();
  void setRV_Defaults();
  
 signals:

  void onUQ_EngineChanged(bool);
  void onNumModelsChanged(int);
  void queryEVT(void); // added KZ

 public slots:

  void engineSelectionChanged(const QString &);

  void relayQueryEVT(void); // added KZ
  void setEventType(QString type); // added KZ
  
private:
  void initialize(bool includeNoneOption, UQ_EngineType type);
  UQ_Engine *theCurrentEngine;
  UQ_Engine *thePreviousEngine;  
  UQ_Engine *theDakotaEngine;
  UQ_Engine *theSimCenterUQEngine;
  UQ_Engine *theUQpyEngine;
  UQ_Engine *theUCSD_Engine;
  UQ_Engine *thefilterEngine;
  UQ_Engine *theCustomEngine;
};

#endif 
