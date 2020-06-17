#ifndef GraphicView2D_H
#define GraphicView2D_H

#include <QWidget>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QCuboidMesh>
#include <QVector3D>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QEntity>

typedef struct _vertexStruct
{
    float position[3];
    float color[3];
} vertexStruct;

class GraphicView2D : public QWidget
{
    Q_OBJECT
public:
    explicit GraphicView2D(QWidget *parent = nullptr);

    void update();
    void reset();
    void drawLine(int tag, float x1, float y1, float x2, float y2, float thick, float r, float g, float b, float w =1.0);
    void drawPoint(int tag, float x1, float y1, int numPixels, float r, float g, float b, float width =9.0, float height =9.0);
    void drawCube(int tag, float x1, float y1, int numPixels, float r, float g, float b, float width =9.0, float height =9.0);

    void drawText(int tag, float x1, float y1, char *text, float r, float g, float b);
    
signals:

public slots:

private:
    void updateCameraPosition(void);

    Qt3DExtras::Qt3DWindow *graphicsWindow;
    Qt3DCore::QEntity *rootEntity;

    void setup3DView();
    void setCamera();
    void setLights(Qt3DCore::QEntity* rootEntity);

    int *pointIDs;
    int numPoint, maxNumPoint;
    float *pointVertices;
    float *pointColors;

    int *lineIDs;
    int numLine, maxNumLine;
    float *lineVertices;
    float *lineColors;

    vertexStruct *thePointVertices;
    vertexStruct *theLineVertices;

    double minX, maxX;
    double minY, maxY;
};

#endif // GraphicView2D
