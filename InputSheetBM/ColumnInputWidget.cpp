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

#include "ColumnInputWidget.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QList>
#include <BimClasses.h>

ColumnInputWidget::ColumnInputWidget(QWidget *parent) : SimCenterTableWidget(parent)
{
    fillingTableFromMap = false;

    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);

    QStringList headings;
    QList<int> dataTypes;
    headings << tr("Name");
    headings << tr("CLine");
    headings << tr("Floor1");
    headings << tr("Floor2");
    headings << tr("section");
    headings << tr("angle");
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    theSpreadsheet = new SpreadsheetWidget(6, 1000, headings, dataTypes, this);

    theLayout->addWidget(theSpreadsheet);

    this->setMinimumWidth(500);

    // connect signals and slots
    connect(theSpreadsheet,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(somethingEntered(int,int,int,int)));
    connect(theSpreadsheet,SIGNAL(cellChanged(int,int)),this,SLOT(somethingChanged(int,int)));
}

ColumnInputWidget::~ColumnInputWidget()
{

}

bool
ColumnInputWidget::outputToJSON(QJsonObject &jsonObj){

    // Does nothing now .. just a view

    return true;
}

bool
ColumnInputWidget::inputFromJSON(QJsonObject &jsonObject){

    fillingTableFromMap = true;
    this->clear();
    currentRow = 0;
    std::map<string, Column *>::iterator it;
    for (it = Column::theColumns.begin(); it != Column::theColumns.end(); it++) {
        Column *theColumn = it->second;

            QString name(QString::fromStdString((theColumn->name)));
            QString floor1(QString::fromStdString((theColumn->floor1)));
            QString floor2(QString::fromStdString((theColumn->floor2)));
            QString cline(QString::fromStdString((theColumn->cline)));
            QString section(QString::fromStdString((theColumn->sections[0])));

            theSpreadsheet->setString(currentRow, 0, name);
            theSpreadsheet->setString(currentRow, 1, cline);
            theSpreadsheet->setString(currentRow, 2, floor1);
            theSpreadsheet->setString(currentRow, 3, floor2);
            theSpreadsheet->setString(currentRow, 4, section);
            theSpreadsheet->setDouble(currentRow, 5, theColumn->angles[0]);

            currentRow++;
        }
     fillingTableFromMap = false;

    return true;
}

void
ColumnInputWidget::clear(void)
{
    theSpreadsheet->clear();
}


void
ColumnInputWidget::somethingEntered(int row, int column, int row2, int col2) {
    if (column == 0) {
        if (theSpreadsheet->getString(row, column, currentName) == false)
            currentName.clear();
    } else
        currentName.clear();
}

void
ColumnInputWidget::somethingChanged(int row, int column) {

    if (fillingTableFromMap == true) {
        return;
    }
    QString name;
    QString cline;
    QString floor1;
    QString floor2;
    QString section;
    double angle;

    string *sections = NULL;
    double *angles = NULL;
    double *ratios = NULL;
    int numSegment = 0;

/*
    QString name;

    double E = 0., Fy =0., Fu = 0., Nu = 0.,  Mass = 0.;
    QString rvE, rvFy, rvFu, rvNu, rvMass;
    string *rvEString = NULL, *rvFyString = NULL, *rvFuString = NULL, *rvNuString = NULL, *rvMassString = NULL;
*/
    QTableWidgetItem *theName = theSpreadsheet->item(row, 0);
    QTableWidgetItem *theCline = theSpreadsheet->item(row,1);
    QTableWidgetItem *theFloor1 = theSpreadsheet->item(row,2);
    QTableWidgetItem *theFloor2 = theSpreadsheet->item(row,3);
    QTableWidgetItem *theSection = theSpreadsheet->item(row,4);
    QTableWidgetItem *theAngle = theSpreadsheet->item(row,5);

    //
    // make sure name exists and is unique
    //   if not unique reset to last value and return w/o doing anything
    //

    // check for name field on row entered
    if (theName == NULL) {
        return; // do not add Column until all data exists
    }

    if (theSpreadsheet->getString(row,0,name) == false) {
        return; // problem with name
    }

    // check name not empty
    if (column == 0) {
        string theStringName = name.toStdString();
        if ((theStringName.empty()) || (theStringName.find_first_not_of(' ') == std::string::npos)) {
            theSpreadsheet->setString(row, 0, currentName);
            return;
        }
    }

    // check name is  unique, if not set string to what it was before entry
    if (column == 0) {
        Column *existingColumn = Column::getColumn(name.toStdString());
        if (existingColumn != NULL) {
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
    // if data exists, get it from cells in spreadsheet row, otherwise return if no data
    //

    if (theCline == NULL || theFloor1 == NULL || theFloor2 == NULL || theSection == NULL || theAngle == NULL)
        return;

    if (theSpreadsheet->getString(row,1,cline) == false) {
        qDebug() << "NO Cline";
        return;
    }
    if (theSpreadsheet->getString(row,2,floor1) == false) {
        qDebug() << "NO Floor1";
        return;
    }
    if (theSpreadsheet->getString(row,3,floor2) == false) {
        qDebug() << "NO Floor1";
        return;
    }
    if (theSpreadsheet->getString(row,4,section) == false) {
        qDebug() << "NO Section";
        return;
    }
    if (theSpreadsheet->getDouble(row,5,angle) == false) {
        qDebug() << "NO Angle";
        return;
    }

    if (column == 0) { // if modified the name, need to remove old before can add as would leave add there
        if (currentName != name) {
            Column::removeColumn(currentName.toStdString());
            qDebug() << " removing Column " << currentName << " to be replaced with: " << name;
        }
    }

    //
    // add the beam
    //
    Column::addColumn(name.toStdString(), cline.toStdString(), floor1.toStdString(), floor2.toStdString(), section.toStdString(), angle);

    currentName = name; // so don't enter again

}

