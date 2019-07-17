#ifndef SIMCENTER_APP_WIDGET_H
#define SIMCENTER_APP_WIDGET_H

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
 *  @date    09/2018
 *  @version 1.0
 *
 *  @section DESCRIPTION
 *
 * The purpose of this class is to define interface for SimCenter widgets that are associated with an application
 * in the Workflow applications. They introduce methods for wrating the application specific data, e.g. AppName, data
 * that are used in workflow applicaions.
 */

#include <SimCenterWidget.h>
class QJsonObject;

class SimCenterAppWidget : public SimCenterWidget
{
    Q_OBJECT
public:
    explicit SimCenterAppWidget(QWidget *parent = 0);
    virtual ~SimCenterAppWidget();
    /** 
     *   @brief outputAppDataToJSON method to write the application data to json object.
     *   @param rvObject the JSON object to be written to
     *   @return bool - true for success, otherwise false
     */  

    virtual bool outputAppDataToJSON(QJsonObject &jsonObject);
    /** 
     *   @brief inputFromJSON method to read applications specific data from a JSON object
     *   @param rvObject the JSON object contaiing data to instantiate the object
     *   @return bool - true for success, otherwise false
     */  
    virtual bool inputAppDataFromJSON(QJsonObject &jsonObject);

    /**
     *   @brief copyFiles method invoked to copy all files aapplication will need to run directory
     *   @param destDir the directory to put files in
     *   @return bool - true for success, otherwise false
     */
    virtual bool copyFiles(QString &destDir);

    /**
     *   @brief returns a boolean indicating whether or not this app can run locally
     *   @return bool - true means the app can run locally, otherwise false
     */
    virtual bool supportsLocalRun();

    static bool copyPath(QString sourceDir, QString destinationDir, bool overWriteDirectory);
    static bool copyFile(QString filename, QString destinationDir);

signals:

public slots:

private:

};

#endif // SIMCENTER_APP_WIDGET_H
