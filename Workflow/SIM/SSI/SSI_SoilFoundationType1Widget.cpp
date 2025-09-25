/* *****************************************************************************
Copyright (c) 2016-2025, The Regents of the University of California (Regents).
All rights reserved.
*************************************************************************** */

#include "SSI_SoilFoundationType1Widget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QComboBox>
#include <QCheckBox>
#include <QTextBrowser>
#include <QTabWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>
#include <RandomVariablesContainer.h>

SSI_SoilFoundationType1Widget::SSI_SoilFoundationType1Widget(QWidget* parent)
    : SSI_SoilFoundationBaseWidget(parent)
{
    auto topLayout = new QVBoxLayout();

    auto tabs = new QTabWidget();

    auto soilTab = new QWidget();
    setupSoilGroup(soilTab);
    tabs->addTab(soilTab, "Soil");

    auto foundationTab = new QWidget();
    setupFoundationGroup(foundationTab);
    tabs->addTab(foundationTab, "Foundation");

    auto pilesTab = new QWidget();
    setupPilesGroup(pilesTab);
    tabs->addTab(pilesTab, "Piles");

    topLayout->addWidget(tabs);
    this->setLayout(topLayout);
}

void SSI_SoilFoundationType1Widget::setupSoilGroup(QWidget* parentWidget) {
    auto hbox = new QHBoxLayout(parentWidget);
    auto leftWidget = new QWidget();
    auto vbox = new QVBoxLayout(leftWidget);

    auto grid = new QGridLayout();
    int row = 0;

    // Row 0: x_min | x_max
    grid->addWidget(new QLabel("x_min"), row, 0); xMinSoil = new QDoubleSpinBox(); xMinSoil->setRange(-1e9, 1e9); xMinSoil->setDecimals(6); xMinSoil->setValue(-64.0); grid->addWidget(xMinSoil, row, 1);
    grid->addWidget(new QLabel("x_max"), row, 2); xMaxSoil = new QDoubleSpinBox(); xMaxSoil->setRange(-1e9, 1e9); xMaxSoil->setDecimals(6); xMaxSoil->setValue(64.0); grid->addWidget(xMaxSoil, row, 3); row++;

    // Row 1: y_min | y_max
    grid->addWidget(new QLabel("y_min"), row, 0); yMinSoil = new QDoubleSpinBox(); yMinSoil->setRange(-1e9, 1e9); yMinSoil->setDecimals(6); yMinSoil->setValue(-64.0); grid->addWidget(yMinSoil, row, 1);
    grid->addWidget(new QLabel("y_max"), row, 2); yMaxSoil = new QDoubleSpinBox(); yMaxSoil->setRange(-1e9, 1e9); yMaxSoil->setDecimals(6); yMaxSoil->setValue(64.0); grid->addWidget(yMaxSoil, row, 3); row++;

    // Row 2: nx | ny
    grid->addWidget(new QLabel("nx"), row, 0); nxSoil = new QSpinBox(); nxSoil->setRange(1, 1000000); nxSoil->setValue(32); grid->addWidget(nxSoil, row, 1);
    grid->addWidget(new QLabel("ny"), row, 2); nySoil = new QSpinBox(); nySoil->setRange(1, 1000000); nySoil->setValue(32); grid->addWidget(nySoil, row, 3); row++;

    // Row 3: gravity_z | num_partitions
    grid->addWidget(new QLabel("gravity_z"), row, 0); gravZSoil = new QDoubleSpinBox(); gravZSoil->setRange(-1e6, 1e6); gravZSoil->setDecimals(6); gravZSoil->setValue(-9.81); grid->addWidget(gravZSoil, row, 1);
    grid->addWidget(new QLabel("num_partitions"), row, 2); numPartsSoil = new QSpinBox(); numPartsSoil->setRange(1, 1000000); numPartsSoil->setValue(4); grid->addWidget(numPartsSoil, row, 3); row++;

    // Row 5: boundary_conditions (label + combo on the left)
    grid->addWidget(new QLabel("boundary_conditions"), row, 0); boundaryCombo = new QComboBox(); boundaryCombo->addItem("periodic"); boundaryCombo->addItem("DRM"); grid->addWidget(boundaryCombo, row, 1); row++;

    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(3, 1);
    vbox->addLayout(grid);

    drmOptionsGroup = new QGroupBox("DRM options");
    auto drmGrid = new QGridLayout(drmOptionsGroup);
    int drow = 0;
    drmGrid->addWidget(new QLabel("absorbing_layer_type"), drow, 0); absorbingLayerCombo = new QComboBox(); absorbingLayerCombo->addItem("Rayleigh"); absorbingLayerCombo->addItem("PML"); drmGrid->addWidget(absorbingLayerCombo, drow, 1); drow++;
    drmGrid->addWidget(new QLabel("num_partitions"), drow, 0); drmNumPartitions = new QSpinBox(); drmNumPartitions->setRange(1, 1000000); drmNumPartitions->setValue(2); drmGrid->addWidget(drmNumPartitions, drow, 1); drow++;
    drmGrid->addWidget(new QLabel("number_of_layers"), drow, 0); drmNumLayers = new QSpinBox(); drmNumLayers->setRange(1, 1000000); drmNumLayers->setValue(4); drmGrid->addWidget(drmNumLayers, drow, 1); drow++;
    drmGrid->addWidget(new QLabel("Rayleigh_damping"), drow, 0); drmRayleighDamping = new QDoubleSpinBox(); drmRayleighDamping->setRange(0.0, 1.0); drmRayleighDamping->setDecimals(6); drmRayleighDamping->setValue(0.95); drmGrid->addWidget(drmRayleighDamping, drow, 1); drow++;
    drmGrid->addWidget(new QLabel("match_damping"), drow, 0); drmMatchDamping = new QCheckBox(); drmMatchDamping->setChecked(false); drmGrid->addWidget(drmMatchDamping, drow, 1); drow++;
    drmOptionsGroup->setVisible(false);
    vbox->addWidget(drmOptionsGroup);

    connect(boundaryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx){
        Q_UNUSED(idx);
        drmOptionsGroup->setVisible(boundaryCombo->currentText() == "DRM");
    });

    // Soil profile table
    soilProfileTable = new QTableWidget();
    soilProfileTable->setColumnCount(8);
    QStringList headers; headers << "z_bot" << "z_top" << "nz" << "material" << "mat_props" << "damping" << "damping_props" << "";
    soilProfileTable->setHorizontalHeaderLabels(headers);
    soilProfileTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    soilProfileTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    soilProfileTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
    vbox->addWidget(soilProfileTable);

    auto soilBtns = new QHBoxLayout();
    auto addRowBtn = new QPushButton("Add row");
    auto removeRowBtn = new QPushButton("Remove");
    auto clearBtn = new QPushButton("Clear");
    soilBtns->addWidget(addRowBtn);
    soilBtns->addWidget(removeRowBtn);
    soilBtns->addWidget(clearBtn);
    soilBtns->addStretch(1);
    vbox->addLayout(soilBtns);

    // Help panel on the right (30% width) with rich, colorized content
    auto help = new QTextBrowser();
    help->setStyleSheet("QTextBrowser { background: #f8fbff; border: 1px solid #d0e7ff; border-radius: 6px; padding: 8px; }");
    help->setHtml(
        "<div style='font-size:13px; line-height:1.35;'>"
        "<h3 style='margin:0 0 8px; color:#1b5e20;'>Soil parameters</h3>"
        "<ul style='margin:0 0 10px 18px;'>"
        "<li><b>x_min, x_max, y_min, y_max</b>: <span style='color:#1565c0;'>Plan bounds (m)</span></li>"
        "<li><b>nx, ny</b>: <span style='color:#1565c0;'>Elements along x and y</span> (≥1)</li>"
        "<li><b>gravity_z</b>: Typically <span style='color:#2e7d32;'>-9.81 m/s²</span></li>"
        "<li><b>num_partitions</b>: MPI partitions for soil mesh</li>"
        "<li><b>boundary_conditions</b>: <span style='color:#6a1b9a;'>periodic</span> or <span style='color:#6a1b9a;'>DRM</span></li>"
        "</ul>"
        "<h4 style='margin:10px 0 6px; color:#6a1b9a;'>DRM options</h4>"
        "<ul style='margin:0 0 10px 18px;'>"
        "<li><b>absorbing_layer_type</b>: <span style='color:#00838f;'>Rayleigh</span> or <span style='color:#00838f;'>PML</span></li>"
        "<li><b>num_partitions</b>: Partitions in absorbing layer</li>"
        "<li><b>number_of_layers</b>: Count of absorbing layers</li>"
        "<li><b>Rayleigh_damping</b>: Additional damping ratio</li>"
        "<li><b>match_damping</b>: Match soil-layer damping when checked</li>"
        "</ul>"
        "<h4 style='margin:10px 0 6px; color:#00695c;'>Soil profile table</h4>"
        "<ul style='margin:0 0 0 18px;'>"
        "<li><b>z_bot, z_top, nz</b>: <span style='color:#ef6c00;'>z_top &gt; z_bot</span>, vertical subdivisions</li>"
        "<li><b>material</b>: <span style='color:#2e7d32;'>Elastic</span> (3 <i>mat_props</i>: E, ν, ρ)</li>"
        "<li><b>damping</b>: <span style='color:#8e24aa;'>Frequency‑Rayleigh</span> (3 <i>damping_props</i>: ζ, f_low, f_high)</li>"
        "</ul>"
        "</div>"
    );

    hbox->addWidget(leftWidget, 7);
    hbox->addWidget(help, 3);

    connect(addRowBtn, &QPushButton::clicked, this, [this]() {
        int row = soilProfileTable->rowCount();
        soilProfileTable->insertRow(row);
        // z_bot/z_top/nz
        for (int c = 0; c < 3; ++c) soilProfileTable->setItem(row, c, new QTableWidgetItem());
        // material combo
        auto mat = new QComboBox(); mat->addItem("Elastic"); soilProfileTable->setCellWidget(row, 3, mat);
        // mat_props
        soilProfileTable->setItem(row, 4, new QTableWidgetItem());
        // damping combo
        auto damp = new QComboBox(); damp->addItem("Frequency-Rayleigh"); soilProfileTable->setCellWidget(row, 5, damp);
        // damping_props
        soilProfileTable->setItem(row, 6, new QTableWidgetItem());
    });
    connect(removeRowBtn, &QPushButton::clicked, this, [this]() {
        auto ranges = soilProfileTable->selectedRanges();
        if (!ranges.isEmpty()) {
            int first = ranges.first().topRow();
            int last = ranges.first().bottomRow();
            for (int r = last; r >= first; --r) soilProfileTable->removeRow(r);
        } else if (soilProfileTable->rowCount() > 0) {
            soilProfileTable->removeRow(soilProfileTable->rowCount() - 1);
        }
    });
    connect(clearBtn, &QPushButton::clicked, this, [this]() { soilProfileTable->setRowCount(0); });

    // auto-register RVs when user edits mat_props
    connect(soilProfileTable, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item){
        if (!item) return;
        if (item->column() == 4) {
            registerRVsFromCsv(item->text());
        }
    });
}

void SSI_SoilFoundationType1Widget::setupFoundationGroup(QWidget* parentWidget) {
    auto hbox = new QHBoxLayout(parentWidget);
    auto leftWidget = new QWidget();
    auto vbox = new QVBoxLayout(leftWidget);

    // Top compact grid: three columns layout
    auto grid = new QGridLayout();
    int row = 0;
    // Row 0: dx | dy | dz
    grid->addWidget(new QLabel("dx"), row, 0); dxFound = new QDoubleSpinBox(); dxFound->setRange(1e-9, 1e9); dxFound->setDecimals(6); dxFound->setValue(2.0); grid->addWidget(dxFound, row, 1);
    grid->addWidget(new QLabel("dy"), row, 2); dyFound = new QDoubleSpinBox(); dyFound->setRange(1e-9, 1e9); dyFound->setDecimals(6); dyFound->setValue(2.0); grid->addWidget(dyFound, row, 3);
    grid->addWidget(new QLabel("dz"), row, 4); dzFound = new QDoubleSpinBox(); dzFound->setRange(1e-9, 1e9); dzFound->setDecimals(6); dzFound->setValue(0.3); grid->addWidget(dzFound, row, 5); row++;
    // Row 1: gravity_z | num_partitions | embedded
    grid->addWidget(new QLabel("gravity_z"), row, 0); gravZFound = new QDoubleSpinBox(); gravZFound->setRange(-1e6, 1e6); gravZFound->setDecimals(6); gravZFound->setValue(-9.81); grid->addWidget(gravZFound, row, 1);
    grid->addWidget(new QLabel("num_partitions"), row, 2); numPartsFound = new QSpinBox(); numPartsFound->setRange(1, 1000000); numPartsFound->setValue(1); grid->addWidget(numPartsFound, row, 3);
    grid->addWidget(new QLabel("embedded"), row, 4); embeddedFoundation = new QCheckBox(); embeddedFoundation->setChecked(true); grid->addWidget(embeddedFoundation, row, 5); row++;

    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(3, 1);
    grid->setColumnStretch(5, 1);
    vbox->addLayout(grid);

    // Building–foundation connection box for embedment depth and section props
    auto connBox = new QGroupBox("Building foundation connection");
    auto connVBox = new QVBoxLayout(connBox);
    // Toggle for advanced section properties
    auto toggleLayout = new QHBoxLayout();
    toggleLayout->addWidget(new QLabel("Advanced section properties:"));
    showSectionPropsCheck = new QCheckBox("Show");
    showSectionPropsCheck->setChecked(false);
    toggleLayout->addWidget(showSectionPropsCheck);
    toggleLayout->addStretch(1);
    connVBox->addLayout(toggleLayout);

    // Embedment depth always visible
    auto embedLayout = new QHBoxLayout();
    embedLayout->addWidget(new QLabel("column_embedment_depth"));
    columnEmbedDepth = new QDoubleSpinBox(); columnEmbedDepth->setRange(0.0, 1e9); columnEmbedDepth->setDecimals(6); columnEmbedDepth->setValue(0.6);
    embedLayout->addWidget(columnEmbedDepth);
    embedLayout->addStretch(1);
    connVBox->addLayout(embedLayout);

    // Container for advanced props; hidden by default
    sectionPropsContainer = new QWidget();
    auto connGrid = new QGridLayout(sectionPropsContainer);
    int crow = 0;
    // Section properties in three columns: E A G
    connGrid->addWidget(new QLabel("Section E"), crow, 0); secE = new QDoubleSpinBox(); secE->setRange(0.0, 1e12); secE->setDecimals(6); secE->setValue(30e6); connGrid->addWidget(secE, crow, 1);
    connGrid->addWidget(new QLabel("Section A"), crow, 2); secA = new QDoubleSpinBox(); secA->setRange(0.0, 1e9); secA->setDecimals(6); secA->setValue(0.282); connGrid->addWidget(secA, crow, 3);
    connGrid->addWidget(new QLabel("Section G"), crow, 4); secG = new QDoubleSpinBox(); secG->setRange(0.0, 1e12); secG->setDecimals(6); secG->setValue(12.5e6); connGrid->addWidget(secG, crow, 5); crow++;
    // Next row: Iy Iz J
    connGrid->addWidget(new QLabel("Section Iy"), crow, 0); secIy = new QDoubleSpinBox(); secIy->setRange(0.0, 1e9); secIy->setDecimals(7); secIy->setValue(0.0063585); connGrid->addWidget(secIy, crow, 1);
    connGrid->addWidget(new QLabel("Section Iz"), crow, 2); secIz = new QDoubleSpinBox(); secIz->setRange(0.0, 1e9); secIz->setDecimals(7); secIz->setValue(0.0063585); connGrid->addWidget(secIz, crow, 3);
    connGrid->addWidget(new QLabel("Section J"), crow, 4); secJ = new QDoubleSpinBox(); secJ->setRange(0.0, 1e9); secJ->setDecimals(6); secJ->setValue(0.012717); connGrid->addWidget(secJ, crow, 5); crow++;
    connGrid->setColumnStretch(1, 1);
    connGrid->setColumnStretch(3, 1);
    connGrid->setColumnStretch(5, 1);
    sectionPropsContainer->setVisible(false);
    connVBox->addWidget(sectionPropsContainer);
    vbox->addWidget(connBox);

    connect(showSectionPropsCheck, &QCheckBox::toggled, this, [this](bool checked){
        sectionPropsContainer->setVisible(checked);
    });

    // Foundation profile table
    foundationProfileTable = new QTableWidget();
    foundationProfileTable->setColumnCount(10);
    QStringList headers; headers << "x_min" << "x_max" << "y_min" << "y_max" << "z_top" << "z_bot" << "material" << "mat_props" << "damping" << "damping_props";
    foundationProfileTable->setHorizontalHeaderLabels(headers);
    foundationProfileTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    foundationProfileTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    foundationProfileTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
    vbox->addWidget(foundationProfileTable);

    auto fBtns = new QHBoxLayout();
    auto addRowBtn = new QPushButton("Add row");
    auto removeRowBtn = new QPushButton("Remove");
    auto clearBtn = new QPushButton("Clear");
    fBtns->addWidget(addRowBtn);
    fBtns->addWidget(removeRowBtn);
    fBtns->addWidget(clearBtn);
    fBtns->addStretch(1);
    vbox->addLayout(fBtns);

    // Help panel on the right (30% width) with rich, colorized content
    auto help = new QTextBrowser();
    help->setStyleSheet("QTextBrowser { background: #fff8e1; border: 1px solid #ffe0b2; border-radius: 6px; padding: 8px; }");
    help->setHtml(
        "<div style='font-size:13px; line-height:1.35;'>"
        "<h3 style='margin:0 0 8px; color:#0d47a1;'>Foundation parameters</h3>"
        "<ul style='margin:0 0 10px 18px;'>"
        "<li><b>dx, dy, dz</b>: Mesh sizes of the foundation block (m)</li>"
        "<li><b>gravity_z</b>: Typically <span style='color:#2e7d32;'>-9.81 m/s²</span></li>"
        "<li><b>num_partitions</b>: MPI partitions for foundation</li>"
        "<li><b>embedded</b>: If checked, foundation is embedded in soil</li>"
        "<li><b>column_embedment_depth</b>: Embedment depth of building columns (m)</li>"
        "</ul>"
        "<h4 style='margin:10px 0 6px; color:#bf360c;'>Advanced section properties</h4>"
        "<p style='margin:0 0 8px;'>Set <b>E, A, G, Iy, Iz, J</b> for column sections.</p>"
        "<h4 style='margin:10px 0 6px; color:#004d40;'>Foundation profile table</h4>"
        "<ul style='margin:0 0 0 18px;'>"
        "<li><b>x/y bounds, z_top, z_bot</b>: Define foundation extent</li>"
        "<li><b>material</b>: <span style='color:#2e7d32;'>Elastic</span> (3 <i>mat_props</i>: E, ν, ρ)</li>"
        "<li><b>damping</b>: <span style='color:#8e24aa;'>Frequency‑Rayleigh</span> (3 <i>damping_props</i>: ζ, f_low, f_high)</li>"
        "</ul>"
        "</div>"
    );

    hbox->addWidget(leftWidget, 7);
    hbox->addWidget(help, 3);

    connect(addRowBtn, &QPushButton::clicked, this, [this]() {
        int row = foundationProfileTable->rowCount();
        foundationProfileTable->insertRow(row);
        for (int c = 0; c < 6; ++c) foundationProfileTable->setItem(row, c, new QTableWidgetItem());
        auto mat = new QComboBox(); mat->addItem("Elastic"); foundationProfileTable->setCellWidget(row, 6, mat);
        foundationProfileTable->setItem(row, 7, new QTableWidgetItem());
        auto damp = new QComboBox(); damp->addItem("Frequency-Rayleigh"); foundationProfileTable->setCellWidget(row, 8, damp);
        foundationProfileTable->setItem(row, 9, new QTableWidgetItem());
    });
    connect(removeRowBtn, &QPushButton::clicked, this, [this]() {
        auto ranges = foundationProfileTable->selectedRanges();
        if (!ranges.isEmpty()) {
            int first = ranges.first().topRow();
            int last = ranges.first().bottomRow();
            for (int r = last; r >= first; --r) foundationProfileTable->removeRow(r);
        } else if (foundationProfileTable->rowCount() > 0) {
            foundationProfileTable->removeRow(foundationProfileTable->rowCount() - 1);
        }
    });
    connect(clearBtn, &QPushButton::clicked, this, [this]() { foundationProfileTable->setRowCount(0); });

    // auto-register RVs when user edits mat_props
    connect(foundationProfileTable, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item){
        if (!item) return;
        if (item->column() == 7) {
            registerRVsFromCsv(item->text());
        }
    });
}

void SSI_SoilFoundationType1Widget::setupPilesGroup(QWidget* parentWidget) {
    auto hbox = new QHBoxLayout(parentWidget);
    auto leftWidget = new QWidget();
    auto vbox = new QVBoxLayout(leftWidget);

    pileProfileTable = new QTableWidget();
    pileProfileTable->setColumnCount(11);
    QStringList headers; headers << "type" << "params" << "z_top" << "z_bot" << "nz" << "r" << "section" << "material" << "mat_props" << "transformation" << "transformation_vector";
    pileProfileTable->setHorizontalHeaderLabels(headers);
    pileProfileTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    pileProfileTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    pileProfileTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
    vbox->addWidget(pileProfileTable);

    auto pGrid = new QGridLayout();
    int row = 0;
    pGrid->addWidget(new QLabel("num_points_on_perimeter"), row, 0); pileNumPointsPerimeter = new QSpinBox(); pileNumPointsPerimeter->setRange(1, 10000); pileNumPointsPerimeter->setValue(8); pGrid->addWidget(pileNumPointsPerimeter, row, 1); row++;
    pGrid->addWidget(new QLabel("num_points_along_length"), row, 0); pileNumPointsLength = new QSpinBox(); pileNumPointsLength->setRange(1, 10000); pileNumPointsLength->setValue(4); pGrid->addWidget(pileNumPointsLength, row, 1); row++;
    pGrid->addWidget(new QLabel("penalty_parameter"), row, 0); pilePenaltyParameter = new QDoubleSpinBox(); pilePenaltyParameter->setRange(0.0, 1e20); pilePenaltyParameter->setDecimals(6); pilePenaltyParameter->setValue(1.0e12); pGrid->addWidget(pilePenaltyParameter, row, 1); row++;
    pGrid->setColumnStretch(1, 1);
    vbox->addLayout(pGrid);

    auto pBtns = new QHBoxLayout();
    auto addRowBtn = new QPushButton("Add row");
    auto removeRowBtn = new QPushButton("Remove");
    auto clearBtn = new QPushButton("Clear");
    pBtns->addWidget(addRowBtn);
    pBtns->addWidget(removeRowBtn);
    pBtns->addWidget(clearBtn);
    pBtns->addStretch(1);
    vbox->addLayout(pBtns);

    // Help panel on the right (30% width) with rich, colorized content
    auto help = new QTextBrowser();
    help->setStyleSheet("QTextBrowser { background: #f3e5f5; border: 1px solid #e1bee7; border-radius: 6px; padding: 8px; }");
    help->setHtml(
        "<div style='font-size:13px; line-height:1.35;'>"
        "<h3 style='margin:0 0 8px; color:#4a148c;'>Pile parameters</h3>"
        "<ul style='margin:0 0 10px 18px;'>"
        "<li><b>type</b>: <span style='color:#ef6c00;'>Grid</span> or <span style='color:#ef6c00;'>Single</span></li>"
        "<li style='margin-top:4px;'><b>Grid params</b> (csv): x_start, y_start, spacing_x, spacing_y, nx, ny</li>"
        "<li><b>Single params</b> (csv): x_top, x_bot, y_top, y_bot</li>"
        "<li><b>z_top, z_bot, nz, r</b>: Vertical geometry and pile radius</li>"
        "<li><b>section</b>: Only <i>No‑Section</i> currently supported</li>"
        "<li><b>material</b>: <span style='color:#2e7d32;'>Elastic</span> with 6 <i>mat_props</i>: E, A, Iy, Iz, G, J</li>"
        "<li><b>transformation</b>: <span style='color:#1565c0;'>Linear</span> or <span style='color:#1565c0;'>PDelta</span></li>"
        "<li><b>transformation_vector</b>: csv of <i>ux, uy, uz</i> (e.g., 0.0, 1.0, 0.0)</li>"
        "</ul>"
        "<h4 style='margin:10px 0 6px; color:#1b5e20;'>Interface discretization</h4>"
        "<ul style='margin:0 0 0 18px;'>"
        "<li><b>num_points_on_perimeter</b>, <b>num_points_along_length</b>, <b>penalty_parameter</b></li>"
        "</ul>"
        "</div>"
    );

    hbox->addWidget(leftWidget, 7);
    hbox->addWidget(help, 3);

    connect(addRowBtn, &QPushButton::clicked, this, [this]() {
        int row = pileProfileTable->rowCount();
        pileProfileTable->insertRow(row);
        // type combo
        auto type = new QComboBox(); type->addItem("Grid"); type->addItem("Single"); pileProfileTable->setCellWidget(row, 0, type);
        // params (csv depending on type)
        pileProfileTable->setItem(row, 1, new QTableWidgetItem());
        // z_top, z_bot, nz, r
        for (int c = 2; c <= 5; ++c) pileProfileTable->setItem(row, c, new QTableWidgetItem());
        // section combo
        auto section = new QComboBox(); section->addItem("No-Section"); pileProfileTable->setCellWidget(row, 6, section);
        // material combo
        auto mat = new QComboBox(); mat->addItem("Elastic"); pileProfileTable->setCellWidget(row, 7, mat);
        // mat props
        pileProfileTable->setItem(row, 8, new QTableWidgetItem());
        // transformation combo + vector (csv)
        auto trans = new QComboBox(); trans->addItem("Linear"); trans->addItem("PDelta"); pileProfileTable->setCellWidget(row, 9, trans);
        pileProfileTable->setItem(row, 10, new QTableWidgetItem("0.0, 1.0, 0.0"));
    });
    connect(removeRowBtn, &QPushButton::clicked, this, [this]() {
        auto ranges = pileProfileTable->selectedRanges();
        if (!ranges.isEmpty()) {
            int first = ranges.first().topRow();
            int last = ranges.first().bottomRow();
            for (int r = last; r >= first; --r) pileProfileTable->removeRow(r);
        } else if (pileProfileTable->rowCount() > 0) {
            pileProfileTable->removeRow(pileProfileTable->rowCount() - 1);
        }
    });
    connect(clearBtn, &QPushButton::clicked, this, [this]() { pileProfileTable->setRowCount(0); });

    // auto-register RVs when user edits mat_props
    connect(pileProfileTable, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item){
        if (!item) return;
        if (item->column() == 8) {
            registerRVsFromCsv(item->text());
        }
    });
}

QList<double> SSI_SoilFoundationType1Widget::parseCsvDoubles(const QString& text, bool* ok) {
    QList<double> values;
    *ok = true;
    const auto parts = text.split(',', Qt::SkipEmptyParts);
    for (const auto& p : parts) {
        bool localOk = false;
        double v = p.trimmed().toDouble(&localOk);
        if (!localOk) { *ok = false; return {}; }
        values.append(v);
    }
    return values;
}

static bool isNumberToken(const QString &s) {
    bool ok=false; s.toDouble(&ok); return ok;
}

static bool isRvToken(const QString &s) {
    return s.startsWith("RV.") && s.size() > 3;
}

QStringList SSI_SoilFoundationType1Widget::parseCsvTokens(const QString& text, bool* ok) {
    QStringList tokens;
    *ok = true;
    const auto parts = text.split(',', Qt::SkipEmptyParts);
    for (const auto &raw : parts) {
        const QString p = raw.trimmed();
        if (p.isEmpty()) { *ok = false; return {}; }
        if (isNumberToken(p) || isRvToken(p)) {
            tokens.append(p);
        } else {
            // accept bare word as RV.name
            if (!p.contains(' ')) tokens.append(QString("RV.") + p);
            else { *ok = false; return {}; }
        }
    }
    return tokens;
}

void SSI_SoilFoundationType1Widget::registerRVsFromCsv(const QString& text) const {
    bool ok=true; const QStringList tokens = parseCsvTokens(text, &ok);
    if (!ok) return;
    RandomVariablesContainer *rvc = RandomVariablesContainer::getInstance();
    for (const QString &t : tokens) {
        if (isRvToken(t)) {
            QString name = t.mid(3);
            rvc->addRandomVariable(name);
        }
    }
}

bool SSI_SoilFoundationType1Widget::validate(QStringList& errors, bool interactiveIfModelMissing) const {
    Q_UNUSED(interactiveIfModelMissing);
    errors.clear();

    if (!(xMinSoil->value() < xMaxSoil->value())) errors << "soil: x_min must be < x_max";
    if (!(yMinSoil->value() < yMaxSoil->value())) errors << "soil: y_min must be < y_max";
    if (nxSoil->value() < 1) errors << "soil: nx must be >= 1";
    if (nySoil->value() < 1) errors << "soil: ny must be >= 1";

    // Soil profile rows check
    if (soilProfileTable->rowCount() == 0) errors << "soil_profile: table is empty";
    for (int r = 0; r < soilProfileTable->rowCount(); ++r) {
        // z_bot, z_top, nz
        bool ok1=false, ok2=false; int nz = soilProfileTable->item(r,2) ? soilProfileTable->item(r,2)->text().toInt(&ok1) : 0;
        double zbot = soilProfileTable->item(r,0) ? soilProfileTable->item(r,0)->text().toDouble(&ok2) : 0.0;
        bool ok3=false; double ztop = soilProfileTable->item(r,1) ? soilProfileTable->item(r,1)->text().toDouble(&ok3) : 0.0;
        if (!(ok1 && ok2 && ok3) || nz < 1 || !(ztop > zbot)) {
            errors << QString("soil_profile row %1: invalid z_bot/z_top/nz").arg(r+1);
        }
        // material
        auto mat = qobject_cast<QComboBox*>(soilProfileTable->cellWidget(r,3));
        if (!mat || mat->currentText() != "Elastic") errors << QString("soil_profile row %1: material must be Elastic").arg(r+1);
        // mat props: 3 tokens (numbers or RV names)
        bool okcsv=false; const QString mp = soilProfileTable->item(r,4)?soilProfileTable->item(r,4)->text():QString();
        auto mtoks = parseCsvTokens(mp, &okcsv);
        if (!okcsv || mtoks.size() != 3) errors << QString("soil_profile row %1: mat_props must have 3 values (numbers or RV names)").arg(r+1);
        // damping: Frequency-Rayleigh with 3 props
        auto damp = qobject_cast<QComboBox*>(soilProfileTable->cellWidget(r,5));
        if (!damp || damp->currentText() != "Frequency-Rayleigh") errors << QString("soil_profile row %1: damping must be Frequency-Rayleigh").arg(r+1);
        const QString dp = soilProfileTable->item(r,6)?soilProfileTable->item(r,6)->text():QString();
        auto dvals = parseCsvDoubles(dp, &okcsv);
        if (!okcsv || dvals.size() != 3) errors << QString("soil_profile row %1: damping_props must have 3 values").arg(r+1);
    }

    if (boundaryCombo->currentText() == "DRM") {
        if (drmNumPartitions->value() < 1) errors << "DRM: num_partitions must be >= 1";
        if (drmNumLayers->value() < 1) errors << "DRM: number_of_layers must be >= 1";
        if (absorbingLayerCombo->currentText() != "Rayleigh" && absorbingLayerCombo->currentText() != "PML")
            errors << "DRM: absorbing_layer_type must be Rayleigh or PML";
    }

    // Foundation profile
    if (foundationProfileTable->rowCount() == 0) errors << "foundation_profile: table is empty";
    for (int r = 0; r < foundationProfileTable->rowCount(); ++r) {
        bool ok = true; for (int c = 0; c < 6; ++c) { if (!foundationProfileTable->item(r,c) || foundationProfileTable->item(r,c)->text().isEmpty()) ok = false; }
        if (!ok) errors << QString("foundation_profile row %1: missing bounds or z values").arg(r+1);
        auto mat = qobject_cast<QComboBox*>(foundationProfileTable->cellWidget(r,6)); if (!mat || mat->currentText() != "Elastic") errors << QString("foundation_profile row %1: material must be Elastic").arg(r+1);
        bool okcsv=false; auto mtoksF = parseCsvTokens(foundationProfileTable->item(r,7)?foundationProfileTable->item(r,7)->text():QString(), &okcsv);
        if (!okcsv || mtoksF.size() != 3) errors << QString("foundation_profile row %1: mat_props must have 3 values (numbers or RV names)").arg(r+1);
        auto damp = qobject_cast<QComboBox*>(foundationProfileTable->cellWidget(r,8)); if (!damp || damp->currentText() != "Frequency-Rayleigh") errors << QString("foundation_profile row %1: damping must be Frequency-Rayleigh").arg(r+1);
        auto dvals = parseCsvDoubles(foundationProfileTable->item(r,9)?foundationProfileTable->item(r,9)->text():QString(), &okcsv);
        if (!okcsv || dvals.size() != 3) errors << QString("foundation_profile row %1: damping_props must have 3 values").arg(r+1);
    }

    // Column section props for columns embedded in foundation
    if (!(secE->value() > 0 && secA->value() > 0 && secIy->value() > 0 && secIz->value() > 0 && secG->value() > 0 && secJ->value() > 0))
        errors << "foundation: all column_section_props must be > 0";

    // Piles
    for (int r = 0; r < pileProfileTable->rowCount(); ++r) {
        auto type = qobject_cast<QComboBox*>(pileProfileTable->cellWidget(r,0));
        if (!type) { errors << QString("pile_profile row %1: missing type").arg(r+1); continue; }
        const QString t = type->currentText().toLower();
        bool okcsv=false; auto params = pileProfileTable->item(r,1)?pileProfileTable->item(r,1)->text():QString();
        auto pvals = parseCsvDoubles(params, &okcsv);
        if (t == "grid") {
            if (!okcsv || pvals.size() != 6) errors << QString("pile_profile row %1: grid params need 6 values (x_start,y_start,spacing_x,spacing_y,nx,ny)").arg(r+1);
        } else if (t == "single") {
            if (!okcsv || pvals.size() != 4) errors << QString("pile_profile row %1: single params need 4 values (x_top,x_bot,y_top,y_bot)").arg(r+1);
        } else {
            errors << QString("pile_profile row %1: type must be Grid or Single").arg(r+1);
        }
        // z_top, z_bot, nz, r
        bool okz=false, okzb=false, oknz=false, okr=false;
        double zt = pileProfileTable->item(r,2)?pileProfileTable->item(r,2)->text().toDouble(&okz):0.0;
        double zb = pileProfileTable->item(r,3)?pileProfileTable->item(r,3)->text().toDouble(&okzb):0.0;
        int nz = pileProfileTable->item(r,4)?pileProfileTable->item(r,4)->text().toInt(&oknz):0;
        double rr = pileProfileTable->item(r,5)?pileProfileTable->item(r,5)->text().toDouble(&okr):0.0;
        if (!(okz && okzb && oknz && okr) || !(zt > zb) || nz < 1 || rr <= 0.0) errors << QString("pile_profile row %1: invalid z_top/z_bot/nz/r").arg(r+1);
        // section
        auto section = qobject_cast<QComboBox*>(pileProfileTable->cellWidget(r,6)); if (!section || section->currentText() != "No-Section") errors << QString("pile_profile row %1: section must be No-Section").arg(r+1);
        // material elastic with 6 props
        auto mat = qobject_cast<QComboBox*>(pileProfileTable->cellWidget(r,7)); if (!mat || mat->currentText() != "Elastic") errors << QString("pile_profile row %1: material must be Elastic").arg(r+1);
        bool okmp=false; auto mtoksP = parseCsvTokens(pileProfileTable->item(r,8)?pileProfileTable->item(r,8)->text():QString(), &okmp);
        if (!okmp || mtoksP.size() != 6) errors << QString("pile_profile row %1: mat_props must have 6 values (numbers or RV names for E,A,Iy,Iz,G,J)").arg(r+1);
        // transformation combo + vector csv
        auto trans = qobject_cast<QComboBox*>(pileProfileTable->cellWidget(r,9)); if (!trans) errors << QString("pile_profile row %1: missing transformation").arg(r+1);
        okcsv = false; auto tvals = parseCsvDoubles(pileProfileTable->item(r,10)?pileProfileTable->item(r,10)->text():QString(), &okcsv);
        if (!okcsv || tvals.size() != 3) errors << QString("pile_profile row %1: transformation_vector must have 3 values (ux,uy,uz)").arg(r+1);
    }

    return errors.isEmpty();
}

bool SSI_SoilFoundationType1Widget::outputToJSON(QJsonObject& soilFoundationInfo) const {
    QJsonObject soilInfo;
    soilInfo["x_min"] = xMinSoil->value(); soilInfo["x_max"] = xMaxSoil->value();
    soilInfo["y_min"] = yMinSoil->value(); soilInfo["y_max"] = yMaxSoil->value();
    soilInfo["nx"] = nxSoil->value(); soilInfo["ny"] = nySoil->value();
    soilInfo["gravity_x"] = 0.0; soilInfo["gravity_y"] = 0.0; soilInfo["gravity_z"] = gravZSoil->value();
    soilInfo["num_partitions"] = numPartsSoil->value();
    soilInfo["boundary_conditions"] = boundaryCombo->currentText();
    if (boundaryCombo->currentText() == "DRM") {
        QJsonObject drm;
        drm["absorbing_layer_type"] = absorbingLayerCombo->currentText();
        drm["num_partitions"] = drmNumPartitions->value();
        drm["number_of_layers"] = drmNumLayers->value();
        drm["Rayleigh_damping"] = drmRayleighDamping->value();
        drm["match_damping"] = drmMatchDamping->isChecked();
        soilInfo["DRM_options"] = drm;
    }

    QJsonArray soilProfile;
    for (int r = 0; r < soilProfileTable->rowCount(); ++r) {
        QJsonObject layer;
        if (soilProfileTable->item(r,0)) layer["z_bot"] = soilProfileTable->item(r,0)->text().toDouble();
        if (soilProfileTable->item(r,1)) layer["z_top"] = soilProfileTable->item(r,1)->text().toDouble();
        if (soilProfileTable->item(r,2)) layer["nz"] = soilProfileTable->item(r,2)->text().toInt();
        auto mat = qobject_cast<QComboBox*>(soilProfileTable->cellWidget(r,3)); if (mat) layer["material"] = mat->currentText();
        bool ok=false; const QString mpTxt = soilProfileTable->item(r,4)?soilProfileTable->item(r,4)->text():QString();
        auto mtoks = parseCsvTokens(mpTxt, &ok);
        QJsonArray mp; for (const QString &t : mtoks) { if (t.startsWith("RV.")) mp.append(t); else { bool lk=false; double v=t.toDouble(&lk); mp.append(lk?QJsonValue(v):QJsonValue()); } } layer["mat_props"] = mp;
        auto damp = qobject_cast<QComboBox*>(soilProfileTable->cellWidget(r,5)); if (damp) layer["damping"] = damp->currentText();
        auto dvals = parseCsvDoubles(soilProfileTable->item(r,6)?soilProfileTable->item(r,6)->text():QString(), &ok); QJsonArray dp; for (double v : dvals) dp.append(v); layer["damping_props"] = dp;
        soilProfile.append(layer);
    }
    soilInfo["soil_profile"] = soilProfile;

    QJsonObject foundationInfo;
    foundationInfo["gravity_x"] = 0.0; foundationInfo["gravity_y"] = 0.0; foundationInfo["gravity_z"] = gravZFound->value();
    foundationInfo["embedded"] = embeddedFoundation->isChecked();
    foundationInfo["dx"] = dxFound->value(); foundationInfo["dy"] = dyFound->value(); foundationInfo["dz"] = dzFound->value();
    foundationInfo["num_partitions"] = numPartsFound->value();
    foundationInfo["column_embedment_depth"] = columnEmbedDepth->value();
    {
        QJsonObject sec; sec["E"] = secE->value(); sec["A"] = secA->value(); sec["Iy"] = secIy->value(); sec["Iz"] = secIz->value(); sec["G"] = secG->value(); sec["J"] = secJ->value();
        foundationInfo["column_section_props"] = sec;
    }
    QJsonArray foundationProfile;
    for (int r = 0; r < foundationProfileTable->rowCount(); ++r) {
        QJsonObject layer;
        if (foundationProfileTable->item(r,0)) layer["x_min"] = foundationProfileTable->item(r,0)->text().toDouble();
        if (foundationProfileTable->item(r,1)) layer["x_max"] = foundationProfileTable->item(r,1)->text().toDouble();
        if (foundationProfileTable->item(r,2)) layer["y_min"] = foundationProfileTable->item(r,2)->text().toDouble();
        if (foundationProfileTable->item(r,3)) layer["y_max"] = foundationProfileTable->item(r,3)->text().toDouble();
        if (foundationProfileTable->item(r,4)) layer["z_top"] = foundationProfileTable->item(r,4)->text().toDouble();
        if (foundationProfileTable->item(r,5)) layer["z_bot"] = foundationProfileTable->item(r,5)->text().toDouble();
        auto mat = qobject_cast<QComboBox*>(foundationProfileTable->cellWidget(r,6)); if (mat) layer["material"] = mat->currentText();
        bool ok=false; auto mtoks = parseCsvTokens(foundationProfileTable->item(r,7)?foundationProfileTable->item(r,7)->text():QString(), &ok); QJsonArray mp; for (const QString &t : mtoks) { if (t.startsWith("RV.")) mp.append(t); else { bool lk=false; double v=t.toDouble(&lk); mp.append(lk?QJsonValue(v):QJsonValue()); } } layer["mat_props"] = mp;
        auto damp = qobject_cast<QComboBox*>(foundationProfileTable->cellWidget(r,8)); if (damp) layer["damping"] = damp->currentText();
        auto dvals = parseCsvDoubles(foundationProfileTable->item(r,9)?foundationProfileTable->item(r,9)->text():QString(), &ok); QJsonArray dp; for (double v : dvals) dp.append(v); layer["damping_props"] = dp;
        foundationProfile.append(layer);
    }
    foundationInfo["foundation_profile"] = foundationProfile;

    QJsonObject pileInfo;
    QJsonArray pileProfile;
    for (int r = 0; r < pileProfileTable->rowCount(); ++r) {
        QJsonObject item;
        auto type = qobject_cast<QComboBox*>(pileProfileTable->cellWidget(r,0)); if (type) item["type"] = type->currentText().toLower();
        bool ok=false; auto pvals = parseCsvDoubles(pileProfileTable->item(r,1)?pileProfileTable->item(r,1)->text():QString(), &ok);
        if (type && type->currentText().toLower() == "grid" && pvals.size() == 6) {
            item["x_start"] = pvals[0]; item["y_start"] = pvals[1]; item["spacing_x"] = pvals[2]; item["spacing_y"] = pvals[3]; item["nx"] = (int)pvals[4]; item["ny"] = (int)pvals[5];
        } else if (type && type->currentText().toLower() == "single" && pvals.size() == 4) {
            item["x_top"] = pvals[0]; item["x_bot"] = pvals[1]; item["y_top"] = pvals[2]; item["y_bot"] = pvals[3];
        }
        if (pileProfileTable->item(r,2)) item["z_top"] = pileProfileTable->item(r,2)->text().toDouble();
        if (pileProfileTable->item(r,3)) item["z_bot"] = pileProfileTable->item(r,3)->text().toDouble();
        if (pileProfileTable->item(r,4)) item["nz"] = pileProfileTable->item(r,4)->text().toInt();
        if (pileProfileTable->item(r,5)) item["r"] = pileProfileTable->item(r,5)->text().toDouble();
        auto section = qobject_cast<QComboBox*>(pileProfileTable->cellWidget(r,6)); if (section) item["section"] = section->currentText();
        auto mat = qobject_cast<QComboBox*>(pileProfileTable->cellWidget(r,7)); if (mat) item["material"] = mat->currentText();
        auto mtoksP = parseCsvTokens(pileProfileTable->item(r,8)?pileProfileTable->item(r,8)->text():QString(), &ok); QJsonArray mp; for (const QString &t : mtoksP) { if (t.startsWith("RV.")) mp.append(t); else { bool lk=false; double v=t.toDouble(&lk); mp.append(lk?QJsonValue(v):QJsonValue()); } } item["mat_props"] = mp;
        auto trans = qobject_cast<QComboBox*>(pileProfileTable->cellWidget(r,9));
        if (trans) {
            bool okcsv=false; auto tvals = parseCsvDoubles(pileProfileTable->item(r,10)?pileProfileTable->item(r,10)->text():QString(), &okcsv);
            double ux = (tvals.size() > 0) ? tvals[0] : 0.0;
            double uy = (tvals.size() > 1) ? tvals[1] : 1.0;
            double uz = (tvals.size() > 2) ? tvals[2] : 0.0;
            QJsonArray tarr; tarr.append(trans->currentText()); tarr.append(ux); tarr.append(uy); tarr.append(uz);
            item["transformation"] = tarr;
        }
        pileProfile.append(item);
    }
    pileInfo["pile_profile"] = pileProfile;
    {
        QJsonObject iface; iface["num_points_on_perimeter"] = pileNumPointsPerimeter->value(); iface["num_points_along_length"] = pileNumPointsLength->value(); iface["penalty_parameter"] = pilePenaltyParameter->value();
        pileInfo["pile_interface"] = iface;
    }

    soilFoundationInfo["soil_info"] = soilInfo;
    soilFoundationInfo["foundation_info"] = foundationInfo;
    soilFoundationInfo["pile_info"] = pileInfo;

    // Add random variables found in soil & foundation mat_props only under soilFoundationInfo
    {
        QSet<QString> names;
        // soil mat_props (col 4)
        for (int r = 0; r < soilProfileTable->rowCount(); ++r) {
            auto it = soilProfileTable->item(r,4);
            if (!it) continue;
            bool okTok=true; const QStringList toks = parseCsvTokens(it->text(), &okTok);
            if (!okTok) continue;
            for (const QString &t : toks) if (t.startsWith("RV.")) names.insert(t.mid(3));
        }
        // foundation mat_props (col 7)
        for (int r = 0; r < foundationProfileTable->rowCount(); ++r) {
            auto it = foundationProfileTable->item(r,7);
            if (!it) continue;
            bool okTok=true; const QStringList toks = parseCsvTokens(it->text(), &okTok);
            if (!okTok) continue;
            for (const QString &t : toks) if (t.startsWith("RV.")) names.insert(t.mid(3));
        }
        if (!names.isEmpty()) {
            QJsonArray rvArray;
            for (const QString &n : names) {
                QJsonObject o; o["name"] = n; o["value"] = QString("RV.") + n; rvArray.append(o);
            }
            soilFoundationInfo["randomVar"] = rvArray;
        }
    }
    return true;
}

bool SSI_SoilFoundationType1Widget::inputFromJSON(const QJsonObject& soilFoundationInfo) {
    // Soil
    const auto soilInfo = soilFoundationInfo.value("soil_info").toObject();
    xMinSoil->setValue(soilInfo.value("x_min").toDouble(xMinSoil->value()));
    xMaxSoil->setValue(soilInfo.value("x_max").toDouble(xMaxSoil->value()));
    yMinSoil->setValue(soilInfo.value("y_min").toDouble(yMinSoil->value()));
    yMaxSoil->setValue(soilInfo.value("y_max").toDouble(yMaxSoil->value()));
    nxSoil->setValue(soilInfo.value("nx").toInt(nxSoil->value()));
    nySoil->setValue(soilInfo.value("ny").toInt(nySoil->value()));
    // gravity_x and gravity_y are fixed to 0.0 in the UI
    gravZSoil->setValue(soilInfo.value("gravity_z").toDouble(gravZSoil->value()));
    numPartsSoil->setValue(soilInfo.value("num_partitions").toInt(numPartsSoil->value()));
    const auto bc = soilInfo.value("boundary_conditions").toString(boundaryCombo->currentText());
    int bcIdx = boundaryCombo->findText(bc); if (bcIdx >= 0) boundaryCombo->setCurrentIndex(bcIdx);
    if (bc == "DRM") {
        drmOptionsGroup->setVisible(true);
        const auto drm = soilInfo.value("DRM_options").toObject();
        int ablIdx = absorbingLayerCombo->findText(drm.value("absorbing_layer_type").toString(absorbingLayerCombo->currentText())); if (ablIdx >= 0) absorbingLayerCombo->setCurrentIndex(ablIdx);
        drmNumPartitions->setValue(drm.value("num_partitions").toInt(drmNumPartitions->value()));
        drmNumLayers->setValue(drm.value("number_of_layers").toInt(drmNumLayers->value()));
        drmRayleighDamping->setValue(drm.value("Rayleigh_damping").toDouble(drmRayleighDamping->value()));
        drmMatchDamping->setChecked(drm.value("match_damping").toBool(drmMatchDamping->isChecked()));
    } else {
        drmOptionsGroup->setVisible(false);
    }
    soilProfileTable->setRowCount(0);
    for (const auto& v : soilInfo.value("soil_profile").toArray()) {
        const auto obj = v.toObject();
        int row = soilProfileTable->rowCount(); soilProfileTable->insertRow(row);
        soilProfileTable->setItem(row, 0, new QTableWidgetItem(QString::number(obj.value("z_bot").toDouble())));
        soilProfileTable->setItem(row, 1, new QTableWidgetItem(QString::number(obj.value("z_top").toDouble())));
        soilProfileTable->setItem(row, 2, new QTableWidgetItem(QString::number(obj.value("nz").toInt())));
        auto mat = new QComboBox(); mat->addItem("Elastic"); int mi = mat->findText(obj.value("material").toString("Elastic")); if (mi>=0) mat->setCurrentIndex(mi); soilProfileTable->setCellWidget(row, 3, mat);
        QJsonArray mp = obj.value("mat_props").toArray(); QStringList mpStr; for (const auto& x : mp) mpStr << (x.isString()? x.toString() : QString::number(x.toDouble())); soilProfileTable->setItem(row, 4, new QTableWidgetItem(mpStr.join(", ")));
        auto damp = new QComboBox(); damp->addItem("Frequency-Rayleigh"); int di = damp->findText(obj.value("damping").toString("Frequency-Rayleigh")); if (di>=0) damp->setCurrentIndex(di); soilProfileTable->setCellWidget(row,5,damp);
        QJsonArray dp = obj.value("damping_props").toArray(); QStringList dpStr; for (const auto& x : dp) dpStr << QString::number(x.toDouble()); soilProfileTable->setItem(row, 6, new QTableWidgetItem(dpStr.join(", ")));
    }

    // Foundation
    const auto fInfo = soilFoundationInfo.value("foundation_info").toObject();
    // gravity_x and gravity_y are fixed to 0.0 in the UI
    gravZFound->setValue(fInfo.value("gravity_z").toDouble(gravZFound->value()));
    embeddedFoundation->setChecked(fInfo.value("embedded").toBool(embeddedFoundation->isChecked()));
    dxFound->setValue(fInfo.value("dx").toDouble(dxFound->value()));
    dyFound->setValue(fInfo.value("dy").toDouble(dyFound->value()));
    dzFound->setValue(fInfo.value("dz").toDouble(dzFound->value()));
    numPartsFound->setValue(fInfo.value("num_partitions").toInt(numPartsFound->value()));
    columnEmbedDepth->setValue(fInfo.value("column_embedment_depth").toDouble(columnEmbedDepth->value()));
    const auto sec = fInfo.value("column_section_props").toObject();
    secE->setValue(sec.value("E").toDouble(secE->value()));
    secA->setValue(sec.value("A").toDouble(secA->value()));
    secIy->setValue(sec.value("Iy").toDouble(secIy->value()));
    secIz->setValue(sec.value("Iz").toDouble(secIz->value()));
    secG->setValue(sec.value("G").toDouble(secG->value()));
    secJ->setValue(sec.value("J").toDouble(secJ->value()));

    foundationProfileTable->setRowCount(0);
    for (const auto& v : fInfo.value("foundation_profile").toArray()) {
        const auto obj = v.toObject();
        int row = foundationProfileTable->rowCount(); foundationProfileTable->insertRow(row);
        foundationProfileTable->setItem(row, 0, new QTableWidgetItem(QString::number(obj.value("x_min").toDouble())));
        foundationProfileTable->setItem(row, 1, new QTableWidgetItem(QString::number(obj.value("x_max").toDouble())));
        foundationProfileTable->setItem(row, 2, new QTableWidgetItem(QString::number(obj.value("y_min").toDouble())));
        foundationProfileTable->setItem(row, 3, new QTableWidgetItem(QString::number(obj.value("y_max").toDouble())));
        foundationProfileTable->setItem(row, 4, new QTableWidgetItem(QString::number(obj.value("z_top").toDouble())));
        foundationProfileTable->setItem(row, 5, new QTableWidgetItem(QString::number(obj.value("z_bot").toDouble())));
        auto mat = new QComboBox(); mat->addItem("Elastic"); foundationProfileTable->setCellWidget(row, 6, mat);
        QJsonArray mp = obj.value("mat_props").toArray(); QStringList mpStr; for (const auto& x : mp) mpStr << (x.isString()? x.toString() : QString::number(x.toDouble())); foundationProfileTable->setItem(row, 7, new QTableWidgetItem(mpStr.join(", ")));
        auto damp = new QComboBox(); damp->addItem("Frequency-Rayleigh"); int di = damp->findText(obj.value("damping").toString("Frequency-Rayleigh")); if (di>=0) damp->setCurrentIndex(di); foundationProfileTable->setCellWidget(row,8,damp);
        QJsonArray dp = obj.value("damping_props").toArray(); QStringList dpStr; for (const auto& x : dp) dpStr << QString::number(x.toDouble()); foundationProfileTable->setItem(row, 9, new QTableWidgetItem(dpStr.join(", ")));
    }

    // Piles
    const auto pile = soilFoundationInfo.value("pile_info").toObject();
    pileProfileTable->setRowCount(0);
    for (const auto& v : pile.value("pile_profile").toArray()) {
        const auto obj = v.toObject();
        int row = pileProfileTable->rowCount(); pileProfileTable->insertRow(row);
        auto type = new QComboBox(); type->addItem("Grid"); type->addItem("Single"); int ti = type->findText(obj.value("type").toString("Grid"), Qt::MatchFixedString | Qt::MatchCaseSensitive); if (ti<0) ti = type->findText(obj.value("type").toString("Grid"), Qt::MatchFixedString); if (ti>=0) type->setCurrentIndex(ti); pileProfileTable->setCellWidget(row,0,type);
        QString params;
        if (type->currentText().toLower() == "grid") {
            params = QString("%1, %2, %3, %4, %5, %6").arg(obj.value("x_start").toDouble()).arg(obj.value("y_start").toDouble()).arg(obj.value("spacing_x").toDouble()).arg(obj.value("spacing_y").toDouble()).arg(obj.value("nx").toInt()).arg(obj.value("ny").toInt());
        } else {
            params = QString("%1, %2, %3, %4").arg(obj.value("x_top").toDouble()).arg(obj.value("x_bot").toDouble()).arg(obj.value("y_top").toDouble()).arg(obj.value("y_bot").toDouble());
        }
        pileProfileTable->setItem(row, 1, new QTableWidgetItem(params));
        pileProfileTable->setItem(row, 2, new QTableWidgetItem(QString::number(obj.value("z_top").toDouble())));
        pileProfileTable->setItem(row, 3, new QTableWidgetItem(QString::number(obj.value("z_bot").toDouble())));
        pileProfileTable->setItem(row, 4, new QTableWidgetItem(QString::number(obj.value("nz").toInt())));
        pileProfileTable->setItem(row, 5, new QTableWidgetItem(QString::number(obj.value("r").toDouble())));
        auto section = new QComboBox(); section->addItem("No-Section"); pileProfileTable->setCellWidget(row, 6, section);
        auto mat = new QComboBox(); mat->addItem("Elastic"); pileProfileTable->setCellWidget(row, 7, mat);
        QJsonArray mp = obj.value("mat_props").toArray(); QStringList mpStr; for (const auto& x : mp) mpStr << (x.isString()? x.toString() : QString::number(x.toDouble())); pileProfileTable->setItem(row, 8, new QTableWidgetItem(mpStr.join(", ")));
        QJsonArray tr = obj.value("transformation").toArray(); QString trName = tr.size() > 0 ? tr.at(0).toString("Linear") : "Linear"; auto trans = new QComboBox(); trans->addItem("Linear"); trans->addItem("PDelta"); int tri = trans->findText(trName); if (tri>=0) trans->setCurrentIndex(tri); pileProfileTable->setCellWidget(row, 9, trans);
        double tx = tr.size() > 1 ? tr.at(1).toDouble(0.0) : 0.0;
        double ty = tr.size() > 2 ? tr.at(2).toDouble(1.0) : 1.0;
        double tz = tr.size() > 3 ? tr.at(3).toDouble(0.0) : 0.0;
        pileProfileTable->setItem(row, 10, new QTableWidgetItem(QString("%1, %2, %3").arg(tx).arg(ty).arg(tz)));
    }
    const auto iface = pile.value("pile_interface").toObject();
    pileNumPointsPerimeter->setValue(iface.value("num_points_on_perimeter").toInt(pileNumPointsPerimeter->value()));
    pileNumPointsLength->setValue(iface.value("num_points_along_length").toInt(pileNumPointsLength->value()));
    pilePenaltyParameter->setValue(iface.value("penalty_parameter").toDouble(pilePenaltyParameter->value()));

    return true;
}

void SSI_SoilFoundationType1Widget::plot() const {
    // Placeholder: no plotting implemented yet
}

QStringList SSI_SoilFoundationType1Widget::getRandomVariableNames() const { return QStringList(); }


int SSI_SoilFoundationType1Widget::getNumberOfCores() const {
    int ncoresofsoil = int(numPartsSoil->value());
    int ncoresoffoundation = int(numPartsFound->value());
    // if using DRM, add its partitions
    if (boundaryCombo->currentText() == "DRM") {
        ncoresofsoil += int(drmNumPartitions->value());
    }
    return ncoresofsoil + ncoresoffoundation;
}


