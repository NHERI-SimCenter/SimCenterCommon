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

#include "SC_TableEdit.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QJsonObject>
#include <QGridLayout>
#include <QPushButton>
#include <QJsonArray>
#include <QDebug>
#include <QLabel>

SC_TableEdit::SC_TableEdit(QString theKey, QStringList colHeadings, int numRows, QStringList dataValues, QStringList *special)
  :QWidget()
{
  key = theKey;

  QStringList headings;
  int numCols = 0;
  foreach (const QString &theHeading, colHeadings) {
    headings << theHeading;
    numCols++;
  }

  theTable = new QTableWidget();
  theTable->setColumnCount(numCols);
  theTable->setRowCount(numRows);
  theTable->setHorizontalHeaderLabels(headings);
  theTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  theTable->verticalHeader()->setVisible(false);

  // fill in data
  for (int i=0; i<numRows; i++) {
    for (int j=0; j<numCols; j++) {
      QString entry = dataValues.at(i*numCols+j);
      QTableWidgetItem *cellItem = new QTableWidgetItem();
      cellItem->setText(entry);
      theTable->setItem(i,j,cellItem);
    }
  }  

  QGridLayout *layout = new QGridLayout();
  QPushButton *addB = nullptr;
  QPushButton *delB = nullptr;
  
  if (special == nullptr || special->size() != 3) {
    
    addB = new QPushButton("Add");    
    delB = new QPushButton("Del");
    
    layout->addWidget(theTable, 0,0);
    layout->addWidget(addB,0,1);
    layout->addWidget(delB,0,2);
    
  } else {

    //
    // if special
    //   add label for table heading (special(0)), put buttons on top with special labels (specials(1) and (2))
    //
    
    layout->addWidget(new QLabel(special->at(0)), 0,0);
    addB = new QPushButton(special->at(1));
    delB = new QPushButton(special->at(2));
    layout->addWidget(addB,0,2);
    layout->addWidget(delB,0,3);    
    
    layout->addWidget(theTable,1,0,1,4);
    layout->setColumnStretch(1,1);
    theTable->verticalHeader()->setVisible(true);    
  }
  
  this->setLayout(layout);

  connect(addB, &QPushButton::released, this, [=]() {
    int row = theTable->currentRow();
    if (row != -1)  // insert below selected
      theTable->insertRow(row+1);
    else            // if none selected, add to end
      theTable->insertRow(theTable->rowCount());      
  });

  connect(delB, &QPushButton::released, this, [=]() {
    int row = theTable->currentRow();
    if ((row != -1) && (theTable->rowCount() != 1)) // don't delete if only 1 row
      theTable->removeRow(row);
  });  
}


SC_TableEdit::~SC_TableEdit()
{

}


bool
SC_TableEdit::outputToJSON(QJsonObject &jsonObject)
{
  int numRow = theTable->rowCount();
  int numColumn = theTable->columnCount();
  //  qDebug() << "SC_TableEdit::outputToJson: numRow: " << numRow << " numCol: " << numCol;
  QJsonArray theArray;
  
  for (int i=0; i<numRow; i++) {

    // add each row as a JSON array, writing double if double
    QJsonArray theRowArray;
    for (int j=0; j<numColumn; j++) {
      QTableWidgetItem *value = theTable->item(i,j);
      QString valueText = value->text();
      bool ok;
      double valueDouble = valueText.QString::toDouble(&ok);
      if (ok == true)
        theRowArray += valueDouble;
      else      
	theRowArray += valueText;
    };
    
    theArray += theRowArray;
  };
    
  jsonObject[key] = theArray;
  
  return true;
}

bool
SC_TableEdit::inputFromJSON(QJsonObject &jsonObject)
{
   if (jsonObject.contains(key)) {

     QJsonValue theValue = jsonObject[key];
     
       if (!theValue.isArray()) {
           return false;
       }

       QJsonArray theArray = theValue.toArray();
       QJsonValue firstRow = theArray.at(0);
       QJsonArray firstRowArray = firstRow.toArray();
       int numRows = theArray.size();       
       int numCols = firstRowArray.size();

       theTable->clearContents();
       theTable->setRowCount(numRows);
       theTable->setColumnCount(numCols);       

       for (int i=0; i<numRows; i++) {
	 
	 QJsonValue theRowValue = theArray.at(i);
	 if (!theRowValue.isArray()) {
           return false;
	 }
	 QJsonArray theRowArray = theRowValue.toArray();
	 int numColsRow = theRowArray.size();
	 if (numColsRow != numCols)
	   return false;
	 
	 for (int j=0; j<numCols; j++) {

	   QTableWidgetItem *cellItem = new QTableWidgetItem();
	   
	   QJsonValue theItemValue = theRowArray.at(j);
	   if (theItemValue.isString()) {
	     cellItem->setText(theItemValue.toString());
	   } else if (theItemValue.isDouble()) {
	     cellItem->setText(QString::number(theItemValue.toDouble()));
	   }
	   theTable->setItem(i,j,cellItem);
	 }
       }
   } else {
     qDebug() << "SC_TableEdit::inputFromJSON - no key: " << key << " found in input";
   }
   
   return true;
}

