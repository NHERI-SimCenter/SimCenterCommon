/* *****************************************************************************
Copyright (c) 2016-2025, The Regents of the University of California (Regents).
All rights reserved.
*************************************************************************** */

#ifndef SSI_SIMULATION_H
#define SSI_SIMULATION_H

#include <SimCenterAppWidget.h>
#include "SSI_BuildingWidgetBase.h"
#include "SSI_SoilFoundationBaseWidget.h"

class QTabWidget;
class QWidget;
class QPushButton;
class QStackedWidget;
class QComboBox;

class SSI_Simulation : public SimCenterAppWidget {
    Q_OBJECT
public:
    explicit SSI_Simulation(QWidget* parent = nullptr);
    ~SSI_Simulation() override;

    bool outputToJSON(QJsonObject &jsonObj) override;
    bool inputFromJSON(QJsonObject &jsonObj) override;
    bool outputAppDataToJSON(QJsonObject &jsonObj) override;
    bool inputAppDataFromJSON(QJsonObject &jsonObj) override;
    

private slots:
    void onPlotClicked();
    void onValidateClicked();

private:
    bool validate(QStringList &errors);

    QTabWidget* tabWidget {nullptr};
    QWidget* buildingTabContainer {nullptr};
    QStackedWidget* buildingStack {nullptr};
    QComboBox* buildingSelector {nullptr};
    SSI_BuildingWidgetBase* currentBuilding {nullptr};
    QWidget* soilTabContainer {nullptr};
    QStackedWidget* soilStack {nullptr};
    QComboBox* soilSelector {nullptr};
    SSI_SoilFoundationBaseWidget* currentSoil {nullptr};

    QPushButton* plotButton {nullptr};
    QPushButton* validateButton {nullptr};
};

#endif // SSI_SIMULATION_H




