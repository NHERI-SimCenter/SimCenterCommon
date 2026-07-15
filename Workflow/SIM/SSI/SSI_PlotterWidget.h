/* *****************************************************************************
Copyright (c) 2016-2025, The Regents of the University of California (Regents).
All rights reserved.
*************************************************************************** */

#ifndef SSI_PLOTTER_WIDGET_H
#define SSI_PLOTTER_WIDGET_H

#include <QWidget>
#include <QJsonObject>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkSmartPointer.h>

class vtkRenderer;
class vtkGenericOpenGLRenderWindow;

class QPlainTextEdit;
class QLabel;

class SSI_PlotterWidget : public QWidget {
    Q_OBJECT
public:
    explicit SSI_PlotterWidget(QWidget* parent = nullptr);
    ~SSI_PlotterWidget() override = default;

    // Render/update view from a Modeling JSON payload
    void renderModel(const QJsonObject& modelingObj);

private:
    QVTKOpenGLNativeWidget* renderWidget {nullptr};
    QPlainTextEdit* jsonView {nullptr};

    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;

    void setupUi();
    void setupOrUpdateVtk();
};

#endif // SSI_PLOTTER_WIDGET_H





