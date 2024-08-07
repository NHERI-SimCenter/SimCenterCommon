#ifndef SIMCENTER_PREFERENCES_H
#define SIMCENTER_PREFERENCES_H

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

// Written: fmckenna

#include <QDialog>
#include <QCheckBox>

class QLineEdit;
class QVBoxLayout;

class SimCenterPreferences : public QDialog
{
    Q_OBJECT

private:
    explicit SimCenterPreferences(QWidget *parent = 0);
    ~SimCenterPreferences();
    static SimCenterPreferences *theInstance;

public:
    static SimCenterPreferences *getInstance(QWidget *parent = 0);
    QString getPython(void); 
    QString getDakota(void);
    QString getOpenSees(void);   
    QString getAppDir(void);
    QString getRemoteAppDir(void);
    QString getRemoteAgaveApp(void);
    QString getRemoteAgaveAppVersion(void);  
    QString getLocalWorkDir(void);
    QString getRemoteWorkDir(void);
    QString getDefaultAllocation(void);				  

public slots:
    void savePreferences(bool);
    void resetPreferences(bool);
    void quitPreferences(bool);
    void loadPreferences(void);

private:
    QString getDefaultAgaveApp(void);
    QString getDefaultAgaveAppVersion(void);  
    QString getDefaultRemoteAppDir(void);  
    QString getDefaultOpenSees(void);
    QString getDefaultDakota(void);
    QString getDefaultPython(void);

    QLineEdit *python;
    QLineEdit *opensees;
    QLineEdit *dakota;
    QLineEdit *localWorkDir;
    QLineEdit *remoteWorkDir;
    QLineEdit *appDir;
    QLineEdit *remoteBackendDir;
  
    QLineEdit *remoteTapisApp;
    QLineEdit *remoteTapisAppVersion;
    QLineEdit *allocation;    

    QCheckBox* customPythonCheckBox;  
    QCheckBox* customOpenSeesCheckBox;
    QCheckBox* customDakotaCheckBox;
    QCheckBox* customAppDirCheckBox;
    QCheckBox* customTapisAppCheckBox;
    QCheckBox* customTapisAppVersionCheckBox;  
    QCheckBox* customRemoteAppDirCheckBox;  
};


#endif // SIMCENTER_PREFERENCES_H
