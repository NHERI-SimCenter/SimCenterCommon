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
#include <QHeaderView>
#include <QJsonObject>
#include <QGridLayout>
#include <QPushButton>

SC_TableEdit::SC_TableEdit(QString theKey, QStringList colHeadings, int numRows, double *data)
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


  QPushButton *addB = new QPushButton("Add");    
  QPushButton *delB = new QPushButton("Del");
 
  QGridLayout *layout = new QGridLayout();
  layout->addWidget(theTable, 0,0);
  layout->addWidget(addB,0,1);
  layout->addWidget(delB,0,2);  
  
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

    return true;
}

bool
SC_TableEdit::inputFromJSON(QJsonObject &jsonObject)
{

    return true;
}

