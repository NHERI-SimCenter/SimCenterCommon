/* *****************************************************************************
Copyright (c) 2016-2025, The Regents of the University of California (Regents).
All rights reserved.
*************************************************************************** */

#ifndef SSI_SOIL_FOUNDATION_TYPE1_WIDGET_H
#define SSI_SOIL_FOUNDATION_TYPE1_WIDGET_H

#include "SSI_SoilFoundationBaseWidget.h"

class QGroupBox;
class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;
class QTableWidget;
class QTableWidgetItem;
class QComboBox;
class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QPushButton;
class QCheckBox;

class SSI_SoilFoundationType1Widget : public SSI_SoilFoundationBaseWidget {
    Q_OBJECT
public:
    explicit SSI_SoilFoundationType1Widget(QWidget* parent = nullptr);
    ~SSI_SoilFoundationType1Widget() override = default;

    QString typeId() const override { return QStringLiteral("soil_foundation_type_1"); }

    bool validate(QStringList& errors, bool interactiveIfModelMissing = false) const override;
    bool outputToJSON(QJsonObject& soilFoundationInfo) const override;
    bool inputFromJSON(const QJsonObject& soilFoundationInfo) override;
    void plot() const override;
    int  getNumberOfCores() const override;

    bool copyFiles(QString &destDir) override { Q_UNUSED(destDir); return true; }
    QStringList getRandomVariableNames() const override;

private:
    // Soil
    QDoubleSpinBox *xMinSoil {nullptr}, *xMaxSoil {nullptr};
    QDoubleSpinBox *yMinSoil {nullptr}, *yMaxSoil {nullptr};
    QSpinBox *nxSoil {nullptr}, *nySoil {nullptr};
    QDoubleSpinBox *gravZSoil {nullptr};
    QSpinBox *numPartsSoil {nullptr};
    QComboBox *boundaryCombo {nullptr};
    QGroupBox *drmOptionsGroup {nullptr};
    QComboBox *absorbingLayerCombo {nullptr};
    QSpinBox *drmNumPartitions {nullptr};
    QSpinBox *drmNumLayers {nullptr};
    QDoubleSpinBox *drmRayleighDamping {nullptr};
    QCheckBox *drmMatchDamping {nullptr};
    QTableWidget* soilProfileTable {nullptr};

    // Foundation
    QDoubleSpinBox *gravZFound {nullptr};
    QCheckBox *embeddedFoundation {nullptr};
    QDoubleSpinBox *dxFound {nullptr}, *dyFound {nullptr}, *dzFound {nullptr};
    QSpinBox *numPartsFound {nullptr};
    QDoubleSpinBox *columnEmbedDepth {nullptr};
    QDoubleSpinBox *secE {nullptr}, *secA {nullptr}, *secIy {nullptr}, *secIz {nullptr}, *secG {nullptr}, *secJ {nullptr};
    QCheckBox* showSectionPropsCheck {nullptr};
    QWidget* sectionPropsContainer {nullptr};
    QTableWidget* foundationProfileTable {nullptr};

    // Piles
    QTableWidget* pileProfileTable {nullptr};
    QSpinBox *pileNumPointsPerimeter {nullptr};
    QSpinBox *pileNumPointsLength {nullptr};
    QDoubleSpinBox *pilePenaltyParameter {nullptr};

    void setupSoilGroup(QWidget* parentWidget);
    void setupFoundationGroup(QWidget* parentWidget);
    void setupPilesGroup(QWidget* parentWidget);

    static QList<double> parseCsvDoubles(const QString& text, bool* ok);
    static QStringList parseCsvTokens(const QString& text, bool* ok);
    void registerRVsFromCsv(const QString& text) const;
};

#endif // SSI_SOIL_FOUNDATION_TYPE1_WIDGET_H





