#ifndef SIM_CENTER_FILE_INPUT_H
#define SIM_CENTER_FILE_INPUT_H

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

// Written: Michael Gardner

#include <QLabel>
#include <QLineEdit>
#include <QJsonObject>
#include <SimCenterWidget.h>

class SimCenterFileInput : public SimCenterWidget {
  Q_OBJECT

public:
  /**
   * @constructor Create new SimCenterFileInput widget
   * @param[in] inputObject Input JSON object
   * @param[in] parent Parent widget. Defaults to null pointer
   */
  SimCenterFileInput(const QJsonValue& inputObject, QWidget *parent = nullptr);

  /**
   * @destructor Default destructor
   */
  virtual ~SimCenterFileInput(){};

  /**
   * Set input file based on user selection
   */
  void chooseInputFile();
  
  /**
   * Instantiate the file input from from input JSON object
   * @param[in] jsonObject JSON object containing input information
   * @return Returns true if successful, false otherwise
   */
  bool inputFromJSON(QJsonObject &jsonObject) override;

  /**
   * Write all current class data to JSON required to reconstruct class
   * @param[in, out] jsonObject JSON object to write output to
   * @return Returns true if successful, false otherwise
   */
  bool outputToJSON(QJsonObject &jsonObject) override;

protected:
  QLineEdit *theFileLineEdit;
  QLabel *theFileLabel;
};

#endif // SIM_CENTER_FILE_INPUT_H
