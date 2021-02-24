#ifndef JSON_CONFIGURED_WIDGET_H
#define JSON_CONFIGURED_WIDGET_H

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

#include <QComboBox>
#include <QLineEdit>
#include <QString>
#include <QWidget>
#include <RandomVariablesContainer.h>
#include <SimCenterWidget.h>

#include "JsonWidgetEnums.h"
#include "SimCenterComboBox.h"
#include "SimCenterDoubleSpinBox.h"
#include "SimCenterLineEdit.h"
#include "SimCenterRVLineEdit.h"
#include "SimCenterFileInput.h"
#include "SimCenterSpinBox.h"

class JsonConfiguredWidget : public SimCenterWidget {
  Q_OBJECT

public:
  /**
   * @constructor Create new JsonConfiguredWidget
   * @param[in, out] random_variables Widget to store random variables to
   * @param[in] configFile String specifying config file location
   * @param[in] parent Parent widget. Defaults to null pointer
   */
  JsonConfiguredWidget(RandomVariablesContainer *random_variables,
                       QWidget *parent = nullptr);

  /**
   * @destructor Default destructor
   */
  virtual ~JsonConfiguredWidget(){};

  /**
   * Instantiate widget generator from input JSON object
   * @param[in] rvObject JSON object containing input information
   * @return Returns true if successful, false otherwise
   */
  bool inputFromJSON(QJsonObject &rvObject) override;

  /**
   * Write all current class data to JSON required to reconstruct class
   * @param[in, out] rvObject JSON object to write output to
   * @return Returns true if successful, false otherwise
   */
  bool outputToJSON(QJsonObject &rvObject) override;

  /**
   * Set config file based on user selection
   */
  void chooseConfigFile();

  /**
   * Get enum based on input string
   * @param[in] inputString String that should be converted to enum
   * @return Enum corresponding to input string
   */
  JsonWidget::Type getEnumIndex(const QString& inputString) const;

protected:
  /**
   * Initialize the widget based on configuration specified in input config file
   * @param[in] configFile JSON configuration file specifying widget layout
   */
  virtual void initialize(const QString &configFile);

  /**
   * Generate widget based on input JSON object
   * @param[in] inputArray JSON array describing widget to be generated
   * @param[in] set Flag indicating whether to set values of widgets 
   * @return Pointer to generated widget
   */
  QWidget *generateWidgetLayout(const QJsonArray &inputArray, bool set = false) const;

  /**
   * Generate combo box based on input JSON object
   * @param[in] inputObject JSON value describing combo box to be generated
   * @return Pointer to generated combo box
   */
  SimCenterWidget *generateComboBox(const QJsonValue &inputValue) const;

  /**
   * Generate random variable line edit based on input JSON value
   * @param[in] inputValue JSON value describing random variable line edit to
   * be generated
   * @return Pointer to generated random variable line edit
   */
  SimCenterWidget *generateRVLineEdit(const QJsonValue &inputValue) const;

  /**
   * Generate line edit for opening input files based on input JSON value
   * @param[in] inputValue JSON value describing file line edit to be
   * generated
   * @return Pointer to generated file line edit
   */
  SimCenterWidget *generateFileInput(const QJsonValue &inputValue) const;

  /**
   * Generate line edit based on input JSON value
   * @param[in] inputValue JSON value describing line edit to be generated
   * @return Pointer to generated line edit
   */
  SimCenterWidget *generateLineEdit(const QJsonValue &inputValue) const;

  /**
   * Generate double spin box based on input JSON value
   * @param[in] inputValue JSON value describing double spin box to be generated
   * @return Pointer to generated double spin box
   */
  SimCenterWidget *generateDoubleSpinBox(const QJsonValue &inputValue) const;

  /**
   * Generate spin box based on input JSON value
   * @param[in] inputValue JSON value describing spin box to be generated
   * @return Pointer to generated spin box
   */
  SimCenterWidget *generateSpinBox(const QJsonValue &inputValue) const;

  /**
   * Write the input widget to JSON. Assumes that widget
   * contains only other widgets or SimCenter widgets
   * @param[in] inputWidget Input widget to write to JSON object
   * @return JSON object of widget
   */
  QJsonArray widgetToJson(QWidget *inputWidget);

  QLineEdit *theConfigFile; /**< Path to file specifying widget configuration */
  RandomVariablesContainer *theRVInputWidget; /**< Widget for inputting random
                                                variables */
  QWidget *theWidget; /**< Stacked widget containing inputs
			 specified to JSON config file */
};

#endif // JSON_CONFIGURED_WIDGET_H
