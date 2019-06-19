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

    // area for external applications
    QGroupBox* externalApplicationsBox = new QGroupBox("External Applications", this);
    QFormLayout* externalApplicationsLayout = new QFormLayout(externalApplicationsBox);

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

    appDir = new QLineEdit();
    QHBoxLayout *appDirLayout = new QHBoxLayout();
    appDirLayout->addWidget(appDir);
    QPushButton *appDirButton = new QPushButton();
    appDirButton->setText("Browse");
    appDirButton->setToolTip(tr("Select Directory containing the Backend directory named applications"));
    appDirLayout->addWidget(appDirButton);

    externalApplicationsLayout->addRow(tr("Local Applications Directory:"), appDirLayout);
    externalApplicationsLayout->setAlignment(Qt::AlignLeft);
    externalApplicationsLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    externalApplicationsLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);


    // connect the pushbutton with code to open file selection and update appDir preferences with selected file
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

    remoteAppDir = new QLineEdit();
    QHBoxLayout *remoteAppDirLayout = new QHBoxLayout();
    remoteAppDirLayout->addWidget(remoteAppDir);
    QPushButton *remoteAppDirButton = new QPushButton();
    
    // no Browse button as remote dir location is stampede2 NOT designsafe & that we cannot touch

    externalApplicationsLayout->addRow(tr("Remote Applications Directory:"), remoteAppDirLayout);
    externalApplicationsLayout->setAlignment(Qt::AlignLeft);
    externalApplicationsLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    externalApplicationsLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);


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
    settingsApp.setValue("remoteAppDir-June2019", remoteAppDir->text());
    
    this->close();
}

void
SimCenterPreferences::resetPreferences(bool) {
    QSettings settingsCommon("SimCenter", "Common");
    settingsCommon.setValue("pythonExePath", python->text());

#ifdef Q_OS_WIN
    QString pythonPath = QStandardPaths::findExecutable("python.exe");
#else
    QString pythonPath = QStandardPaths::findExecutable("python");
#endif
    settingsCommon.setValue("pythonExePath", pythonPath);
    python->setText(pythonPath);

    QSettings settingsApplication("SimCenter", QCoreApplication::applicationName());
    QString appDirLocation = QCoreApplication::applicationDirPath();
    settingsApplication.setValue("appDir", appDirLocation);
    appDir->setText(appDirLocation);
    
    QString remoteAppDirLocation = QString("/home1/00477/tg457427/SimCenterBackendApplications/June-2019");
    settingsApplication.setValue("remoteAppDir", remoteAppDirLocation);
    remoteAppDir->setText(remoteAppDirLocation);
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


    // appDir
    QVariant  appDirVariant = settingsApplication.value("appDir");
    if (!appDirVariant.isValid()) {
      QString appDirLocation = QCoreApplication::applicationDirPath();
      settingsApplication.setValue("appDir", appDirLocation);
      appDir->setText(appDirLocation);
    } else {
        appDir->setText(appDirVariant.toString());
    }

    // remoteAppDir NOTE: we cannot allow QSettings here as would not be able to upgrade!
    // so probably stupid putting in QSettings, should just put in a QString
    QVariant  remoteAppDirVariant = settingsApplication.value("remoteAppDir-June2019");
    if (!remoteAppDirVariant.isValid()) {
      QString remoteAppDirLocation = QString("/home1/00477/tg457427/SimCenterBackendApplications/June-2019");
      settingsApplication.setValue("remoteAppDir", remoteAppDirLocation);
      remoteAppDir->setText(remoteAppDirLocation);
    } else {
        remoteAppDir->setText(remoteAppDirVariant.toString());
    }
}

QString
SimCenterPreferences::getPython(void) {
    QSettings settingsCommon("SimCenter", "Common");
    QVariant  pythonPathVariant = settingsCommon.value("pythonExePath");

    // if python not set .. get default
    if (!pythonPathVariant.isValid()) {
#ifdef Q_OS_WIN
        QString pythonPath = QStandardPaths::findExecutable("python.exe");
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

    QSettings settingsApplication("SimCenter", QCoreApplication::applicationName());
    QVariant  appDirVariant = settingsApplication.value("appDir");

    // if not set, use default & set default as application directory
    if (!appDirVariant.isValid()) {
        QString appDirLocation = QCoreApplication::applicationDirPath();
        settingsApplication.setValue("appDir", appDirLocation);
	return appDirLocation;
    } 

    return appDirVariant.toString();
}

QString
SimCenterPreferences::getRemoteAppDir(void) {

    QSettings settingsApplication("SimCenter", QCoreApplication::applicationName());
    QVariant  remoteAppDirVariant = settingsApplication.value("remoteAppDir");

    // if not set, use default & set default as application directory
    if (!remoteAppDirVariant.isValid()) {
      QString remoteAppDirLocation = QString("/home1/00477/tg457427/SimCenterBackendApplications/June-2019");
      settingsApplication.setValue("remoteAppDir-June2019", remoteAppDirLocation);
      return remoteAppDirLocation;
    } 
    
    return remoteAppDirVariant.toString();
}
