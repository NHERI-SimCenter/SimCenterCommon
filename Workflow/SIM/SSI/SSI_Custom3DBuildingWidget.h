/* *****************************************************************************
Copyright (c) 2016-2025, The Regents of the University of California (Regents).
All rights reserved.
*************************************************************************** */

#ifndef SSI_CUSTOM3D_BUILDING_WIDGET_H
#define SSI_CUSTOM3D_BUILDING_WIDGET_H

#include "SSI_BuildingWidgetBase.h"

class QGroupBox;
class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QTableWidget;

class SSI_Custom3DBuildingWidget : public SSI_BuildingWidgetBase {
    Q_OBJECT
public:
    explicit SSI_Custom3DBuildingWidget(QWidget* parent = nullptr);
    ~SSI_Custom3DBuildingWidget() override = default;

    QString typeId() const override { return QStringLiteral("custom_3d_building"); }

    bool validate(QStringList& errors, bool interactiveIfModelMissing = false) const override;
    bool outputToJSON(QJsonObject& structureInfo) const override;
    bool inputFromJSON(const QJsonObject& structureInfo) override;
    void plot() const override;

private:
    QLineEdit* modelFileLineEdit {nullptr};
    QLineEdit* meshFileLineEdit {nullptr};
    QSpinBox* numPartitionsSpin {nullptr};
    QTableWidget* columnsTable {nullptr};
    QDoubleSpinBox *xMinSpin {nullptr}, *xMaxSpin {nullptr};
    QDoubleSpinBox *yMinSpin {nullptr}, *yMaxSpin {nullptr};
    QDoubleSpinBox *zMinSpin {nullptr}, *zMaxSpin {nullptr};

    void setupFilesAndCoresGroup(QWidget* parentWidget);
    void setupColumnsBaseTable(QWidget* parentWidget);
    void setupBoundsGroup(QWidget* parentWidget);

    bool getTableRow(int row, int& tag, double& x, double& y, double& z) const;
};

#endif // SSI_CUSTOM3D_BUILDING_WIDGET_H




