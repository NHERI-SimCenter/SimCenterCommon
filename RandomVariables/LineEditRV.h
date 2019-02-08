#ifndef LINE_EDIT_RV_H
#define LINE_EDIT_RV_H

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

/**
 *  @author  fmckenna
 *  @date    2/2017
 *  @version 1.0
 *
 *  @section DESCRIPTION
 *
 *  This is an abstract interface providing interface SimCenter widgets must adhere to. At present limited to
 * providing methods to read and write from/to JSON objects and signals the class can use to invoke slot methods
 * in main window classes. This is to allow uniform handling of error messages in an application comprised of
 * different widgets.
 */

#include <QLineEdit>
class QJsonObject;
class RandomVariablesContainer;

class LineEditRV : public QLineEdit
{
    Q_OBJECT
public:
    explicit LineEditRV(RandomVariablesContainer *theRVC, QWidget *parent = 0);
    ~LineEditRV();
    /** 
     *   @brief outputToJSON method to write all objects data neeed to reconstruct object to JsonObject
     *   @param rvObject the JSON object to be written to
     *   @param  key for objects value
     *   @return bool - true for success, otherwise false
     */  

    bool outputToJSON(QJsonObject &rvObject, QString key);
    /** 
     *   @brief inputFromJSON method to instantiate itself from a JSON object
     *   @param rvObject the JSON object contaiing data to instantiate the object
     *   @param key the key for the object value
     *   @return bool - true for success, otherwise false
     */  
    bool inputFromJSON(QJsonObject &rvObject, QString key);

signals:

public slots:
   void on_editingFinished();

private:

  RandomVariablesContainer *theRVC;
  QString oldText;
};

#endif // LINEEDIT_RV_H
