/* *****************************************************************************
Copyright (c) 2016-2025, The Regents of the University of California (Regents).
All rights reserved.
*************************************************************************** */

#include "SSI_Custom3DBuildingWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QJsonObject>
#include <QJsonArray>
#include <QAbstractItemView>
#include <QSet>

SSI_Custom3DBuildingWidget::SSI_Custom3DBuildingWidget(QWidget* parent) : SSI_BuildingWidgetBase(parent) {
    auto topLayout = new QVBoxLayout();

    auto filesCoresBox = new QGroupBox("Files & Cores");
    setupFilesAndCoresGroup(filesCoresBox);
    topLayout->addWidget(filesCoresBox);

    auto columnsBox = new QGroupBox("Columns base");
    setupColumnsBaseTable(columnsBox);
    topLayout->addWidget(columnsBox);

    auto boundsBox = new QGroupBox("Bounds");
    setupBoundsGroup(boundsBox);
    topLayout->addWidget(boundsBox);

    topLayout->addStretch(1);
    this->setLayout(topLayout);
}

void SSI_Custom3DBuildingWidget::setupFilesAndCoresGroup(QWidget* parentWidget) {
    auto grid = new QGridLayout(parentWidget);
    int row = 0;

    grid->addWidget(new QLabel("Model file"), row, 0);
    modelFileLineEdit = new QLineEdit();
    modelFileLineEdit->setPlaceholderText("/path/to/model.tcl");
    auto chooseModel = new QPushButton("Choose");
    grid->addWidget(modelFileLineEdit, row, 1);
    grid->addWidget(chooseModel, row, 2);
    row++;

    grid->addWidget(new QLabel("Mesh file (optional)"), row, 0);
    meshFileLineEdit = new QLineEdit();
    meshFileLineEdit->setPlaceholderText("(Optional)");
    auto chooseMesh = new QPushButton("Choose");
    grid->addWidget(meshFileLineEdit, row, 1);
    grid->addWidget(chooseMesh, row, 2);
    row++;

    grid->addWidget(new QLabel("Num partitions"), row, 0);
    numPartitionsSpin = new QSpinBox();
    numPartitionsSpin->setMinimum(1);
    numPartitionsSpin->setMaximum(1000000);
    numPartitionsSpin->setValue(1);
    grid->addWidget(numPartitionsSpin, row, 1);
    row++;

    grid->setColumnStretch(1, 1);

    connect(chooseModel, &QPushButton::clicked, this, [this]() {
        auto fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("All files (*.*)"));
        if (!fileName.isEmpty())
            modelFileLineEdit->setText(fileName);
    });
    connect(chooseMesh, &QPushButton::clicked, this, [this]() {
        auto fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("All files (*.*)"));
        if (!fileName.isEmpty())
            meshFileLineEdit->setText(fileName);
    });
}

void SSI_Custom3DBuildingWidget::setupColumnsBaseTable(QWidget* parentWidget) {
    auto vbox = new QVBoxLayout(parentWidget);
    columnsTable = new QTableWidget();
    columnsTable->setColumnCount(4);
    QStringList headers; headers << "tag" << "x" << "y" << "z";
    columnsTable->setHorizontalHeaderLabels(headers);
    columnsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    columnsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    columnsTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
    vbox->addWidget(columnsTable);

    auto hbox = new QHBoxLayout();
    auto addRowBtn = new QPushButton("Add row");
    auto removeRowBtn = new QPushButton("Remove");
    auto clearBtn = new QPushButton("Clear");
    hbox->addWidget(addRowBtn);
    hbox->addWidget(removeRowBtn);
    hbox->addWidget(clearBtn);
    hbox->addStretch(1);
    vbox->addLayout(hbox);

    connect(addRowBtn, &QPushButton::clicked, this, [this]() {
        int row = columnsTable->rowCount();
        columnsTable->insertRow(row);
        for (int c = 0; c < 4; ++c) {
            columnsTable->setItem(row, c, new QTableWidgetItem());
        }
    });
    connect(removeRowBtn, &QPushButton::clicked, this, [this]() {
        auto ranges = columnsTable->selectedRanges();
        if (!ranges.isEmpty()) {
            int first = ranges.first().topRow();
            int last = ranges.first().bottomRow();
            for (int r = last; r >= first; --r)
                columnsTable->removeRow(r);
        } else if (columnsTable->rowCount() > 0) {
            columnsTable->removeRow(columnsTable->rowCount() - 1);
        }
    });
    connect(clearBtn, &QPushButton::clicked, this, [this]() {
        columnsTable->setRowCount(0);
    });
}

void SSI_Custom3DBuildingWidget::setupBoundsGroup(QWidget* parentWidget) {
    auto grid = new QGridLayout(parentWidget);
    int row = 0;

    grid->addWidget(new QLabel("x_min"), row, 0);
    xMinSpin = new QDoubleSpinBox(); xMinSpin->setDecimals(6); xMinSpin->setRange(-1e9, 1e9); xMinSpin->setValue(-13.716);
    grid->addWidget(xMinSpin, row, 1); row++;

    grid->addWidget(new QLabel("x_max"), row, 0);
    xMaxSpin = new QDoubleSpinBox(); xMaxSpin->setDecimals(6); xMaxSpin->setRange(-1e9, 1e9); xMaxSpin->setValue(13.716);
    grid->addWidget(xMaxSpin, row, 1); row++;

    grid->addWidget(new QLabel("y_min"), row, 0);
    yMinSpin = new QDoubleSpinBox(); yMinSpin->setDecimals(6); yMinSpin->setRange(-1e9, 1e9); yMinSpin->setValue(-13.716);
    grid->addWidget(yMinSpin, row, 1); row++;

    grid->addWidget(new QLabel("y_max"), row, 0);
    yMaxSpin = new QDoubleSpinBox(); yMaxSpin->setDecimals(6); yMaxSpin->setRange(-1e9, 1e9); yMaxSpin->setValue(13.716);
    grid->addWidget(yMaxSpin, row, 1); row++;

    grid->addWidget(new QLabel("z_min"), row, 0);
    zMinSpin = new QDoubleSpinBox(); zMinSpin->setDecimals(6); zMinSpin->setRange(-1e9, 1e9); zMinSpin->setValue(0.0);
    grid->addWidget(zMinSpin, row, 1); row++;

    grid->addWidget(new QLabel("z_max"), row, 0);
    zMaxSpin = new QDoubleSpinBox(); zMaxSpin->setDecimals(6); zMaxSpin->setRange(-1e9, 1e9); zMaxSpin->setValue(40.8432);
    grid->addWidget(zMaxSpin, row, 1); row++;

    grid->setColumnStretch(1, 1);
}

bool SSI_Custom3DBuildingWidget::getTableRow(int row, int& tag, double& x, double& y, double& z) const {
    auto itemTag = columnsTable->item(row, 0);
    auto itemX = columnsTable->item(row, 1);
    auto itemY = columnsTable->item(row, 2);
    auto itemZ = columnsTable->item(row, 3);
    if (!itemTag || !itemX || !itemY || !itemZ)
        return false;
    bool okTag = false, okX = false, okY = false, okZ = false;
    tag = itemTag->text().toInt(&okTag);
    x = itemX->text().toDouble(&okX);
    y = itemY->text().toDouble(&okY);
    z = itemZ->text().toDouble(&okZ);
    return okTag && okX && okY && okZ;
}

bool SSI_Custom3DBuildingWidget::validate(QStringList& errors, bool interactiveIfModelMissing) const {
    errors.clear();

    if (numPartitionsSpin->value() < 1)
        errors << "num_partitions must be >= 1";

    if (!(xMinSpin->value() < xMaxSpin->value())) errors << "x_min must be < x_max";
    if (!(yMinSpin->value() < yMaxSpin->value())) errors << "y_min must be < y_max";
    if (!(zMinSpin->value() < zMaxSpin->value())) errors << "z_min must be < z_max";

    const QString modelPath = modelFileLineEdit->text();
    if (modelPath.isEmpty()) {
        errors << "model_file is required";
    } else if (!QFileInfo::exists(modelPath)) {
        if (!interactiveIfModelMissing)
            errors << QString("model_file does not exist: %1").arg(modelPath);
    }

    QSet<int> seenTags;
    const double xmin = xMinSpin->value(), xmax = xMaxSpin->value();
    const double ymin = yMinSpin->value(), ymax = yMaxSpin->value();
    const double zmin = zMinSpin->value(), zmax = zMaxSpin->value();

    if (columnsTable->rowCount() == 0) {
        errors << "columns table is empty";
    }


    for (int r = 0; r < columnsTable->rowCount(); ++r) {
        int tag; double x, y, z;
        if (!getTableRow(r, tag, x, y, z)) {
            errors << QString("Row %1 has invalid data").arg(r + 1);
            continue;
        }
        if (seenTags.contains(tag))
            errors << QString("Row %1 tag %2 is duplicated").arg(r + 1).arg(tag);
        seenTags.insert(tag);
        if (!(x >= xmin && x <= xmax && y >= ymin && y <= ymax && z >= zmin && z <= zmax))
            errors << QString("Row %1 point is outside bounds").arg(r + 1);
    }

    return errors.isEmpty();
}

bool SSI_Custom3DBuildingWidget::outputToJSON(QJsonObject& structureInfo) const {
    structureInfo["num_partitions"] = numPartitionsSpin->value();
    structureInfo["x_min"] = xMinSpin->value();
    structureInfo["y_min"] = yMinSpin->value();
    structureInfo["z_min"] = zMinSpin->value();
    structureInfo["x_max"] = xMaxSpin->value();
    structureInfo["y_max"] = yMaxSpin->value();
    structureInfo["z_max"] = zMaxSpin->value();

    const QString modelPath = modelFileLineEdit->text();
    structureInfo["model_file"] = modelPath;
    const QString meshPath = meshFileLineEdit->text();
    if (meshPath.isEmpty())
        structureInfo["mesh_file"] = QJsonValue();
    else
        structureInfo["mesh_file"] = meshPath;

    QJsonArray columnsArr;
    for (int r = 0; r < columnsTable->rowCount(); ++r) {
        int tag; double x, y, z;
        if (!getTableRow(r, tag, x, y, z))
            continue;
        QJsonObject col;
        col["tag"] = tag;
        col["x"] = x;
        col["y"] = y;
        col["z"] = z;
        columnsArr.append(col);
    }
    structureInfo["columns_base"] = columnsArr;
    return true;
}

bool SSI_Custom3DBuildingWidget::inputFromJSON(const QJsonObject& structureInfo) {
    // No validation here; simply populate fields if keys exist
    if (structureInfo.contains("num_partitions"))
        numPartitionsSpin->setValue(structureInfo.value("num_partitions").toInt(1));

    xMinSpin->setValue(structureInfo.value("x_min").toDouble(xMinSpin->value()));
    yMinSpin->setValue(structureInfo.value("y_min").toDouble(yMinSpin->value()));
    zMinSpin->setValue(structureInfo.value("z_min").toDouble(zMinSpin->value()));
    xMaxSpin->setValue(structureInfo.value("x_max").toDouble(xMaxSpin->value()));
    yMaxSpin->setValue(structureInfo.value("y_max").toDouble(yMaxSpin->value()));
    zMaxSpin->setValue(structureInfo.value("z_max").toDouble(zMaxSpin->value()));

    modelFileLineEdit->setText(structureInfo.value("model_file").toString());
    auto meshVal = structureInfo.value("mesh_file");
    if (meshVal.isUndefined() || meshVal.isNull())
        meshFileLineEdit->clear();
    else
        meshFileLineEdit->setText(meshVal.toString());

    // columns_base
    columnsTable->setRowCount(0);
    auto cols = structureInfo.value("columns_base").toArray();
    for (const auto& v : cols) {
        auto obj = v.toObject();
        int row = columnsTable->rowCount();
        columnsTable->insertRow(row);
        columnsTable->setItem(row, 0, new QTableWidgetItem(QString::number(obj.value("tag").toInt())));
        columnsTable->setItem(row, 1, new QTableWidgetItem(QString::number(obj.value("x").toDouble())));
        columnsTable->setItem(row, 2, new QTableWidgetItem(QString::number(obj.value("y").toDouble())));
        columnsTable->setItem(row, 3, new QTableWidgetItem(QString::number(obj.value("z").toDouble())));
    }
    return true;
}

void SSI_Custom3DBuildingWidget::plot() const {
    QMessageBox::information(nullptr, QString("Plot"), QString("Plotting not implemented yet."));
}