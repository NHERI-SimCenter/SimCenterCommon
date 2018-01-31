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

#include "InputWidgetSheetBM.h"
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
#include <QTreeView>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QModelIndex>


#include "GeneralInformationWidget.h"
#include "ClineInputWidget.h"
#include "FloorInputWidget.h"
#include "BeamInputWidget.h"
#include "ColumnInputWidget.h"
#include "BraceInputWidget.h"
#include "SteelInputWidget.h"
#include "ConcreteInputWidget.h"
#include "SpreadsheetWidget.h"

InputWidgetSheetBM::InputWidgetSheetBM(QWidget *parent) : QWidget(parent), currentWidget(0)
{
  horizontalLayout = new QHBoxLayout();
  this->setLayout(horizontalLayout);

  //
  // create a tree widget, assign it a mode and add to layout
  //

  treeView = new QTreeView();
  standardModel = new QStandardItemModel ;
  QStandardItem *rootNode = standardModel->invisibleRootItem();

  //defining bunch of items for inclusion in model
  QStandardItem *infoItem    = new QStandardItem("GeneralInformation");
  //QStandardItem *unitsItem    = new QStandardItem("Units");
  QStandardItem *layoutItem   = new QStandardItem("Layout");
  QStandardItem *floorsItem   = new QStandardItem("Floors");
  QStandardItem *clinesItem   = new QStandardItem("Clines");
  QStandardItem *geometryItem = new QStandardItem("Geometry");
  QStandardItem *beamsItem    = new QStandardItem("Beams");
  QStandardItem *columnsItem  = new QStandardItem("Columns");
  QStandardItem *bracesItem   = new QStandardItem("Braces");
  QStandardItem *wallsItem    = new QStandardItem("Walls");
  QStandardItem *propertiesItem = new QStandardItem("Properties");
  QStandardItem *materialsItem = new QStandardItem("Materials");
  QStandardItem *steelItem = new QStandardItem("Steel");
  QStandardItem *concreteItem = new QStandardItem("Concrete");
  QStandardItem *sectionsItem  = new QStandardItem("Sections");
  QStandardItem *concRectBeamItem  = new QStandardItem("ConcreteRectangularBeam");
  QStandardItem *concTBeamItem  = new QStandardItem("ConcreteTBeam");
  QStandardItem *concRectColItem  = new QStandardItem("ConcreteRectangularColumn");

  //building up the hierarchy of the model
  rootNode->appendRow(infoItem);
  //rootNode->appendRow(unitsItem);
  rootNode->appendRow(layoutItem);
  layoutItem->appendRow(floorsItem);
  layoutItem->appendRow(clinesItem);
  rootNode->appendRow(geometryItem);
  geometryItem->appendRow(beamsItem);
  geometryItem->appendRow(columnsItem);
  geometryItem->appendRow(bracesItem);
  geometryItem->appendRow(wallsItem);
  rootNode->appendRow(propertiesItem);
  propertiesItem->appendRow(materialsItem);
  materialsItem->appendRow(concreteItem);
  materialsItem->appendRow(steelItem);
  propertiesItem->appendRow(sectionsItem);
  sectionsItem->appendRow(concRectBeamItem);
  sectionsItem->appendRow(concTBeamItem);
  sectionsItem->appendRow(concRectColItem);

  //register the model
  treeView->setModel(standardModel);
  treeView->expandAll();
  treeView->setHeaderHidden(true);
  treeView->setMaximumWidth(200);

  // set up so that a slection change triggers yje selectionChanged slot
  QItemSelectionModel *selectionModel= treeView->selectionModel();
  connect(selectionModel,
          SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
          this,
          SLOT(selectionChangedSlot(const QItemSelection &, const QItemSelection &)));

  // add the TreeView widget to the layout
 horizontalLayout->addWidget(treeView);
  horizontalLayout->addStretch();

  //
  // create the input widgets for the different types
  //
  theGeneralInformationInput = new GeneralInformationWidget();
  theClineInput = new ClineInputWidget();
  theFloorInput = new FloorInputWidget();
  theBeamInput = new BeamInputWidget();
  theBraceInput = new BraceInputWidget();
  theColumnInput = new ColumnInputWidget();
  theSteelInput = new SteelInputWidget();
  theConcreteInput = new ConcreteInputWidget();
}

InputWidgetSheetBM::~InputWidgetSheetBM()
{

}
void InputWidgetSheetBM::setMainWindow(MainWindow* main)
{
    window = main;
}

const SpreadsheetWidget * InputWidgetSheetBM::getActiveSpreadsheet()
{
    return currentWidget->getSpreadsheetWidget();
}


void InputWidgetSheetBM::selectionChangedSlot(const QItemSelection & /*newSelection*/, const QItemSelection & /*oldSelection*/)
{

    //get the text of the selected item
    const QModelIndex index = treeView->selectionModel()->currentIndex();
    QString selectedText = index.data(Qt::DisplayRole).toString();
    //qDebug() << "new tree selection: " + selectedText;

    // remove current widget from layout
    if (currentWidget != 0) {
        if (currentWidget != (SimCenterTableWidget *) theGeneralInformationInput ) {
            //qDebug() << "disconnect edit menu items ";
            window->disconnectMenuItems(currentWidget);
        }
        horizontalLayout->removeWidget(currentWidget);
        currentWidget->setParent(0);
    }

    //get the text of the selected item
    //const QModelIndex index = treeView->selectionModel()->currentIndex();
    //QString selectedText = index.data(Qt::DisplayRole).toString();

    // add the user selected widget for editing
    if (selectedText == tr("GeneralInformation")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theGeneralInformationInput, 1);
        currentWidget = (SimCenterTableWidget *) theGeneralInformationInput;
    } else if (selectedText == tr("Clines")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theClineInput, 1);
        currentWidget = theClineInput;
        window->connectMenuItems(currentWidget);
    } else if (selectedText == tr("Floors")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theFloorInput, 1);
        currentWidget = theFloorInput;
        window->connectMenuItems(currentWidget);
    } else if (selectedText == tr("Beams")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theBeamInput, 1);
        currentWidget = theBeamInput;
        window->connectMenuItems(currentWidget);
    } else if (selectedText == tr("Columns")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theColumnInput, 1);
        currentWidget = theColumnInput;
        window->connectMenuItems(currentWidget);
    } else if (selectedText == tr("Braces")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theBraceInput, 1);
        currentWidget = theBraceInput;
        window->connectMenuItems(currentWidget);
    } else if (selectedText == tr("Steel")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theSteelInput, 1);
        currentWidget = theSteelInput;
        window->connectMenuItems(currentWidget);
    } else if (selectedText == tr("Concrete")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theConcreteInput, 1);
        currentWidget = theConcreteInput;
        window->connectMenuItems(currentWidget);
    }
  }



void
InputWidgetSheetBM::outputToJSON(QJsonObject &jsonObjectTop)
{
    QJsonObject jsonObject;

    // add GeneralInformation
    QJsonObject jsonObjGenInfo;
    theGeneralInformationInput->outputToJSON(jsonObjGenInfo);
    jsonObjectTop["GeneralInformation"] = jsonObjGenInfo;

    // add layout
    QJsonObject jsonObjLayout;
    theClineInput->outputToJSON(jsonObjLayout);
    theFloorInput->outputToJSON(jsonObjLayout);
    jsonObject["layout"]=jsonObjLayout;

    // add geometry
    QJsonObject jsonObjGeometry;
    theBeamInput->outputToJSON(jsonObjGeometry);
    theColumnInput->outputToJSON(jsonObjGeometry);
    theBraceInput->outputToJSON(jsonObjGeometry);

    jsonObject["geometry"]=jsonObjGeometry;

    // add properties
    QJsonObject jsonObjProperties;

    //
    // create a json array and get all material inputs to enter their data
    //
    QJsonArray theMaterialsArray;
    jsonObjProperties["materials"]=theMaterialsArray;

    theSteelInput->outputToJSON(theMaterialsArray);
    theConcreteInput->outputToJSON(theMaterialsArray);
    jsonObjProperties["materials"]=theMaterialsArray;


    jsonObject["properties"]=jsonObjProperties;

    QJsonObject jsonObjStructInfo = (*jsonObjOrig)["StructuralInformation"].toObject();

    jsonObject["type"] = jsonObjStructInfo["type"];
    jsonObjectTop["StructuralInformation"] = jsonObject;

}

void
InputWidgetSheetBM::clear(void)
{
    theGeneralInformationInput->clear();
    theClineInput->clear();
    theFloorInput->clear();
    theColumnInput->clear();
    theBeamInput->clear();
    theBraceInput->clear();
    theSteelInput->clear();
    theConcreteInput->clear();
    if (jsonObjOrig) {
        delete jsonObjOrig;
    }
}

void
InputWidgetSheetBM::inputFromJSON(QJsonObject &jsonObject)
{
   jsonObjOrig = new QJsonObject(jsonObject);

   QJsonObject jsonObjGeneralInformation = jsonObject["GeneralInformation"].toObject();
   theGeneralInformationInput->inputFromJSON(jsonObjGeneralInformation);

   QJsonObject jsonObjStructuralInformation = jsonObject["StructuralInformation"].toObject();

   QJsonObject jsonObjLayout = jsonObjStructuralInformation["layout"].toObject();
   theClineInput->inputFromJSON(jsonObjLayout);
   theFloorInput->inputFromJSON(jsonObjLayout);

   //
   // parse the properties
   //

   QJsonObject jsonObjProperties = jsonObjStructuralInformation["properties"].toObject();

   // first the materials
   // get the array and for every object in array determine it's type and get
   // the approprate inputwidget to parse the data
   //

   QJsonArray theMaterialArray = jsonObjProperties["materials"].toArray();
   foreach (const QJsonValue &theValue, theMaterialArray) {

       QJsonObject theObject = theValue.toObject();
       QString theType = theObject["type"].toString();

       if (theType == QString(tr("steel"))) {
            theSteelInput->inputFromJSON(theObject);
       } else if (theType == QString(tr("concrete"))) {
           theConcreteInput->inputFromJSON(theObject);
      }
   }


   //
   // parse the geometry
   //

   QJsonObject jsonObjGeometry = jsonObjStructuralInformation["geometry"].toObject();
   theColumnInput->inputFromJSON(jsonObjGeometry);
   theBeamInput->inputFromJSON(jsonObjGeometry);
   theBraceInput->inputFromJSON(jsonObjGeometry);


}


