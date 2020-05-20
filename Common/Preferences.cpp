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
    layout = new QVBoxLayout();

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

    appDir = new QLineEdit();
    QHBoxLayout *appDirLayout = new QHBoxLayout();
    appDirLayout->addWidget(appDir);
    QPushButton *appDirButton = new QPushButton();
    appDirButton->setText("Browse");
    appDirButton->setToolTip(tr("Select Directory containing the Backend directory named applications"));
    appDirLayout->addWidget(appDirButton);

    customAppDirCheckBox = new QCheckBox("Custom Local Applications:");
    customAppDirCheckBox->setChecked(false);
    appDir->setEnabled(false);
    appDirButton->setEnabled(false);
    locationDirectoriesLayout->addRow(customAppDirCheckBox, appDirLayout);
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
                                                                tr("Select SimCenter Workflow Applications Directory"),
                                                                existingDir,
                                                                QFileDialog::ShowDirsOnly);
        if(!selectedDir.isEmpty()) {
            appDir->setText(selectedDir);
        }
    }
    );

    connect(customAppDirCheckBox, &QCheckBox::toggled, this, [this, appDirButton](bool checked)
    {
        this->appDir->setEnabled(checked);
        appDirButton->setEnabled(checked);
        appDirButton->setFlat(!checked);
        this->appDir->setText(this->getAppDir());
    });

    //
    // entry for remoteAppDir location .. basically as before
    //   - note using time stamp in name so can update setting variable with new releases
    //

    remoteAppDir = new QLineEdit();
    QHBoxLayout *remoteAppDirLayout = new QHBoxLayout();
    remoteAppDirLayout->addWidget(remoteAppDir);
    
    // no Browse button as remote dir location is stampede2 NOT designsafe & that we cannot touch

    remoteSettingsLayout->addRow(tr("Remote Applications Directory:"), remoteAppDirLayout);


    remoteAgaveApp = new QLineEdit();
    QHBoxLayout *remoteAppLayout = new QHBoxLayout();
    remoteAppLayout->addWidget(remoteAgaveApp);
    remoteSettingsLayout->addRow(tr("Remote Agave App:"), remoteAppLayout);

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

    connect(remoteAppDir, &QLineEdit::textChanged, this, [this](QString newValue){
        if (newValue.contains('\\'))
            remoteAppDir->setText(newValue.replace('\\','/'));
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
    QSettings settingsCommon("SimCenter", "Common");
    settingsCommon.setValue("pythonExePath", python->text());

    QSettings settingsApp("SimCenter", QCoreApplication::applicationName());
    settingsApp.setValue("appDir", appDir->text());
    settingsApp.setValue("customAppDir", customAppDirCheckBox->isChecked());
    settingsApp.setValue("remoteAppDir-Mar2020", remoteAppDir->text());
    settingsApp.setValue("remoteAgaveApp-Mar2020", remoteAgaveApp->text());
    settingsApp.setValue("localWorkDir", localWorkDir->text());
    settingsApp.setValue("remoteWorkDir", remoteWorkDir->text());
    
    this->close();
}

void
SimCenterPreferences::resetPreferences(bool) {
    QSettings settingsCommon("SimCenter", "Common");
    settingsCommon.setValue("pythonExePath", python->text());


#ifdef Q_OS_WIN
    QStringList paths{QCoreApplication::applicationDirPath().append("/applications/python")};
    QString pythonPath = QStandardPaths::findExecutable("python.exe", paths);
    if(pythonPath.isEmpty())
        pythonPath = QStandardPaths::findExecutable("python.exe");
#else
    QString pythonPath = QStandardPaths::findExecutable("python3");
    if (pythonPath.isEmpty()) {
        QFileInfo localPython3("/usr/local/bin/python3");
        if (localPython3.exists())
            pythonPath = localPython3.filePath();
        else
            pythonPath = QStandardPaths::findExecutable("python");
    }
#endif
    settingsCommon.setValue("pythonExePath", pythonPath);
    python->setText(pythonPath);

    QSettings settingsApplication("SimCenter", QCoreApplication::applicationName());

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
    
    QString remoteAppDirLocation = QString("/home1/00477/tg457427/SimCenterBackendApplications/Mar-2020");
    settingsApplication.setValue("remoteAppDir", remoteAppDirLocation);
    remoteAppDir->setText(remoteAppDirLocation);

    QString remoteAppName;
    if (QCoreApplication::applicationName() == QString("WE-UQ")) 
      remoteAppName = QString("simcenter-openfoam-dakota-1.1.0u2");
    else
      remoteAppName = QString("simcenter-dakota-1.0.0u1");

    settingsApplication.setValue("remoteAgaveApp-Mar2020", remoteAppName);
    remoteAgaveApp->setText(remoteAppName);

}


void
SimCenterPreferences::loadPreferences() {
    QSettings settingsCommon("SimCenter", "Common");
    QVariant  pythonPathVariant = settingsCommon.value("pythonExePath");

    // python
    if (!pythonPathVariant.isValid()) {
#ifdef Q_OS_WIN
        QString pythonPath = QStandardPaths::findExecutable("python.exe");
#else
        QString pythonPath = QStandardPaths::findExecutable("python");
#endif
        settingsCommon.setValue("pythonExePath", pythonPath);
        python->setText(pythonPath);
    } else {
        python->setText(pythonPathVariant.toString());
    }


    QSettings settingsApplication("SimCenter", QCoreApplication::applicationName());

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


    // remoteAppDir NOT quite as before as need to allow future releases to bring new ones
    QVariant  remoteAppDirVariant = settingsApplication.value("remoteAppDir-Mar2020");
    if (!remoteAppDirVariant.isValid()) {
      QString remoteAppDirLocation = QString("/home1/00477/tg457427/SimCenterBackendApplications/Oct-2019");
      settingsApplication.setValue("remoteAppDir-Mar2020", remoteAppDirLocation);
      remoteAppDir->setText(remoteAppDirLocation);
    } else {
        remoteAppDir->setText(remoteAppDirVariant.toString());
    }

    QVariant  remoteAppNameVariant = settingsApplication.value("remoteAgaveApp-Mar2020");
    if (!remoteAppNameVariant.isValid()) {
      QString remoteAppName = QString("simcenter-dakota-1.0.0u1");
      settingsApplication.setValue("remoteAgaveApp-Mar2020", remoteAppName);
      remoteAgaveApp->setText(remoteAppName);
    } else {
        remoteAgaveApp->setText(remoteAppNameVariant.toString());
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
    QVariant  remoteAppDirVariant = settingsApplication.value("remoteAppDir-Mar2020");

    // if not set, use default & set default as application directory
    if (!remoteAppDirVariant.isValid()) {
      QString remoteAppDirLocation = QString("/home1/00477/tg457427/SimCenterBackendApplications/Oct-2019");
      settingsApplication.setValue("remoteAppDir-Mar2020", remoteAppDirLocation);
      return remoteAppDirLocation;
    } 
    
    return remoteAppDirVariant.toString();
}

QString
SimCenterPreferences::getRemoteAgaveApp(void) {

    QSettings settingsApplication("SimCenter", QCoreApplication::applicationName());
    QVariant  remoteAppNameVariant = settingsApplication.value("remoteAgaveApp-Mar2020");

    // if not set, use default & set default as application directory
    if (!remoteAppNameVariant.isValid()) {
      QString remoteAppName;
      if (QCoreApplication::applicationName() == QString("WE-UQ")) 
	remoteAppName = QString("simcenter-openfoam-dakota-1.1.0u1");
      else
	remoteAppName = QString("simcenter-dakota-1.0.0u1");

      settingsApplication.setValue("remoteAgaveApp-Mar2020", remoteAppName);
      return remoteAppName;
    } 
    
    return remoteAppNameVariant.toString();
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
