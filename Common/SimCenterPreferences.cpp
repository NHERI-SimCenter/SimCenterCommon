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
        QString existingDir = QStandardPaths::displayName(QStandardPaths::HomeLocation);
        if (pythonPathVariant.isValid()) {
            QString existingF = pythonPathVariant.toString();
            QFileInfo existingFile(existingF);
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
    // push buttons at bottom of Widget, save & whatever else
    //

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();

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

}

SimCenterPreferences::~SimCenterPreferences()
{

}


void
SimCenterPreferences::savePreferences(bool) {
    QSettings settings("SimCenter", "Common");
    settings.setValue("pythonExePath", python->text());

    this->close();
}

void
SimCenterPreferences::loadPreferences() {
    QSettings settings("SimCenter", "Common");
    QVariant  pythonPathVariant = settings.value("pythonExePath");

    // set python
    if (!pythonPathVariant.isValid()) {
#ifdef Q_OS_WIN
        QString pythonPath = QStandardPaths::findExecutable("python.exe");
#else
        QString pythonPath = QStandardPaths::findExecutable("python");
#endif

        settings.setValue("pythonExePath", pythonPath);
        python->setText(pythonPath);
    } else {
        python->setText(pythonPathVariant.toString());
    }
}
