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

#include "SteelInputWidget.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QList>
#include <BimClasses.h>
#include <jansson.h>

SteelInputWidget::SteelInputWidget(QWidget *parent) : SimCenterTableWidget(parent)
{
    fillingTableFromMap = false;

    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);

    QStringList headings;
    QList<int> dataTypes;
    headings << tr("Name");
    headings << tr("E");
    headings << tr("Fy");
    headings << tr("Fu");
    headings << tr("nu");
    headings << tr("rho");
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    theSpreadsheet = new SpreadsheetWidget(6, 1000, headings, dataTypes, this);

    theLayout->addWidget(theSpreadsheet);
    this->setMinimumWidth(200);

    // connect signals and slots
    connect(theSpreadsheet,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(somethingEntered(int,int,int,int)));
    connect(theSpreadsheet,SIGNAL(cellChanged(int,int)),this,SLOT(somethingChanged(int,int)));
}

SteelInputWidget::~SteelInputWidget()
{

}


bool
SteelInputWidget::outputToJSON(QJsonObject &jsonObj){
    // this is just a view, data sent elsewhere
    return true;
}

bool
SteelInputWidget::outputToJSON(QJsonArray &jsonArray){
  return true;
}

bool
SteelInputWidget::inputFromJSON(QJsonObject &theObject)
{
    // qDebug() << "SteelInputFromJSON";

    fillingTableFromMap = true;
    this->clear();
    currentRow = 0;
    std::map<string, Materiall *>::iterator it;
    for (it = Materiall::theMaterials.begin(); it != Materiall::theMaterials.end(); it++) {
        Materiall *theOrigMaterial = it->second;

        if (theOrigMaterial->matType == STEEL_TYPE) {

            Steel *theMaterial = dynamic_cast<Steel *>(theOrigMaterial);

            QString name(QString::fromStdString((theMaterial->name)));

            theSpreadsheet->setString(currentRow, 0, name);

            if (theMaterial->rvE != 0) {
                QString name(QString::fromStdString(*(theMaterial->rvE)));
                theSpreadsheet->setString(currentRow, 1, name);
            } else
                theSpreadsheet->setDouble(currentRow, 1, theMaterial->E);

            if (theMaterial->rvFy != 0) {
                QString name(QString::fromStdString(*(theMaterial->rvFy)));
                theSpreadsheet->setString(currentRow, 2, name);
            } else
                theSpreadsheet->setDouble(currentRow, 2, theMaterial->fy);

            if (theMaterial->rvFu != 0) {
                QString name(QString::fromStdString(*(theMaterial->rvFu)));
                theSpreadsheet->setString(currentRow, 3, name);
            } else
                theSpreadsheet->setDouble(currentRow, 3, theMaterial->fu);

            if (theMaterial->rvNu != 0) {
                QString name(QString::fromStdString(*(theMaterial->rvNu)));
                theSpreadsheet->setString(currentRow, 4, name);
            } else
                theSpreadsheet->setDouble(currentRow, 4, theMaterial->nu);

            if (theMaterial->rvMass != 0) {
                QString name(QString::fromStdString(*(theMaterial->rvMass)));
                theSpreadsheet->setString(currentRow, 5, name);
            } else
                theSpreadsheet->setDouble(currentRow, 5, theMaterial->massPerVolume);

            currentRow++;
        }
    }
    fillingTableFromMap = false;
    return true;
}

void
SteelInputWidget::clear(void)
{
    currentRow = 0;
    theSpreadsheet->clear();
}

void
SteelInputWidget::somethingEntered(int row, int column, int row2, int col2) {
    if (column == 0) {
        if (theSpreadsheet->getString(row, column, currentName) == false)
            currentName.clear();
    } else
        currentName.clear();
}

void
SteelInputWidget::somethingChanged(int row, int column) {

    if (fillingTableFromMap == true) {
        return;
    }

    QString name;
    double E = 0., Fy =0., Fu = 0., Nu = 0.,  Mass = 0.;
    QString rvE, rvFy, rvFu, rvNu, rvMass;
    string *rvEString = NULL, *rvFyString = NULL, *rvFuString = NULL, *rvNuString = NULL, *rvMassString = NULL;

    QTableWidgetItem *theName = theSpreadsheet->item(row, 0);
    QTableWidgetItem *theE = theSpreadsheet->item(row,1);
    QTableWidgetItem *theFy = theSpreadsheet->item(row,2);
    QTableWidgetItem *theFu = theSpreadsheet->item(row,3);
    QTableWidgetItem *theNu = theSpreadsheet->item(row,4);
    QTableWidgetItem *theMass = theSpreadsheet->item(row,5);

    //
    // make sure name exists and is unique
    //   if not unique reset to last value and return w/o doing anything
    //

    if (theName == NULL) {
        qDebug() << "NO CELL";
        return; // do not add Floor until all data exists
    }

    if (theSpreadsheet->getString(row,0,name) == false) {
        qDebug() << "NO NAME";
        return; // problem with name
    }


    if (column == 0) {
        string theStringName = name.toStdString();
        if ((theStringName.empty()) || (theStringName.find_first_not_of(' ') == std::string::npos)) {
            theSpreadsheet->setString(row, 0, currentName);
            // qDebug() << "BLANK NAME";
            return;
        }
    }

    // check name is  unique, if not set string to what it was before entry
    if (column == 0) {
        Materiall *existingMaterial = Materiall::getMaterial(name.toStdString());
        if (existingMaterial != NULL) {
            theSpreadsheet->setString(row, 0, currentName);
            return;
        }
    } else { // is name is empty or conatins blanks, return
        string theStringName = name.toStdString();
        if ((theStringName.empty()) || (theStringName.find_first_not_of(' ') == std::string::npos)) {
            return;
        }
    }

    //
    // if height exists, update map in Floors with new entry
    //

    if (theE == NULL || theFy == NULL || theFu == NULL || theNu == NULL || theMass == NULL)
        return;

    //  qDebug() << "steel changed - all there";

    if (theSpreadsheet->getDouble(row,1,E) == false) {
        if (theSpreadsheet->getString(row,1,rvE) == false) {
            qDebug() << "NO E";
            return;
        } else {
            rvEString = new string(rvE.toStdString());
        }
    }

    if (theSpreadsheet->getDouble(row,2,Fy) == false) {
        if (theSpreadsheet->getString(row,2,rvFy) == false) {
            qDebug() << "NO Fy";
            return;
        } else {
            rvFyString = new string(rvFy.toStdString());
        }
    }

    if (theSpreadsheet->getDouble(row,3,Fu) == false) {
        if (theSpreadsheet->getString(row,3,rvFu) == false) {
            qDebug() << "NO Fu";
            return;
        } else {
            rvFuString = new string(rvFu.toStdString());
        }
    }

    if (theSpreadsheet->getDouble(row,4,Nu) == false) {
        if (theSpreadsheet->getString(row,4,rvNu) == false) {
            qDebug() << "NO Nu";
            return;
        } else {
            rvNuString = new string(rvNu.toStdString());
        }
    }

    if (theSpreadsheet->getDouble(row,5,Mass) == false) {
        if (theSpreadsheet->getString(row,5,rvMass) == false) {
            qDebug() << "NO Mass";
            return;
        } else {
            rvNuString = new string(rvNu.toStdString());
        }
    }


    if (column == 0) { // if modified the name, need to remove old before can add as would leave add there
        if (currentName != name) {
            Materiall::removeMaterial(currentName.toStdString());
        }
    }

    Steel::addSteelMaterial(name.toStdString(), E, Fy, Fu, Nu, Mass,
                            rvEString, rvFyString, rvFuString, rvNuString, rvMassString);
    currentName = name;

}
