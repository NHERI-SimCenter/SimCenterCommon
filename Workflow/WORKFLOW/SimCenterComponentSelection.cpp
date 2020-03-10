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

#include <SimCenterComponentSelection.h>
#include <QHBoxLayout>
#include <QTreeView>
#include <QTreeView>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <CustomizedItemModel.h>
#include <QModelIndex>
#include <QStackedWidget>
#include <QDebug>

SimCenterComponentSelection::SimCenterComponentSelection(QWidget *parent)
    :QWidget(parent)
{

  QHBoxLayout *horizontalLayout = new QHBoxLayout();

  //  
  // create a TreeView widget 
  //

  treeView = new QTreeView();
  standardModel = new CustomizedItemModel; 
  rootNode = standardModel->invisibleRootItem();

  infoItemIdx = rootNode->index();

  //register the model                                                                                             
  treeView->setModel(standardModel);
  treeView->expandAll();
  treeView->setHeaderHidden(true);
  treeView->setMinimumWidth(100);
  treeView->setMaximumWidth(100);
  treeView->setMinimumWidth(100);
  treeView->setEditTriggers(QTreeView::EditTrigger::NoEditTriggers);//Disable Edit 

  //                                                                                                               
  // customize the apperance of the menu on the left                                                               
  //                                                                                                               

  treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff ); // hide the horizontal scroll bar               
  treeView->setObjectName("treeViewOnTheLeft");
  treeView->setIndentation(0);

  QItemSelectionModel *selectionModel= treeView->selectionModel();
  connect(selectionModel,
	  SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
	  this,
	  SLOT(selectionChangedSlot(const QItemSelection &, const QItemSelection &)));

  // add the TreeView widget to the layout
  horizontalLayout->addWidget(treeView);

  theStackedWidget = new QStackedWidget();
  horizontalLayout->addWidget(theStackedWidget);

  this->setLayout(horizontalLayout);
}

SimCenterComponentSelection::~SimCenterComponentSelection()
{

}


bool
SimCenterComponentSelection::addComponent(QString text, QWidget *theWidget)
{
    if (textIndices.indexOf(text) == -1) {
        QStandardItem *theItem = new QStandardItem(text);
        rootNode->appendRow(theItem);
        QModelIndex modelIndex = theItem->index();
        theStackedWidget->addWidget(theWidget);
        textIndices.append(text);
        modelIndices.append(modelIndex);
        return true;
    } else
        qDebug() << "ComponentSelection: text: " << text << " option already exists";

    return false;
}

void
SimCenterComponentSelection::selectionChangedSlot(const QItemSelection &, const QItemSelection &)
{
    //
    // get the text of the selected item
    //

    const QModelIndex index = treeView->selectionModel()->currentIndex();

    QString selectedText = index.data(Qt::DisplayRole).toString();

    //
    // find text in list
    //

    int stackIndex = textIndices.lastIndexOf(selectedText);

    //
    // get stacked widget to display current if of course it exists
    //

    if (stackIndex != -1)
        theStackedWidget->setCurrentIndex(stackIndex);
}

QWidget *
SimCenterComponentSelection::swapComponent(QString text, QWidget *theWidget)
{
    QWidget *theRes = NULL;

    //
    // find text iin list
    //

    int index = textIndices.indexOf(text);

    //
    // get stacked widget to display current if of course it exists
    //

    if (index != -1) {
        theRes=theStackedWidget->widget(index);
        if (theRes != NULL)
            theStackedWidget->removeWidget(theRes);
        theStackedWidget->insertWidget(index, theWidget);
    }
    return theRes;
}

bool
SimCenterComponentSelection::displayComponent(QString text)
{
    //
    // find index of text in list and display corresponding widget if index found
    //

    int index = textIndices.indexOf(text);

    if (index != -1) {

        QModelIndex index1 = modelIndices.at(index);
        treeView->setCurrentIndex(index1);
        // theStackedWidget->setCurrentIndex(index);
        return true;
    }

    return false;
}
