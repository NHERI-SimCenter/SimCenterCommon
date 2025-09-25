/* *****************************************************************************
Copyright (c) 2016-2025, The Regents of the University of California (Regents).
All rights reserved.
*************************************************************************** */

#include "SSI_Simulation.h"
#include "SSI_Custom3DBuildingWidget.h"
#include "SSI_SoilFoundationType1Widget.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSet>
#include <RandomVariablesContainer.h>
#include <QDir>
#include <SimCenterAppWidget.h>

SSI_Simulation::SSI_Simulation(QWidget* parent) : SimCenterAppWidget(parent) {
    auto mainLayout = new QVBoxLayout();

    tabWidget = new QTabWidget();
    soilTabContainer = new QWidget();
    buildingTabContainer = new QWidget();
    auto buildingContainerLayout = new QVBoxLayout(buildingTabContainer);
    buildingSelector = new QComboBox();
    buildingStack = new QStackedWidget();

    // Register available building widgets
    auto custom3D = new SSI_Custom3DBuildingWidget();
    buildingSelector->addItem("Custom 3D Building", custom3D->typeId());
    buildingStack->addWidget(custom3D);
    currentBuilding = custom3D;

    buildingContainerLayout->addWidget(buildingSelector);
    buildingContainerLayout->addWidget(buildingStack, 1);

    // Soil tab UI with selector and stacked widget
    auto soilContainerLayout = new QVBoxLayout(soilTabContainer);
    soilSelector = new QComboBox();
    soilStack = new QStackedWidget();
    // register Soil & Foundation Type 1
    auto soil1 = new SSI_SoilFoundationType1Widget();
    soilSelector->addItem("Soil & Foundation Type 1", soil1->typeId());
    soilStack->addWidget(soil1);
    currentSoil = soil1;
    soilContainerLayout->addWidget(soilSelector);
    soilContainerLayout->addWidget(soilStack, 1);

    tabWidget->addTab(buildingTabContainer, "Building");
    tabWidget->addTab(soilTabContainer, "Soil and Foundation");
    mainLayout->addWidget(tabWidget, 1);

    auto buttonsLayout = new QHBoxLayout();
    plotButton = new QPushButton("Plot");
    validateButton = new QPushButton("Validate");
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(plotButton);
    buttonsLayout->addWidget(validateButton);
    mainLayout->addLayout(buttonsLayout);

    setLayout(mainLayout);

    connect(plotButton, &QPushButton::clicked, this, &SSI_Simulation::onPlotClicked);
    connect(validateButton, &QPushButton::clicked, this, &SSI_Simulation::onValidateClicked);

    connect(buildingSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        buildingStack->setCurrentIndex(idx);
        currentBuilding = qobject_cast<SSI_BuildingWidgetBase*>(buildingStack->currentWidget());
    });

    connect(soilSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        soilStack->setCurrentIndex(idx);
        currentSoil = qobject_cast<SSI_SoilFoundationBaseWidget*>(soilStack->currentWidget());
    });
}

SSI_Simulation::~SSI_Simulation() {}

bool SSI_Simulation::outputToJSON(QJsonObject &jsonObj) {
    // first validate
    QStringList errors;
    if (!this->validate(errors)) {
        // error message box
        QMessageBox msgBox(QMessageBox::Critical,
                   tr("Validation Error"),
                   tr("Cannot output to for SImulation SSI. \nPlease fix the following errors on SSI:\n\n --") + errors.join("\n --"),
                   QMessageBox::Ok,
                   this);
        msgBox.ensurePolished();
        const QPoint parentCenterGlobal = this->mapToGlobal(this->rect().center());
        const QSize dialogSize = msgBox.sizeHint();
        msgBox.move(parentCenterGlobal - QPoint(dialogSize.width() / 2, dialogSize.height() / 2));
        msgBox.exec();
        return false;
    } else {
        // continue
        QMessageBox msgBox(QMessageBox::Information,
                   tr("Validation"),
                   tr("Validation passed."),
                   QMessageBox::Ok,
                   this);
        msgBox.ensurePolished();
        const QPoint parentCenterGlobal = this->mapToGlobal(this->rect().center());
        const QSize dialogSize = msgBox.sizeHint();
        msgBox.move(parentCenterGlobal - QPoint(dialogSize.width() / 2, dialogSize.height() / 2));
        msgBox.exec();  
    }

    jsonObj["type"] = "SSISimulation";
    jsonObj["building_type"] = currentBuilding->typeId();
    QJsonObject structureInfo;
    if (currentBuilding->outputToJSON(structureInfo)) {
        jsonObj["structure_info"] = structureInfo;
    } else {
        QMessageBox::critical(this, "Error", "Failed to output structure info to JSON for SSI Simulation.");
        return false;
    }
    if (currentSoil) {
        jsonObj["soil_type"] = currentSoil->typeId();
        QJsonObject soilInfo;
        if (currentSoil->outputToJSON(soilInfo)) {
            jsonObj["soil_foundation_info"] = soilInfo;
        } else {
            QMessageBox::critical(this, "Error", "Failed to output soil & foundation info to JSON for SSI Simulation.");
            return false;
        }
    }
    // Aggregate Random Variables reported by active widgets
    {
        QJsonArray rvArray;
        QSet<QString> seen;

        if (currentBuilding) {
            const QStringList names = currentBuilding->getRandomVariableNames();
            for (const QString &name : names) {
                if (name.isEmpty() || seen.contains(name)) continue;
                seen.insert(name);
                QJsonObject rvObj; rvObj["name"] = name; rvObj["value"] = QString("RV.") + name; rvArray.append(rvObj);
            }
        }
        if (currentSoil) {
            const QStringList names = currentSoil->getRandomVariableNames();
            for (const QString &name : names) {
                if (name.isEmpty() || seen.contains(name)) continue;
                seen.insert(name);
                QJsonObject rvObj; rvObj["name"] = name; rvObj["value"] = QString("RV.") + name; rvArray.append(rvObj);
            }
        }

        jsonObj["randomVar"] = rvArray;
    }

    // add numCores
    int const totalCores = getNumberOfCores();
    jsonObj["numCores"] = totalCores;

    return true;
}

bool SSI_Simulation::inputFromJSON(QJsonObject &jsonObj) {
    if (jsonObj.contains("type") && jsonObj.value("type").toString() != "SSISimulation") {
        QMessageBox::critical(this, "Error", "The provided JSON is not of type 'SSISimulation'.");
        return false;
    }
    if (jsonObj.contains("type") && jsonObj.value("type").toString() == "SSISimulation") {
        if (jsonObj.contains("building_type")) {
            const QString type = jsonObj.value("building_type").toString();
            for (int i = 0; i < buildingSelector->count(); ++i) {
                if (buildingSelector->itemData(i).toString() == type) {
                    buildingSelector->setCurrentIndex(i);
                    break;
                }
            }
        }
        // Delegate to current building
        if (currentBuilding && jsonObj.contains("structure_info")) {
            const QJsonObject structureInfo = jsonObj.value("structure_info").toObject();
            if (!currentBuilding->inputFromJSON(structureInfo))
                QMessageBox::critical(this, "Error", "Failed to load structure info from JSON to the current building widget.");
        } else if (!currentBuilding) {
            QMessageBox::critical(this, "Error", "No building widget is currently selected to load structure info.");
            return false;
        } else if (!jsonObj.contains("structure_info")) {
            QMessageBox::warning(this, "Warning", "No structure_info found in JSON to load.");
        }

        if (jsonObj.contains("soil_type")) {
            const QString soilType = jsonObj.value("soil_type").toString();
            for (int i = 0; i < soilSelector->count(); ++i) {
                if (soilSelector->itemData(i).toString() == soilType) {
                    soilSelector->setCurrentIndex(i);
                    break;
                }
            }
        }
        if (currentSoil && jsonObj.contains("soil_foundation_info")) {
            const QJsonObject soilInfo = jsonObj.value("soil_foundation_info").toObject();
            if (!currentSoil->inputFromJSON(soilInfo))
                QMessageBox::critical(this, "Error", "Failed to load soil & foundation info from JSON to the current soil widget.");
        } else if (jsonObj.contains("soil_foundation_info") && !currentSoil) {
            QMessageBox::warning(this, "Warning", "Soil & foundation info is present but no soil widget is registered.");
        }
    }
    return true;
}

bool SSI_Simulation::outputAppDataToJSON(QJsonObject &jsonObj) {
    // Ensure Applications.Modeling is present when saving
    jsonObj["Application"] = QString("SSISimulation");
    QJsonObject appData; // reserved for future options
    jsonObj["ApplicationData"] = appData;
    return true;
}

bool SSI_Simulation::inputAppDataFromJSON(QJsonObject &jsonObj) {
    // Accept legacy or minimal Modeling entries gracefully
    Q_UNUSED(jsonObj);
    return true;
}

bool SSI_Simulation::copyFiles(QString &destDir) {
    // Delegate to child widgets when possible; copy any referenced files needed to run
    bool ok = true;

    // Building: invoke widget's own copyFiles
    if (currentBuilding) {
        ok = currentBuilding->copyFiles(destDir) && ok;
    }

    // Soil/Foundation: invoke widget's own copyFiles
    if (currentSoil) {
        ok = currentSoil->copyFiles(destDir) && ok;
    }

    return ok;
}

bool SSI_Simulation::validate(QStringList &errors) {
  bool valid = currentBuilding ? currentBuilding->validate(errors, true) : false;
  if (!valid)
      return false;
  if (currentSoil) {
      QStringList soilErrors;
      bool soilValid = currentSoil->validate(soilErrors, true);
      if (!soilValid) {
          errors.append(soilErrors);
          return false;
      }
  }
  return true;
}

void SSI_Simulation::onValidateClicked() {

    // call the validate function
    QStringList errors;
    bool valid = true;
    valid = this->validate(errors);

    // show the message box
    if (!valid) {
        // Show the warning dialog 200px down and to the right of this widget
        QMessageBox msgBox(QMessageBox::Warning,
                   tr("Validation"),
                   errors.join("\n"),
                   QMessageBox::Ok,
                   this);

        // Center the message box over the parent widget
        msgBox.ensurePolished();
        const QPoint parentCenterGlobal = this->mapToGlobal(this->rect().center());
        const QSize dialogSize = msgBox.sizeHint();
        msgBox.move(parentCenterGlobal - QPoint(dialogSize.width() / 2, dialogSize.height() / 2));

        msgBox.exec();
        
        return;
    }

    // Show the information dialog centered over this widget
    QMessageBox msgBox(QMessageBox::Information,
               tr("Validation"),
               tr("Validation passed."),
               QMessageBox::Ok,
               this);
    msgBox.ensurePolished();
    const QPoint parentCenterGlobal = this->mapToGlobal(this->rect().center());
    const QSize dialogSize = msgBox.sizeHint();
    msgBox.move(parentCenterGlobal - QPoint(dialogSize.width() / 2, dialogSize.height() / 2));
    msgBox.exec();
}

void SSI_Simulation::onPlotClicked() {
    // Build a minimal Modeling JSON from current widgets and send to plotter
    QJsonObject modeling;
    modeling["type"] = "SSISimulation";
    if (currentBuilding) {
        modeling["building_type"] = currentBuilding->typeId();
        QJsonObject structureInfo;
        if (currentBuilding->outputToJSON(structureInfo))
            modeling["structure_info"] = structureInfo;
    }
    if (currentSoil) {
        modeling["soil_type"] = currentSoil->typeId();
        QJsonObject sfi;
        if (currentSoil->outputToJSON(sfi))
            modeling["soil_foundation_info"] = sfi;
    }
}


int SSI_Simulation::getNumberOfCores() const {
    int numCores = 0;
    if (currentBuilding) {
        numCores += currentBuilding->getNumberOfCores();
    }
    if (currentSoil) {
        numCores += currentSoil->getNumberOfCores();
    }
    return numCores;
}