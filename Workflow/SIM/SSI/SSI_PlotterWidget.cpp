/* *****************************************************************************
Copyright (c) 2016-2025, The Regents of the University of California (Regents).
All rights reserved.
*************************************************************************** */

#include "SSI_PlotterWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QJsonDocument>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>

SSI_PlotterWidget::SSI_PlotterWidget(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void SSI_PlotterWidget::setupUi() {
    auto layout = new QVBoxLayout(this);
    auto header = new QLabel("Plotter (VTK placeholder)\nThis will render the current model. For now, it echoes JSON.");
    header->setWordWrap(true);
    layout->addWidget(header);

    renderWidget = new QVTKOpenGLNativeWidget();
    renderWidget->setMinimumHeight(300);
    layout->addWidget(renderWidget, 2);

    jsonView = new QPlainTextEdit();
    jsonView->setReadOnly(true);
    layout->addWidget(jsonView, 1);
}

void SSI_PlotterWidget::setupOrUpdateVtk() {
    if(!renderer) {
        renderer = vtkSmartPointer<vtkRenderer>::New();
        renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
        renderWindow->AddRenderer(renderer);
        renderWidget->setRenderWindow(renderWindow);
    }
    renderWindow->Render();
}

void SSI_PlotterWidget::renderModel(const QJsonObject& modelingObj) {
    // Update VTK scene here later; for now show JSON
    QJsonDocument doc(modelingObj);
    jsonView->setPlainText(QString::fromUtf8(doc.toJson(QJsonDocument::Indented)));
    setupOrUpdateVtk();
}



