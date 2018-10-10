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

#include "SteelTubeSectionInputWidget.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QList>
#include <BimClasses.h>
#include <jansson.h>

SteelTubeSectionInputWidget::SteelTubeSectionInputWidget(QWidget *parent) : SimCenterTableWidget(parent)
{
    fillingTableFromMap = false;

    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);

    QStringList headings;
    QList<int> dataTypes;
    headings << tr("Name");
    headings << tr("Material");
    headings << tr("Depth");
    headings << tr("Width");
    headings << tr("Flange Thickness");
    headings << tr("Web Thickness");
    headings << tr("radius");
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    theSpreadsheet = new SpreadsheetWidget(7, 1000, headings, dataTypes, this);

    theLayout->addWidget(theSpreadsheet);
    this->setMinimumWidth(200);

    // connect signals and slots
    connect(theSpreadsheet,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(somethingEntered(int,int,int,int)));
    connect(theSpreadsheet,SIGNAL(cellChanged(int,int)),this,SLOT(somethingChanged(int,int)));
}

SteelTubeSectionInputWidget::~SteelTubeSectionInputWidget()
{

}


bool
SteelTubeSectionInputWidget::outputToJSON(QJsonObject &jsonObj){
    // this is just a view, data sent elsewhere
    return true;
}

bool
SteelTubeSectionInputWidget::outputToJSON(QJsonArray &jsonArray){
    return true;
}

bool
SteelTubeSectionInputWidget::inputFromJSON(QJsonObject &theObject)
{
    fillingTableFromMap = true;
    this->clear();
    currentRow = 0;
    std::map<string, FrameSection *>::iterator it;
    for (it = FrameSection::theFrameSections.begin(); it != FrameSection::theFrameSections.end(); it++) {
        FrameSection *theOrigFrameSection = it->second;

        if (theOrigFrameSection->sectType == STEEL_TUBE_SECTION) {

            SteelTubeSection *theFrameSection = dynamic_cast<SteelTubeSection *>(theOrigFrameSection);

            QString name(QString::fromStdString((theFrameSection->name)));
            QString material(QString::fromStdString((theFrameSection->material)));

            theSpreadsheet->setString(currentRow, 0, name);
            theSpreadsheet->setString(currentRow, 1, material);

            if (theFrameSection->rvD != 0) {
                QString rv(QString::fromStdString(*(theFrameSection->rvD)));
                theSpreadsheet->setString(currentRow, 2, rv);
            } else
                theSpreadsheet->setDouble(currentRow, 2, theFrameSection->depth);

            if (theFrameSection->rvW != 0) {
                QString rv(QString::fromStdString(*(theFrameSection->rvW)));
                theSpreadsheet->setString(currentRow, 3, rv);
            } else
                theSpreadsheet->setDouble(currentRow, 3, theFrameSection->width);

            if (theFrameSection->rvFT != 0) {
                QString rv(QString::fromStdString(*(theFrameSection->rvFT)));
                theSpreadsheet->setString(currentRow, 4, rv);
            } else
                theSpreadsheet->setDouble(currentRow, 4, theFrameSection->flangeT);

            if (theFrameSection->rvWT != 0) {
                QString rv(QString::fromStdString(*(theFrameSection->rvWT)));
                theSpreadsheet->setString(currentRow, 5, rv);
            } else
                theSpreadsheet->setDouble(currentRow, 5, theFrameSection->webT);

            if (theFrameSection->rvR != 0) {
                QString rv(QString::fromStdString(*(theFrameSection->rvR)));
                theSpreadsheet->setString(currentRow, 6, rv);
            } else
                theSpreadsheet->setDouble(currentRow, 6, theFrameSection->r);

            currentRow++;
        }
    }
    fillingTableFromMap = false;
    return true;
}

void
SteelTubeSectionInputWidget::clear(void)
{
    currentRow = 0;
    theSpreadsheet->clear();
}

void
SteelTubeSectionInputWidget::somethingEntered(int row, int column, int row2, int col2) {
    if (column == 0) {
        if (theSpreadsheet->getString(row, column, currentName) == false)
            currentName.clear();
    } else
        currentName.clear();
}

void
SteelTubeSectionInputWidget::somethingChanged(int row, int column) {

    if (fillingTableFromMap == true) {
        return;
    }

    QString name, material;
    double fT = 0., w =0., wT = 0., r = 0., d=0.;
    QString rvFT, rvW, rvWT, rvR, rvD;
    string *rvFtString = NULL, *rvWString = NULL, *rvWtString = NULL, *rvRString = NULL, *rvDString = NULL;

    QTableWidgetItem *theName = theSpreadsheet->item(row, 0);
    QTableWidgetItem *theMaterial = theSpreadsheet->item(row, 1);
    QTableWidgetItem *theD = theSpreadsheet->item(row,2);
    QTableWidgetItem *theW = theSpreadsheet->item(row,3);
    QTableWidgetItem *theFT = theSpreadsheet->item(row,4);
    QTableWidgetItem *theWT = theSpreadsheet->item(row,5);
    QTableWidgetItem *theR = theSpreadsheet->item(row,6);

    //
    // make sure name exists and is unique
    //   if not unique reset to last value and return w/o doing anything
    //

    if (theName == NULL) {
        return; // do not add Floor until all data exists
    }

    if (theSpreadsheet->getString(row,0,name) == false) {
        return; // problem with name
    }

    if (column == 0) {
        string theStringName = name.toStdString();
        if ((theStringName.empty()) || (theStringName.find_first_not_of(' ') == std::string::npos)) {
            theSpreadsheet->setString(row, 0, currentName);
            return;
        }
    } else { // if name empty, don't add
        string theStringName = name.toStdString();
        if ((theStringName.empty()) || (theStringName.find_first_not_of(' ') == std::string::npos)) {
            return;
        }
    }

    // check name is  unique, if not set string to what it was before entry
    if (column == 0) {
        FrameSection *existingFrameSection = FrameSection::getFrameSection(name.toStdString());
        if (existingFrameSection != NULL) {
            theSpreadsheet->setString(row, 0, currentName);
            return;
        }
    }

    //
    // if height exists, update map in Floors with new entry
    //

    if (theMaterial == NULL || theD == NULL || theFT == NULL || theW == NULL || theWT == NULL || theR == NULL)
        return;

    //  qDebug() << "steel changed - all there";
    if (theSpreadsheet->getString(row,1,material) == false) {
      qDebug() << "no material";
    }

    if (theSpreadsheet->getDouble(row,2,d) == false) {
        if (theSpreadsheet->getString(row,2,rvD) == false) {
            qDebug() << "NO E";
            return;
        } else {
            rvDString = new string(rvD.toStdString());
        }
    }

    if (theSpreadsheet->getDouble(row,4,w) == false) {
        if (theSpreadsheet->getString(row,4,rvW) == false) {
            qDebug() << "NO FW";
            return;
        } else {
            rvWString = new string(rvW.toStdString());
        }
    }


    if (theSpreadsheet->getDouble(row,4,fT) == false) {
        if (theSpreadsheet->getString(row,4,rvFT) == false) {
            qDebug() << "NO Ft";
            return;
        } else {
            rvFtString = new string(rvFT.toStdString());
        }
    }


    if (theSpreadsheet->getDouble(row,5,wT) == false) {
      if (theSpreadsheet->getString(row,5, rvWT ) == false) {
            qDebug() << "NO WT";
            return;
        } else {
            rvWtString = new string(rvWT.toStdString());
        }
    }

    if (theSpreadsheet->getDouble(row,4,r) == false) {
        if (theSpreadsheet->getString(row,4, rvR ) == false) {
            qDebug() << "NO R";
            return;
        } else {
            rvRString = new string(rvR.toStdString());
        }
    }

    if (column == 0) { // if modified the name, need to remove old before can add as would leave add there
        if (currentName != name) {
            FrameSection::removeFrameSection(currentName.toStdString());
            qDebug() << " removing section " << currentName << " to be replaced with: " << name;
        }
    }

    SteelTubeSection::addSteelTubeSection(name.toStdString(), material.toStdString(), d, w, fT, wT, r,
					  rvDString, rvWString, rvFtString, rvWtString, rvRString);
    currentName = name;

}
