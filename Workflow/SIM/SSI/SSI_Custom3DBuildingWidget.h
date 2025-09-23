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
    ~SSI_Custom3DBuildingWidget() override;

    QString typeId() const override { return QStringLiteral("custom_3d_building"); }

    bool validate(QStringList& errors, bool interactiveIfModelMissing = false) const override;
    bool outputToJSON(QJsonObject& structureInfo) const override;
    bool inputFromJSON(const QJsonObject& structureInfo) override;
    void plot() const override;

    bool copyFiles(QString &destDir) override;
    QStringList getRandomVariableNames() const override;

private:
    QLineEdit* modelFileLineEdit {nullptr};
    QLineEdit* meshFileLineEdit {nullptr};
    QLineEdit* responseNodesLineEdit {nullptr};
    QSpinBox* numPartitionsSpin {nullptr};
    QTableWidget* columnsTable {nullptr};
    QDoubleSpinBox *xMinSpin {nullptr}, *xMaxSpin {nullptr};
    QDoubleSpinBox *yMinSpin {nullptr}, *yMaxSpin {nullptr};
    QDoubleSpinBox *zMinSpin {nullptr}, *zMaxSpin {nullptr};
    QStringList psetVarNamesAndValues;

    void setupFilesAndCoresGroup(QWidget* parentWidget);
    void setupColumnsBaseTable(QWidget* parentWidget);
    void setupBoundsGroup(QWidget* parentWidget);

    bool getTableRow(int row, int& tag, double& x, double& y, double& z) const;

    void onModelFileChanged(const QString& filePath);
    void removeRegisteredPsets();
};

#endif // SSI_CUSTOM3D_BUILDING_WIDGET_H




