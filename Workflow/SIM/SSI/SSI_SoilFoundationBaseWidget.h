/* *****************************************************************************
Copyright (c) 2016-2025, The Regents of the University of California (Regents).
All rights reserved.
*************************************************************************** */

#ifndef SSI_SOIL_FOUNDATION_BASE_WIDGET_H
#define SSI_SOIL_FOUNDATION_BASE_WIDGET_H

#include <QWidget>
#include <QJsonObject>
#include <QStringList>

class SSI_SoilFoundationBaseWidget : public QWidget {
    Q_OBJECT
public:
    explicit SSI_SoilFoundationBaseWidget(QWidget* parent = nullptr) : QWidget(parent) {}
    ~SSI_SoilFoundationBaseWidget() override = default;

    // Unique string identifier for this soil/foundation widget implementation
    virtual QString typeId() const = 0;

    // Validate current configuration; interactiveIfModelMissing mirrors building base semantics
    virtual bool validate(QStringList& errors, bool interactiveIfModelMissing = false) const = 0;

    // Serialize/deserialize soil & foundation configuration
    virtual bool outputToJSON(QJsonObject& soilFoundationInfo) const = 0;
    virtual bool inputFromJSON(const QJsonObject& soilFoundationInfo) = 0;

    // Optional visualization hook
    virtual void plot() const = 0;
};

#endif // SSI_SOIL_FOUNDATION_BASE_WIDGET_H





