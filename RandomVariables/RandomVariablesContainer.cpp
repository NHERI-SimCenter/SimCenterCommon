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
// padhye modified

//#include "InputWidgetUQ.h"
#include "RandomVariablesContainer.h"
#include "ConstantDistribution.h"
#include "NormalDistribution.h"
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QDebug>
#include <sectiontitle.h>
#include <QLineEdit>
#include <QTableWidget>
#include <QDialog>
#include <QGridLayout>
#include <QHeaderView>
#include <QApplication>

// To check validity of correlation matrix
#include <Eigen/Dense>

RandomVariablesContainer::RandomVariablesContainer(QWidget *parent)
    : SimCenterWidget(parent), correlationDialog(NULL), correlationMatrix(NULL), checkbox(NULL)
{
    randomVariableClass = QString("Uncertain");
    uqEngineName=QString("Dakota");
    verticalLayout = new QVBoxLayout();
    this->setLayout(verticalLayout);
    verticalLayout->setMargin(0);
    this->makeRV();
}

RandomVariablesContainer::RandomVariablesContainer(QString &theClass, QString uqgin, QWidget *parent)
    : SimCenterWidget(parent), correlationDialog(NULL), correlationMatrix(NULL), checkbox(NULL)
{
    randomVariableClass = theClass;
    uqEngineName=uqgin;
    verticalLayout = new QVBoxLayout();
    this->setLayout(verticalLayout);
    this->makeRV();
}

void
RandomVariablesContainer::addConstantRVs(QStringList &varNamesAndValues)
{
    int numVar = varNamesAndValues.count();
    for (int i=0; i<numVar; i+= 2) {

        QString varName = varNamesAndValues.at(i);
        QString value = varNamesAndValues.at(i+1);

        double dValue = value.toDouble();
        ConstantDistribution *theDistribution = new ConstantDistribution(dValue, 0);
        RandomVariable *theRV = new RandomVariable(randomVariableClass, varName, *theDistribution, uqEngineName);

        this->addRandomVariable(theRV);
    }
}

void
RandomVariablesContainer::addRandomVariable(QString &varName) {

    NormalDistribution *theDistribution = new NormalDistribution();
    RandomVariable *theRV = new RandomVariable(randomVariableClass, varName, *theDistribution, uqEngineName);

    this->addRandomVariable(theRV);
}

void
RandomVariablesContainer::removeRandomVariable(QString &varName)
{
    //
    // find the RV, if refCout > 1 decrement refCount otherwise remove and delete the RV
    //

    int numRandomVariables = theRandomVariables.size();
    for (int j =0; j < numRandomVariables; j++) {
        RandomVariable *theRV = theRandomVariables.at(j);
        if (theRV->variableName->text() == varName) {
            if (theRV->refCount > 1) {
                theRV->refCount = theRV->refCount-1;
            } else {
                theRV->close();
                rvLayout->removeWidget(theRV);
                theRandomVariables.remove(j);
                theRV->setParent(0);
                delete theRV;

                // remove name from List
                randomVariableNames.removeAt(j);

                // remove row & col from correlation matrix
                if (correlationMatrix != NULL) {
                    correlationMatrix->removeRow(j);
                    correlationMatrix->removeColumn(j);
                    correlationMatrix->setHorizontalHeaderLabels(randomVariableNames);
                    correlationMatrix->setVerticalHeaderLabels(randomVariableNames);
                }

            }
            j=numRandomVariables; // get out of loop if foud
        }
    }
}


void
RandomVariablesContainer::removeRandomVariables(QStringList &varNames)
{
    //
    // just loop over list, get varName & invoke removeRandomVariable with varName
    //

    int numVar = varNames.count();
    for (int i=0; i<numVar; i++) {
        QString varName = varNames.at(i);
        this->removeRandomVariable(varName);
    }
}


RandomVariablesContainer::~RandomVariablesContainer()
{
  qDebug() << "RandomVariablesContainer::~RandomVariablesContainer()";
}

// see the RandomVariablesContainer.h and this a private member function
void
RandomVariablesContainer::makeRV(void)
{
    // title & add button
    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setMargin(10);

    SectionTitle *title=new SectionTitle();
    title->setText(tr("Input Random Variables"));
    title->setMinimumWidth(250);
    QSpacerItem *spacer1 = new QSpacerItem(50,10);
    QSpacerItem *spacer2 = new QSpacerItem(20,10);
    QSpacerItem *spacer3 = new QSpacerItem(50,10);


    QPushButton *addRV = new QPushButton();
    addRV->setMinimumWidth(75);
    addRV->setMaximumWidth(75);
    addRV->setText(tr("Add"));
    connect(addRV,SIGNAL(clicked()),this,SLOT(addRandomVariable()));


    QPushButton *removeRV = new QPushButton();
    removeRV->setMinimumWidth(75);
    removeRV->setMaximumWidth(75);
    removeRV->setText(tr("Remove"));
    connect(removeRV,SIGNAL(clicked()),this,SLOT(removeRandomVariable()));


    // padhye, adding the button for correlation matrix, we need to add a condition here
    // that whether the uqMehod selected is that of Dakota and sampling type? only then we need correlation matrix

    /* FMK */
    QPushButton *addCorrelation = new QPushButton(tr("Correlation Matrix"));
    connect(addCorrelation,SIGNAL(clicked()),this,SLOT(addCorrelationMatrix()));

    flag_for_correlationMatrix=1;

    /********************* moving to sampling method input ***************************
    QCheckBox *checkbox =new QCheckBox("Sobolev Index", this);
    connect(checkbox,SIGNAL(clicked(bool)),this,SLOT(addSobolevIndices(bool)));
    flag_for_sobolev_indices=0;
    ******************************************************************************** */

    titleLayout->addWidget(title);
    titleLayout->addItem(spacer1);
    titleLayout->addWidget(addRV);
    titleLayout->addItem(spacer2);
    titleLayout->addWidget(removeRV);
    titleLayout->addItem(spacer3);

    //titleLayout->addWidget(addCorrelation,0,Qt::AlignTop);
    QString appName = QApplication::applicationName();
    if (appName == "quoFEM")
        titleLayout->addWidget(addCorrelation);

    titleLayout->addStretch();

    verticalLayout->addLayout(titleLayout);

    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    //rv = new QGroupBox(tr(""));
    rv = new QWidget;

    rvLayout = new QVBoxLayout;
    rvLayout->addStretch();
    rv->setLayout(rvLayout);

   // this->addRandomVariable();
     sa->setWidget(rv);
     verticalLayout->addWidget(sa);
     verticalLayout->setSpacing(0);
     verticalLayout->setMargin(0);

}


void
RandomVariablesContainer::variableNameChanged(const QString &newValue) {

    Q_UNUSED(newValue);

    int numRandomVariables = theRandomVariables.size();


    if(correlationMatrix!=NULL) {

        QStringList table_header;
        for (int i = 0; i < numRandomVariables; i++) {

            //     qDebug()<< "\n the variable name is       "<<theRandomVariables.at(i)->getVariableName();
            table_header.append(theRandomVariables.at(i)->getVariableName());
            // RandomVariable *theRV = theRandomVariables.at(i);
            //if (theRV->isSelectedForRemoval())
            //{
            //   theRV->close();
            // rvLayout->removeWidget(theRV);
            // theRandomVariables.remove(i);
            // theRV->setParent(0);
            // delete theRV;
            //}
        }
        // qDebug()<<"\n the table_header is       "<<table_header;
        correlationMatrix->setHorizontalHeaderLabels(table_header);
        correlationMatrix->setVerticalHeaderLabels(table_header);
    }

}


void
RandomVariablesContainer::addRandomVariable(void) {

    RandomVariable *theRV = new RandomVariable(randomVariableClass, uqEngineName);
    theRandomVariables.append(theRV);
    rvLayout->insertWidget(rvLayout->count()-1, theRV);
    connect(this,SLOT(randomVariableErrorMessage(QString)), theRV, SIGNAL(sendErrorMessage(QString)));

    connect(theRV->variableName, SIGNAL(textEdited(const QString &)), this, SLOT(variableNameChanged(const QString &)));

    //if(uq["uqType"].toString()=="sampling")
    {

        if(correlationMatrix!=NULL)
        {

            int numRandomVariables=theRandomVariables.size();

            correlationMatrix->insertRow(numRandomVariables-1);

            correlationMatrix->insertColumn(numRandomVariables-1);


            //correlationMatrix->takeHorizontalHeaderItem(numRandomVariables-1);// takeHorizontalHeaderItem(int column)
            //correlationMatrix->takeVerticalHeaderItem(numRandomVariables-1);



            QStringList table_header;
            for (int i = 0; i < numRandomVariables; i++)
            {

                table_header.append(theRandomVariables.at(i)->getVariableName());

            }

            correlationMatrix->setHorizontalHeaderLabels(table_header);
            correlationMatrix->setVerticalHeaderLabels(table_header);

            for(int i = 0; i < numRandomVariables-1; i++)
            {
                correlationMatrix->setColumnWidth(i,100);
                QTableWidgetItem *newItem1,*newItem2;
                newItem1 = new QTableWidgetItem("0.0");                      // lower triangle
                correlationMatrix->setItem(numRandomVariables-1,i,newItem1);
                newItem2 = new QTableWidgetItem("0.0");                      // upper triangle
                newItem2->setFlags(newItem2->flags() ^ Qt::ItemIsEditable);  // FIX upper triangle
                correlationMatrix->setItem(i,numRandomVariables-1,newItem2);
                correlationMatrix->item(i,numRandomVariables-1)->setBackground(Qt::gray); // fix values
            }

            correlationMatrix->setColumnWidth(numRandomVariables-1,100);
            QTableWidgetItem *newItem;

            newItem = new QTableWidgetItem("1.0");                          // diagonal
            newItem->setFlags(newItem->flags() ^ Qt::ItemIsEditable);       // FIX diagonal
            correlationMatrix->setItem(numRandomVariables-1,numRandomVariables-1, newItem);
            correlationMatrix->item(numRandomVariables-1,numRandomVariables-1)->setBackground(Qt::gray);

            correlationMatrix->resizeColumnsToContents();
            correlationMatrix->resizeRowsToContents();

        }
    }
    //
}



void RandomVariablesContainer::removeRandomVariable(void)
{
    // find the ones selected & remove them
    int numRandomVariables = theRandomVariables.size();

    int *index_selected_to_remove;int size_selected_to_remove=0;

    index_selected_to_remove = (int *)malloc(numRandomVariables*sizeof(int));

    for (int i = numRandomVariables-1; i >= 0; i--) {
        qDebug()<<"\n the value of i is     "<<i;
        RandomVariable *theRV = theRandomVariables.at(i);
        if (theRV->isSelectedForRemoval()) {
            theRV->close();
            rvLayout->removeWidget(theRV);
            theRandomVariables.remove(i);
            theRV->setParent(0);
            delete theRV;
            index_selected_to_remove[size_selected_to_remove]=i;

            size_selected_to_remove=size_selected_to_remove+1;

        }
    }

    if(correlationMatrix!=NULL) {
        int counter_for_removal=size_selected_to_remove-1;//=size_selected_to_remove;
        while (counter_for_removal>=0) {
            correlationMatrix->removeRow(index_selected_to_remove[counter_for_removal]);
            correlationMatrix->removeColumn(index_selected_to_remove[counter_for_removal]);

            if(counter_for_removal>0) {

                for (int ii=0;ii<size_selected_to_remove;++ii) {
                    index_selected_to_remove[ii]=index_selected_to_remove[ii]-1;
                }

            }
            counter_for_removal--;

        }
    }


    free(index_selected_to_remove);
}


void
RandomVariablesContainer::addRandomVariable(RandomVariable *theRV) {

    if (randomVariableNames.contains(theRV->variableName->text())) {

        //
        // if exists, get index and increment refCount of current RV, deletig new
        //

        int index = randomVariableNames.indexOf(theRV->variableName->text());
        RandomVariable *theCurrentRV = theRandomVariables.at(index);
        theCurrentRV->refCount = theCurrentRV->refCount+1;
        delete theRV;

    } else {

        //
        // if does not exist we add it
        //    set refCount to 1, don;t allow others to edit it, set connections & finally and add at end
        //

        theRandomVariables.append(theRV);
        rvLayout->insertWidget(rvLayout->count()-1, theRV);
        theRV->refCount = 1;
        theRV->variableName->setReadOnly(true);

        connect(this,SLOT(randomVariableErrorMessage(QString)), theRV, SIGNAL(sendErrorMessage(QString)));
        connect(theRV->variableName, SIGNAL(textEdited(const QString &)), this, SLOT(variableNameChanged(const QString &)));

        randomVariableNames << theRV->variableName->text();


        //
        // update correlation matrix if set
        //

        int numRVs = randomVariableNames.size();

        if (correlationMatrix != NULL) {

            correlationMatrix->insertRow(correlationMatrix->rowCount());
            correlationMatrix->insertColumn(correlationMatrix->columnCount());
            correlationMatrix->setHorizontalHeaderLabels(randomVariableNames);
            correlationMatrix->setVerticalHeaderLabels(randomVariableNames);

            for (int i=0; i<numRVs-1; i++) {
                correlationMatrix->setItem(numRVs-1, i, new QTableWidgetItem(QString("0.0")));
                correlationMatrix->setItem(i,numRVs-1, new QTableWidgetItem(QString("0.0")));
            }
            correlationMatrix->setItem(numRVs-1,numRVs-1, new QTableWidgetItem(QString("1.0")));
        }
    }
}


// correlation matrix function
void RandomVariablesContainer::addCorrelationMatrix(void) {

    int numRandomVariables = theRandomVariables.size();

    if(correlationDialog==NULL && numRandomVariables>0) {

        correlationDialog = new QDialog(this);
        correlationDialog->setModal(true);
        correlationDialog->setWindowTitle(tr("Correlation Matrix"));
        QGridLayout *correlationLayout = new QGridLayout();
        QHBoxLayout *checkCorrLayout = new QHBoxLayout();

        // correlation matrix
        correlationMatrix = new QTableWidget;

        // push button
        QPushButton *correlationButton = new QPushButton("  OK  ");

        // error message
        correlationError = new QLabel();

        checkCorrLayout->addWidget(correlationButton,0);
        checkCorrLayout->addWidget(correlationError,1);
        correlationLayout->addWidget(correlationMatrix,0,0);
        correlationLayout->addLayout(checkCorrLayout,1,0);
        correlationDialog->setLayout(correlationLayout);

        flag_for_correlationMatrix=1;

        correlationMatrix->setRowCount(numRandomVariables);
        correlationMatrix->setColumnCount(numRandomVariables);

        QStringList table_header;

        for (int i = 0; i < numRandomVariables; i++)
            table_header.append(theRandomVariables.at(i)->getVariableName());

        correlationMatrix->setHorizontalHeaderLabels(table_header);
        correlationMatrix->setVerticalHeaderLabels(table_header);

        correlationMatrix->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        //correlationMatrix->setSizeAdjustPolicy(QTableWidget::AdjustToContents);
        //correlationMatrix->setSizePolicy(QSizePolicy::Policy::Minimum,QSizePolicy::Policy::Minimum);

        for(int i = 0; i < numRandomVariables; i++) {
            correlationMatrix->setColumnWidth(i,100 );
            for(int j = 0; j < i+1; j++) {
                QTableWidgetItem *newItem, *newItem2;

                if (i==j) { // Not editable
                    newItem = new QTableWidgetItem("1.0");
                    newItem->setFlags(newItem->flags() ^ Qt::ItemIsEditable);
                    correlationMatrix->setItem(i,j, newItem);
                    correlationMatrix->item(i,j)->setBackground(Qt::gray);
                } else {
                    newItem = new QTableWidgetItem("0.0");
                    correlationMatrix->setItem(i,j, newItem);
                    newItem2 = new QTableWidgetItem("0.0");
                    newItem2->setFlags(newItem2->flags() ^ Qt::ItemIsEditable);
                    correlationMatrix->setItem(j,i, newItem2);
                    correlationMatrix->item(j,i)->setBackground(Qt::gray);
                }
            }
        }

        correlationMatrix->resizeColumnsToContents();
        correlationMatrix->resizeRowsToContents();

        connect(correlationMatrix,SIGNAL(cellChanged(int, int)),this,SLOT(makeCorrSymmetric(int,int)));
        connect(correlationButton,SIGNAL(clicked()),this,SLOT(checkCorrValidity()));
    }
    if (correlationDialog != NULL)
        correlationDialog->show();

}

// automatically fill in the upper triangle of corr. matrix

void RandomVariablesContainer::makeCorrSymmetric(int i, int j) {
    // If lower triangle part changes, make this matrix symmetric
    if (i>j) {
        // Check validity of input
        QString valueStr = (((correlationMatrix->item(i,j))->text()));
        bool isdouble = 1;
        if ((valueStr.toDouble()-("1"+valueStr+"1").toDouble())==0.0) {
            isdouble = 0;
        }
        double value = valueStr.toDouble();

        // If input is invalid, make it zero
        if ((abs(value)>=1.0) || !isdouble) {
            valueStr="0.0";
            QTableWidgetItem *cellItem1= new QTableWidgetItem(valueStr);
            correlationMatrix->setItem(i,j, cellItem1);

            correlationError->setText("should be in range (-1,1)");
            correlationError->setStyleSheet("color : red; }");
        } else {
        // Make value at (j,i) = value at (i,j)
            QTableWidgetItem *cellItem2= new QTableWidgetItem(valueStr);
            cellItem2->setFlags(cellItem2->flags() ^ Qt::ItemIsEditable);

            correlationMatrix->setItem(j,i, cellItem2);
            correlationMatrix->item(j,i)->setBackground(Qt::gray);
            correlationError->setText("");
        }
    }
}

// Check validity of corr. matrix (Valid if all eigen values are non-negative)

void
RandomVariablesContainer::checkCorrValidity(void) {

    int dim=theRandomVariables.size();
    Eigen::MatrixXd correlationData(dim,dim);
    for (int i = 0; i <dim; ++i) {
        for (int j = 0; j <dim; ++j) {
            QTableWidgetItem *cellItemFromTable=correlationMatrix->item(i,j);
            if (cellItemFromTable!=NULL) {
                correlationData(i,j)=((cellItemFromTable->text()).toDouble());
            } else {
                correlationData(i,j) = 0.;
            }
        }
    }

    // compute the Cholesky decomposition of correlation matrix
    Eigen::LLT<Eigen::MatrixXd> llt(correlationData);
    if(llt.info() == Eigen::NumericalIssue)
    {
        correlationError->setText("should be positive definite.");
        correlationError->setStyleSheet("color : red; }");
    } else {
        correlationError->setText("");
        correlationDialog->hide();
    }

}

// loop over random variables, removing from layout & deleting

void
RandomVariablesContainer::clear(void) {

  // loop over random variables, removing from layout & deleting
  for (int i = 0; i <theRandomVariables.size(); ++i) {
    RandomVariable *theRV = theRandomVariables.at(i);
    rvLayout->removeWidget(theRV);
    delete theRV;
  }

  theRandomVariables.clear();
  randomVariableNames.clear();

  if (correlationMatrix != NULL) {
       rvLayout->removeWidget(correlationMatrix);
       delete correlationMatrix;
       correlationMatrix = NULL;
  }
}


// this will fill in the information of random variables into the json


bool
RandomVariablesContainer::outputToJSON(QJsonObject &rvObject) {

    bool result = true;
    QJsonArray rvArray;
    for (int i = 0; i <theRandomVariables.size(); ++i) {
        QJsonObject rv;
        if (theRandomVariables.at(i)->outputToJSON(rv)) {
            rvArray.append(rv);
        } else {
            qDebug() << "OUTPUT FAILED" << theRandomVariables.at(i)->variableName->text();
            result = false;
        }
    }

    rvObject["randomVariables"]=rvArray;

    if (correlationMatrix != NULL) {
      
      qDebug() << "WRITING CORRELATION MATRIX";

        QJsonArray correlationData;
        for (int i = 0; i <theRandomVariables.size(); ++i) {
            for (int j = 0; j <theRandomVariables.size(); ++j) {

                QTableWidgetItem *cellItemFromTable=correlationMatrix->item(i,j);
                double value = 0.0;
                if (cellItemFromTable!=NULL) {
                    value=((cellItemFromTable->text()).toDouble());
		} else {
		  value = 0.;
		}
		correlationData.append(value);
            }
        }
        rvObject["correlationMatrix"]=correlationData;
    }

    return result;
}


QStringList
RandomVariablesContainer::getRandomVariableNames(void)
{
    QStringList results;
    for (int i = 0; i <theRandomVariables.size(); ++i) {
        results.append(theRandomVariables.at(i)->getVariableName());
    }
    return results;
}

int
RandomVariablesContainer::getNumRandomVariables(void)
{
    return theRandomVariables.size(); 
}

bool
RandomVariablesContainer::inputFromJSON(QJsonObject &rvObject)
{
  bool result = true;

  // clean out current list
  this->clear();

  //
  // go get randomvariables array from the JSON object
  // for each object in array:
  //    1)get it'is type,
  //    2)instantiate one
  //    4) get it to input itself
  //    5) finally add it to layout
  //


  // get randomVariables & add
  int numRandomVariables = 0;
  if (rvObject.contains("randomVariables")) {
      if (rvObject["randomVariables"].isArray()) {

          QJsonArray rvArray = rvObject["randomVariables"].toArray();

          // foreach object in array
          foreach (const QJsonValue &rvValue, rvArray) {

              QJsonObject rvObject = rvValue.toObject();

              if (rvObject.contains("variableClass")) {
                  QJsonValue typeRV = rvObject["variableClass"];
                  RandomVariable *theRV = 0;
                  QString classType = typeRV.toString();
                  theRV = new RandomVariable(classType,uqEngineName);
                  connect(theRV->variableName, SIGNAL(textEdited(const QString &)), this, SLOT(variableNameChanged(const QString &)));

                  connect(theRV,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));

                  if (theRV->inputFromJSON(rvObject)) { // this method is where type is set
                      theRandomVariables.append(theRV);
                      randomVariableNames << theRV->variableName->text();
                      theRV->variableName->setReadOnly(true);

                      rvLayout->insertWidget(rvLayout->count()-1, theRV);
                      numRandomVariables++;
                  } else {
                      result = false;
                  }
              } else {
                  result = false;
              }
          }
      }
  }

  // get correlationMatrix if present and add data if it is int
  if (rvObject.contains("correlationMatrix")) {

      if (rvObject["correlationMatrix"].isArray()) {

          correlationDialog = NULL; // reset correlationDialog
          this->addCorrelationMatrix();

          QJsonArray rvArray = rvObject["correlationMatrix"].toArray();
          // foreach object in array
          //int row = 0; int col = 0;

          //foreach (const QJsonValue &rvValue, rvArray) {
         //     double value = rvValue.toDouble();
         //     QTableWidgetItem *item = correlationMatrix->item(row,col);
          //    item->setText(QString::number(value));
          //    col++;
         //     if (col == numRandomVariables) {
          //        row++; col=0;
          //    }
         // }

      for (int row=0; row<numRandomVariables; row++) {
             for (int col=0; col<row; col++) {
                 double value = rvArray[col+row*numRandomVariables].toDouble();
                 QTableWidgetItem *item = correlationMatrix->item(row,col);
                 item->setText(QString::number(value));
             }
      }


      }
      // hide the dialog so matrix not shown
      correlationDialog->hide();
  }
  return result;
}


void
RandomVariablesContainer::errorMessage(QString message){
    emit sendErrorMessage(message);
}

