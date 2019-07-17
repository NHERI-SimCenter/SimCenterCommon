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

#include "InputWidgetSheetSIM.h"
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
#include <QTreeView>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QModelIndex>


#include "ClineInputWidget.h"
#include "FloorInputWidget.h"
#include "BeamInputWidget.h"
#include "WallInputWidget.h"
#include "ColumnInputWidget.h"
#include "BraceInputWidget.h"
#include "SteelInputWidget.h"
#include "ConcreteInputWidget.h"
//#include "FramesectionInputWidget.h"
#include "SlabsectionInputWidget.h"
#include "WallsectionInputWidget.h"
#include "ConnectionInputWidget.h"
#include "PointInputWidget.h"
#include "SpreadsheetWidget.h"
#include "SteelWSectionInputWidget.h"
#include "SteelTubeSectionInputWidget.h"

/*
 * if you encounter a compile error in the below include
 * statement, get the missing files from:
 *
 *    https://github.com/akheron/jansson
 *
 * Read the install instructions at the bottom of that page.
 */
#include <jansson.h>
#include <BimClasses.h>

class RandomVariablesContainer;

InputWidgetSheetSIM::InputWidgetSheetSIM(RandomVariablesContainer *theRandomVariableIW, QWidget *parent)
    : SimCenterAppWidget(parent), currentWidget(0), theRandomVariablesContainer(theRandomVariableIW)
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
  QStandardItem *layoutItem   = new QStandardItem("Layout");
  QStandardItem *geometryItem = new QStandardItem("Geometry");
  QStandardItem *propertiesItem = new QStandardItem("Properties");
  QStandardItem *materialsItem = new QStandardItem("Materials");
  QStandardItem *framesectionsItem = new QStandardItem("Framesections");
  QStandardItem *wallsectionsItem = new QStandardItem("Wallsections");
  QStandardItem *connectionsItem = new QStandardItem("Connections");


  rootNode->appendRow(layoutItem);
  infoItemIdx = rootNode->index();

  layoutItem->appendRow(new QStandardItem("Floors"));
  layoutItem->appendRow(new QStandardItem("Clines"));

  rootNode->appendRow(geometryItem);
  geometryItem->appendRow(new QStandardItem("Beams"));
  geometryItem->appendRow(new QStandardItem("Columns"));
  geometryItem->appendRow(new QStandardItem("Walls"));
  geometryItem->appendRow(new QStandardItem("Braces"));

  rootNode->appendRow(propertiesItem);
  propertiesItem->appendRow(materialsItem);
  materialsItem->appendRow(new QStandardItem("Concrete"));
  materialsItem->appendRow(new QStandardItem("Steel"));


  propertiesItem->appendRow(framesectionsItem);
  framesectionsItem->appendRow(new QStandardItem("Steel W Section"));
  framesectionsItem->appendRow(new QStandardItem("Steel Tube Section"));
  /*

  QStringList framesectionTypes(std::initializer_list<QString>({
        "Concrete Rectangular Column", "Concrete Box Column",
        "Concrete Circular Column", "Concrete Pipe Column",
        "Concrete Rectangular Beam", "Concrete Tee Beam",
        "Concrete L Beam", "Concrete Cross Beam", "Steel Wide Flange",
        "Steel Channel", "Steel Double Channel", "Steel Tee",
        "Steel Angle", "Steel Double Angle", "Steel Tube",
        "Filled Steel Tube", "Steel Pipe", "Filled Steel Pipe",
        "Steel Plate", "Steel Rod", "Buckling Restrained Brace"
        }));
  for (int i=0; i<framesectionTypes.size(); i++) {
      framesectionsItem->appendRow(new QStandardItem(framesectionTypes[i]));
      theFramesectionTypes << framesectionTypes[i].toLower();
  }
*/
  propertiesItem->appendRow(new QStandardItem("Slabsections"));

  propertiesItem->appendRow(wallsectionsItem);
  QStringList wallsectionTypes(std::initializer_list<QString>({
        "Concrete Rectangular Wall", "Concrete Flanged Wall",
        "Concrete Barbell Wall"}));
  for (int i=0; i<wallsectionTypes.size(); i++) {
      wallsectionsItem->appendRow(new QStandardItem(wallsectionTypes[i]));
      theWallsectionTypes << wallsectionTypes[i].toLower();
  }

  propertiesItem->appendRow(connectionsItem);
  QStringList connectionTypes(std::initializer_list<QString>({
        "Gusset with Foldline", "Gusset without Foldline",
        "Baseplate Gusset with Foldline", "Baseplate Gusset without Foldline",
        "Welded Shear Tab", "Bolted Shear Tab"}));
  for (int i=0; i<connectionTypes.size(); i++) {
      connectionsItem->appendRow(new QStandardItem(connectionTypes[i]));
      theConnectionTypes << connectionTypes[i].toLower();
  }

  propertiesItem->appendRow(new QStandardItem("Points"));

  //register the model
  treeView->setModel(standardModel);
  treeView->expandAll();
  treeView->setHeaderHidden(true);
  treeView->setMaximumWidth(250);

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
  theClineInput = new ClineInputWidget();
  theFloorInput = new FloorInputWidget();
  theBeamInput = new BeamInputWidget();
  theBraceInput = new BraceInputWidget();
  theWallInput = new WallInputWidget();
  theColumnInput = new ColumnInputWidget();
  theSteelInput = new SteelInputWidget();
  theConcreteInput = new ConcreteInputWidget();
  theSteelWSectionInput = new SteelWSectionInputWidget();
   theSteelTubeSectionInput = new SteelTubeSectionInputWidget();
  /*
  for (int i=0; i<theFramesectionTypes.size(); i++) {
      theFramesectionInputs[theFramesectionTypes[i]] = new FramesectionInputWidget(theFramesectionTypes[i]);
  }
  */
  theSlabsectionInput = new SlabsectionInputWidget();
  for (int i=0; i<theWallsectionTypes.size(); i++) {
      theWallsectionInputs[theWallsectionTypes[i]] = new WallsectionInputWidget(theWallsectionTypes[i]);
  }
  for (int i=0; i<theConnectionTypes.size(); i++) {
      theConnectionInputs[theConnectionTypes[i]] = new ConnectionInputWidget(theConnectionTypes[i]);
  }
  thePointInput = new PointInputWidget();


  treeView->setCurrentIndex( infoItemIdx );
}

InputWidgetSheetSIM::~InputWidgetSheetSIM()
{

}

/*
void InputWidgetSheetSIM::setMainWindow(MainWindow* main)
{
    window = main;
    treeView->setCurrentIndex( infoItemIdx );
    //treeView->selectionModel()->select(infoItemIdx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

    //treeView->selectionModel()->setCurrentIndex(infoItemIdx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

    //QItemSelection selection (infoItemIdx, infoItemIdx);
    //selectionChangedSlot(selection)

}
*/




const SpreadsheetWidget * InputWidgetSheetSIM::getActiveSpreadsheet()
{
    return currentWidget->getSpreadsheetWidget();
}

void InputWidgetSheetSIM::selectionChangedSlot(const QItemSelection & /*newSelection*/, const QItemSelection & /*oldSelection*/)
{

    //get the text of the selected item
    const QModelIndex index = treeView->selectionModel()->currentIndex();
    QString selectedText = index.data(Qt::DisplayRole).toString();
    //qDebug() << "new tree selection: " + selectedText;

    // remove current widget from layout

    if (currentWidget != 0) {
        if (currentWidget != (SimCenterTableWidget *) theClineInput ) {
            //qDebug() << "disconnect edit menu items ";
            // up call to detach the MainWindow Edit menun from this sheet
            emit disconnectMenuItems(currentWidget);
        }
        horizontalLayout->removeWidget(currentWidget);
        currentWidget->setParent(0);
    }


    //get the text of the selected item
    //const QModelIndex index = treeView->selectionModel()->currentIndex();
    //QString selectedText = index.data(Qt::DisplayRole).toString();

    // add the user selected widget for editing
    if (selectedText == tr("Clines")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theClineInput, 1);
        currentWidget = theClineInput;
    } else if (selectedText == tr("Floors")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theFloorInput, 1);
        currentWidget = theFloorInput;
    } else if (selectedText == tr("Beams")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theBeamInput, 1);
        currentWidget = theBeamInput;
    } else if (selectedText == tr("Columns")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theColumnInput, 1);
        currentWidget = theColumnInput;
    } else if (selectedText == tr("Walls")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theWallInput, 1);
        currentWidget = theWallInput;
    } else if (selectedText == tr("Braces")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theBraceInput, 1);
        currentWidget = theBraceInput;
    } else if (selectedText == tr("Steel")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theSteelInput, 1);
        currentWidget = theSteelInput;
    } else if (selectedText == tr("Concrete")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theConcreteInput, 1);
        currentWidget = theConcreteInput;
    } else if (selectedText == tr("Steel W Section")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theSteelWSectionInput, 1);
        currentWidget = theSteelWSectionInput;
    } else if (selectedText == tr("Steel Tube Section")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theSteelTubeSectionInput, 1);
        currentWidget = theSteelTubeSectionInput;
        /*
    } else if (theFramesectionTypes.contains(selectedText.toLower())) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theFramesectionInputs[selectedText.toLower()], 1);
        currentWidget = theFramesectionInputs[selectedText.toLower()];
        */
    } else if (selectedText == tr("Slabsections")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theSlabsectionInput, 1);
        currentWidget = theSlabsectionInput;
    } else if (theWallsectionTypes.contains(selectedText.toLower())) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theWallsectionInputs[selectedText.toLower()], 1);
        currentWidget = theWallsectionInputs[selectedText.toLower()];
    } else if (theConnectionTypes.contains(selectedText.toLower())) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, theConnectionInputs[selectedText.toLower()], 1);
        currentWidget = theConnectionInputs[selectedText.toLower()];
    } else if (selectedText == tr("Points")) {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, thePointInput, 1);
        currentWidget = thePointInput;
    }

    if (currentWidget != 0) {
        // up call to connect the MainWindow Edit menu to this sheet
        emit connectMenuItems(currentWidget);
    }
}



// NOTE: method to be redone when all these spreadsheet widgets classes have migrated to just being
// views, and the C++ objects write themselves. method for Data
bool
InputWidgetSheetSIM::outputToJSON(QJsonObject &jsonObject)
{
    jsonObject["type"]="SimCenterSIM";

    //QJsonObject jsonObject;

    // add layout
    QJsonObject jsonObjLayout;
    theClineInput->outputToJSON(jsonObjLayout);
    theFloorInput->outputToJSON(jsonObjLayout);
    jsonObject["layout"]=jsonObjLayout;

    // add geometry
    QJsonObject jsonObjGeometry;

    //
    // add the beams
    //    warning: following is a kludgy hack as mixing jansson and QJSON.
    //

    // wrte beam data to jansson object,
    json_t *objBeams = json_array();
    Beam::writeObjects(objBeams);
    json_t *objJanBeams = json_object();
    json_object_set(objJanBeams, "beam", objBeams);

    // get the string of that jansson object
    char *jsonTextBeams = json_dumps(objJanBeams, JSON_COMPACT);

    // use the string to create a QJsonDoc and then from that document get a JSON object
    QJsonDocument docBeams = QJsonDocument::fromJson(jsonTextBeams);
    if (docBeams.isNull()) {
       qDebug() << "Beam invalid JSON";
    }
    QJsonObject objQtBeams = docBeams.object();

    // get the JSON object "beam" and write to the QJSON object,
    //   freeing up memory that json_dumps created as we go
    QJsonArray theBeamsArray = objQtBeams["beam"].toArray();
    free(jsonTextBeams);
    jsonObjGeometry["beams"]=theBeamsArray;

    //
    // now columns .. same ugly code .. see beam above for doc
    //

    json_t *objColumns = json_array();
    Column::writeObjects(objColumns);
    json_t *objJanColumns = json_object();
    json_object_set(objJanColumns, "column", objColumns);
    char *jsonTextColumns = json_dumps(objJanColumns, JSON_COMPACT);
    QJsonDocument docColumns = QJsonDocument::fromJson(jsonTextColumns);
    if (docColumns.isNull()) {
       qDebug() << "Column invalid JSON";
    }
    QJsonObject objQtColumns = docColumns.object();
    QJsonArray theColumnsArray = objQtColumns["column"].toArray();
    free(jsonTextColumns);
    jsonObjGeometry["columns"]=theColumnsArray;

    //
    // now wall .. same ugly code .. see beam above for doc
    //

    json_t *objWalls = json_array();
    Wall::writeObjects(objWalls);
    json_t *objJanWalls = json_object();
    json_object_set(objJanWalls, "wall", objWalls);
    char *jsonTextWalls = json_dumps(objJanWalls, JSON_COMPACT);
    QJsonDocument docWalls = QJsonDocument::fromJson(jsonTextWalls);
    if (docWalls.isNull()) {
       qDebug() << "Wall invalid JSON";
    }
    QJsonObject objQtWalls = docWalls.object();
    QJsonArray theWallsArray = objQtWalls["wall"].toArray();
    free(jsonTextWalls);
    jsonObjGeometry["walls"]=theWallsArray;

    /*
    theBeamInput->outputToJSON(jsonObjGeometry);
    theColumnInput->outputToJSON(jsonObjGeometry);
    theBraceInput->outputToJSON(jsonObjGeometry);
    theWallInput->outputToJSON(jsonObjectGeomtry)
    */

    jsonObject["geometry"]=jsonObjGeometry;

    // add properties
    QJsonObject jsonObjProperties;
    theSlabsectionInput->outputToJSON(jsonObjProperties);

    thePointInput->outputToJSON(jsonObjProperties);

    /*
    QJsonArray theFramesectionsArray;
    jsonObjProperties["framesections"]=theFramesectionsArray;

    for (int i=0; i<theFramesectionTypes.size(); i++) {
        theFramesectionInputs[theFramesectionTypes[i]]->outputToJSON(theFramesectionsArray);
    }

    jsonObjProperties["framesections"]=theFramesectionsArray;
    */

    QJsonArray theWallsectionsArray;
    jsonObjProperties["wallsections"]=theWallsectionsArray;

    for (int i=0; i<theWallsectionTypes.size(); i++) {
        theWallsectionInputs[theWallsectionTypes[i]]->outputToJSON(theWallsectionsArray);
    }
    jsonObjProperties["wallsections"]=theWallsectionsArray;

    QJsonArray theConnectionsArray;
    jsonObjProperties["connections"]=theConnectionsArray;

    for (int i=0; i<theConnectionTypes.size(); i++) {
        theConnectionInputs[theConnectionTypes[i]]->outputToJSON(theConnectionsArray);
    }
    jsonObjProperties["connections"]=theConnectionsArray;

    //
    // create a json array and get all material inputs to enter their data
    //

    QJsonArray theMaterialsArray;
    jsonObjProperties["materials"]=theMaterialsArray;

   // Material::theMaterials outputToJson will do what outputToJson did
   // i.e. replace:
   //    theSteelInput->outputToJSON(theMaterialsArray);
   //    theConcreteInput->outputToJSON(theMaterialsArray);
   // with:
   //     use jansson to set up a jansson JSON object

    // kinda ugly, due to mixing JSON libs, write to jansson, convert to char *, and use that string to encode to QJson

    json_t *objMaterials = json_array();
    Materiall::writeObjects(objMaterials);
    json_t *objJan = json_object();

    json_object_set(objJan, "materials", objMaterials);

    // dump that jansson object to a QJsonDoc and convertinto QJson object
    char *jsonText = json_dumps(objJan, JSON_COMPACT);

    QJsonDocument doc = QJsonDocument::fromJson(jsonText);

    if (doc.isNull()) {
       qDebug() << "Floor invalid JSON";
    }

    QJsonObject objQt = doc.object();
    theMaterialsArray = objQt["materials"].toArray();

    // free memory that json_dumps allocaed
    free(jsonText);

    jsonObjProperties["materials"]=theMaterialsArray;

    //
    // now FrameSections, same as materials
    //

    QJsonArray theFrameSectionsArray;
    jsonObjProperties["frameSections"]=theFrameSectionsArray;

    json_t *objFrameSections = json_array();
    FrameSection::writeObjects(objFrameSections);
    json_t *objJanFS = json_object();

    json_object_set(objJanFS, "frameSections", objFrameSections);

    // dump that jansson object to a QJsonDoc and convertinto QJson object
    char *jsonTextFrameSections = json_dumps(objJanFS, JSON_COMPACT);

    QJsonDocument docFS = QJsonDocument::fromJson(jsonTextFrameSections);

    if (docFS.isNull()) {
       qDebug() << "FrameSectuons invalid JSON";
    }

    QJsonObject objQtFS = docFS.object();
    theFrameSectionsArray = objQtFS["frameSections"].toArray();

    // free memory that json_dumps allocaed
    free(jsonTextFrameSections);

    jsonObjProperties["frameSections"]=theFrameSectionsArray;

    jsonObject["properties"]=jsonObjProperties;

   // jsonObjectTop["StructuralInformation"] = jsonObject;

    return true;
}

void
InputWidgetSheetSIM::clear(void)
{
    theClineInput->clear();
    theFloorInput->clear();
    theColumnInput->clear();
    theBeamInput->clear();
    theBraceInput->clear();
    theWallInput->clear();
    theSteelInput->clear();
    theConcreteInput->clear();
    theSteelWSectionInput->clear();
    theSteelTubeSectionInput->clear();
    /*
    for (int i=0; i<theFramesectionTypes.size(); i++) {
        theFramesectionInputs[theFramesectionTypes[i]]->clear();
    }
    */

    theSlabsectionInput->clear();

    for (int i=0; i<theWallsectionTypes.size(); i++) {
        theWallsectionInputs[theWallsectionTypes[i]]->clear();
    }

    for (int i=0; i<theConnectionTypes.size(); i++) {
        theConnectionInputs[theConnectionTypes[i]]->clear();
    }

    thePointInput->clear();

    if (jsonObjOrig) {
        delete jsonObjOrig;
    }

    Materiall::removeAllMaterial();
    FrameSection::removeAllFrameSection();
    Beam::removeAllBeam();
    Column::removeAllColumn();
    Wall::removeAllWall();
    Floor::removeAllFloor();
}

bool
InputWidgetSheetSIM::inputFromJSON(QJsonObject &jsonObjStructuralInformation)
{

   QJsonObject jsonObjLayout = jsonObjStructuralInformation["layout"].toObject();
   theClineInput->inputFromJSON(jsonObjLayout);
   theFloorInput->inputFromJSON(jsonObjLayout);

   //
   // parse the properties
   //

   QJsonObject jsonObjProperties = jsonObjStructuralInformation["properties"].toObject();
   theSlabsectionInput->inputFromJSON(jsonObjProperties);
   thePointInput->inputFromJSON(jsonObjProperties);

   /*
   QJsonArray theFramesectionsArray = jsonObjProperties["framesections"].toArray();
   for (int i=0; i<theFramesectionTypes.size(); i++) {
       theFramesectionInputs[theFramesectionTypes[i]]->inputFromJSON(theFramesectionsArray);
   }
*/

   QJsonArray theWallsectionsArray = jsonObjProperties["wallsections"].toArray();
   for (int i=0; i<theWallsectionTypes.size(); i++) {
       theWallsectionInputs[theWallsectionTypes[i]]->inputFromJSON(theWallsectionsArray);
   }

   QJsonArray theConnectionsArray = jsonObjProperties["connections"].toArray();
   for (int i=0; i<theConnectionTypes.size(); i++) {
       theConnectionInputs[theConnectionTypes[i]]->inputFromJSON(theConnectionsArray);
   }

   // first the materials
   // get the array and for every object in array determine it's type and get
   // the approprate inputwidget to parse the data
   //

   // ugly code again as mixing libraries, take QJson, convert to string, and load into jansson

   QJsonArray theMaterialArray = jsonObjProperties["materials"].toArray();
   QJsonDocument doc(theMaterialArray);
   QString strJson(doc.toJson(QJsonDocument::Compact));

   json_t *janssonObj = json_object();
   json_error_t error;
   janssonObj = json_loads(strJson.toStdString().c_str(), 0, &error);
   //qDebug() << json_dumps(janssonObj, JSON_COMPACT);

   Materiall::removeAllMaterial();
   Materiall::readObjects(janssonObj, Materiall::theMaterials);

   // now get tables to fill themselves in
   QJsonObject blank;
   theSteelInput->inputFromJSON(blank);
   theConcreteInput->inputFromJSON(blank);

   //
   // now the frame sections
   //

   QJsonArray theFrameSectionsArray = jsonObjProperties["frameSections"].toArray();
   QJsonDocument docFS(theFrameSectionsArray);
   QString strJsonFS(docFS.toJson(QJsonDocument::Compact));
   //qDebug() << "strJSONFS: " << strJsonFS;

   json_t *janssonObjFS = json_object();
   janssonObjFS = json_loads(strJsonFS.toStdString().c_str(), 0, &error);
   //qDebug() << "FRAMESECTIONS: " << json_dumps(janssonObjFS, JSON_COMPACT);

   FrameSection::removeAllFrameSection();
   FrameSection::readObjects(janssonObjFS, FrameSection::theFrameSections);

   theSteelWSectionInput->inputFromJSON(blank);
   theSteelTubeSectionInput->inputFromJSON(blank);

   //
   // parse the geometry
   //

   QJsonObject jsonObjGeometry = jsonObjStructuralInformation["geometry"].toObject();

   // first beams .. kludgy again
   QJsonArray theBeamArray = jsonObjGeometry["beams"].toArray();
   json_t *janssonObjBeam = json_object();

   if (janssonObjBeam != NULL) {
     QJsonDocument docBeam(theBeamArray);
     QString strJsonBeam(docBeam.toJson(QJsonDocument::Compact));
     janssonObjBeam = json_loads(strJsonBeam.toStdString().c_str(), 0, &error);
   }

   Beam::removeAllBeam();
   Beam::readObjects(janssonObjBeam, Beam::theBeams);

   // now columns .. again kludgy
   QJsonArray theColumnArray = jsonObjGeometry["columns"].toArray();
   json_t *janssonObjColumn = json_object();

   if (janssonObjColumn != NULL) {
       QJsonDocument docColumn(theColumnArray);
       QString strJsonColumn(docColumn.toJson(QJsonDocument::Compact));
       janssonObjColumn = json_loads(strJsonColumn.toStdString().c_str(), 0, &error);
   }

   Column::removeAllColumn();
   Column::readObjects(janssonObjColumn, Column::theColumns);

   // now walls
   QJsonArray theWallArray = jsonObjGeometry["walls"].toArray();
   json_t *janssonObjWall = json_object();
   if (janssonObjWall != NULL) {
       QJsonDocument docWall(theWallArray);
       QString strJsonWall(docWall.toJson(QJsonDocument::Compact));
       janssonObjWall = json_loads(strJsonWall.toStdString().c_str(), 0, &error);
   }
   Wall::removeAllWall();
   Wall::readObjects(janssonObjWall, Wall::theWalls);


   theColumnInput->inputFromJSON(jsonObjGeometry);
   theBeamInput->inputFromJSON(blank);
   theBraceInput->inputFromJSON(jsonObjGeometry);
   theWallInput->inputFromJSON(jsonObjGeometry);

   return true;
}


