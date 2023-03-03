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

// Written: fmckenna, sangri

#include "SurrogateEDP.h"
#include <RandomVariablesContainer.h>
#include <QMessageBox>
//#include <InputWidgetParameters.h>

SurrogateEDP *SurrogateEDP::theInstance = 0;


SurrogateEDP *
SurrogateEDP::getInstance() {
    if (theInstance == 0)
        theInstance = new SurrogateEDP();
    return theInstance;
}


SurrogateEDP::SurrogateEDP(QWidget *parent)
    : SimCenterAppWidget(parent)
{
    verticalLayout = new QVBoxLayout();
    this->setLayout(verticalLayout);


    QPushButton *resetEDP = new QPushButton();
    resetEDP->setMinimumWidth(75);
    resetEDP->setMaximumWidth(75);
    resetEDP->setText(tr("Reset"));
    connect(resetEDP,SIGNAL(clicked()),this,SLOT(resetEDP()));
    resetEDP->hide();
    // sy - for release

    verticalLayout->addWidget(resetEDP);
    // title & add button

    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    edp = new QFrame();
    edp->setFrameShape(QFrame::NoFrame);
    edp->setLineWidth(0);

    edpLayout = new QVBoxLayout;

    //setLayout(layout);
    edp->setLayout(edpLayout);
    sa->setWidget(edp);
    verticalLayout->addWidget(sa);

    x_button_clicked_before=false;
}

SurrogateEDP::~SurrogateEDP()
{

}


void
SurrogateEDP::clear(void)
{

}


void
SurrogateEDP::addEDPs(QStringList EDPnames)
{
    for (int i = edpLayout->count()-1; i >=0; --i) {
        QLayout *layout = edpLayout->itemAt(i)->layout();
        if (layout != nullptr) {
            if (layout->count()>0) {
                if (QPushButton *removeButton = qobject_cast<QPushButton *>(layout->itemAt(0)->widget())) {
                    emit removeButton->clicked(); // remove all
                }
            }
        }
    }
    while (QLayoutItem* item = edpLayout->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    fullEDPnames = EDPnames;
    for (int i=0; i<EDPnames.length(); i++) {
        QHBoxLayout * newEDP = new QHBoxLayout();
        QPushButton * removeButton = new QPushButton("×");
        QLabel * tmpName = new QLabel(EDPnames.at(i));

        newEDP->addWidget(removeButton);
        newEDP->addWidget(tmpName);

        const QSize BUTTON_SIZE = QSize(15, 15);
        removeButton->setFixedSize(BUTTON_SIZE);
        removeButton->setStyleSheet("QPushButton { font-size:15px;  font-weight: bold;padding: 0px 0px 2px 0px; }");
        removeButton->hide();
        // sy - for release

        edpLayout->addLayout(newEDP);
        connect(removeButton, &QPushButton::clicked, this, [=](){
            if (x_button_clicked_before == false) {
                x_button_clicked_before = true;
                QMessageBox::StandardButton reply;
                reply = QMessageBox::information(this,
                                              "Remove EDP",
                                              "Are you sure you want to remove this EDP?",
                                               QMessageBox::Yes|QMessageBox::No);

                if (reply == QMessageBox::No)
                    return;
            }
        });
        connect(removeButton, &QPushButton::clicked, removeButton, &QPushButton::deleteLater);
        connect(removeButton, &QPushButton::clicked, tmpName, &QLabel::deleteLater);

    }
    edpLayout->addStretch();
    emit surrogateSelected();
}

void
SurrogateEDP::resetEDP()
{
    this->addEDPs(fullEDPnames);
}

bool
SurrogateEDP::outputToJSON(QJsonObject &jsonObject)
{
    // just need to send the class type here.. type needed in object in case user screws up
    jsonObject["type"]="SurrogateEDP";

    return true;
}


bool
SurrogateEDP::inputFromJSON(QJsonObject &jsonObject)
{
    Q_UNUSED(jsonObject);
    return true;
}


bool
SurrogateEDP::outputAppDataToJSON(QJsonObject &jsonObject) {

    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    jsonObject["Application"] = "SurrogateEDP";
    QJsonArray edpList;
    for (int i = 0; i < edpLayout->count(); ++i) {
        QLayout *layout = edpLayout->itemAt(i)->layout();
        if (layout != nullptr) {
            if (layout->count()>1) {
                if (QLabel *label = qobject_cast<QLabel *>(layout->itemAt(1)->widget())) {
                    QJsonObject edpObj;
                    edpObj["length"]=1;
                    edpObj["type"]="scalar";
                    edpObj["name"]= label->text();
                    edpList.append(edpObj);
                }
            }
        }
    }
    QJsonObject edpObj;
    edpObj["EDP"] = edpList;
    jsonObject["ApplicationData"] = edpObj;

    return true;
}
bool
SurrogateEDP::inputAppDataFromJSON(QJsonObject &jsonObject) {
    Q_UNUSED(jsonObject);
    return true;
}


bool
SurrogateEDP::copyFiles(QString &dirName) {
    Q_UNUSED(dirName);
    return true;
}

