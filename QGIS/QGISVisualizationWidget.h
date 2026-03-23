#ifndef QGISVISUALIZATIONWIDGET_H
#define QGISVISUALIZATIONWIDGET_H
/* *****************************************************************************
Copyright (c) 2016-2021, The Regents of the University of California (Regents).
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
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

// Written by: Stevan Gavrilovic

#include "VisualizationWidget.h"
#include "qgssinglesymbolrenderer.h"

#include <qgsfeatureid.h>
#include <qgsgeometry.h>
#include <qgspointxy.h>
#include <qgsmarkersymbollayer.h>
#include <qgsfeatureiterator.h>

#include <QPointer>

class GISSelectable;
class SimCenterMapcanvasWidget;

class QMainWindow;
class QComboBox;

class QgsRasterLayer;
class QgisApp;
class QgsLayerTreeView;
class QgsMapCanvasItem;
class QgsVectorLayer;
class QgsSymbol;
class QgsMapLayer;
class QgsMapCanvas;
class QgsLayerTreeGroup;

class QGISVisualizationWidget : public VisualizationWidget
{
public:
    QGISVisualizationWidget(QMainWindow *parent = nullptr);
    ~QGISVisualizationWidget();

    QWidget *getVisWidget();
    double getLatFromScreenPoint(const QPointF& point, const QgsMapCanvas* canvas);
    double getLongFromScreenPoint(const QPointF& point, const QgsMapCanvas* canvas);

    QPointF getScreenPointFromLatLong(const double& latitude, const double& longitude, const QgsMapCanvas* canvas);

    void clear(void);
    void clearSelection(void);

    // If you want the QGIS widget to send back selected features, you need to register the layer and corresponding input widget
    void registerLayerForSelection(const QString layerId, GISSelectable* widget);
    int deregisterLayerForSelection(const QString layerId);

    // The layerPath is a path to a file if the vector layer is being loaded from a file
    // Otherwise, if the layer is being created programatically within R2D, the layerPath needs to define the type of geometry that will be present in the layer, e.g., "point", "linestring", "polygon","multipoint","multilinestring","multipolygon"
    // The provider key tells QGIS where the layer data is coming from. Examples of provider keys used here in R2D include:
    //     1) Use provider key "ogr" if the layer is a vector layer being loaded from a file on the users computer
    //     2) Use provider key "memory" if layer is created within the R2D program
    //     3) Use provider key "wms" if layer is loaded from an online 'wms' GIS server, e.g., basemaps
    //     4) Use provider key "gdal" if the layer is a raster layer being loaded from a file on the users computer
    QgsVectorLayer* addVectorLayer(const QString &layerPath, const QString &name, const QString &providerKey = "memory");


    QgsRasterLayer* addRasterLayer(const QString &layerPath, const QString &name, const QString &providerKey);

    // Duplicates an exisiting layer and returns the layer. Returns a nullptr if resulting layer is invalid.
    // Note: creates a deep copy, and therefore may take a while.
    // If you want a superficial copy where two layers share the same data source use the layer->clone() functionality, e.g., just to render the layer differently
    QgsVectorLayer* duplicateExistingLayer(QgsVectorLayer* layer);

    // Adds a new map layer to the project, useful when cloning or duplicating an exisiting layer
    void addMapLayer(QgsMapLayer* layer);

    QgsGeometry getPolygonGeometryFromJson(const QString& geoJson);
    QgsGeometry getPolygonGeometryFromJson(const QJsonArray& geoJson);
    QgsGeometry getMultilineStringGeometryFromJson(const QJsonArray& geoJson);
    QgsGeometry getMultilineStringGeometryFromJson(const QString& geoJson);

    void removeLayer(QgsMapLayer* layer);

    SimCenterMapcanvasWidget* getMapViewWidget(const QString& name);

    void markDirty();

    void zoomToExtent(QgsRectangle zoomRectangle);

    void zoomToActiveLayer(void);

    void setActiveLayerFillNull(void);

    void createSymbolRenderer(Qgis::MarkerShape symbolShape, QColor color, double size, QgsVectorLayer * layer);

    void createPrettyGraduatedRenderer(const QString attrName, const QColor color1, const QColor color2, const int nclasses, QgsVectorLayer* vlayer, QgsSymbol* symbol = nullptr);

    void createCategoryRenderer(const QString attrName, QgsVectorLayer * vlayer, QgsSymbol* symbol, QVector<QColor> colors = QVector<QColor>());

    int createCustomClassBreakRenderer(const QString attrName, QgsVectorLayer * vlayer, Qgis::SymbolType symbolType, const QVector<QPair<double,double>>& classBreaks, const QVector<QColor>& colors, const QVector<QString> labels  = QVector<QString>(), double size = 2.0);

    // The same symbol will be used to render every feature
    void createSimpleRenderer(QgsSymbol* symbol, QgsVectorLayer * layer);
    void createSimpleRenderer(QColor color, QgsVectorLayer * layer);

    void setLayerVisibility(QgsMapLayer* layer, bool value);

    // Create a group of layers
    // Must pass at least one layer
    QgsLayerTreeGroup* createLayerGroup(const QVector<QgsMapLayer*>& layers, const QString groupName);
    QgsLayerTreeGroup* getLayerGroup(const QString groupName);
    // BZ - 221220
    QList<QgsMapLayer*> addVectorInGroup(const QString &layerPath, const QString &name, const QString &providerKey = "memory");
    void removeLayerGroup(const QString& name);

    void turnOnSelectionTool();

    void setActiveLayer(QgsMapLayer* layer);

    void selectLayerInTree(QgsMapLayer* layer);

    void deselectAllTreeItems(void);

    QgisApp* getQgis(void);

    QgsMapCanvas* getMainCanvas(void);

    void showFeaturePopUp(QgsFeatureIterator& features);

    QgsLayerTreeView *getLayerTreeView() const;

    // Returns the value of the raster layer in the given band
    // Note that band numbers start from 1 and not 0!
    double sampleRaster(const double& x, const double& y, QgsRasterLayer* rasterlayer, const int& bandNumber);

    // Adds new or additional feature attributes to exisiting features in a layer
    // There has to be as many attribute values as there are features in the exisiting layer
    // The attributes should be sorted according to feature id, ascending from lowest to highest
    int addNewFeatureAttributesToLayer(QgsVectorLayer* layer, const QStringList& fieldNames, const QVector<QgsAttributes>& values, QString& error);

    // Join layers - the feature attributes from the left layer will be added to the right layer
    int joinLayers(const QgsVectorLayer* left, QgsVectorLayer* right, const QString& tag, QString& errMsg);

public slots:

    void handleSelectButton(void);
    void handleIdentifyButton(void);

    void handleBasemapSelection(int index);

    void handleSelectAssetsMap(void);
    void handleClearAssetsMap(void);

    void selectionChanged(const QgsFeatureIds &selected, const QgsFeatureIds &deselected, bool clearAndSelect);

    void handleLegendChange(const QString layerUID);
    void zoomToLayer(const QString layerID);
    void zoomToLayer(QgsMapLayer* layer);

private:
    QgsLayerTreeView* layerTreeView = nullptr;
    QgsMapLayer* baseMapLayer = nullptr;

    void moveLayerToBottom(const QString& layerID);

    QPointer<QgisApp> qgis;

    QComboBox* baseMapCombo = nullptr;

    void testVectorLayer();
    SimCenterMapcanvasWidget* testNewMapCanvas();
    void testNewMapCanvas2();

    QModelIndex getLayerIndex(QgsMapLayer* layer);

    // Select layers programmatically
    void selectLayersInTree(const QVector<QgsMapLayer*>& layers);

    QMap<QString,GISSelectable*> mapSelectableAssetWidgets;

    // Returns the map point in WGS84 format (lat/lon) of a screen point
    QgsPointXY getPointFromScreenCoord(const QPoint& point, const QgsMapCanvas* canvas);

    // Returns the screen point from a map point in WGS84 format (lat/lon)
    QgsPointXY getScreenCoordFromPoint(const QgsPointXY& point, const QgsMapCanvas* canvas);
};

#endif // QGISVISUALIZATIONWIDGET_H
