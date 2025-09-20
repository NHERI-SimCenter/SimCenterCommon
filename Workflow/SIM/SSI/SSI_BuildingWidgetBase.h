/* *****************************************************************************
Copyright (c) 2016-2025, The Regents of the University of California (Regents).
All rights reserved.
*************************************************************************** */

#ifndef SSI_BUILDING_WIDGET_BASE_H
#define SSI_BUILDING_WIDGET_BASE_H

#include <QWidget>
#include <QJsonObject>
#include <QStringList>

class SSI_BuildingWidgetBase : public QWidget {
    Q_OBJECT
public:
    explicit SSI_BuildingWidgetBase(QWidget* parent = nullptr) : QWidget(parent) {}
    ~SSI_BuildingWidgetBase() override = default;

    virtual QString typeId() const = 0;

    virtual bool validate(QStringList& errors, bool interactiveIfModelMissing = false) const = 0;
    virtual bool outputToJSON(QJsonObject& structureInfo) const = 0;
    virtual bool inputFromJSON(const QJsonObject& structureInfo) = 0;
    virtual void plot() const = 0;
};

#endif // SSI_BUILDING_WIDGET_BASE_H


