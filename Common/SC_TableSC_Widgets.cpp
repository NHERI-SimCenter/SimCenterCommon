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

#include "SC_TableSC_Widgets.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QJsonObject>
#include <QGridLayout>
#include <QPushButton>
#include <QJsonArray>
#include <QDebug>

#include <QLabel>
#include <QLineEdit>


SC_TableSC_Widgets::SC_TableSC_Widgets(QString theKey,
				       QStringList colHeadings,
				       bool addRemove)
{
  key = theKey;

  QStringList headings;
  numCols = 0;
  foreach (const QString &theHeading, colHeadings) {
    headings << theHeading;

    numCols++;
  }

  theTable = new QTableWidget();

  theTable->setColumnCount(numCols);
  theTable->setRowCount(0);
  theTable->setHorizontalHeaderLabels(headings);
  theTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  theTable->verticalHeader()->setVisible(false);
  
  QGridLayout *layout = new QGridLayout();

  if (addRemove == true){  
    QPushButton *addB = nullptr;
    QPushButton *delB = nullptr;

    addB = new QPushButton("Add");    
    delB = new QPushButton("Del");
    
    layout->addWidget(theTable, 0,0);
    layout->addWidget(addB,0,1);
    layout->addWidget(delB,0,2);
    

    connect(addB, &QPushButton::released, this, [=]() {
      emit addRow();
    });
    
    connect(delB, &QPushButton::released, this, [=]() {
      int row = theTable->currentRow();
      if (row != -1)      
	theTable->removeRow(row);
        theWidgets.removeAt(row);
    });  
  }
  
  this->setLayout(layout);

}


SC_TableSC_Widgets::~SC_TableSC_Widgets()
{

}

bool
SC_TableSC_Widgets::outputToJSON(QJsonObject &jsonObject)
{
  int numRow = theTable->rowCount();

  QJsonArray theArray;
  
  for (int row=0; row<numRow; row++) {
    QJsonObject rowData;
    for (int col=0; col<numCols; col++) {
      SC_WidgetJSON *theWidget = theWidgets[row][col];
      theWidget->outputToJSON(rowData);
    }
    theArray.append(rowData);
  }
    
  jsonObject[key] = theArray;
  
  return true;
}

bool
SC_TableSC_Widgets::inputFromJSON(QJsonObject &jsonObject)
{
  theTable->clearContents();
  theWidgets.clear();
  theTable->setColumnCount(numCols);            
     
   if (jsonObject.contains(key)) {
     
     QJsonValue theValue = jsonObject[key];
     
     if (!theValue.isArray()) {
       return false;
     }

     // add rows
     QJsonArray theArray = theValue.toArray();
     int numRows = theArray.size();
     for (int row=0; row<numRows; row++) {

       // get "owner" to add the row of widgets
       emit addRow();

       // get JSON object for row entry
       // and looping over widgets for that row get them to read their data
     
       QJsonValue theRowValue = theArray.at(row);
       if (!theRowValue.isObject())
	 return false;
       
       QJsonObject theRowObject = theRowValue.toObject();

       QList<SC_WidgetJSON *>theRowWidgets = theWidgets[row];
	 
       for (int col = 0; col < theRowWidgets.size(); col++) {
	 SC_WidgetJSON* theWidget = theRowWidgets[col];
	 if (theWidget) 
	   if (!theWidget->inputFromJSON(theRowObject)) {
	     qDebug() << "SC_TableSC_Widgets:: inputFromJSON .. empty ROW!";
	     return false;
	   }
       }
     }
   }
   
   return true;
}

bool
SC_TableSC_Widgets::addRowWidgets(QList<SC_WidgetJSON *>rowWidgets)
{
  if (rowWidgets.size() != numCols)
    return false;

  int row = theTable->rowCount();
  theTable->insertRow(row);
  theWidgets.append(rowWidgets);
  for (int col=0; col < numCols; col++) {
    SC_WidgetJSON *theWidget = rowWidgets[col];
    if (auto* qobjectable = dynamic_cast<QWidget*>(theWidget)) {
      theTable->setCellWidget(row,col,qobjectable);
    }
  }

  return true;
}
