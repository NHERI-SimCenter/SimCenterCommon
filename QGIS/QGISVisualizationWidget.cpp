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

#include "QGISVisualizationWidget.h"
#include "PopUpWidget.h"
#include "SimCenterMapcanvasWidget.h"
#include "GISSelectable.h"
#include <QPixMap>
#include <QPainter>
#include <QLinearGradient>
#include <QSplashScreen>

// Test to delete
//#include "RectangleGrid.h"
//#include "NodeHandle.h"
// Test to delete end

#include <qgsuserprofilemanager.h>
#include <qgslayertreeutils.h>
#include <qgslayertreeview.h>
#include <qgisapp.h>
#include <qgsmaplayer.h>
#include <qgslayertree.h>
#include <qgslayertreemodel.h>
#include <qgslayertreenode.h>
#include <qgslayertreegroup.h>
#include <qgslayertreeviewdefaultactions.h>
#include <qgsmapcanvas.h>
#include <qgsrubberband.h>
#include <qgsvertexmarker.h>
#include <qgslayertreemapcanvasbridge.h>
#include <qgsmaptooladdfeature.h>
#include <qgsmapcanvas.h>
#include <qgsfield.h>
#include <qgsvectorlayer.h>
#include <qgscategorizedsymbolrenderer.h>
#include <qgsmarkersymbol.h>
#include <qgsfillsymbol.h>
#include <qgslinesymbol.h>
#include <qgscolorramp.h>
#include <qgscolorrampimpl.h>
#include <qgsexpressioncontextutils.h>
#include <qgsrulebasedrenderer.h>
#include <qgsclassificationmethod.h>
#include <qgssymbollayerutils.h>
#include <qgsrendererrange.h>
#include <qgsgraduatedsymbolrenderer.h>
#include <qgsclassificationmethodregistry.h>
#include <qgsapplication.h>
#include <qgssinglesymbolrenderer.h>
#include <qgslayertreenode.h>
#include <qgssinglesymbolrenderer.h>
#include <qgsrasterlayer.h>
#include <qgsrasterdataprovider.h>
#include "qgsfillsymbollayer.h"
#include "qgsproviderregistry.h"
#include "qgsprovidersublayerdetails.h"
#include <qgscoordinatetransform.h>
#include <qgssinglesymbolrenderer.h>

#include <QStandardPaths>
#include <QSplitter>
#include <Utils/FileOperations.h>

QGISVisualizationWidget::QGISVisualizationWidget(QMainWindow *parent) : VisualizationWidget(parent)
{

    QSplitter* mainWidget = new QSplitter(this);
    mainWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    //mainLayout->setMargin(0);

    mainLayout->addWidget(mainWidget);

    bool mySkipVersionCheck = true;
    bool myRestorePlugins = true;
    auto profileName = QLatin1String( "default" );
    auto configLocalStorageLocation = QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ).value( 0 );
    QString rootProfileFolder = QgsUserProfileManager::resolveProfilesFolder( configLocalStorageLocation );

    QString appWorkDir = SCUtils::getAppWorkDir();


    QgsApplication::initQgis();    
    QgsApplication::setAuthDatabaseDirPath(appWorkDir);

    
    // qgis = new QgisApp(parent, myRestorePlugins, mySkipVersionCheck, rootProfileFolder, profileName, nullptr, Qt::Widget);

    /*
    QString splashPath( QgsCustomization::instance()->splashPath() );
    QPixmap pixmap( splashPath + u"splash.png"_s );
    int w = 600 * pixmap.devicePixelRatioF();
    int h = 300 * pixmap.devicePixelRatioF();
    */

    QPixmap pixmap(600, 300);
    pixmap.fill(Qt::white); // Background color
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    QLinearGradient gradient(0, 0, 0, 300);
    gradient.setColorAt(0, QColor(0, 51, 102)); // NHERI Dark Blue
    gradient.setColorAt(1, QColor(0, 102, 204)); // SimCenter Lighter Blue
    painter.fillRect(pixmap.rect(), gradient);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, "SimCenter | NHERI");
    painter.setFont(QFont("Arial", 10));
    painter.drawText(pixmap.rect().adjusted(0, 0, -10, -10), 
		     Qt::AlignBottom | Qt::AlignRight, "v2026.1.0 - Initializing...");
    painter.end();
    int w = 600 * pixmap.devicePixelRatioF();
    int h = 300 * pixmap.devicePixelRatioF();
    
    QSplashScreen *mypSplash = new QSplashScreen( pixmap.scaled( w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
  
    QgisApp::AppOptions options = QgisApp::AppOption::RestorePlugins | QgisApp::AppOption::SkipVersionCheck;
    qgis = new QgisApp(mypSplash, // splash
		       options,                              // options
		       rootProfileFolder,                    // rootProfileLocation
		       profileName,                          // activeProfile
		       parent,                               // parent
		       Qt::Widget                            // fl (WindowFlags)
		       );



    
    qgis->setObjectName( QStringLiteral( "QgisApp" ) );
    qgis->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    //    QThread *thread = new QThread();
    //    qgis->moveToThread(thread);
    //    QWidget::connect(thread, SIGNAL(finished()), qgis, SLOT(deleteLater()));

    //    thread->start();

    layerTreeView = qgis->layerTreeView();

    qgis->mapCanvas()->setCenter(QgsPointXY(37.8717450069,-122.2609607382));

//#ifdef OpenSRA
//    auto crs = QgsCoordinateReferenceSystem(QStringLiteral("EPSG:4326"));
//#else
    auto crs = QgsCoordinateReferenceSystem(QStringLiteral("EPSG:3857"));
//#endif

    qgis->mapCanvas()->setDestinationCrs(crs);

    auto leftHandWidget = new QWidget();
    leftHandWidget->setContentsMargins(0,0,0,0);

    QVBoxLayout* leftHandLayout = new QVBoxLayout(leftHandWidget);
    //leftHandLayout->setMargin(0);

    QLabel* basemapText = new QLabel(this);
    basemapText->setText("Add a basemap:");

    baseMapCombo = new QComboBox();
    baseMapCombo->addItem("OpenStreetMap");
    baseMapCombo->addItem("OpenStreetMap Topo");
    baseMapCombo->addItem("Google Satellite");
    baseMapCombo->addItem("Google Road");
    baseMapCombo->addItem("Google Road & Satellite");
    baseMapCombo->addItem("Bing Aerial");
    baseMapCombo->addItem("Stamen Toner Lite");
    baseMapCombo->addItem("ESRI World Topo");

    connect(baseMapCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &QGISVisualizationWidget::handleBasemapSelection);


    QPushButton *identifyAssets = new QPushButton("Identify",this);
    connect(identifyAssets,&QPushButton::pressed,this,&QGISVisualizationWidget::handleIdentifyButton);
    QPushButton *selectAssets = new QPushButton("Select",this);
    connect(selectAssets,&QPushButton::pressed,this,&QGISVisualizationWidget::handleSelectButton);

    auto lhbLayout = new QHBoxLayout();
    lhbLayout->addWidget(identifyAssets);
    lhbLayout->addWidget(selectAssets);

    QLabel* topText = new QLabel(this);
    topText->setText("Add selected assets to analysis list");
    topText->setStyleSheet("font-weight: bold; color: black; text-align: center");

    QPushButton *addAssetsButton = new QPushButton(this);
    addAssetsButton->setText(tr("Add Assets"));
    //    addAssetsButton->setMaximumWidth(150);


    QLabel* bottomText = new QLabel(this);
    bottomText->setText("Clear all selected assets");
    bottomText->setStyleSheet("font-weight: bold; color: black; text-align: center");

    QPushButton *clearButton = new QPushButton(this);
    clearButton->setText(tr("Clear"));
    //    clearButton->setMaximumWidth(150);

    connect(addAssetsButton,&QPushButton::clicked,this,&QGISVisualizationWidget::handleSelectAssetsMap);
    connect(clearButton,&QPushButton::clicked,this,&QGISVisualizationWidget::handleClearAssetsMap);

    leftHandLayout->addWidget(basemapText);
    leftHandLayout->addWidget(baseMapCombo);
    leftHandLayout->addWidget(layerTreeView);
    leftHandLayout->addLayout(lhbLayout);
    leftHandLayout->addWidget(topText);
    leftHandLayout->addWidget(addAssetsButton);
    leftHandLayout->addWidget(clearButton);
    leftHandLayout->addWidget(bottomText);
    leftHandLayout->addWidget(clearButton);

    mainWidget->addWidget(leftHandWidget);
    mainWidget->addWidget(qgis);

    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

#ifdef OpenSRA
    baseMapCombo->setCurrentIndex(7);
#else
    baseMapCombo->setCurrentIndex(0);
    handleBasemapSelection(0);
#endif

    // Set the map tool to select
    handleClearAssetsMap();

    // Now add the splitter handle
    // Note: index 0 handle is always hidden, index 1 is between the two widgets
    QSplitterHandle *handle = mainWidget->handle(1);

    if(handle == nullptr)
    {
        qDebug()<<"Error getting the handle";
        return;
    }

    auto buttonHandle = new QToolButton(handle);
    QVBoxLayout *layout = new QVBoxLayout(handle);
    layout->setSpacing(0);
    //layout->setMargin(0);

    mainWidget->setHandleWidth(15);

    buttonHandle->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    buttonHandle->setDown(false);
    buttonHandle->setAutoRaise(false);
    buttonHandle->setCheckable(false);
    buttonHandle->setArrowType(Qt::RightArrow);
    buttonHandle->setStyleSheet("QToolButton{border:0px solid}; QToolButton:pressed {border:0px solid}");
    buttonHandle->setIconSize(buttonHandle->size());
    layout->addWidget(buttonHandle);

    //    testVectorLayer();

    //    testNewMapCanvas();

    //    testNewMapCanvas2();
}


QGISVisualizationWidget::~QGISVisualizationWidget()
{
    // QgsApplication::exitQgis(); To do... call this last so it does not cause a crash
}


SimCenterMapcanvasWidget* QGISVisualizationWidget::getMapViewWidget(const QString& name)
{
    SimCenterMapcanvasWidget* mapCanvasWidget = new SimCenterMapcanvasWidget(name, this);

    QgsMapCanvas *mapCanvas = mapCanvasWidget->mapCanvas();
    mapCanvas->freeze( true );
    mapCanvas->setObjectName( name );
    mapCanvas->setProject(QgsProject::instance());

    connect(qgis->layerTreeCanvasBridge(), &QgsLayerTreeMapCanvasBridge::canvasLayersChanged, mapCanvas, &QgsMapCanvas::setLayers );

    mapCanvas->setCanvasColor( QgsProject::instance()->backgroundColor() );
    mapCanvas->setSelectionColor( QgsProject::instance()->selectionColor() );

    QgsSettings settings;
    mapCanvas->enableAntiAliasing( settings.value( QStringLiteral( "qgis/enable_anti_aliasing" ), true ).toBool() );
    double zoomFactor = settings.value( QStringLiteral( "qgis/zoom_factor" ), 1.5 ).toDouble();
    mapCanvas->setWheelFactor( zoomFactor );
    mapCanvas->setCachingEnabled( settings.value( QStringLiteral( "qgis/enable_render_caching" ), true ).toBool() );
    mapCanvas->setParallelRenderingEnabled( settings.value( QStringLiteral( "qgis/parallel_rendering" ), true ).toBool() );
    mapCanvas->setMapUpdateInterval( settings.value( QStringLiteral( "qgis/map_update_interval" ), 125 ).toInt() );
    mapCanvas->setSegmentationTolerance( settings.value( QStringLiteral( "qgis/segmentationTolerance" ), "0.01745" ).toDouble() );
    mapCanvas->setSegmentationToleranceType( QgsAbstractGeometry::SegmentationToleranceType( settings.enumValue( QStringLiteral( "qgis/segmentationToleranceType" ), QgsAbstractGeometry::MaximumAngle ) ) );

    //    // add existing annotations to canvas
    //    const auto constAnnotations = QgsProject::instance()->annotationManager()->annotations();
    //    for ( QgsAnnotation *annotation : constAnnotations )
    //    {
    //        QgsMapCanvasAnnotationItem *canvasItem = new QgsMapCanvasAnnotationItem( annotation, mapCanvas );
    //        Q_UNUSED( canvasItem ) //item is already added automatically to canvas scene
    //    }

    //    qgis->markDirty();    
    QgsProject::instance()->setDirty( true );

    //    connect( mapCanvasWidget, &QWidget::close, qgis, &QgisApp::markDirty );

    mapCanvas->setInteractive(true);

    mapCanvas->setLayers( qgis->mapCanvas()->layers() );
    mapCanvas->setExtent( qgis->mapCanvas()->extent() );
    QgsDebugMsgLevel( QStringLiteral( "QgisApp::createNewMapCanvas -2- : QgsProject::instance()->crs().description[%1]ellipsoid[%2]" ).arg( QgsProject::instance()->crs().description(), QgsProject::instance()->crs().ellipsoidAcronym() ), 3 );
    //    mapCanvas->setDestinationCrs( QgsProject::instance()->crs() );

//#ifdef OpenSRA
//    mapCanvas->setDestinationCrs(QgsCoordinateReferenceSystem(QStringLiteral("EPSG:4326")));
//#else
    mapCanvas->setDestinationCrs(QgsCoordinateReferenceSystem(QStringLiteral("EPSG:3857")));
//#endif
    mapCanvas->freeze(false);

    return mapCanvasWidget;
}


void QGISVisualizationWidget::testNewMapCanvas2()
{
    auto mapCanvas = qgis->createNewMapCanvas("Test2");

    QWidget* testWidget = new QWidget();

    QVBoxLayout* newBoxLayout = new QVBoxLayout(testWidget);

    newBoxLayout->addWidget(mapCanvas);

    testWidget->show();

    mapCanvas->zoomToFullExtent();
}


void QGISVisualizationWidget::markDirty()
{
  //  qgis->markDirty();  
  QgsProject::instance()->setDirty( true );

  //    qgis->refreshMapCanvas(true);
  qgis->mapCanvas()->refresh();
}

void QGISVisualizationWidget::turnOnSelectionTool()
{
  // qgis->selectFeatures();
  qgis->actionSelect()->trigger();
}


void QGISVisualizationWidget::zoomToExtent(QgsRectangle zoomRectangle)
{
    auto mapCanvas = qgis->mapCanvas();
    mapCanvas->setExtent(zoomRectangle);
    mapCanvas->refresh();
}

void QGISVisualizationWidget::zoomToActiveLayer(void)
{
    auto mapCanvas = qgis->mapCanvas();
    QgsMapLayer* activeLayer = mapCanvas->currentLayer();
    QgsRectangle extent = activeLayer->extent();

    QgsCoordinateReferenceSystem srcCrs("EPSG:4326");
    QgsCoordinateReferenceSystem destCrs("EPSG:3857");

    // Create a coordinate transform object
    QgsCoordinateTransform transform(srcCrs, destCrs, QgsProject::instance());

    // Transform the extent from EPSG 4326 to EPSG 3857
    QgsRectangle extent3857 = transform.transform(extent);

    mapCanvas->setExtent(extent3857);
    mapCanvas->refresh();
}


void QGISVisualizationWidget::setActiveLayerFillNull(void)
{
    auto mapCanvas = qgis->mapCanvas();
    QgsMapLayer* activeLayer = mapCanvas->currentLayer();
    QgsVectorLayer* vectorLayer = dynamic_cast<QgsVectorLayer*>(activeLayer);
    QgsSymbol* symbol = nullptr;

    //auto geomType = vectorLayer->geometryType();

    QgsSimpleFillSymbolLayer* fillLayer = new QgsSimpleFillSymbolLayer();
    fillLayer->setStrokeColor(Qt::red);
    fillLayer->setFillColor(QColor(0, 0, 0, 0));
    fillLayer->setStrokeWidth(1);
    symbol = new QgsFillSymbol();
    symbol->deleteSymbolLayer(0);
    symbol->appendSymbolLayer(fillLayer->clone());

    QgsSingleSymbolRenderer* renderer = new QgsSingleSymbolRenderer(symbol);

    vectorLayer->setRenderer(renderer);
    vectorLayer->triggerRepaint();

}


SimCenterMapcanvasWidget* QGISVisualizationWidget::testNewMapCanvas()
{
    auto mapCanvasWidget = this->getMapViewWidget("Test");

    mapCanvasWidget->setWindowFlag(Qt::Window);

    //    auto mapCanvas = mapCanvasWidget->mapCanvas();

    mapCanvasWidget->show();

    //    QGraphicsScene* scene = mapCanvas->scene();

    //    auto grid = new RectangleGrid(mapCanvas);

    //    auto sceneRect = scene->sceneRect();

    //    auto centerScene = sceneRect.center();

    //    auto sceneWidth = sceneRect.width();
    //    auto sceneHeight = sceneRect.height();

    //    // Set the initial grid size if it has not already been set
    //    if(grid->getBottomLeftNode()->pos().isNull() || grid->getTopRightNode()->pos().isNull() || grid->getTopLeftNode()->pos().isNull() || grid->getBottomRightNode()->pos().isNull() )
    //    {
    //        grid->setWidth(0.5*sceneWidth);
    //        grid->setHeight(0.5*sceneHeight);
    //        grid->setPos(centerScene.toPoint());

    //        scene->addItem(grid);
    //    }

    //    grid->show();

    //    mapCanvasWidget->mapCanvas()->zoomToFullExtent();

    return mapCanvasWidget;
}


void QGISVisualizationWidget::handleBasemapSelection(int index)
{

    QString uri;
    QString baseName;
    QString key;

    if(index == 0)
    {
        uri = "tilePixelRatio=2&type=xyz&url=http://tile.openstreetmap.org/%7Bz%7D/%7Bx%7D/%7By%7D.png&zmax=19&zmin=0";
        baseName = "Open Street Map";
        key = "wms";
    }
    else if(index == 1)
    {

        uri = "tilePixelRatio=2&type=xyz&url=http://tile.opentopomap.org/%7Bz%7D/%7Bx%7D/%7By%7D.png&zmax=19&zmin=0";
        baseName = "Open Street Map Topo";
        key = "wms";
    }
    else if(index == 2)
    {
        uri = "tilePixelRatio=2&type=xyz&url=https://mt1.google.com/vt/lyrs%3Ds%26x%3D%7Bx%7D%26y%3D%7By%7D%26z%3D%7Bz%7D&zmax=19&zmin=0";
        baseName = "Google Satellite";
        key = "wms";
    }
    else if(index == 3)
    {
        uri = "tilePixelRatio=2&type=xyz&url=https://mt1.google.com/vt/lyrs%3Dm%26x%3D%7Bx%7D%26y%3D%7By%7D%26z%3D%7Bz%7D&zmax=19&zmin=0";
        baseName = "Google Road";
        key = "wms";

    }
    else if(index == 4)
    {
        uri = "tilePixelRatio=2&type=xyz&url=https://mt1.google.com/vt/lyrs%3Dy%26x%3D%7Bx%7D%26y%3D%7By%7D%26z%3D%7Bz%7D&zmax=19&zmin=0";
        baseName = "Google Road and Satellite";
        key = "wms";

    }
    else if(index == 5)
    {
        uri = "tilePixelRatio=2&type=xyz&url=http://ecn.t3.tiles.virtualearth.net/tiles/a%7Bq%7D.jpeg?g%3D1&zmax=18&zmin=0";
        baseName = "Bing Aerial";
        key = "wms";

    }
    else if(index == 6)
    {
        uri = "tilePixelRatio=2&type=xyz&url=https://stamen-tiles.a.ssl.fastly.net/toner-lite/%7Bz%7D/%7Bx%7D/%7By%7D.png&zmax=20&zmin=0";
        baseName = "Stamen Toner Lite";
        key = "wms";

    }
    else if(index == 7)
    {
        uri = "tilePixelRatio=2&type=xyz&url=https://services.arcgisonline.com/ArcGIS/rest/services/World_Topo_Map/MapServer/tile/%7Bz%7D/%7By%7D/%7Bx%7D&zmax=20&zmin=0";
        baseName = "ESRI World Topo";
        key = "wms";

    }
    else
    {
        uri = QString();
        baseName = QString();
        key = QString();
    }

    if(baseName.isEmpty() || uri.isEmpty() || key.isEmpty())
    {
        this->errorMessage("Error setting basemap layer");
        return;
    }


    if(baseMapLayer)
    {
        if(baseMapLayer->name() == baseName)
            return;

        QgsMapLayer::LayerFlags flags = baseMapLayer->flags();
        flags |= QgsMapLayer::Removable;
        baseMapLayer->setFlags( flags );

        this->removeLayer(baseMapLayer);
    }


    QgsMapLayer* newBaseMapLayer = qgis->addRasterLayer(uri,baseName,key);

    if (newBaseMapLayer == nullptr)
    {
        this->errorMessage("Error setting basemap layer");
        return;
    }

    moveLayerToBottom(newBaseMapLayer->id());

    baseMapLayer = newBaseMapLayer ;

    // Make the basemap non-removable so that it does not get deleted when the project is cleared
    QgsMapLayer::LayerFlags flags = baseMapLayer->flags();

    //    flags |= QgsMapLayer::Identifiable; // yes flag
    //    flags &= ~QgsMapLayer::Identifiable; // no flag

    //    flags |= QgsMapLayer::Removable;
    flags &= ~QgsMapLayer::Removable;

    //    flags |= QgsMapLayer::Searchable;
    //    flags &= ~QgsMapLayer::Searchable;

    //    flags |= QgsMapLayer::Private;
    //    flags &= ~QgsMapLayer::Private;

    baseMapLayer->setFlags( flags );

//#ifdef OpenSRA
//    auto crs = QgsCoordinateReferenceSystem(QStringLiteral("EPSG:4326"));
//#else
    auto crs = QgsCoordinateReferenceSystem(QStringLiteral("EPSG:3857"));
//#endif
    baseMapLayer->setCrs(crs);
}


/*
void QGISVisualizationWidget::handleSelectAssetsMap(void)
{
    auto mapCanvas = qgis->mapCanvas();

    auto layers = mapCanvas->layers();

    for(auto&& layer : layers)
    {

        auto currVecLayer = static_cast<QgsVectorLayer*>(layer);

        if(currVecLayer == nullptr)
            continue;

        auto layerId = layer->id();

        auto selectWidget = mapSelectableAssetWidgets.value(layerId,nullptr);
        if(selectWidget == nullptr)
            continue;

        QgsFeatureIds selectedAssets = currVecLayer->selectedFeatureIds();

        if(selectedAssets.isEmpty())
            continue;

        selectWidget->insertSelectedAssets(selectedAssets);
    }

    qgis->deselectAll();
}
*/

void QGISVisualizationWidget::handleSelectAssetsMap(void)
{
    auto mapCanvas = qgis->mapCanvas();
    auto layers = mapCanvas->layers();

    for(auto&& layer : layers)
    {
        // Use qobject_cast for safe type checking
        auto currVecLayer = qobject_cast<QgsVectorLayer*>(layer);

        if(currVecLayer == nullptr)
            continue;

        auto layerId = layer->id();

        auto selectWidget = mapSelectableAssetWidgets.value(layerId, nullptr);
        if(selectWidget != nullptr)
        {
            QgsFeatureIds selectedAssets = currVecLayer->selectedFeatureIds();

            if(!selectedAssets.isEmpty())
            {
                selectWidget->insertSelectedAssets(selectedAssets);
            }
        }

        // FIX: Manually clear the selection for each vector layer
        // This replaces both qgis->deselectAll() and QgsProject::setSelection()
        currVecLayer->removeSelection();
    }

    // Ensure the canvas repaints to show the highlights are gone
    mapCanvas->refresh();
}


void QGISVisualizationWidget::handleClearAssetsMap(void)
{
    // Iterate through the map and clear all selected assets
    QMapIterator<QString,GISSelectable*> it(mapSelectableAssetWidgets);
    while (it.hasNext()) {
        it.next();
        it.value()->clearSelectedAssets();
    }

    handleBasemapSelection(0);
    
}


QgsRasterLayer* QGISVisualizationWidget::addRasterLayer(const QString &layerPath, const QString &name, const QString &providerKey)
{
    this->deselectAllTreeItems();

    auto layer = qgis->addRasterLayer(layerPath, name, providerKey);

    if(layer != nullptr)
        layer->setCrs(QgsCoordinateReferenceSystem("EPSG:4326"));


    return layer;
}



void QGISVisualizationWidget::moveLayerToBottom(const QString& layerID)
{
    auto root = QgsProject::instance()->layerTreeRoot();

    QgsLayerTreeLayer* myExistingLayer = root->findLayer(layerID);

    if(myExistingLayer==nullptr)
        return;

    auto myClone = myExistingLayer->clone();

    root->addChildNode(myClone);

    auto parent = myExistingLayer->parent();

    parent->takeChild(myExistingLayer);
}


QgsVectorLayer* QGISVisualizationWidget::addVectorLayer(const QString &layerPath, const QString &name, const QString &providerKey)
{
    this->deselectAllTreeItems();

    auto layer = qgis->addVectorLayer(layerPath, name, providerKey);

    if(layer != nullptr)
        layer->setCrs(QgsCoordinateReferenceSystem("EPSG:4326"));

    return layer;
}


QgsVectorLayer* QGISVisualizationWidget::duplicateExistingLayer(QgsVectorLayer* layer)
{
    if(layer == nullptr)
    {
        this->errorMessage("nullptr came into function");
        return nullptr;
    }

    // Get the layer type
    QString layerType;
    auto geomType = layer->geometryType();

    if(geomType == Qgis::GeometryType::Polygon)
        layerType = "polygon";
    else if (geomType == Qgis::GeometryType::Point)
        layerType = "point";
    else if (geomType == Qgis::GeometryType::Line)
        layerType = "linestring";
    else
    {
        this->errorMessage("Error in duplicateExistingLayer .. cannot process Unknown Geometry type for layer "+layer->name());
        return nullptr;
    }

    QgsVectorLayer* dupLayer = this->addVectorLayer(layerType,"All Pipelines");
    if (dupLayer == nullptr)
    {
        this->errorMessage("Error copying the layer "+layer->name() + " could not create a layer in the memory");
        return nullptr;
    }

    auto pr = dupLayer->dataProvider();
    dupLayer->startEditing();

    auto res = pr->addAttributes(layer->fields().toList());

    if(!res)
    {
        this->errorMessage("Error adding attributes to the duplicate layer " + dupLayer->name() + ". Error copying the layer "+layer->name());
        return nullptr;
    }

    dupLayer->updateFields(); // tell the vector layer to fetch changes from the provider

    // Check if the layer is created, but perhaps not valid
    if (!dupLayer->isValid())
    {
        this->errorMessage("Error copying the layer "+layer->name()+" duplicate layer is not valid");
        return nullptr;
    }

    // Set the crs of the duplicated layer to the crs of the original layer
    dupLayer->setCrs(layer->crs());

    auto layerDupName = layer->name() + ' ' + "copy";
    dupLayer->setName(layerDupName);

    // There could be a better approach to do this... perform a deep copy of the features for now
    QgsFeatureIterator featIt = layer->getFeatures();

    QList<QgsFeature> featureList;
    QgsFeature feat;
    while (featIt.nextFeature(feat))
    {
        auto newFeat = QgsFeature(feat); // Use the feature copy constructor

        // Do a deep clone of the geometry
        newFeat.setGeometry(QgsGeometry(feat.geometry().get()->clone()));
        featureList.append(newFeat);
    }

    // Start editing on the layer
    dupLayer->startEditing();

    dupLayer->dataProvider()->addFeatures(featureList);

    dupLayer->commitChanges(true);
    dupLayer->updateExtents();

    this->addMapLayer(dupLayer);

    return dupLayer;
}


void QGISVisualizationWidget::addMapLayer(QgsMapLayer* layer)
{
  //  qgis->addMapLayer(layer);  
  QgsProject::instance()->addMapLayer(layer);
}


void QGISVisualizationWidget::testVectorLayer()
{

    // QgsVectorLayer( const QString &path = QString(), const QString &baseName = QString(), const QString &providerLib = "ogr", const QgsVectorLayer::LayerOptions &options = QgsVectorLayer::LayerOptions() );

    auto vl = this->addVectorLayer("Point", "Feature Layer");

    auto vl2 = this->addVectorLayer("Point", "Feature Layer2");

    connect(vl,&QgsVectorLayer::selectionChanged,this, &QGISVisualizationWidget::selectionChanged);

    auto pr = vl->dataProvider();

    auto pr2 = vl2->dataProvider();

    // add fields
    QList<QgsField> attrib = {QgsField("name", QVariant::String),
                              QgsField("age",  QVariant::Int),
                              QgsField("size", QVariant::Double),
                              QgsField("repairRatio", QVariant::Double)};

    QVector<QString> buildTypes = {"Res 1", "Res 2", "Comm 1", "Comm 2", "Ind 1", "Ind 2"};

    auto res = pr->addAttributes(attrib);

    if(!res)
        qDebug()<<"Error adding attributes";

    pr2->addAttributes(attrib);

    vl->updateFields(); // tell the vector layer to fetch changes from the provider

    vl2->updateFields(); // tell the vector layer to fetch changes from the provider

    int numFeat = 1000;
    QgsFeatureList featList;
    QgsFeatureList featList2;

    int numTypes = buildTypes.size()-1;

    auto minX =  -120;
    auto minY = 30;

    float maxD = 5.5;
    float minD = 3.2;

    for(int i = 0; i < numFeat; ++i)
    {

        auto x = minX + minD + static_cast <float> (rand()) / ( static_cast <float> (RAND_MAX/(maxD-minD)));
        auto y = minY + minD + static_cast <float> (rand()) / ( static_cast <float> (RAND_MAX/(maxD-minD)));

        // add a feature
        QgsFeature feature;
        feature.setGeometry(QgsGeometry::fromPointXY(QgsPointXY(x,y)));

        QgsAttributes featAttrb(attrib.size());

        featAttrb[0] = QVariant(buildTypes[(rand() % numTypes) + 1]);
        featAttrb[1] = QVariant(2);
        featAttrb[2] = QVariant(0.3);
        featAttrb[3] = QVariant((double) rand()/RAND_MAX);

        feature.setAttributes(featAttrb);

        if(i%2 == 0)
            featList.append(feature);
        else
            featList2.append(feature);
    }


    pr->addFeatures(featList);
    pr2->addFeatures(featList2);

    vl->updateExtents();
    vl2->updateExtents();

    //  createPrettyGraduatedRenderer("repairRatio",Qt::yellow, Qt::red,5,vl);
    //    createCategoryRenderer("name",vl);

    QVector<QPair<double,double>>  classBreaks;
    classBreaks.append(QPair<double,double>(0.0,0.4));
    classBreaks.append(QPair<double,double>(0.4,0.6));
    classBreaks.append(QPair<double,double>(0.6,0.8));
    classBreaks.append(QPair<double,double>(0.8,1.0));

    QVector<QColor> colors;
    colors.push_back(Qt::black);
    colors.push_back(Qt::blue);
    colors.push_back(Qt::green);
    colors.push_back(Qt::red);

    createCustomClassBreakRenderer("repairRatio",vl,Qgis::SymbolType::Marker,classBreaks,colors);

    createCustomClassBreakRenderer("repairRatio",vl2,Qgis::SymbolType::Marker,classBreaks,colors);

    QVector<QgsMapLayer*> vecLayers= {vl,vl2};

    this->createLayerGroup(vecLayers, "Test layer group");
}


void QGISVisualizationWidget::selectLayersInTree(const QVector<QgsMapLayer*>& layers)
{
    // First get all of the indexes
    QVector<QModelIndex> vecIndexes;
    vecIndexes.reserve(layers.size());
    for(auto&& it: layers)
    {
        auto index = this->getLayerIndex(it);

        if(!index.isValid())
            this->errorMessage("Invalid index in select layers");

        vecIndexes.push_back(index);
    }

    // Then do the selection
    auto selectModel = layerTreeView->selectionModel();
    for(auto&& it : vecIndexes)
    {
        selectModel->select(it , QItemSelectionModel::Select);
    }
}


QgsLayerTreeGroup* QGISVisualizationWidget::createLayerGroup(const QVector<QgsMapLayer*>& layers, const QString groupName)
{
    if(layers.size() < 2)
    {
        this->errorMessage("Two or more layers are required to create a group");
        return nullptr;
    }

    this->deselectAllTreeItems();

    // First select the layers
    this->selectLayersInTree(layers);

    if(layerTreeView->selectedLayers().empty())
    {
        this->errorMessage("Layer selection failed in create group");
        return nullptr;
    }

    // Add the group
    layerTreeView->defaultActions()->addGroup();

    // Rename the group
    auto group = layerTreeView->currentGroupNode();

    if(group != nullptr)
    {
        group->setName(groupName);
        group->setExpanded(false);
    }
    else
        this->errorMessage("Failed to create a group layer");


    // Clear the selection otherwise future layers added to the map will automatically be part of the group
    this->deselectAllTreeItems();

    return group;
}


QgsLayerTreeGroup* QGISVisualizationWidget::getLayerGroup(const QString groupName)
{

    // Get the root group
    QgsLayerTreeGroup *rootGroup = layerTreeView->layerTreeModel()->rootGroup();

    // Find group node with specified name. Searches recursively the whole sub-tree. - will return nullptr if not found!
    auto grp = rootGroup->findGroup(groupName);

    return grp;
}



void QGISVisualizationWidget::removeLayerGroup(const QString& name)
{
    // Get the layer group
    auto lyrGroup = this->getLayerGroup(name);

    if(!lyrGroup)
    {
        this->errorMessage("Error deleting the layer group. "+name+" Could not find the layer group.");
        return;
    }

    // Get the layers recursively
    auto layerList = lyrGroup->findLayers();

    // When all layers are removed from the group the group will disappear
    for(auto&& it : layerList)
        this->removeLayer(it->layer());
}



void QGISVisualizationWidget::deselectAllTreeItems(void)
{
    layerTreeView->setCurrentLayer(nullptr);
}


void QGISVisualizationWidget::showFeaturePopUp(QgsFeatureIterator& features)
{

    if(!features.isValid())
        return;

    std::unique_ptr<PopUpWidget> popUp = std::make_unique<PopUpWidget>(this);

    QgsFeature feat;
    while (features.nextFeature(feat))
    {
        auto fields = feat.fields();
        auto attributes = feat.attributes();

        if(fields.size() != attributes.size())
        {
            this->errorMessage("Error getting the fields and attributes in information popup");
            return;
        }

        QStringList attrbKeyList;
        QStringList attrbValList;

        for(int i = 0; i<attributes.size(); ++i)
        {
            auto atrb = attributes.at(i).toString();
            auto field = fields.at(i).name();

            // Do not list empty attributes in the popup
            if(atrb.isEmpty())
                continue;

            if(QString::compare(atrb,"ObjectID") == 0 || QString::compare(atrb,"AssetType") == 0 || QString::compare(atrb,"TabName") == 0)
                continue;

            attrbKeyList.append(field);
            attrbValList.append(atrb);
        }

        // Create a table to display the attributes of this element
        auto attributeTableWidget = new QTableWidget(popUp.get());
        attributeTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        attributeTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        attributeTableWidget->horizontalHeader()->hide();
        attributeTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

        auto numRows = attrbValList.size();

        attributeTableWidget->setColumnCount(1);
        attributeTableWidget->setRowCount(numRows);

        attributeTableWidget->setVerticalHeaderLabels(attrbKeyList);

        for(int i = 0; i<numRows; ++i)
        {
            auto item = new QTableWidgetItem(attrbValList[i]);

            attributeTableWidget->setItem(i,0, item);
        }

        QString label = feat.attribute("TabName").toString();

        // If the label is empty, append the layer name
        if(label.isEmpty())
        {
            label = "Feature Information";
        }

        popUp->addTab(attributeTableWidget,label);
    }

    popUp->exec();

}



QModelIndex QGISVisualizationWidget::getLayerIndex(QgsMapLayer* layer)
{
    layerTreeView->setCurrentLayer(layer);
    auto node = layerTreeView->currentNode();

    auto index = layerTreeView->node2index(node);

    return index;
}


int QGISVisualizationWidget::createCustomClassBreakRenderer(const QString attrName, QgsVectorLayer * vlayer, Qgis::SymbolType symbolType, const QVector<QPair<double,double>>& classBreaks, const QVector<QColor>& colors, const QVector<QString> labels, double size)
{

    if(vlayer == nullptr)
        return -1;

    if(colors.size() != classBreaks.size())
    {
        this->errorMessage("Mismatch between number of class breaks and colors, they have to be the same size");
        return -1;
    }

    // qgsclassificationcustom
    QString methodId = "Custom";
    // QgsClassificationMethod *method = QgsApplication::classificationMethodRegistry()->method(methodId);
    auto methodPtr = QgsApplication::classificationMethodRegistry()->method(methodId);
    QgsClassificationMethod* method = methodPtr.get();  // non-owning
    

    QgsRangeList rngList;
    rngList.reserve(classBreaks.size());

    for(int i = 0; i<classBreaks.size(); ++i)
    {
        QgsSymbol* symbol = nullptr;
        if(symbolType == Qgis::SymbolType::Marker)
        {
            auto markerSymbol = new QgsMarkerSymbol();
            markerSymbol->setSize(size);
            symbol = markerSymbol;
        }
        else if(symbolType == Qgis::SymbolType::Line)
        {
            auto lineSymbol = new QgsLineSymbol();
            lineSymbol->setWidth(size);
            symbol = lineSymbol;
        }
        else if(symbolType == Qgis::SymbolType::Fill)
        {
            symbol = new QgsFillSymbol();
        }
        else
        {
            this->errorMessage("Could not recognize the symbol type");
            return -1;
        }

        symbol->setColor(colors[i]);
        auto cBreak = classBreaks.at(i);

        auto lowEnd = cBreak.first;
        auto highEnd = cBreak.second;

        QString label;

        if(!labels.empty())
            label = labels[i];
        else
            label = QString::number(lowEnd) + " - " + QString::number(highEnd);

        QgsRendererRange newRange(lowEnd,highEnd, symbol,label);

        rngList.push_back(newRange);
    }

    QgsGraduatedSymbolRenderer* renderer = new QgsGraduatedSymbolRenderer(attrName,rngList);

    // set method to renderer
    renderer->setClassificationMethod(method);

    // create and set new renderer
    renderer->setClassAttribute(attrName);

    vlayer->setRenderer(renderer);

    return 0;
}


void QGISVisualizationWidget::createPrettyGraduatedRenderer(const QString attrName, const QColor color1, const QColor color2, const int nclasses, QgsVectorLayer * vlayer, QgsSymbol* symbol)
{

    if(vlayer == nullptr)
        return;

    // *** Graduated renderer

    // qgsclassificationcustom
    // qgsclassificationequalinterval
    // qgsclassificationquantile
    // qgsclassificationjenks
    // qgsclassificationstandarddeviation
    // qgsclassificationprettybreaks
    // qgsclassificationlogarithmic

    QString methodId = "Pretty";
    // QgsClassificationMethod *method = QgsApplication::classificationMethodRegistry()->method(methodId);
    auto methodPtr = QgsApplication::classificationMethodRegistry()->method(methodId);
    QgsClassificationMethod* method = methodPtr.get();  // non-owning

    QgsGraduatedSymbolRenderer* renderer = new QgsGraduatedSymbolRenderer(attrName);

    if(symbol != nullptr)
        renderer->setSourceSymbol(symbol);

    // set method to renderer
    renderer->setClassificationMethod(method);

    // create and set new renderer
    renderer->setClassAttribute(attrName);

    QgsGradientColorRamp* ramp = new QgsGradientColorRamp(color1, color2);

    renderer->setSourceColorRamp(ramp);

    //renderer->updateClasses(vlayer, nclasses);
    QString err;
    renderer->updateClasses(vlayer, nclasses, err);
    if (!err.isEmpty()) {
      this->errorMessage(QString("QGisVisualizationWidget::createPrettyGraduateRenderer: ") + err);
    }

    vlayer->setRenderer(renderer);
}


void QGISVisualizationWidget::createCategoryRenderer(const QString attrName, QgsVectorLayer * vlayer, QgsSymbol* symbol, QVector<QColor> colors)
{
    QList<QVariant> uniqueValues;

    const int idx = vlayer->fields().lookupField(attrName);
    if (idx == -1)
    {
        // Lets assume it's an expression
        std::unique_ptr<QgsExpression> expression = std::make_unique<QgsExpression>(attrName);
        QgsExpressionContext context;
        context << QgsExpressionContextUtils::globalScope()
                << QgsExpressionContextUtils::projectScope(QgsProject::instance())
                << QgsExpressionContextUtils::atlasScope(nullptr)
                << QgsExpressionContextUtils::layerScope(vlayer);

        expression->prepare(&context);
        QgsFeatureIterator fit = vlayer->getFeatures();
        QgsFeature feature;
        while (fit.nextFeature(feature))
        {
            context.setFeature(feature);
            const QVariant value = expression->evaluate(&context);
            if (uniqueValues.contains(value))
                continue;
            uniqueValues << value;
        }
    }
    else
    {
        uniqueValues = qgis::setToList(vlayer->uniqueValues(idx));
    }

    if(uniqueValues.isEmpty())
    {
        qDebug()<<"No unique values found for the attribute " + attrName + " so no categories will be created";
        return;
    }

    QgsCategoryList cats = QgsCategorizedSymbolRenderer::createCategories(uniqueValues, symbol, vlayer, attrName);

    auto numCats = cats.size();

    if(!colors.empty())
    {
        if(colors.size() != numCats)
        {
            qDebug()<<"Warning, not using the provided colors as there is a size mismatch between the number of colors provided and number of categories. Assigning random colors to the categories instead";

            QgsRandomColorRamp ramp;
            ramp.setTotalColorCount(numCats);

            for (int i = 0; i < numCats; ++i)
                cats.at(i).symbol()->setColor(ramp.color(i));
        }
        else
        {
            for (int i = 0; i < numCats; ++i)
                cats.at(i).symbol()->setColor(colors[i]);
        }
    }
    else
    {
        QgsRandomColorRamp ramp;
        ramp.setTotalColorCount(numCats);

        for (int i = 0; i < numCats; ++i)
            cats.at(i).symbol()->setColor(ramp.color(i));
    }

    QgsCategorizedSymbolRenderer* renderer = new QgsCategorizedSymbolRenderer(attrName, cats);
    renderer->setSourceSymbol(symbol);

    vlayer->setRenderer(renderer);

}


void QGISVisualizationWidget::createSimpleRenderer(QgsSymbol* symbol, QgsVectorLayer * layer)
{
    QgsSingleSymbolRenderer* renderer = new QgsSingleSymbolRenderer(symbol);

    layer->setRenderer(renderer);
}


void QGISVisualizationWidget::createSimpleRenderer(QColor color, QgsVectorLayer * layer)
{

    QgsSymbol* symbol = nullptr;

    auto geomType = layer->geometryType();

    if(geomType == Qgis::GeometryType::Polygon)
    {
        symbol = new QgsFillSymbol();
    }
    else if(geomType == Qgis::GeometryType::Point)
    {
        symbol = new QgsMarkerSymbol();
    }
    else if(geomType == Qgis::GeometryType::Line)
    {
        symbol = new QgsLineSymbol();
    }
    else
    {
        this->errorMessage("Error, could not parse the geometry type");
        return;
    }

    symbol->setColor(color);

    QgsSingleSymbolRenderer* renderer = new QgsSingleSymbolRenderer(symbol);

    layer->setRenderer(renderer);
}

void QGISVisualizationWidget::createSymbolRenderer(Qgis::MarkerShape symbolShape, QColor color, double size, QgsVectorLayer * layer)
{
    QgsSimpleMarkerSymbolLayer *mSimpleMarkerLayer = new QgsSimpleMarkerSymbolLayer();
    mSimpleMarkerLayer->setColor(color);
    mSimpleMarkerLayer->setStrokeColor(Qt::black);
    mSimpleMarkerLayer->setShape(symbolShape);
    mSimpleMarkerLayer->setSize(size);

    QgsMarkerSymbol* mMarkerSymbol = new QgsMarkerSymbol();
    mMarkerSymbol->changeSymbolLayer(0, mSimpleMarkerLayer);
    mMarkerSymbol->setColor(color);

    QgsSingleSymbolRenderer *mSymbolRenderer = mSymbolRenderer = new QgsSingleSymbolRenderer(mMarkerSymbol);

    layer->setRenderer(mSymbolRenderer);
}


QWidget* QGISVisualizationWidget::getVisWidget()
{
    return this;
}


double QGISVisualizationWidget::getLatFromScreenPoint(const QPointF& point, const QgsMapCanvas* canvas)
{
    const QgsPointXY transformedPoint = getPointFromScreenCoord(point.toPoint(),canvas);

    return transformedPoint.y();
}


double QGISVisualizationWidget::getLongFromScreenPoint(const QPointF& point, const QgsMapCanvas* canvas)
{
    const QgsPointXY transformedPoint = getPointFromScreenCoord(point.toPoint(),canvas);

    return transformedPoint.x();
}


QPointF QGISVisualizationWidget::getScreenPointFromLatLong(const double& latitude, const double& longitude, const QgsMapCanvas* canvas)
{
    const QgsPointXY mapPoint(longitude,latitude);

    auto screenPointXY = getScreenCoordFromPoint(mapPoint,canvas);

    return QPointF(screenPointXY.x(),screenPointXY.y());
}


void QGISVisualizationWidget::clear(void)
{
    qgis->layerTreeView()->selectAll();

    const QList<QgsMapLayer *> selectedLayers = qgis->layerTreeView()->selectedLayersRecursive();

    for(auto&& it: selectedLayers)
        this->removeLayer(it);

    mapSelectableAssetWidgets.clear();

    // This will clear everything including background layers
    // qgis->layerTreeView()->layerTreeModel()->rootGroup()->clear();

    
}


void QGISVisualizationWidget::removeLayer(QgsMapLayer* layer)
{
    if(layer == nullptr)
        return;

    this->deregisterLayerForSelection(layer->id());

    /*
    auto layerTreeView = qgis->layerTreeView();
    layerTreeView->setCurrentLayer(layer);
    // False to not show prompt asking user
    qgis->removeLayer(false);
    */
    
    QgsProject::instance()->removeMapLayer(layer->id());

    layer = nullptr;

    // Remove the empty groups
    qgis->layerTreeView()->layerTreeModel()->rootGroup()->removeChildrenGroupWithoutLayers();
    
}


void QGISVisualizationWidget::handleLegendChange(const QString layerUID)
{

}


void QGISVisualizationWidget::clearSelection(void)
{

    return;
}


void QGISVisualizationWidget::selectionChanged( const QgsFeatureIds &selected, const QgsFeatureIds &deselected, bool clearAndSelect )
{
    if(clearAndSelect == true)
        qDebug()<<"Clear and select true";
    else
        qDebug()<<"Clear and select false";
}


QgsGeometry QGISVisualizationWidget::getPolygonGeometryFromJson(const QString& geoJson)
{
    QRegularExpression rx("[^\\[\\]]+(?=\\])");

    QRegularExpressionMatchIterator i = rx.globalMatch(geoJson);

    QStringList pointsList;
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();

        if(!match.hasMatch())
            continue;

        QString word = match.captured(0);
        pointsList << word;
    }

    if(pointsList.empty())
        return QgsGeometry();

    //    typedef QVector<QgsPolylineXY> QgsPolygonXY;
    QgsPolygonXY polygon;
    QgsPolylineXY polyLineRing;
    for(auto&& it : pointsList)
    {
        auto points = it.split(",");

        // Point could have 2 or 3 dims, x,y or x,y,z
        if(points.size() < 2 || points.size() > 3)
            return QgsGeometry();

        bool OK = false;
        double lat = points.at(0).toDouble(&OK);

        if(!OK)
            return QgsGeometry();

        double lon = points.at(1).toDouble(&OK);
        if(!OK)
            return QgsGeometry();

        polyLineRing.append(QgsPointXY(lat,lon));
    }

    polygon.append(polyLineRing);

    QgsGeometry geom = QgsGeometry::fromPolygonXY(polygon);

    return geom;
}


void QGISVisualizationWidget::selectLayerInTree(QgsMapLayer* layer)
{
    auto layerIndex = this->getLayerIndex(layer);

    auto selectModel = layerTreeView->selectionModel();

    selectModel->select(layerIndex , QItemSelectionModel::Select);
}


void QGISVisualizationWidget::setActiveLayer(QgsMapLayer* layer)
{
    qgis->setActiveLayer(layer);
}


void QGISVisualizationWidget::zoomToLayer(QgsMapLayer* layer)
{
  //    this->selectLayerInTree(layer);
  //    qgis->zoomToLayerExtent();
    
    if (layer) {
      this->selectLayerInTree(layer);
      qgis->mapCanvas()->setExtent(layer->extent());
      qgis->mapCanvas()->refresh();
    }

    // Ensure that we are not zoomed in too far
    if ( baseMapLayer)
    {
      baseMapLayer->setScaleBasedVisibility(true);
      auto mMapCanvas = qgis->mapCanvas();
      const double scale = mMapCanvas->scale();

      auto minScale = baseMapLayer->minimumScale();
      auto maxScale = baseMapLayer->maximumScale();

      // Clip the max scale at 3000 - no point in zooming in too far
      if (maxScale < 3000.0)
          maxScale = 3000.0;

      if ( scale > minScale && minScale > 0 )
      {
        mMapCanvas->zoomScale( minScale * Qgis::SCALE_PRECISION );
      }
      else if ( scale <= maxScale && maxScale > 0 )
      {
        mMapCanvas->zoomScale(maxScale);
      }
    }

    return;
}

QgsLayerTreeView *QGISVisualizationWidget::getLayerTreeView() const
{
    return layerTreeView;
}


QgisApp* QGISVisualizationWidget::getQgis(void)
{
    return qgis;
}


QgsMapCanvas* QGISVisualizationWidget::getMainCanvas(void)
{
    return qgis->mapCanvas();
}


void QGISVisualizationWidget::zoomToLayer(const QString /*layerID*/)
{

    this->statusMessage("Implement me QGISVisualizationWidget::zoomToLayer");

    return;
}


void QGISVisualizationWidget::setLayerVisibility(QgsMapLayer* layer, bool value)
{
    layerTreeView->setLayerVisible(layer, value);
}


QgsGeometry QGISVisualizationWidget::getPolygonGeometryFromJson(const QJsonArray& geoJson)
{

    if(geoJson.size() == 0)
        return QgsGeometry();


    QgsPolygonXY polygon;
    QgsPolylineXY polyLineRing;
    for(auto&& it : geoJson)
    {

        auto points = it.toArray();

        if(points.size() != 2)
            return QgsGeometry();

        double lat = points.at(1).toDouble(360.0);
        double lon = points.at(0).toDouble(360.0);

        if(lat == 360.0 || lon == 360.0)
            return QgsGeometry();

        polyLineRing.append(QgsPointXY(lat,lon));
    }

    polygon.append(polyLineRing);

    QgsGeometry geom = QgsGeometry::fromPolygonXY(polygon);

    return geom;
}


QgsGeometry QGISVisualizationWidget::getMultilineStringGeometryFromJson(const QJsonArray& geoJson)
{

    if(geoJson.size() == 0)
        return QgsGeometry();

    QgsPolylineXY polyLine;

    for(auto&& it : geoJson)
    {
        auto points = it.toArray();

        // Must be more than two points to make a polyline
        if(points.size() < 2)
            return QgsGeometry();

        for(auto&& it2 : points)
        {
            auto point = it2.toArray();

            double lat = point.at(0).toDouble(360.0);
            double lon = point.at(1).toDouble(360.0);

            if(lat == 360.0 || lon == 360.0)
                return QgsGeometry();

            polyLine.append(QgsPointXY(lat,lon));
        }
    }

    QgsGeometry geom = QgsGeometry::fromPolylineXY(polyLine);

    return geom;
}


QgsGeometry QGISVisualizationWidget::getMultilineStringGeometryFromJson(const QString& geoJson)
{
    QRegularExpression rx("[^\\[\\]]+(?=\\])");

    QRegularExpressionMatchIterator i = rx.globalMatch(geoJson);

    QStringList pointsList;
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();

        if(!match.hasMatch())
            continue;

        QString word = match.captured(0);
        pointsList << word;
    }

    if(pointsList.empty())
        return QgsGeometry();

    QgsPolylineXY polyLine;

    for(auto&& it : pointsList)
    {
        auto points = it.split(",");

        // Point could have 2 or 3 dims, x,y or x,y,z
        if(points.size() < 2 || points.size() > 3)
            return QgsGeometry();

        bool OK = false;
        double lat = points.at(0).toDouble(&OK);

        if(!OK)
            return QgsGeometry();

        double lon = points.at(1).toDouble(&OK);
        if(!OK)
            return QgsGeometry();

        polyLine.append(QgsPointXY(lat,lon));
    }

    QgsGeometry geom = QgsGeometry::fromPolylineXY(polyLine);

    return geom;
}


void QGISVisualizationWidget::registerLayerForSelection(const QString layerId, GISSelectable* widget)
{
    mapSelectableAssetWidgets.insert(layerId,widget);
}


int QGISVisualizationWidget::deregisterLayerForSelection(const QString layerId)
{
    return mapSelectableAssetWidgets.remove(layerId);
}


void QGISVisualizationWidget::handleSelectButton(void)
{
   //qgis->selectFeatures();
  qgis->actionSelect()->trigger();
}


void QGISVisualizationWidget::handleIdentifyButton(void)
{
  //    qgis->identify();  
  qgis->actionIdentify()->trigger();
}


QgsPointXY QGISVisualizationWidget::getPointFromScreenCoord(const QPoint& point, const QgsMapCanvas* canvas)
{
    auto pointXY =canvas->mapSettings().mapToPixel().toMapCoordinates(point);

    auto crs = QgsCoordinateReferenceSystem(QStringLiteral("EPSG:4326"));
    QgsCoordinateTransform ct(canvas->mapSettings().destinationCrs(), crs, canvas->mapSettings().transformContext());

    const QgsPointXY transformedPoint = ct.transform( pointXY );

    return transformedPoint;
}


QgsPointXY QGISVisualizationWidget::getScreenCoordFromPoint(const QgsPointXY& point, const QgsMapCanvas* canvas)
{
    auto crs = QgsCoordinateReferenceSystem(QStringLiteral("EPSG:4326"));
    QgsCoordinateTransform ct(crs, canvas->mapSettings().destinationCrs(), canvas->mapSettings().transformContext());

    const QgsPointXY transformedPoint = ct.transform(point);

    auto screenPointXY = canvas->mapSettings().mapToPixel().transform(transformedPoint);

    return screenPointXY;
}


double QGISVisualizationWidget::sampleRaster(const double& x, const double& y, QgsRasterLayer* rasterlayer, const int& bandNumber)
{
    if(rasterlayer == nullptr)
    {
        this->errorMessage("Error, attempting to sample a raster layer that is a nullptr");
        return std::numeric_limits<double>::quiet_NaN();
    }

    auto numBands = rasterlayer->bandCount();

    if(bandNumber>numBands)
    {
        this->errorMessage("Error, the band number given "+QString::number(bandNumber)+" is greater than the number of bands in the raster: "+QString::number(numBands));
        return std::numeric_limits<double>::quiet_NaN();
    }

    QgsPointXY point(x,y);

    // Use the sample method below as this is considerably more efficient than other methods
    bool OK;
    auto sampleVal = rasterlayer->dataProvider()->sample(point,bandNumber,&OK,QgsRectangle(),2,2);

    if(!OK)
        this->errorMessage("Error, sampling the raster " + rasterlayer->name());


    // Test val will be NAN at failure
    return sampleVal;
}


int  QGISVisualizationWidget::joinLayers(const QgsVectorLayer* left, QgsVectorLayer* right, const QString& tag, QString& errMsg)
{
    if(left == nullptr || right == nullptr)
    {
        this->errorMessage("Error, in join layers, provided layers are nullptrs");
        return -1;
    }

    auto blockLayerFields = left->fields();

    QStringList fieldNames;
    for(int i = 0; i<blockLayerFields.count(); ++i)
    {
        auto field = blockLayerFields.at(i);
        auto fieldName = field.name();

        if(fieldName.compare("fid") == 0)
            continue;

        fieldNames.append(tag+fieldName);
    }

    std::vector<QgsFeature> lhsFeatVec;
    lhsFeatVec.reserve(left->featureCount());

    auto lhsFeatures = left->getFeatures();
    QgsFeature lhsFeat;

    // Coordinate transformation to transform from the block layer crs to the building layer crs - in the case where they are different
    if(right->crs() != left->crs())
    {
        QgsCoordinateTransform coordTrans(left->crs(), right->crs(), QgsProject::instance());
        while (lhsFeatures.nextFeature(lhsFeat))
        {
            auto geom = lhsFeat.geometry();
            geom.get()->transform(coordTrans);
            lhsFeat.setGeometry(geom);
            lhsFeatVec.push_back(lhsFeat);
        }
    }
    else
    {
        while (lhsFeatures.nextFeature(lhsFeat))
            lhsFeatVec.push_back(lhsFeat);
    }

    auto numFeat = right->featureCount();

    if(numFeat == 0)
    {
        errMsg = "Error, number of features is zero in the buildings layer "+right->name();
        return -1;
    }


    auto testInPolygon = [](const QgsGeometry& polygonGeom, const QgsPointXY& featCentroid, const QgsGeometryEngine* polygonGeometryEngine) -> bool
    {

        // Do initial bounding box check which is very fast to exclude points that are far away
        auto bb = polygonGeom.boundingBox();

        if(bb.contains(featCentroid))
        {
            if(polygonGeometryEngine->intersects(polygonGeom.constGet()))
            {
                return true;
            }
        }

        return false;
    };


    QVector< QgsAttributes > fieldAttributes(numFeat, QgsAttributes(fieldNames.size()));

    // Need to return the features with ascending ids so that when we set the updated features to the layer, things will be in order
    QgsFeatureRequest featRequest;
    QgsFeatureRequest::OrderByClause orderByClause(QString("id"),true);
    QList<QgsFeatureRequest::OrderByClause> obcList = {orderByClause};
    QgsFeatureRequest::OrderBy orderBy(obcList);
    featRequest.setOrderBy(orderBy);

    auto rhsFeatures = right->getFeatures(featRequest);
    QgsFeature rhsFeat;
    int count = 0;
    while (rhsFeatures.nextFeature(rhsFeat))
    {
        auto featGeom = rhsFeat.geometry();

        // Get the building centroid
        auto featCentroidPoint = featGeom.centroid().asPoint();

        std::unique_ptr< QgsGeometryEngine > polygonGeometryEngine(QgsGeometry::createGeometryEngine(featGeom.constGet()));

        polygonGeometryEngine->prepareGeometry();

        // Did we find the information that we are looking for
        bool found = false;

        //Iterate through the lhs layer features
        for(auto&& it : lhsFeatVec)
        {
            auto lhsFeatGeom = it.geometry();

            // Do bounding box check which is very fast to check if building is within a block group
            auto bb = lhsFeatGeom.boundingBox();
            if(bb.contains(featCentroidPoint))
            {
                if(testInPolygon(lhsFeatGeom,featCentroidPoint,polygonGeometryEngine.get()) == true)
                {
                    QgsAttributes featAtrbs = it.attributes();

                    //                auto fieldIDx = it.fieldNameIndex("fid");

                    for(int i = 0; i<featAtrbs.size(); ++i)
                    {
                        auto atrb = featAtrbs.at(i);

                        fieldAttributes[count][i] = atrb;
                    }

                    found = true;
                    break;
                }
            }
        }

        if(!found)
        {
            errMsg = "Error: could not find a census block for the feature  "+QString::number(rhsFeat.id());
            return -1;
        }

        ++count;
    }

    if(fieldNames.size() != fieldAttributes.front().size())
    {
        errMsg = "Error: inconsistency between the field names and number of attributes  "+QString::number(rhsFeat.id());
        return -1;
    }

    auto res = this->addNewFeatureAttributesToLayer(right,fieldNames,fieldAttributes,errMsg);
    if(res != 0)
        return res;

    return 0;
}


int QGISVisualizationWidget::addNewFeatureAttributesToLayer(QgsVectorLayer* layer, const QStringList& fieldNames, const QVector<QgsAttributes>& values, QString& error)
{
    if(layer == nullptr)
    {
        error = "Error, nullptr layer. Could not add new component attributes";
        return -1;
    }

    auto numFeatLayer = layer->featureCount();

    if(values.size() != numFeatLayer)
    {
        error = "Error, the number of fields and values ("+QString::number(values.size())+ ") should be equal to the number of features in the layer' (" + QString::number(numFeatLayer)+ "). /n Failed to batch add new attributes.";
        return -1;
    }


    if(values.empty())
    {
        error = "Error, empty attribute values in input. Could not add new component attributes";
        return false;
    }


    auto numNewFields = fieldNames.size();
    auto firstRow = values.first();

    if(firstRow.size() != numNewFields)
    {
        error = "Error, the number of values must match the number of fields";
        return false;
    }

    QgsFields existingFields = layer->dataProvider()->fields();

    auto fidIdx = existingFields.indexOf("fid");

    // Remove the fid index if it exists
    if(fidIdx != -1)
        existingFields.remove(fidIdx);

    for(int i = 0; i <numNewFields; ++i)
    {
        auto fieldName = fieldNames[i];

        auto fieldType = firstRow.at(i).type();

        existingFields.append(QgsField(fieldName, fieldType));
    }


    QgsFeatureRequest featRequest;

    // Need to return the features with ascending ids so that when we set the updated features to the layer, things will be in order
    QgsFeatureRequest::OrderByClause orderByClause(QString("id"),true);
    QList<QgsFeatureRequest::OrderByClause> obcList = {orderByClause};
    QgsFeatureRequest::OrderBy orderBy(obcList);
    featRequest.setOrderBy(orderBy);

    auto featIt = layer->getFeatures(featRequest);

    QgsFeatureList featList;
    featList.reserve(values.size());

    int count = 0;
    QgsFeature feat;
    while (featIt.nextFeature(feat))
    {
        auto existingAtrb = feat.attributes();

        // auto id = feat.id();
        feat.setFields(existingFields, true);

        auto newAtrb = values[count];

        existingAtrb.append(newAtrb);

        if(existingAtrb.size() != feat.fields().size())
        {
            error = "Error, the number of attributes is "+ QString::number(existingAtrb.size()) + " while the number of fields is "+QString::number(feat.fields().size());
            return -1;
        }

        // The number of provided attributes need to exactly match the number of the feature's fields.
        feat.setAttributes(existingAtrb);

        featList.push_back(feat);
        ++count;
    }

    if(values.size() != featList.size())
    {
        error = "Error, inconsistent sizes of values to update and number of updated features. Please contact developers. Could not add fields to "+layer->name();
        return -1;
    }

    // Start editing on the layer
    layer->startEditing();

    auto res = layer->dataProvider()->truncate();

    if(!res)
    {
        error = "Error, failed to remove existing features in the "+layer->name()+" layers data provider. Please contact developers";
        return -1;
    }

    //    qDebug()<<"Num attributes after truncate is "<<layer->dataProvider()->attributeIndexes().size();
    //    qDebug()<<"Num existing attributes is "<<existingFields.toList().size();

    //    for(auto&& it : existingFields.toList())
    //    {
    //       qDebug()<<it.displayName();
    //       qDebug()<<it.type();
    //    }

    res = layer->dataProvider()->addAttributes(existingFields.toList());

    if(!res)
    {
        error = "Error adding attributes to the layer " + layer->name();
        return -1;
    }

    //    qDebug()<<"Num provider fields before update is "<<layer->dataProvider()->attributeIndexes().size();



    layer->updateFields(); // tell the vector layer to fetch changes from the provider

    //    auto numAtrb = featList.first().attributeCount();
    //    auto numField = featList.first().fields().count();

    //    qDebug()<<"Num feat. fields is "<<numField;
    //    qDebug()<<"Num feat. attributes is "<<numAtrb;

    //    qDebug()<<"Num layer fields is "<<layer->fields().count();
    //    qDebug()<<"Num layer attributes is "<<layer->attributeList().count();

    //    for(auto&& it : layer->fields().toList())
    //    {
    //       qDebug()<<it.name();
    //    }

    res = layer->dataProvider()->addFeatures(featList);

    if(!res)
    {
        error = "Error, failed to add features to the 'Selected Asset Layer' data provider. Please contact developers. Could not add fields to " + layer->name();
        return -1;
    }

    layer->commitChanges(true);
    layer->updateExtents();

    return 0;
}

// BZ - 221220
QList<QgsMapLayer*> QGISVisualizationWidget::addVectorInGroup(const QString &layerPath, const QString &name, const QString &providerKey)
{
    QVariantMap uriElements = QgsProviderRegistry::instance()->decodeUri( providerKey, layerPath );
    QString path = layerPath;
    if ( uriElements.contains( QStringLiteral( "path" ) ) )
    {
      // run layer path through QgsPathResolver so that all inbuilt paths and other localised paths are correctly expanded
      path = QgsPathResolver().readPath( uriElements.value( QStringLiteral( "path" ) ).toString() );
      uriElements[ QStringLiteral( "path" ) ] = path;
    }
    // Not all providers implement decodeUri(), so use original uri if uriElements is empty
    const QString updatedUri = uriElements.isEmpty() ? layerPath : QgsProviderRegistry::instance()->encodeUri( providerKey, uriElements );

    // query sublayers
    QList< QgsProviderSublayerDetails > sublayers = QgsProviderRegistry::instance()->providerMetadata( providerKey ) ?
        QgsProviderRegistry::instance()->providerMetadata( providerKey )->querySublayers( updatedUri, Qgis::SublayerQueryFlag::IncludeSystemTables )
        : QgsProviderRegistry::instance()->querySublayers( updatedUri );

    // now add sublayers
    QList<QgsMapLayer*> mapLayers;
    if ( sublayers.empty() )
        this->errorMessage("Vector file does not contain any layers.");
    else {

      // Fetch the transform context from the current project
      QgsCoordinateTransformContext transformContext = QgsProject::instance()->transformContext();
      
      // Initialize LayerOptions with the context
      QgsProviderSublayerDetails::LayerOptions options(transformContext);
      
      for (const QgsProviderSublayerDetails &sublayer : sublayers) {
	QgsMapLayer *layer = sublayer.toLayer(options);
        
	if (layer) {
	  layer->setName(name);
	  QgsProject::instance()->addMapLayer(layer);
	  mapLayers << layer;
	}
      }
      
    }

#ifdef OpenSRA
    // force CRS=4326 - temp solution for incorrect inference of CRS during import
    for (int i=0; i<mapLayers.count(); ++i)
        mapLayers.value(i)->setCrs(QgsCoordinateReferenceSystem("EPSG:4326"));
#endif

    return mapLayers;
}
