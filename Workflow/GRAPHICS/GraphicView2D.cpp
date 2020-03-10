
#include "GraphicView2D.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QAspectEngine>
#include <QColor>

#include <Qt3DInput/QInputAspect>
//#include <Qt3DRender/QCuboidMesh>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QPointSize>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongAlphaMaterial>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DRender/QDirectionalLight>
#include <Qt3DExtras/QCuboidMesh>


GraphicView2D::GraphicView2D(QWidget *parent) : QWidget(parent), rootEntity(NULL),
    minX(0), maxX(0), minY(0), maxY(0)
{

    auto layout = new QHBoxLayout(this);

    graphicsWindow = new Qt3DExtras::Qt3DWindow();
    graphicsWindow->setFlags(Qt::FramelessWindowHint);

    auto container = QWidget::createWindowContainer(graphicsWindow, this);
    layout->addWidget(container);

    this->setLayout(layout);
    this->setStyleSheet("QWidget{border:2px solid black}");

    this->setup3DView();

    numPoint = 0;
    maxNumPoint = 16;
    pointIDs = new int[maxNumPoint];
    pointVertices = new float[maxNumPoint*3];
    pointColors = new float[maxNumPoint*3];

    numLine = 0;
    maxNumLine = 16;
    lineIDs = new int[maxNumLine];
    lineVertices = new float[maxNumLine*2*3];
    lineColors = new float[maxNumLine*2*3];

    thePointVertices = new vertexStruct[maxNumPoint];
    theLineVertices = new vertexStruct[maxNumLine*2]; 

}

void GraphicView2D::update(){

    return;
}

void GraphicView2D::updateCameraPosition(){
    Qt3DRender::QCamera *camera = graphicsWindow->camera();
    float height = maxY-minY;
    float width = maxX-minX;

    float boundHmax = maxY + height*.1;
    float boundW = width*1.1;
    float boundHmin = minY - (height*.1);
    if (boundHmax == 0)
        boundHmax = 10.0;
    if (boundW == 0)
        boundW = 10.0;

    camera->lens()->setOrthographicProjection(-boundW, boundW, boundHmin, boundHmax,-1000.0f, 1000.0f);
    camera->setPosition(QVector3D(0., 0, 500.));
}

void GraphicView2D::setup3DView()
{
    rootEntity = new Qt3DCore::QEntity();

    setCamera();

    setLights(rootEntity);

    graphicsWindow->setRootEntity(rootEntity);
}


void GraphicView2D::setCamera()
{
    Qt3DRender::QCamera *camera = graphicsWindow->camera();
    float height = maxY-minY;
    float width = maxX-minX;

    float boundH = height*1.1;
    float boundW = width*1.1;
    if (boundH == 0)
        boundH = 10.0;
    if (boundW == 0)
        boundW = 10.0;

    camera->lens()->setOrthographicProjection(minX, maxX, minY, maxY,-1000.0f, 1000.0f);
    camera->setPosition(QVector3D((maxX+minX)/2., (maxY+minY)/2, 100.));
}

void GraphicView2D::setLights(Qt3DCore::QEntity *rootEntity)
{
    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(rootEntity);
    auto light = new Qt3DRender::QDirectionalLight(lightEntity);
    light->setWorldDirection(QVector3D(-80.0, -10.0, -25.0));
    light->setColor("white");
    light->setIntensity(100.0);
    lightEntity->addComponent(light);

    auto lightTransform = new Qt3DCore::QTransform;
    lightTransform->setTranslation(QVector3D(80.0, 0, 25.0));
    lightEntity->addComponent(lightTransform);
}


void
GraphicView2D::drawPoint(int tag, float x1, float y1, int numPixels, float r, float g, float b, float w, float h)
{
    Q_UNUSED(tag);
    Q_UNUSED(w);
    Q_UNUSED(h);

    bool resetCamera = false;

    if (x1 < minX) {
        resetCamera = true;
        minX = x1;
    } else if (x1 > maxX) {
        resetCamera = true;
        maxX = x1;
    }
    if (y1 < minY) {
        resetCamera = true;
        minY = y1;
    } else if (y1 > maxY) {
        resetCamera = true;
        maxY = y1;
    }

    Qt3DCore::QEntity* pointEntity = new Qt3DCore::QEntity(rootEntity);

    QByteArray coordsBytes;
    coordsBytes.resize(3 * 1 * sizeof(float)); 
    float *positions = reinterpret_cast<float*>(coordsBytes.data());
    *positions++ = x1;
    *positions++ = y1;
    *positions++ = 0;

    auto *coordsBuffer = new Qt3DRender::QBuffer();
    coordsBuffer->setData(coordsBytes);

    auto pointVerticesAttribute = new Qt3DRender::QAttribute(
		coordsBuffer,
                Qt3DRender::QAttribute::defaultPositionAttributeName(),
                Qt3DRender::QAttribute::Float,
                3,
                1,
                0,
                3 * sizeof (float));

    auto pointGeometry = new Qt3DRender::QGeometry();
    pointGeometry->addAttribute(pointVerticesAttribute);

    auto pointRenderer = new Qt3DRender::QGeometryRenderer();
    pointRenderer->setGeometry(pointGeometry);
    pointRenderer->setFirstVertex(0);
    pointRenderer->setVertexCount(1);
    pointRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Points);

    auto pointMaterial = new Qt3DExtras::QPhongMaterial();
    QColor color(255*r,255*g,255*b);
    pointMaterial->setAmbient(color);
    pointMaterial->setSpecular(color);
    pointMaterial->setDiffuse(color);

    auto effect = pointMaterial->effect();
    for (auto technique : effect->techniques()) {
        for (auto renderPass : technique->renderPasses()) {
            auto pointSize = new Qt3DRender::QPointSize();
            pointSize->setSizeMode(Qt3DRender::QPointSize::SizeMode::Fixed);
            pointSize->setValue(numPixels*1.0f);
            renderPass->addRenderState(pointSize);
        }
    }

    auto pointTransform = new Qt3DCore::QTransform();

    pointEntity->addComponent(pointRenderer);
    pointEntity->addComponent(pointMaterial);
    pointEntity->addComponent(pointTransform);

    //POINT
    /*
    auto *pointEntity = new Qt3DCore::QEntity(rootEntity);

    pointEntity->addComponent(cuboid);
    pointEntity->addComponent(material);
    pointEntity->addComponent(transform);
    */


    if (resetCamera == true)
      this->updateCameraPosition();
}

void
GraphicView2D::drawLine(int tag, float x1, float y1, float x2, float y2, float thick, float r, float g, float b, float w)
{
    Q_UNUSED(tag);
    Q_UNUSED(w);
    Q_UNUSED(thick);

    bool resetCamera = false;

    if (x1 < minX) {
        resetCamera = true;
        minX = x1;
    } else if (x1 > maxX) {
        resetCamera = true;
        maxX = x1;
    }
    if (y1 < minY) {
        resetCamera = true;
        minY = y1;
    } else if (y1 > maxY) {
        resetCamera = true;
        maxY = y1;
    }

    if (x2 < minX) {
        resetCamera = true;
        minX = x2;
    } else if (x2 > maxX) {
        resetCamera = true;
        maxX = x2;
    }
    if (y2 < minY) {
        resetCamera = true;
        minY = y2;
    } else if (y2 > maxY) {
        resetCamera = true;
        maxY = y2;
    }

    auto *geometry = new Qt3DRender::QGeometry();

    // position vertices (start and end)
    QByteArray bufferBytes;
    bufferBytes.resize(3 * 2 * sizeof(float)); 
    float *positions = reinterpret_cast<float*>(bufferBytes.data());
    *positions++ = x1;
    *positions++ = y1;
    *positions++ = 0;
    *positions++ = x2;
    *positions++ = y2;
    *positions++ = 0;

    auto *buf = new Qt3DRender::QBuffer(geometry);
    buf->setData(bufferBytes);

    auto *positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buf);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(2);
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

    // connectivity between vertices
    QByteArray indexBytes;
    indexBytes.resize(2 * sizeof(unsigned int)); // start to end
    unsigned int *indices = reinterpret_cast<unsigned int*>(indexBytes.data());
    *indices++ = 0;
    *indices++ = 1;

    auto *indexBuffer = new Qt3DRender::QBuffer(geometry);
    indexBuffer->setData(indexBytes);

    auto *indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(2);
    geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

    // mesh
    auto *line = new Qt3DRender::QGeometryRenderer();
    line->setGeometry(geometry);
    line->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    auto *material = new Qt3DExtras::QPhongMaterial();
    QColor color(255*r,255*g,255*b);
    material->setAmbient(color);

    // entity
    auto *lineEntity = new Qt3DCore::QEntity(rootEntity);
    lineEntity->addComponent(line);
    lineEntity->addComponent(material);

  //  rootEntity->addComponent(lineEntity);

    if (resetCamera == true)
        this->updateCameraPosition();
}


void
GraphicView2D::drawCube(int tag, float x1, float y1, int numPixels, float r, float g, float b, float w, float h)
{
    Q_UNUSED(tag);
    Q_UNUSED(numPixels);

    bool resetCamera = false;

    if (x1 < minX) {
        resetCamera = true;
        minX = x1;
    } else if (x1 > maxX) {
        resetCamera = true;
        maxX = x1;
    }
    if (y1 < minY) {
        resetCamera = true;
        minY = y1;
    } else if (y1 > maxY) {
        resetCamera = true;
        maxY = y1;
    }


    Qt3DExtras::QCuboidMesh *cuboid = new Qt3DExtras::QCuboidMesh();
    auto transform = new Qt3DCore::QTransform();
    transform->setScale3D(QVector3D(w,h,0.0));
    transform->setTranslation(QVector3D(x1, y1, 0.));

       // CuboidMesh Transform
      // Qt3DExtras::QScaleTransform *cuboidScale = new Qt3DExtras::QScaleTransform();
      // Qt3D::QTranslateTransform *cuboidTranslation = new Qt3D::QTranslateTransform();
      // Qt3D::QTransform *cuboidTransforms = new Qt3D::QTransform();

      // cuboidScale->setScale3D(QVector3D(4.0f, 4.0f, 4.0f));
      // cuboidTranslation->setTranslation(QVector3D(x1, y1, 0.0f));

       //cuboidTransforms->addTransform(cuboidTranslation);
       //cuboidTransforms->addTransform(cuboidScale);

    auto *material = new Qt3DExtras::QPhongMaterial();
    QColor color(255*r,255*g,255*b);
    material->setDiffuse(color);

    //   Qt3D::QPhongMaterial *cuboidMaterial = new Qt3D::QPhongMaterial();
    //   cuboidMaterial->setDiffuse(QColor(r,g,b));

       //Cuboid
    auto *pointEntity = new Qt3DCore::QEntity(rootEntity);

       pointEntity->addComponent(cuboid);
       pointEntity->addComponent(material);
       pointEntity->addComponent(transform);

      // rootEntity->addComponent(pointEntity);

    /*
    auto *geometry = new Qt3DRender::QGeometry(rootEntity);

    auto coordsBuffer = new Qt3DRender::QBuffer();
    float coords[12]{-0.5f, -0.5f, 0.,
                    0.5f, -0.5f, 0.,
                    0.5f, 0.5f, 0.,
                    -0.5f, 0.5f, 0.};

    QByteArray coordsBytes(reinterpret_cast<const char *>(&coords), sizeof (float) * 8);
    coordsBuffer->setData(coordsBytes);

    auto *positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(coordsBuffer);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(4);
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry


    // connectivity between vertices
    auto connectivityBuffer = new Qt3DRender::QBuffer();
    uint connectivity[8]{0, 1, 1, 2, 2, 3, 3, 0};

    QByteArray connectivityBytes(reinterpret_cast<const char *>(&connectivity), sizeof (uint) * 8);
    connectivityBuffer->setData(connectivityBytes);

    auto indexAttribute = new Qt3DRender::QAttribute(
                connectivityBuffer,
                Qt3DRender::QAttribute::defaultJointIndicesAttributeName(),
                Qt3DRender::QAttribute::UnsignedInt,
                2,
                8,
                0,
                2 * sizeof (uint));


    auto *indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(connectivityBuffer);
    indexAttribute->setCount(8);

    geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

    // mesh
    auto *line = new Qt3DRender::QGeometryRenderer(rootEntity);
    line->setGeometry(geometry);
    line->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    auto *material = new Qt3DExtras::QPhongMaterial(rootEntity);
    QColor color(0,0,0);
    material->setAmbient(color);
    material->setDiffuse(color);

    // entity
    auto *pointEntity = new Qt3DCore::QEntity(rootEntity);
    pointEntity->addComponent(line);
    pointEntity->addComponent(material);

    auto transform = new Qt3DCore::QTransform(rootEntity);
    transform->setScale3D(QVector3D(9.0,9.0,0.0));
    transform->setTranslation(QVector3D(x1, y1, 0.));
    pointEntity->addComponent(transform);
    */

       if (resetCamera == true)
           this->updateCameraPosition();
}


/*
void
GraphicView2D::drawLine(int tag, float x1, float y1, float x2, float y2, float thick, float r, float g, float b)
{

    numLine++;
    if (numLine > maxNumLine) {
        float *oldLineColors = lineColors;
        float *oldLineVertices = lineVertices;
        int *oldLineIDs = lineIDs;

        int newLineSize = (maxNumLine+32)*2*3;
        lineVertices = new float[newLineSize];
        lineColors = new float[newLineSize];
        lineIDs = new int[(maxNumLine+32)];

        for (int i=0; i<2*maxNumLine*3; i++) {
            lineVertices[i] = oldLineVertices[i];
            lineColors[i] = oldLineColors[i];
        }
        for (int i=0; i<maxNumLine; i++) {
            lineIDs[i] = oldLineIDs[i];
        }
        if (oldLineVertices != 0)
            delete [] oldLineVertices;
        if (oldLineColors != 0)
            delete [] oldLineColors;
        if (oldLineIDs != 0)
            delete [] oldLineIDs;
        maxNumLine += 32;
    }

    float *locInVertices = &lineVertices[2*(numLine-1)*3];
    float *locInColors = &lineColors[2*(numLine-1)*3];

    locInVertices[0] = x1;
    locInVertices[1] = y1;
    locInVertices[2] = 0.;
    locInVertices[3] = x2;
    locInVertices[4] = y2;
    locInVertices[5] = 0.;

    locInColors[0] = r;
    locInColors[1] = g;
    locInColors[2] = b;
    locInColors[3] = r;
    locInColors[4] = g;
    locInColors[5] = b;
}
*/
void GraphicView2D::reset() {

    rootEntity->childNodes().clear();

    graphicsWindow->setRootEntity(NULL);
    delete rootEntity;
    rootEntity = NULL;

    this->setup3DView();

    numPoint = 0;
    numLine = 0;
    minX = 0;
    maxX = 0;
    minY = 0;
    maxY = 0;

    return;
}
