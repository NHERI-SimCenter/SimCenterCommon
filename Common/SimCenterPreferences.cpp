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


#include "SimCenterPreferences.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QCoreApplication>
#include <QFileInfo>

#include <QGridLayout>

SimCenterPreferences *
SimCenterPreferences::getInstance(QWidget *parent) {
  if (theInstance == 0)
    theInstance = new SimCenterPreferences(parent);

  return theInstance;
}

SimCenterPreferences *SimCenterPreferences::theInstance = 0;

SimCenterPreferences::SimCenterPreferences(QWidget *parent) 
    : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout();

    // create QGroup Boxes for sorting the preferences into groups
    QGroupBox* externalApplicationsBox = new QGroupBox("External Applications", this);
    QFormLayout* externalApplicationsLayout = new QFormLayout(externalApplicationsBox);

    QGroupBox* locationDirectoriesBox = new QGroupBox("Local Directories", this);
    QFormLayout* locationDirectoriesLayout = new QFormLayout(locationDirectoriesBox);

    QGroupBox* remoteSettingsBox = new QGroupBox("Remote Application Settings", this);
    QFormLayout* remoteSettingsLayout = new QFormLayout(remoteSettingsBox);

    //
    // add row for python interpreter
    //

    python = new QLineEdit();
    QHBoxLayout *pythonLayout = new QHBoxLayout();
    pythonLayout->addWidget(python);
    QPushButton *pythonButton = new QPushButton();
    pythonButton->setText("Browse");
    pythonButton->setToolTip(tr("Select your Python interpreter"));
    pythonLayout->addWidget(pythonButton);

    externalApplicationsLayout->addRow(tr("Python:"), pythonLayout);
    externalApplicationsLayout->setAlignment(Qt::AlignLeft);
    externalApplicationsLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    externalApplicationsLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    // connect the pushbutton with code to open file selection and update python preferences with selected file
    connect(pythonButton, &QPushButton::clicked, this, [this](){
        QSettings settings("SimCenter", "Common"); //These names will need to be constants to be shared
        QVariant  pythonPathVariant = settings.value("pythonExePath");
        QString existingDir = QCoreApplication::applicationDirPath();
        if (pythonPathVariant.isValid()) {
            QString existingF = pythonPathVariant.toString();
            QFileInfo existingFile(existingF);
	    if (existingFile.exists())
	      existingDir = existingFile.absolutePath();
        }

        QString selectedFile = QFileDialog::getOpenFileName(this,
                                                            tr("Select Python Interpreter"),
                                                            existingDir,
                                                            "All files (*.*)");

        if(!selectedFile.isEmpty()) {
            python->setText(selectedFile);
        }
    }
    );


    // opensees
    opensees = new QLineEdit();
    QHBoxLayout *openseesLayout = new QHBoxLayout();
    customOpenSeesCheckBox = new QCheckBox("Custom:");
    openseesLayout->addWidget(customOpenSeesCheckBox);
    openseesLayout->addWidget(opensees);
    QPushButton *openseesButton = new QPushButton();
    openseesButton->setText("Browse");
    openseesButton->setToolTip(tr("Select your OpenSees application"));
    openseesLayout->addWidget(openseesButton);
    customOpenSeesCheckBox->setChecked(false);
    opensees->setEnabled(false);
    openseesButton->setEnabled(false);

    externalApplicationsLayout->addRow(tr("OpenSees:"), openseesLayout);
    externalApplicationsLayout->setAlignment(Qt::AlignLeft);
    externalApplicationsLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    externalApplicationsLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    // connect the pushbutton with code to open file selection and update opensees preferences with selected file
    connect(openseesButton, &QPushButton::clicked, this, [this](){
        QSettings settings("SimCenter", QCoreApplication::applicationName()); 
        QVariant  openseesPathVariant = settings.value("openseesExePath");
        QString existingDir = QCoreApplication::applicationDirPath();
        if (openseesPathVariant.isValid()) {
            QString existingF = openseesPathVariant.toString();
            QFileInfo existingFile(existingF);
	    if (existingFile.exists())
	      existingDir = existingFile.absolutePath();
        }

        QString selectedFile = QFileDialog::getOpenFileName(this,
                                                            tr("Select Opensees Interpreter"),
                                                            existingDir,
                                                            "All files (*)");

        if(!selectedFile.isEmpty()) {
            opensees->setText(selectedFile);
        }
    }
    );

    connect(customOpenSeesCheckBox, &QCheckBox::toggled, this, [this, openseesButton](bool checked)
    {
        opensees->setEnabled(checked);
        openseesButton->setEnabled(checked);
        openseesButton->setFlat(!checked);
        if (checked == false) opensees->setText(this->getDefaultOpenSees());
    });
    // opensees

    // dakota
    dakota = new QLineEdit();
    QHBoxLayout *dakotaLayout = new QHBoxLayout();
    customDakotaCheckBox = new QCheckBox("Custom");
    dakotaLayout->addWidget(customDakotaCheckBox);
    dakotaLayout->addWidget(dakota);
    QPushButton *dakotaButton = new QPushButton();
    dakotaButton->setText("Browse");
    dakotaButton->setToolTip(tr("Select your Dakota application"));
    dakotaLayout->addWidget(dakotaButton);
    customDakotaCheckBox->setChecked(false);
    dakota->setEnabled(false);
    dakotaButton->setEnabled(false);
    externalApplicationsLayout->addRow(tr("Dakota:"), dakotaLayout);
    externalApplicationsLayout->setAlignment(Qt::AlignLeft);
    externalApplicationsLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    externalApplicationsLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    // connect the pushbutton with code to open file selection and update dakota preferences with selected file
    connect(dakotaButton, &QPushButton::clicked, this, [this](){
        QSettings settings("SimCenter", QCoreApplication::applicationName()); 
        QVariant  dakotaPathVariant = settings.value("dakotaExePath");
        QString existingDir = QCoreApplication::applicationDirPath();
        if (dakotaPathVariant.isValid()) {
            QString existingF = dakotaPathVariant.toString();
            QFileInfo existingFile(existingF);
	    if (existingFile.exists())
	      existingDir = existingFile.absolutePath();
        }

        QString selectedFile = QFileDialog::getOpenFileName(this,
                                                            tr("Select Dakota Interpreter"),
                                                            existingDir,
                                                            "All files (*)");

        if(!selectedFile.isEmpty()) {
            dakota->setText(selectedFile);
        }
    }
    );

    connect(customDakotaCheckBox, &QCheckBox::toggled, this, [this, dakotaButton](bool checked)
    {
        dakota->setEnabled(checked);
        dakotaButton->setEnabled(checked);
        dakotaButton->setFlat(!checked);
        if (checked == false) dakota->setText(this->getDefaultDakota());
    });
    // dakota


    //
    // entry for localWorkDir location .. basically as before
    //

    localWorkDir = new QLineEdit();
    QHBoxLayout *localWorkDirLayout = new QHBoxLayout();
    localWorkDirLayout->addWidget(localWorkDir);
    
    QPushButton *localWorkDirButton = new QPushButton();
    localWorkDirButton->setText("Browse");
    localWorkDirButton->setToolTip(tr("Select Work directory where local jobs will run"));
    localWorkDirLayout->addWidget(localWorkDirButton);

    locationDirectoriesLayout->addRow(tr("Local Jobs Directory:"), localWorkDirLayout);
    locationDirectoriesLayout->setAlignment(Qt::AlignLeft);
    locationDirectoriesLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    locationDirectoriesLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    connect(localWorkDirButton, &QPushButton::clicked, this, [this](){
        QSettings settings("SimCenter", QCoreApplication::applicationName()); 
        QVariant  localWorkDirPathVariant = settings.value("localWorkDir");
        QString   existingDir = QCoreApplication::applicationDirPath();
        if (localWorkDirPathVariant.isValid()) {
            QString existingDString = localWorkDirPathVariant.toString();
            QDir existingD(existingDString);
	    if (existingD.exists())
	      existingDir = existingD.absolutePath();
        }

        QString selectedDir = QFileDialog::getExistingDirectory(this,
                                                                tr("Select Local Directory where local job data is staged, directory remains after job is run"),
                                                                 existingDir,
                                                                QFileDialog::ShowDirsOnly);
        if(!selectedDir.isEmpty()) {

            localWorkDir->setText(selectedDir);
        }
    }
    );

    //
    // entry for remoteWorkDir location .. basically as before
    //

    remoteWorkDir = new QLineEdit();
    QHBoxLayout *remoteWorkDirLayout = new QHBoxLayout();
    remoteWorkDirLayout->addWidget(remoteWorkDir);
    
    QPushButton *remoteWorkDirButton = new QPushButton();
    remoteWorkDirButton->setText("Browse");
    remoteWorkDirButton->setToolTip(tr("Select Work directory where local jobs will run"));
    remoteWorkDirLayout->addWidget(remoteWorkDirButton);

    locationDirectoriesLayout->addRow(tr("Remote Jobs Directory:"), remoteWorkDirLayout);
    locationDirectoriesLayout->setAlignment(Qt::AlignLeft);
    locationDirectoriesLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    locationDirectoriesLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    connect(remoteWorkDirButton, &QPushButton::clicked, this, [this](){
        QSettings settings("SimCenter", QCoreApplication::applicationName()); 
        QVariant  remoteWorkDirPathVariant = settings.value("remoteWorkDir");
        QString   existingDir = QCoreApplication::applicationDirPath();
        if (remoteWorkDirPathVariant.isValid()) {
            QString existingDString = remoteWorkDirPathVariant.toString();
            QDir existingD(existingDString);
	    if (existingD.exists())
	      existingDir = existingD.absolutePath();
        }

        QString selectedDir = QFileDialog::getExistingDirectory(this,
                                                                tr("Select Local directory were remote job data staged"),
                                                                existingDir,
                                                                QFileDialog::ShowDirsOnly);
        if(!selectedDir.isEmpty()) {
            remoteWorkDir->setText(selectedDir);
        }
    }
    );



    //
    // entry for appDir location .. basically as before
    //


    QHBoxLayout *appDirLayout = new QHBoxLayout();

    appDir = new QLineEdit();
    customAppDirCheckBox = new QCheckBox("Custom");
    QPushButton *appDirButton = new QPushButton();
    appDirButton->setText("Browse");
    appDirButton->setToolTip(tr("Select Directory containing the Backend directory named applications"));

    appDirLayout->addWidget(customAppDirCheckBox);
    appDirLayout->addWidget(appDir);
    appDirLayout->addWidget(appDirButton);

    customAppDirCheckBox->setChecked(false);
    appDir->setEnabled(false);
    appDirButton->setEnabled(false);

    locationDirectoriesLayout->addRow("Backend Applications:", appDirLayout);
    locationDirectoriesLayout->setAlignment(Qt::AlignLeft);
    locationDirectoriesLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    locationDirectoriesLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    connect(appDirButton, &QPushButton::clicked, this, [this](){
        QSettings settings("SimCenter", QCoreApplication::applicationName()); 
        QVariant  appDirPathVariant = settings.value("appDir");
        QString   existingDir = QCoreApplication::applicationDirPath();
        if (appDirPathVariant.isValid()) {
            QString existingDString = appDirPathVariant.toString();
            QDir existingD(existingDString);
	    if (existingD.exists())
	      existingDir = existingD.absolutePath();
        }

        QString selectedDir = QFileDialog::getExistingDirectory(this,
                                                                tr("Select SimCenter Backend Applications Directory"),
                                                                existingDir,
                                                                QFileDialog::ShowDirsOnly);
        if(!selectedDir.isEmpty()) {
            appDir->setText(selectedDir);
        }
    }
    );

    connect(customAppDirCheckBox, &QCheckBox::toggled, this, [this, appDirButton](bool checked)
    {
        appDir->setEnabled(checked);
        appDirButton->setEnabled(checked);
        appDirButton->setFlat(!checked);
        if (checked == false) appDir->setText(this->getAppDir());
    });

    //
    // entry for remoteBackendDir location .. basically as before
    //   - note using time stamp in name so can update setting variable with new releases
    //

    remoteBackendDir = new QLineEdit();
    QHBoxLayout *remoteBackendDirLayout = new QHBoxLayout();

    remoteBackendDirLayout->addWidget(remoteBackendDir);
    
    // no Browse button as remote dir location is stampede2 NOT designsafe & that we cannot touch

    remoteSettingsLayout->addRow(tr("Remote Applications Directory:"), remoteBackendDirLayout);

    

    QHBoxLayout *remoteAppLayout = new QHBoxLayout();

    remoteTapisApp = new QLineEdit();
    customTapisAppCheckBox = new QCheckBox("Custom:");
    customTapisAppCheckBox->setChecked(false);
    remoteTapisApp->setEnabled(false);

    connect(customTapisAppCheckBox, &QCheckBox::toggled, this, [this](bool checked)
    {
        qDebug() << "TAPIS: " << checked;
        this->remoteTapisApp->setEnabled(checked);
        this->remoteTapisApp->setText(this->getRemoteAgaveApp());
    });

    remoteAppLayout->addWidget(customTapisAppCheckBox);
    remoteAppLayout->addWidget(remoteTapisApp);

    remoteSettingsLayout->addRow("Tapis App:", remoteAppLayout);
    remoteSettingsLayout->setAlignment(Qt::AlignLeft);
    remoteSettingsLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    remoteSettingsLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    //
    // push buttons at bottom of Widget, save & whatever else
    //

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();

    QPushButton *resetButton = new QPushButton();
    resetButton->setText("Reset");
    resetButton->setToolTip(tr("Reset Preferences"));
    connect(resetButton, SIGNAL(clicked(bool)), this, SLOT(resetPreferences(bool)));
    buttonsLayout->addWidget(resetButton);

    QPushButton *saveButton = new QPushButton();
    saveButton->setText("Save");
    saveButton->setToolTip(tr("Save Preferences"));
    connect(saveButton, SIGNAL(clicked(bool)), this, SLOT(savePreferences(bool)));
    buttonsLayout->addWidget(saveButton);

    QPushButton *quitButton = new QPushButton();
    quitButton->setText("Cancel");
    quitButton->setToolTip(tr("Quit without Saving"));
    connect(quitButton, SIGNAL(clicked(bool)), this, SLOT(quitPreferences(bool)));
    buttonsLayout->addWidget(quitButton);

    //
    // add boxes to layout, set widgets layout to layout and load existing to fill in QLineEdits
    //

    layout->addWidget(externalApplicationsBox);
    layout->addWidget(locationDirectoriesBox);
    layout->addWidget(remoteSettingsBox);
    layout->addLayout(buttonsLayout);

    this->setLayout(layout);
    this->loadPreferences();
    this->setModal(true);

    // give it some dimension
    int nWidth = 800;
    int nHeight = 300;
    if (parent != NULL)
        setGeometry(parent->x() + parent->width()/2 - nWidth/2,
                    parent->y() + parent->height()/2 - nHeight/2,
                    nWidth, nHeight);
    else
        resize(nWidth, nHeight);

    //Automatically changing to forward slash
    connect(appDir, &QLineEdit::textChanged, this, [this](QString newValue){
        if (newValue.contains('\\'))
            appDir->setText(newValue.replace('\\','/'));
    });

    connect(remoteBackendDir, &QLineEdit::textChanged, this, [this](QString newValue){
        if (newValue.contains('\\'))
            remoteBackendDir->setText(newValue.replace('\\','/'));
    });

    connect(localWorkDir, &QLineEdit::textChanged, this, [this](QString newValue){
        if (newValue.contains('\\'))
            localWorkDir->setText(newValue.replace('\\','/'));
    });

    connect(remoteWorkDir, &QLineEdit::textChanged, this, [this](QString newValue){
        if (newValue.contains('\\'))
            remoteWorkDir->setText(newValue.replace('\\','/'));
    });
}

SimCenterPreferences::~SimCenterPreferences()
{

}


void
SimCenterPreferences::savePreferences(bool) {

    QString currentVersion = QCoreApplication::applicationVersion();

    QSettings settingsCommon("SimCenter", "Common");
    settingsCommon.setValue("pythonExePath", python->text());

    QSettings settingsApp("SimCenter", QCoreApplication::applicationName());

    settingsApp.setValue("version", currentVersion);
    settingsApp.setValue("appDir", appDir->text());
    settingsApp.setValue("customAppDir", customAppDirCheckBox->isChecked());
    settingsApp.setValue("customOpenSees", customOpenSeesCheckBox->isChecked());
    settingsApp.setValue("customDakota", customDakotaCheckBox->isChecked());
    settingsApp.setValue("customTapis", customTapisAppCheckBox->isChecked());
    settingsApp.setValue("remoteBackendDir", remoteBackendDir->text());
    settingsApp.setValue("remoteTapisApp", remoteTapisApp->text());

    settingsApp.setValue("localWorkDir", localWorkDir->text());
    settingsApp.setValue("remoteWorkDir", remoteWorkDir->text());
    settingsApp.setValue("openseesPath", opensees->text());
    settingsApp.setValue("dakotaPath", dakota->text());
    
    this->close();
}

void
SimCenterPreferences::quitPreferences(bool) {  
    this->close();
}

void
SimCenterPreferences::resetPreferences(bool) {

    qDebug() << "RESET_PREFERENCES";

    QSettings settingsCommon("SimCenter", "Common");

    QString pythonPath = this->getDefaultPython();
    settingsCommon.setValue("pythonExePath", pythonPath);
    python->setText(pythonPath);

    QSettings settingsApplication("SimCenter", QCoreApplication::applicationName());

    QString currentVersion = QCoreApplication::applicationVersion();
    settingsApplication.setValue("version", currentVersion);

    QDir workingDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QString remoteWorkDirLocation = workingDir.filePath(QCoreApplication::applicationName() + "/RemoteWorkDir");
    settingsApplication.setValue("remoteWorkDir", remoteWorkDirLocation);
    remoteWorkDir->setText(remoteWorkDirLocation);

    QString localWorkDirLocation = workingDir.filePath(QCoreApplication::applicationName() + "/LocalWorkDir");
    settingsApplication.setValue("localWorkDir", localWorkDirLocation);
    localWorkDir->setText(localWorkDirLocation);

    customAppDirCheckBox->setChecked(false);
    QString appDirLocation = getAppDir();
    settingsApplication.setValue("appDir", appDirLocation);
    appDir->setText(appDirLocation);
    
    QString remoteBackendDirLocation = QString("/home1/00477/tg457427/SimCenterBackendApplications/v2.3.0");
    settingsApplication.setValue("remoteBackendDir", remoteBackendDirLocation);
    remoteBackendDir->setText(remoteBackendDirLocation);

    QString remoteAppName = this->getDefaultAgaveApp();
    settingsApplication.setValue("remoteTapisApp", remoteAppName);
    remoteTapisApp->setText(remoteAppName);

    QString openseesPath=this->getDefaultOpenSees();
    settingsApplication.setValue("openseesPath", openseesPath);
    opensees->setText(openseesPath);

    QString dakotaPath=this->getDefaultDakota();
    settingsApplication.setValue("dakotaPath", dakotaPath);
    dakota->setText(dakotaPath);

    customAppDirCheckBox->setChecked(false);
    customDakotaCheckBox->setChecked(false);
    customOpenSeesCheckBox->setChecked(false);
    customTapisAppCheckBox->setChecked(false);

    // finally save them to make sure all saved
    this->savePreferences(true);
}


void
SimCenterPreferences::loadPreferences() {

    QString currentVersion = QCoreApplication::applicationVersion();

    QSettings settingsApplication("SimCenter", QCoreApplication::applicationName());

    //
    // if version not current, redo preferences
    //

    QVariant  versionVariant = settingsApplication.value("version");
    if (versionVariant.isValid()) {
        QString versionInSettings = versionVariant.toString();
        if (versionInSettings != currentVersion) {
            this->resetPreferences(true);
            return;
        }
    } else {
        this->resetPreferences(true);
        return;
    }

    //
    // common setting first
    //

    QSettings settingsCommon("SimCenter", "Common");

    QVariant  pythonPathVariant = settingsCommon.value("pythonExePath");

    // python
    if (!pythonPathVariant.isValid()) {
        QString pythonPath=this->getPython();
        settingsCommon.setValue("pythonExePath", pythonPath);
        python->setText(pythonPath);
    } else {
        python->setText(pythonPathVariant.toString());
    }

    //
    // now app specific settings
    //

    // localWorkDir
    QVariant  localWorkDirVariant = settingsApplication.value("localWorkDir");
    if (!localWorkDirVariant.isValid()) {
      QString localWorkDirLocation = this->getLocalWorkDir();
      settingsApplication.setValue("localWorkDir", localWorkDirLocation);
      localWorkDir->setText(localWorkDirLocation);
    } else {
        localWorkDir->setText(localWorkDirVariant.toString());
    }

    // remoteWorkDir
    QVariant  remoteWorkDirVariant = settingsApplication.value("remoteWorkDir");
    if (!remoteWorkDirVariant.isValid()) {
      QString remoteWorkDirLocation = this->getRemoteWorkDir();
      settingsApplication.setValue("remoteWorkDir", remoteWorkDirLocation);
      remoteWorkDir->setText(remoteWorkDirLocation);
    } else {
        remoteWorkDir->setText(remoteWorkDirVariant.toString());
    }

    // appDir
    QString currentAppDir = QCoreApplication::applicationDirPath();
    auto customAppDir = settingsApplication.value("customAppDir", false);

    if(customAppDir.isValid() && customAppDir.toBool() == true)
    {
        customAppDirCheckBox->setChecked(true);
        QVariant  appDirVariant = settingsApplication.value("appDir");
        if (appDirVariant.isValid())
            currentAppDir = appDirVariant.toString();
    }
    else
        customAppDirCheckBox->setChecked(false);

    appDir->setText(currentAppDir);

    // remoteBackendDir NOT quite as before as need to allow future releases to bring new ones
    QVariant  remoteBackendDirVariant = settingsApplication.value("remoteBackendDir");
    if (!remoteBackendDirVariant.isValid()) {
      QString remoteBackendDirLocation = QString("/home1/00477/tg457427/SimCenterBackendApplications/v2.3.0");
      settingsApplication.setValue("remoteBackendDir", remoteBackendDirLocation);
      remoteBackendDir->setText(remoteBackendDirLocation);
    } else {
        remoteBackendDir->setText(remoteBackendDirVariant.toString());
    }

    //remoteApp
    auto customTapisApp = settingsApplication.value("customTapis", false);
    if (customTapisApp.isValid() && customTapisApp.toBool() == true) {
        QVariant  tapisAppVariant = settingsApplication.value("remoteTapisApp");
        if (!tapisAppVariant.isValid()) {
            customTapisAppCheckBox->setChecked(false);
            QString tapisApp=this->getDefaultAgaveApp();
            settingsApplication.setValue("remoteTapisApp", tapisApp);
            remoteTapisApp->setText(tapisApp);
        } else {
            customTapisAppCheckBox->setChecked(true);
            remoteTapisApp->setText(tapisAppVariant.toString());
        }
    } else {
        QString tapisApp=this->getDefaultAgaveApp();
        remoteTapisApp->setText(tapisApp);
    }

    
    // opensees
    auto customOpenSees = settingsApplication.value("customOpenSees", false);
    if (customOpenSees.isValid() && customOpenSees.toBool() == true) {
        QVariant  openseesPathVariant = settingsApplication.value("openseesPath");
        if (!openseesPathVariant.isValid()) {
            customOpenSeesCheckBox->setChecked(false);
            QString openseesPath=this->getDefaultOpenSees();
            settingsApplication.setValue("openseesPath", openseesPath);
            opensees->setText(openseesPath);
        } else {
            customOpenSeesCheckBox->setChecked(true);
            opensees->setText(openseesPathVariant.toString());
        }
    } else {
        QString openSeesApp=this->getDefaultOpenSees();
        opensees->setText(openSeesApp);
    }

    // dakota
    auto customDakota = settingsApplication.value("customDakota", false);
    if (customDakota.isValid() && customDakota.toBool() == true) {
        QVariant  dakotaPathVariant = settingsApplication.value("dakotaPath");
        if (!dakotaPathVariant.isValid()) {
            customDakotaCheckBox->setChecked(false);
            QString dakotaPath=this->getDefaultDakota();
            settingsApplication.setValue("dakotaPath", dakotaPath);
            dakota->setText(dakotaPath);
        } else {
            customDakotaCheckBox->setChecked(true);
            dakota->setText(dakotaPathVariant.toString());
        }
    } else {
        QString dakotaApp=this->getDefaultDakota();
        dakota->setText(dakotaApp);
    }
}

QString
SimCenterPreferences::getPython(void) {

    QSettings settingsCommon("SimCenter", "Common");
    QVariant  pythonPathVariant = settingsCommon.value("pythonExePath");

    // if python not set .. get default
    if (!pythonPathVariant.isValid()) {
#ifdef Q_OS_WIN
        QStringList paths{QCoreApplication::applicationDirPath().append("/applications/python")};
        QString pythonPath = QStandardPaths::findExecutable("python.exe", paths);
        if(pythonPath.isEmpty())
            pythonPath = QStandardPaths::findExecutable("python.exe");
#else
        QString pythonPath = QStandardPaths::findExecutable("python");
#endif
        if (pythonPath.isEmpty())
            pythonPath = QString("python");

        settingsCommon.setValue("pythonExePath", pythonPath);
        return pythonPath;
    }

    return pythonPathVariant.toString();
}

QString
SimCenterPreferences::getAppDir(void) {

    //Default appDir is the location of the application
    auto currentAppDir = QCoreApplication::applicationDirPath();

    //If custom is checked we will try to get the custom app dir defined
    if (customAppDirCheckBox->checkState() == Qt::CheckState::Checked)
    {
        QSettings settingsApplication("SimCenter", QCoreApplication::applicationName());
        QVariant  customAppDirSetting = settingsApplication.value("appDir");

        // if valid use it, otherwise it remains the default
        if (customAppDirSetting.isValid())
            currentAppDir = customAppDirSetting.toString();
    }

    return currentAppDir;
}

QString
SimCenterPreferences::getRemoteAppDir(void) {

    QSettings settingsApplication("SimCenter", QCoreApplication::applicationName());
    QVariant  remoteBackendDirVariant = settingsApplication.value("remoteBackendDir");

    // if not set, use default & set default as application directory
    if (!remoteBackendDirVariant.isValid()) {
      QString remoteBackendDirLocation = QString("/home1/00477/tg457427/SimCenterBackendApplications/v2.3.0");
      settingsApplication.setValue("remoteBackendDir", remoteBackendDirLocation);
      return remoteBackendDirLocation;
    } 
    
    return remoteBackendDirVariant.toString();
}



QString
SimCenterPreferences::getRemoteAgaveApp(void) {

    //Default appDir is the location of the application

    QString remoteApp = this->getDefaultAgaveApp();

    //If custom is checked we will try to get the custom app dir defined
    if (customTapisAppCheckBox->checkState() == Qt::CheckState::Checked)
    {
        QSettings settingsApplication("SimCenter", QCoreApplication::applicationName());
        QVariant  customAppNameSetting = settingsApplication.value("appName");

        // if valid use it, otherwise it remains the default
        if (customAppNameSetting.isValid())
            remoteApp = customAppNameSetting.toString();
    }

    return remoteApp;    
}


QString
SimCenterPreferences::getLocalWorkDir(void) {

    QSettings settingsApplication("SimCenter", QCoreApplication::applicationName());
    QVariant  localWorkDirVariant = settingsApplication.value("localWorkDir");

    // if not set, use default & set default as application directory
    if (!localWorkDirVariant.isValid()) {
      QDir workingDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
      QString localWorkDirLocation = workingDir.filePath(QCoreApplication::applicationName() + "/LocalWorkDir");
      settingsApplication.setValue("localWorkDir", localWorkDirLocation);
      localWorkDir->setText(localWorkDirLocation);
      return localWorkDirLocation;
    } 
    
    return localWorkDirVariant.toString();
}

QString
SimCenterPreferences::getRemoteWorkDir(void) {

    QSettings settingsApplication("SimCenter", QCoreApplication::applicationName());
    QVariant  remoteWorkDirVariant = settingsApplication.value("remoteWorkDir");

    // if not set, use default & set default as application directory
    if (!remoteWorkDirVariant.isValid()) {
      QDir workingDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
      QString remoteWorkDirLocation = workingDir.filePath(QCoreApplication::applicationName() + "/RemoteWorkDir");
      settingsApplication.setValue("remoteWorkDir", remoteWorkDirLocation);
      remoteWorkDir->setText(remoteWorkDirLocation);
      return remoteWorkDirLocation;
    } 
    
    return remoteWorkDirVariant.toString();
}

QString
SimCenterPreferences::getDefaultAgaveApp(void) {

    //Default appDir is the location of the application
    QString appName = QCoreApplication::applicationName();
    QString remoteApp("simcenter-dakota-1.0.0.u1");
    if (appName == QString("WE-UQ"))
      remoteApp = QString("simcenter-openfoam-dakota-1.2.2u1");
    if (appName == QString("R2D"))
      remoteApp = QString("rWhale-2.3.0u1");

    return remoteApp;
}

QString
SimCenterPreferences::getDefaultOpenSees(void) {
    QString currentAppDir = QCoreApplication::applicationDirPath();
#ifdef Q_OS_WIN
    QString openseesApp = currentAppDir + QDir::separator() + "applications" + QDir::separator() + "opensees" + QDir::separator() + "bin" + QDir::separator() + "OpenSees.exe";
#else
    QString openseesApp = currentAppDir + QDir::separator() + "applications" + QDir::separator() + "opensees" + QDir::separator() + "bin" + QDir::separator() + "OpenSees";
#endif

    return openseesApp;
}

QString
SimCenterPreferences::getDefaultDakota(void) {
    QString currentAppDir = QCoreApplication::applicationDirPath();

#ifdef Q_OS_WIN
    QString dakotaApp = currentAppDir + QDir::separator() + "applications" + QDir::separator() + "dakota" + QDir::separator() + "bin" + QDir::separator() + "dakota.exe";
#else
    QString dakotaApp = currentAppDir + QDir::separator() + "applications" + QDir::separator() + "dakota" + QDir::separator() + "bin" + QDir::separator() + "dakota";
#endif

    return dakotaApp;

}

QString
SimCenterPreferences::getDefaultPython(void) {

#ifdef Q_OS_WIN
    QStringList paths{QCoreApplication::applicationDirPath().append("/applications/python")};
    QString pythonPath = QStandardPaths::findExecutable("python.exe", paths);
    if(pythonPath.isEmpty())
        pythonPath = QStandardPaths::findExecutable("python.exe");
#else
    QString pythonPath;// = QStandardPaths::findExecutable("python3");
    // this is where python.org installer puts it
    QFileInfo installedPython38("/Library/Frameworks/Python.framework/Versions/3.8/bin/python3");
    QFileInfo installedPython37("/Library/Frameworks/Python.framework/Versions/3.7/bin/python3");
    if (installedPython38.exists()) {
        pythonPath = installedPython38.filePath();
    } else if (installedPython37.exists()) {
        pythonPath = installedPython37.filePath();
    } else {
        // maybe user has a local installed copy .. look in standard path
        QFileInfo localPython3("/usr/local/bin/python3");
        if (localPython3.exists()) {
            pythonPath = localPython3.filePath();
        } else {
            // assume user has it correct in shell startup script
            pythonPath = QStandardPaths::findExecutable("python3");
        }
    }
#endif

    return pythonPath;
}
