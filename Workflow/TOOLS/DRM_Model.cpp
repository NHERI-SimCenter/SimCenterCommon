/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS 
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */


// Written: Amin Pakzad
#include <SimCenterAppWidget.h>
#include <DRM_Model.h>
#include <TapisV3.h>
#include <SC_DoubleLineEdit.h>
#include <SC_IntLineEdit.h>
#include <QGroupBox>
#include <QGridLayout>
#include <QApplication>
#include <QLabel>
#include <QComboBox>
#include <QSplitter>
#include <SC_DirEdit.h>
#include <SC_CheckBox.h>
#include <QPushButton>
#include <QWebEngineView>
#include <SimCenterPreferences.h>
#include <QDir>
#include <QFileDialog>
#include <RunPythonInThread.h>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileDialog>
#include <QListView>
#include <QStringListModel>
#include <QScreen>
#include <QScrollArea>

DRM_Model::DRM_Model(QWidget *parent)
    : SimCenterAppWidget(parent)
{
    QString tenant("designsafe");
    QString storage("designsafe.storage.default/"); 
    the_DesignSafe_Storage = new TapisV3(tenant, storage);
    
    // Create a main layout
    QWidget *theWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout();
    theWidget->setLayout(mainLayout);


    // scroll area
    QVBoxLayout *layoutWithScroll = new QVBoxLayout();
    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);
    sa->setWidget(theWidget);
    layoutWithScroll->addWidget(sa);
    this->setLayout(layoutWithScroll);





    // Create a QSplitter
    QSplitter *mainSplitter = new QSplitter();
    mainLayout->addWidget(mainSplitter);

    // create left and right widgets
    QWidget *leftWidget = new QWidget();
    QWidget *rightWidget = new QWidget();

    QGridLayout *leftLayout = new QGridLayout(leftWidget);
    QGridLayout *rightLayout = new QGridLayout(rightWidget);

    leftLayout->setAlignment(Qt::AlignTop);
    rightLayout->setAlignment(Qt::AlignTop);

    mainSplitter->addWidget(leftWidget);
    mainSplitter->addWidget(rightWidget);


    // ==========================================================
    // left widget
    // ==========================================================
    QGroupBox *SoilProperties = new QGroupBox("Soil Properties");
    QGridLayout *SoilPropertiesLayout = new QGridLayout(SoilProperties);
    QGroupBox *SoilMesh = new QGroupBox("Interior Soil Mesh");
    QGridLayout *SoilMeshLayout = new QGridLayout(SoilMesh);


    // soil width x y z
    SoilMeshLayout->addWidget(new QLabel("Width in X (m)"), 0, 0);
    soilwidth_x = new SC_DoubleLineEdit("Width of the soil in X direction", 100);
    soilwidth_x->setPlaceholderText("Width of the soil in X direction");
    SoilMeshLayout->addWidget(soilwidth_x, 0, 1);

    SoilMeshLayout->addWidget(new QLabel("Width in Y (m)"), 1, 0);
    soilwidth_y = new SC_DoubleLineEdit("Width of the soil in Y direction",100);
    soilwidth_y->setPlaceholderText("Width of the soil in Y direction");
    SoilMeshLayout->addWidget(soilwidth_y, 1, 1);

    SoilMeshLayout->addWidget(new QLabel("Width in Z (m)"), 2, 0);
    soilwidth_z = new SC_DoubleLineEdit("Width of the soil in Z direction",30);
    soilwidth_z->setPlaceholderText("Width of the soil in Z direction");
    SoilMeshLayout->addWidget(soilwidth_z, 2, 1);

    // number of elements in x y z
    SoilMeshLayout->addWidget(new QLabel("Number of Elements in X"), 0, 2);
    soilNumEle_x = new SC_IntLineEdit("Number of elements in X direction",40);
    soilNumEle_x->setPlaceholderText("Number of elements in X direction");
    SoilMeshLayout->addWidget(soilNumEle_x, 0, 3);

    SoilMeshLayout->addWidget(new QLabel("Number of Elements in Y"), 1, 2);
    soilNumEle_y = new SC_IntLineEdit("Number of elements in Y direction",40);
    soilNumEle_y->setPlaceholderText("Number of elements in Y direction");
    SoilMeshLayout->addWidget(soilNumEle_y, 1, 3);

    SoilMeshLayout->addWidget(new QLabel("Number of Elements in Z"), 2, 2);
    soilNumEle_z = new SC_IntLineEdit("Number of elements in Z direction",12);
    soilNumEle_z->setPlaceholderText("Number of elements in Z direction");
    SoilMeshLayout->addWidget(soilNumEle_z, 2, 3);


    // make the columns equal width
    SoilMeshLayout->setColumnStretch(0, 1);
    SoilMeshLayout->setColumnStretch(1, 1);
    SoilMeshLayout->setColumnStretch(2, 1);
    SoilMeshLayout->setColumnStretch(3, 1);



    // add soil properties to the layout
    QGroupBox *MaterialLayer = new QGroupBox("Soil Material");
    QGridLayout *MaterialLayerLayout = new QGridLayout(MaterialLayer);

    SoilPropertiesLayout->addWidget(MaterialLayer);

    Vs = new SC_DoubleLineEdit("Shear Wave Velocity (m/s)", 750.0);
    // MaterialLayerLayout->setPlaceholderText("Shear Wave Velocity (m/s)");
    MaterialLayerLayout->addWidget(new QLabel("Shear Wave Velocity (m/s)"), 0, 0);
    MaterialLayerLayout->addWidget(Vs, 0, 1);

    nu = new SC_DoubleLineEdit("Poisson's Ratio", 0.26163);
    // MaterialLayerLayout->setPlaceholderText("Poisson's Ratio");
    MaterialLayerLayout->addWidget(new QLabel("Poisson's Ratio"), 0, 2);
    MaterialLayerLayout->addWidget(nu, 0, 3);

    rho = new SC_DoubleLineEdit("Density (kg/m^3)", 2400.0);
    // MaterialLayerLayout->setPlaceholderText("Density (kg/m^3)");
    MaterialLayerLayout->addWidget(new QLabel("Density (kg/m^3)"), 0, 4);
    MaterialLayerLayout->addWidget(rho, 0, 5);


    // make the columns equal width
    MaterialLayerLayout->setColumnStretch(0, 1);
    MaterialLayerLayout->setColumnStretch(1, 1);
    MaterialLayerLayout->setColumnStretch(2, 1);
    MaterialLayerLayout->setColumnStretch(3, 1);
    MaterialLayerLayout->setColumnStretch(4, 1);

    SoilPropertiesLayout->addWidget(SoilMesh);
    SoilPropertiesLayout->addWidget(MaterialLayer);


    // ==========================================================
    // adding DRM box to the left widget
    // ==========================================================
    QGroupBox *DRMBox = new QGroupBox("DRM Layer");
    QGridLayout *DRMBoxLayout = new QGridLayout(DRMBox);

    DRM_Sofrware = new QComboBox();
    DRM_Sofrware->addItem("ShakerMaker");
    DRM_Sofrware->addItem("Custom");
    DRMBoxLayout->addWidget(new QLabel("DRM Software"), 0, 0);
    // hint for the user when the mouse is over the widget
    DRM_Sofrware->setToolTip("The software used to generate the DRM file");
    DRMBoxLayout->addWidget(DRM_Sofrware, 0, 1);

    DRM_numLayers = new SC_IntLineEdit("Number of Layers", 3);
    DRM_numLayers->setPlaceholderText("Number of Layers");
    DRMBoxLayout->addWidget(new QLabel("Number of Layers"), 0, 2);
    DRMBoxLayout->addWidget(DRM_numLayers, 0, 3);


    DRM_CoordinateTransformation = new SC_CheckBox("Coordinate Transformation");
    DRM_CoordinateTransformation->setToolTip("Check if the DRM file needs coordinate transformation");
    DRM_CoordinateTransformation->setChecked(true);
    DRMBoxLayout->addWidget(new QLabel("Coordinate Transformation"), 0, 4);
    DRMBoxLayout->addWidget(DRM_CoordinateTransformation, 0, 5);


    DRM_laodFactor = new SC_DoubleLineEdit("Load Factor", 1.0);
    DRM_laodFactor->setPlaceholderText("Load Factor");
    DRM_laodFactor->setToolTip("Load factor for the DRM file");
    DRMBoxLayout->addWidget(new QLabel("Load Factor"), 1, 0);
    DRMBoxLayout->addWidget(DRM_laodFactor, 1, 1);

    DRM_crdScale = new SC_DoubleLineEdit("Coordinate Scale", 1000.0);
    DRM_crdScale->setPlaceholderText("Coordinate Scale");
    DRM_crdScale->setToolTip("Scale for the DRM coordinates");
    DRMBoxLayout->addWidget(new QLabel("Coordinate Scale"), 1, 2);
    DRMBoxLayout->addWidget(DRM_crdScale, 1, 3);
    

    DRM_tolernace = new SC_DoubleLineEdit("Tolerance", 0.001);
    DRM_tolernace->setPlaceholderText("Tolerance");
    DRM_tolernace->setToolTip("Tolerance for matching the DRM nodes to the mesh nodes");
    DRMBoxLayout->addWidget(new QLabel("Tolerance"), 1, 4);
    DRMBoxLayout->addWidget(DRM_tolernace, 1, 5);

    DRM_T00 = new SC_DoubleLineEdit("T11", 0.0);
    DRM_T00->setPlaceholderText(" Coordinate Transformation Matrix in row 1 column 1");
    DRM_T00->setToolTip("Coordinate Transformation Matrix in row 1 column 1");
    DRMBoxLayout->addWidget(new QLabel("T11"), 2, 0);
    DRMBoxLayout->addWidget(DRM_T00, 2, 1);

    DRM_T01 = new SC_DoubleLineEdit("T12", 1.0);
    DRM_T01->setPlaceholderText(" Coordinate Transformation Matrix in row 1 column 2");
    DRM_T01->setToolTip("Coordinate Transformation Matrix in row 1 column 2");
    DRMBoxLayout->addWidget(new QLabel("T12"), 2, 2);
    DRMBoxLayout->addWidget(DRM_T01, 2, 3);

    DRM_T02 = new SC_DoubleLineEdit("T13", 0.0);
    DRM_T02->setPlaceholderText(" Coordinate Transformation Matrix in row 1 column 3");
    DRM_T02->setToolTip("Coordinate Transformation Matrix in row 1 column 3");
    DRMBoxLayout->addWidget(new QLabel("T13"), 2, 4);
    DRMBoxLayout->addWidget(DRM_T02, 2, 5);

    DRM_T10 = new SC_DoubleLineEdit("T21", 1.0);
    DRM_T10->setPlaceholderText(" Coordinate Transformation Matrix in row 2 column 1");
    DRM_T10->setToolTip("Coordinate Transformation Matrix in row 2 column 1");
    DRMBoxLayout->addWidget(new QLabel("T21"), 3, 0);
    DRMBoxLayout->addWidget(DRM_T10, 3, 1);

    DRM_T11 = new SC_DoubleLineEdit("T22", 0.0);
    DRM_T11->setPlaceholderText(" Coordinate Transformation Matrix in row 2 column 2");
    DRM_T11->setToolTip("Coordinate Transformation Matrix in row 2 column 2");
    DRMBoxLayout->addWidget(new QLabel("T22"), 3, 2);
    DRMBoxLayout->addWidget(DRM_T11, 3, 3);

    DRM_T12 = new SC_DoubleLineEdit("T23", 0.0);
    DRM_T12->setPlaceholderText(" Coordinate Transformation Matrix in row 2 column 3");
    DRM_T12->setToolTip("Coordinate Transformation Matrix in row 2 column 3");
    DRMBoxLayout->addWidget(new QLabel("T23"), 3, 4);
    DRMBoxLayout->addWidget(DRM_T12, 3, 5);

    DRM_T20 = new SC_DoubleLineEdit("T31", 0.0);
    DRM_T20->setPlaceholderText(" Coordinate Transformation Matrix in row 3 column 1");
    DRM_T20->setToolTip("Coordinate Transformation Matrix in row 3 column 1");
    DRMBoxLayout->addWidget(new QLabel("T31"), 4, 0);
    DRMBoxLayout->addWidget(DRM_T20, 4, 1);

    DRM_T21 = new SC_DoubleLineEdit("T32", 0.0);
    DRM_T21->setPlaceholderText(" Coordinate Transformation Matrix in row 3 column 2");
    DRM_T21->setToolTip("Coordinate Transformation Matrix in row 3 column 2");
    DRMBoxLayout->addWidget(new QLabel("T32"), 4, 2);
    DRMBoxLayout->addWidget(DRM_T21, 4, 3);

    DRM_T22 = new SC_DoubleLineEdit("T33", -1.0);
    DRM_T22->setPlaceholderText(" Coordinate Transformation Matrix in row 3 column 3");
    DRM_T22->setToolTip("Coordinate Transformation Matrix in row 3 column 3");
    DRMBoxLayout->addWidget(new QLabel("T33"), 4, 4);
    DRMBoxLayout->addWidget(DRM_T22, 4, 5);

    DRM_originX = new SC_DoubleLineEdit("Origin X", 0.0);
    DRM_originX->setPlaceholderText("Coordinate of the center of the DRM in X direction after transformation");
    DRM_originX->setToolTip("Coordinate of the center of the DRM in X direction after transformation");
    DRMBoxLayout->addWidget(new QLabel("Origin X"), 5, 0);
    DRMBoxLayout->addWidget(DRM_originX, 5, 1);

    DRM_originY = new SC_DoubleLineEdit("Origin Y", 0.0);
    DRM_originY->setPlaceholderText("Coordinate of the center of the DRM in Y direction after transformation");
    DRM_originY->setToolTip("Coordinate of the center of the DRM in Y direction after transformation");
    DRMBoxLayout->addWidget(new QLabel("Origin Y"), 5, 2);
    DRMBoxLayout->addWidget(DRM_originY, 5, 3);

    DRM_originZ = new SC_DoubleLineEdit("Origin Z", 0.0);
    DRM_originZ->setPlaceholderText("Coordinate of the center of the DRM in Z direction after transformation");
    DRM_originZ->setToolTip("Coordinate of the center of the DRM in Z direction after transformation");
    DRMBoxLayout->addWidget(new QLabel("Origin Z"), 5, 4);
    DRMBoxLayout->addWidget(DRM_originZ, 5, 5);


    DRM_Location = new QComboBox();
    DRM_Location->addItem("Local");
    DRM_Location->addItem("DesignSafe");

    DRMBoxLayout->addWidget(new QLabel("DRM File Location"), 6, 0);
    DRMBoxLayout->addWidget(DRM_Location, 6, 1);
    
    DRM_filePath = new QLineEdit();
    DRMBoxLayout->addWidget(new QLabel("DRM File Path"), 6, 2);
    DRMBoxLayout->addWidget(DRM_filePath, 6, 3, 1, 2);



    // put a  browse push button
    QPushButton *browseButton = new QPushButton("Browse");
    DRMBoxLayout->addWidget(browseButton, 6, 5);
    connect(browseButton, &QPushButton::clicked, [=]() {
        if (DRM_Location->currentText() == "Local") {
            QString fileName = QFileDialog::getOpenFileName(this, tr("Open DRM File"), QDir::currentPath());
            DRM_filePath->setText(fileName);
        } 
        if (DRM_Location->currentText() == "DesignSafe") {
            // check that user name and password is not empty
            // if (tapisUsername.isEmpty() || tapisPassword.isEmpty()) {
            //     this->get_credentials(tapisUsername, tapisPassword);
            // }
            if (!loggedIn) {
                this->get_credentials(tapisUsername, tapisPassword);
                bool status =the_DesignSafe_Storage->login(tapisUsername, tapisPassword);
                if (!status) {
                    errorMessage("Login Failed!");
                    loggedIn = false;
                    return;
                } else {
                    loggedIn = true;
                }
            } 

            QStringList files = this->getfilelist(tapisUsername);


            // creating a interface that shows the files in the DesignSafe and 
            // the user can select the file and go forward in the directory tree

            QListView *listView = new QListView();
            QStringListModel *model = new QStringListModel();
            model->setStringList(files);

            QString CurrentPath = tapisUsername;

            listView->setModel(model);
            listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
            listView->setSelectionMode(QAbstractItemView::SingleSelection);
            listView->setSelectionBehavior(QAbstractItemView::SelectRows);
            listView->setAlternatingRowColors(true);
            listView->setStyleSheet("QListView::item { border-bottom: 1px solid black; }");


            QDialog *dialog = new QDialog();
            QVBoxLayout *dialogLayout = new QVBoxLayout();
            dialog->setLayout(dialogLayout);
            dialogLayout->addWidget(listView);



            QPushButton *selectButton = new QPushButton("Select");
            dialogLayout->addWidget(selectButton);
            connect(selectButton, &QPushButton::clicked, [=]() {
                QModelIndex index = listView->currentIndex();
                DRM_filePath->setText(index.data().toString());
                dialog->close();
            });

            QPushButton *forwardButton = new QPushButton("Forward");
            dialogLayout->addWidget(forwardButton);
            connect(forwardButton, &QPushButton::clicked, [=, &CurrentPath]() {
                QModelIndex index = listView->currentIndex();
                QString fileName = index.data().toString();
                // go forward in the directory tree
                QStringList newFiles = this->getfilelist(fileName);
                model->setStringList(newFiles);
                CurrentPath = fileName;
            });


            QPushButton *backButton = new QPushButton("Backward");
            dialogLayout->addWidget(backButton);
            connect(backButton, &QPushButton::clicked, [=, &CurrentPath]() {
                // delet the last element after the last /
                QStringList parts = CurrentPath.split("/");
                parts.removeLast();
                QString newFileName = parts.join("/");
                QStringList newFiles = this->getfilelist(newFileName);
                model->setStringList(newFiles);
                CurrentPath = newFileName;
            });

            dialog->exec();





        }

        // use the curl to login and get the JWT token
        // curl -H "Content-type: application/json" -d '{"username": "YOUR LOGIN", "password": "YOUR PASSWORD", "grant_type": "password" }' https://designsafe.tapis.io/v3/oauth2/tokens
        

    });
    
    





    // it he user unchecked the coordinate transformation, the transformation mattix should be 1 0 0; 0 1 0; 0 0 1
    connect(DRM_CoordinateTransformation, &SC_CheckBox::stateChanged, [=](int state) {
        if (state == 0) {
            DRM_T00->setText("1.0");
            DRM_T01->setText("0.0");
            DRM_T02->setText("0.0");
            DRM_T10->setText("0.0");
            DRM_T11->setText("1.0");
            DRM_T12->setText("0.0");
            DRM_T20->setText("0.0");
            DRM_T21->setText("0.0");
            DRM_T22->setText("1.0");
            DRM_originX->setText("0.0");
            DRM_originY->setText("0.0");
            DRM_originZ->setText("0.0");
        }
    });



    // if the user selects custom software shakermaker, T matrix should be 0 1 0; 1 0 0; 0 0 -1
    connect(DRM_Sofrware, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        if (index == 0) {
            DRM_T00->setText("0.0");
            DRM_T01->setText("1.0");
            DRM_T02->setText("0.0");
            DRM_T10->setText("1.0");
            DRM_T11->setText("0.0");
            DRM_T12->setText("0.0");
            DRM_T20->setText("0.0");
            DRM_T21->setText("0.0");
            DRM_T22->setText("-1.0");
        }
    });
    

    
    // make the columns equal width
    DRMBoxLayout->setColumnStretch(0, 1);
    DRMBoxLayout->setColumnStretch(1, 1);
    DRMBoxLayout->setColumnStretch(2, 1);
    DRMBoxLayout->setColumnStretch(3, 1);
    DRMBoxLayout->setColumnStretch(4, 1);
    DRMBoxLayout->setColumnStretch(5, 1);




    // ==========================================================
    // adding absorbing layer box
    // ==========================================================
    QGroupBox *AbsorbingBox = new QGroupBox("Absorbing Layer");
    QGridLayout *AbsorbingBoxLayout = new QGridLayout(AbsorbingBox);

    Absorb_HaveAbsorbingElements = new SC_CheckBox("Use Absorbing Elements");
    Absorb_HaveAbsorbingElements->setChecked(true);
    Absorb_HaveAbsorbingElements->setToolTip("Check if want to use absorbing elements");
    AbsorbingBoxLayout->addWidget(new QLabel("Use Absorbing Elements"), 0, 0);
    AbsorbingBoxLayout->addWidget(Absorb_HaveAbsorbingElements, 0, 1);

    Absorb_type = new QComboBox();
    Absorb_type->addItem("Perfectly Matched Layer");
    Absorb_type->addItem("ASDA Absorbing Elements");
    Absorb_type->addItem("Normal Elements with Rayleigh Damping");
    AbsorbingBoxLayout->addWidget(new QLabel("Absorbing Type"), 1, 0);
    AbsorbingBoxLayout->addWidget(Absorb_type, 1, 1);

    Absorb_NumAbsorbingElements = new SC_DoubleLineEdit("Number of Layers", 2);
    Absorb_NumAbsorbingElements->setPlaceholderText("Number of Absorbing Elements");
    Absorb_NumAbsorbingElements->setToolTip("Number of Absorbing Elements");
    AbsorbingBoxLayout->addWidget(new QLabel("Number of Layers"), 1, 2);
    AbsorbingBoxLayout->addWidget(Absorb_NumAbsorbingElements, 1, 3);

    Absorb_rayleighAlpha = new SC_DoubleLineEdit("Rayleigh Alpha",0.12441951103);
    Absorb_rayleighAlpha->setPlaceholderText("Rayleigh Alpha");
    Absorb_rayleighAlpha->setToolTip("Rayleigh Alpha");
    AbsorbingBoxLayout->addWidget(new QLabel("Rayleigh Alpha"), 2, 0);
    AbsorbingBoxLayout->addWidget(Absorb_rayleighAlpha, 2, 1);

    Absorb_rayleighBeta = new SC_DoubleLineEdit("Rayleigh Beta",  0.00078789575);
    Absorb_rayleighBeta->setPlaceholderText("Rayleigh Beta");
    Absorb_rayleighBeta->setToolTip("Rayleigh Beta");
    AbsorbingBoxLayout->addWidget(new QLabel("Rayleigh Beta"), 2, 2);
    AbsorbingBoxLayout->addWidget(Absorb_rayleighBeta, 2, 3);

    // lock the rayleigh alpha and beta
    Absorb_rayleighAlpha->setDisabled(true);
    Absorb_rayleighBeta->setDisabled(true);

    // lock the rayleigh alpha and beta 
    connect(Absorb_type, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        if (index != 2) {
            Absorb_rayleighAlpha->setDisabled(true);
            Absorb_rayleighBeta->setDisabled(true);
        } else {
            Absorb_rayleighAlpha->setDisabled(false);
            Absorb_rayleighBeta->setDisabled(false);
        }
    });


    // make the columns equal width
    AbsorbingBoxLayout->setColumnStretch(0, 1);
    AbsorbingBoxLayout->setColumnStretch(1, 1);
    AbsorbingBoxLayout->setColumnStretch(2, 1);
    AbsorbingBoxLayout->setColumnStretch(3, 1);
    // AbsorbingBoxLayout->setColumnStretch(4, 1);
    // AbsorbingBoxLayout->setColumnStretch(5, 1);


    // ==========================================================
    // adding the analysis box
    // ==========================================================
    QGroupBox *AnalysisBox = new QGroupBox("Analysis Information");
    QGridLayout *AnalysisBoxLayout = new QGridLayout(AnalysisBox);

    dt = new SC_DoubleLineEdit("Time Step (s)", 0.005);
    dt->setPlaceholderText("Time Step  for the analysis");
    AnalysisBoxLayout->addWidget(new QLabel("Time Step (s)"), 0, 0);
    AnalysisBoxLayout->addWidget(dt, 0, 1);

    t_final = new SC_DoubleLineEdit("Final Time (s)", 60.0);
    t_final->setPlaceholderText("Final Time for the analysis");
    AnalysisBoxLayout->addWidget(new QLabel("Final Time (s)"), 0, 2);
    AnalysisBoxLayout->addWidget(t_final, 0, 3);

    recording_dt = new SC_DoubleLineEdit("Recording Time Step (s)", 0.005);
    recording_dt->setPlaceholderText("Recording Time Step for the analysis");
    AnalysisBoxLayout->addWidget(new QLabel("Recording Time Step (s)"), 0, 4);
    AnalysisBoxLayout->addWidget(recording_dt, 0, 5);

    QString path = SimCenterPreferences::getInstance()->getLocalWorkDir() + QDir::separator() + "DRM_Model";
    outputDir = new SC_DirEdit("Output Directory");
    outputDir->setDirName(path);
    AnalysisBoxLayout->addWidget(new QLabel("Output Directory"), 1, 0);
    AnalysisBoxLayout->addWidget(outputDir, 1, 1, 1, 5);

    // check that recording time step bigger than the analysis time step
    // every time the user changes the analysis time step, the recording time step should be bigger
    connect(dt, &SC_DoubleLineEdit::editingFinished, [=]() {
        if (recording_dt->text().toDouble() < dt->text().toDouble()) {
            recording_dt->setText(dt->text());
        }
    });

    // check every time the user changes the recording time step
    connect(recording_dt, &SC_DoubleLineEdit::editingFinished, [=]() {
        if (recording_dt->text().toDouble() < dt->text().toDouble()) {
            recording_dt->setText(dt->text());
        }
    });



    // put a push buttton to create the model
    QPushButton *createModel = new QPushButton("Create Model");
    AnalysisBoxLayout->addWidget(createModel, 2, 0, 1, 6);

    // connect the qpushbutton to a create model function
    connect(createModel, &QPushButton::clicked, [=]() {
        // create the model
        this->createModel();
    });




    // ===============================================================
    // partitiones
    // ===============================================================
    QGroupBox *PartitionesBox = new QGroupBox("Partitions");
    QGridLayout *PartitionesBoxLayout = new QGridLayout(PartitionesBox);

    PartitionesBoxLayout->addWidget(new QLabel("Partitioning Algorithm"), 0, 0);
    partitioningAlgorithm = new QComboBox();
    partitioningAlgorithm->addItem("Kd-Tree");
    PartitionesBoxLayout->addWidget(partitioningAlgorithm, 0, 1);

   
    
    // split the text into two lines
    QString text = "Kd-Tree: Uses a Kd-Tree algorithm to balance cell centers across partitions. Only supports power-of-2 partitions. If a non-power-of-2 value is given, it defaults to the next power of two.";
    QLabel *partitioningAlgorithmText = new QLabel(text);
    partitioningAlgorithmText->setWordWrap(true);
    PartitionesBoxLayout->addWidget(partitioningAlgorithmText, 0, 2, 1, 4);



    PartitionesBoxLayout->addWidget(new QLabel("Number of Soil Partitions"), 1, 0);
    numSoilPartitions = new SC_IntLineEdit("Number of Soil Partitions", 1);
    PartitionesBoxLayout->addWidget(numSoilPartitions, 1, 1);

    PartitionesBoxLayout->addWidget(new QLabel("Number of DRM Partitions"), 1, 2);
    numDRMPartitions = new SC_IntLineEdit("Number of DRM Partitions", 1);
    PartitionesBoxLayout->addWidget(numDRMPartitions, 1, 3);

    PartitionesBoxLayout->addWidget(new QLabel("Number of Absorbing Partitions"), 1, 4);
    numAbsorbingPartitions = new SC_IntLineEdit("Number of Absorbing Partitions", 1);
    PartitionesBoxLayout->addWidget(numAbsorbingPartitions, 1, 5);





    // change the number of soil partitions to the next power of 2 if user changes the number of soil partitions
    connect(numSoilPartitions, &SC_IntLineEdit::editingFinished, [=]() {
        // check first if the algorithm is Kd-Tree
        if (partitioningAlgorithm->currentText() == "Kd-Tree") {
            int num = numSoilPartitions->text().toInt();
            int nextPowerOf2 = 1;
            while (nextPowerOf2 < num) {
                nextPowerOf2 *= 2;
            }
            numSoilPartitions->setText(QString::number(nextPowerOf2));
        }
    });
    
    connect(numDRMPartitions, &SC_IntLineEdit::editingFinished, [=]() {
        // check first if the algorithm is Kd-Tree
        if (partitioningAlgorithm->currentText() == "Kd-Tree") {
            int num = numDRMPartitions->text().toInt();
            int nextPowerOf2 = 1;
            while (nextPowerOf2 < num) {
                nextPowerOf2 *= 2;
            }
            numDRMPartitions->setText(QString::number(nextPowerOf2));
        }
    });

    connect(numAbsorbingPartitions, &SC_IntLineEdit::editingFinished, [=]() {
        // check first if the algorithm is Kd-Tree
        if (partitioningAlgorithm->currentText() == "Kd-Tree") {
            int num = numAbsorbingPartitions->text().toInt();
            int nextPowerOf2 = 1;
            while (nextPowerOf2 < num) {
                nextPowerOf2 *= 2;
            }
            numAbsorbingPartitions->setText(QString::number(nextPowerOf2));
        }
    });

    // now make the partiones if user change the algorithm to Kd-Tree
    connect(partitioningAlgorithm, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        if (partitioningAlgorithm->currentText() == "Kd-Tree") {
            int num = numSoilPartitions->text().toInt();
            int nextPowerOf2 = 1;
            while (nextPowerOf2 < num) {
                nextPowerOf2 *= 2;
            }
            numSoilPartitions->setText(QString::number(nextPowerOf2));

            num = numDRMPartitions->text().toInt();
            nextPowerOf2 = 1;
            while (nextPowerOf2 < num) {
                nextPowerOf2 *= 2;
            }
            numDRMPartitions->setText(QString::number(nextPowerOf2));

            num = numAbsorbingPartitions->text().toInt();
            nextPowerOf2 = 1;
            while (nextPowerOf2 < num) {
                nextPowerOf2 *= 2;
            }
            numAbsorbingPartitions->setText(QString::number(nextPowerOf2));
        }
    });


    // make the columns equal width
    PartitionesBoxLayout->setColumnStretch(0, 1);
    PartitionesBoxLayout->setColumnStretch(1, 1);
    PartitionesBoxLayout->setColumnStretch(2, 1);
    PartitionesBoxLayout->setColumnStretch(3, 1);
    PartitionesBoxLayout->setColumnStretch(4, 1);
    PartitionesBoxLayout->setColumnStretch(5, 1);

    // add the partitiones box to the layout


    // ==========================================================
    // jub box
    // ==========================================================
    QGroupBox *JobBox = new QGroupBox("Job Information");
    QGridLayout *JobBoxLayout = new QGridLayout(JobBox);

    JobBoxLayout->addWidget(new QLabel("System"), 0, 0);
    Job_System = new QComboBox();
    Job_System->addItem("frontera");
    Job_System->addItem("stampede3");
    JobBoxLayout->addWidget(Job_System, 0, 1);

    JobBoxLayout->addWidget(new QLabel("Queue"), 0, 2);

    Job_Queue = new QComboBox();
    Job_Queue->addItem("development");
    Job_Queue->addItem("small");
    Job_Queue->addItem("normal");
    Job_Queue->addItem("large");

    JobBoxLayout->addWidget(Job_Queue, 0, 3);

    JobBoxLayout->addWidget(new QLabel("Maximum Run Time (hh:mm:ss)") , 0, 4);


    Job_maxRunTime = new QTimeEdit();
    Job_maxRunTime->setDisplayFormat("hh:mm:ss");
    Job_maxRunTime->setTime(QTime(2, 0, 0));

    JobBoxLayout->addWidget(Job_maxRunTime, 0, 5);

    QPushButton *Submitjob = new QPushButton("Submit Job");
    JobBoxLayout->addWidget(Submitjob, 2, 3, 1, 3);

    // connect the qpushbutton to a create model function
    connect(Submitjob, &QPushButton::clicked, [=]() {
        // create the model
        this->submitJob();
    });

    JobUUID = new QLineEdit();
    JobBoxLayout->addWidget(new QLabel("Job UUID"), 3, 0);
    JobBoxLayout->addWidget(JobUUID, 3, 1, 1, 2);

    QPushButton *GetResults = new QPushButton("Get Results");
    JobBoxLayout->addWidget(GetResults, 3, 3, 1, 3);

    // connect the qpushbutton to a create model function
    connect(GetResults, &QPushButton::clicked, [=]() {
        // check that the user is logged in
        if (!loggedIn) {
            this->get_credentials(tapisUsername, tapisPassword);
            bool status =the_DesignSafe_Storage->login(tapisUsername, tapisPassword);
            if (!status) {
                errorMessage("Login Failed!");
                loggedIn = false;
                return;
            } else {
                loggedIn = true;
            }
        }

        // get the job results
        statusMessage("Getting the job results");
        QString jobUUID = JobUUID->text();
        QJsonObject res = the_DesignSafe_Storage->getJobDetails(jobUUID);
        if (res.isEmpty()) {
            errorMessage("Job UUID is not correct");
            return;
        }
        QString savedir = res["archiveSystemDir"].toString();
        if (res["status"].toString() != "FINISHED") {
            errorMessage("Job Failed or not finished yet");
            errorMessage("Please check the job status");
            return;
        }
        bool ACCfound = false;
        bool VELfound = false;
        bool DISPfound = false;
        // list the files in the directory
        QStringList files = getfilelist(savedir);
        for (int i = 0; i < files.size(); i++) {
            // split the file name and get the last element
            QStringList parts = files[i].split("/");
            QString fileName = parts[parts.size() - 1];
            // Check if the file starts with Accel*
            if (fileName.startsWith("Accel") && ACCfound == false) {
                // download the file
                QString localdir = outputDir->getDirName() + QDir::separator() + "Results/ACC.txt";
                the_DesignSafe_Storage->downloadFile(files[i], localdir);
                ACCfound = true;
            }
            if (fileName.startsWith("Vel") && VELfound == false) {
                // download the file
                QString localdir = outputDir->getDirName() + QDir::separator() + "Results/VEL.txt";
                the_DesignSafe_Storage->downloadFile(files[i], localdir);
                VELfound = true;
            }
            if (fileName.startsWith("Disp") && DISPfound == false) {
                // download the file
                QString localdir = outputDir->getDirName() + QDir::separator() + "Results/DISP.txt";
                the_DesignSafe_Storage->downloadFile(files[i], localdir);
                DISPfound = true;
            }
        }

        //  read the downloaded files which is has 4 columns time accx accy accz and make it a json file
        QFile file(outputDir->getDirName() + QDir::separator() + "Results/ACC.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            errorMessage("Could not open the file");
            return;
        }
        QTextStream in(&file);
        QJsonArray acc_x;
        QJsonArray acc_y;
        QJsonArray acc_z;
        QJsonArray time;
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(" ");
            time.append(parts[0].toDouble()/9.81);
            acc_x.append(parts[1].toDouble()/9.81);
            acc_y.append(parts[2].toDouble()/9.81);
            acc_z.append(parts[3].toDouble()/9.81);
        }
        QJsonObject accObject;
        accObject["unit"] = "g";
        accObject["type"] = "acceleration";
        accObject["data"] = "Time histroy of the acceleration from DRM model";
        accObject["name"] = "DRM Model Acceleration";
        accObject["time"] = time;
        accObject["acc_x"] = acc_x;
        accObject["acc_y"] = acc_y;
        accObject["acc_z"] = acc_z;

        // write the json object to a file
        QFile jsonfile(outputDir->getDirName() + QDir::separator() + "Results/Acceleration.json");
        if (!jsonfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            errorMessage("Could not open the file");
            return;
        }
        QTextStream out(&jsonfile);
        out << QJsonDocument(accObject).toJson();
        jsonfile.close();
        // delete the txt file
        file.remove();
        file.close();

        // do the same for the velocity and displacement
        QFile file2(outputDir->getDirName() + QDir::separator() + "Results/VEL.txt");
        if (!file2.open(QIODevice::ReadOnly | QIODevice::Text)) {
            errorMessage("Could not open the file");
            return;
        }
        QTextStream in2(&file2);
        QJsonArray vel_x;
        QJsonArray vel_y;
        QJsonArray vel_z;
        while (!in2.atEnd()) {
            QString line = in2.readLine();
            QStringList parts = line.split(" ");
            vel_x.append(parts[1].toDouble());
            vel_y.append(parts[2].toDouble());
            vel_z.append(parts[3].toDouble());
        }
        QJsonObject velObject;
        velObject["unit"] = "m/s";
        velObject["type"] = "velocity";
        velObject["data"] = "Time histroy of the velocity from DRM model";
        velObject["name"] = "DRM Model Velocity";
        velObject["time"] = time;
        velObject["vel_x"] = vel_x;
        velObject["vel_y"] = vel_y;
        velObject["vel_z"] = vel_z;

        // write the json object to a file
        QFile jsonfile2(outputDir->getDirName() + QDir::separator() + "Results/Velocity.json");
        if (!jsonfile2.open(QIODevice::WriteOnly | QIODevice::Text)) {
            errorMessage("Could not open the file");
            return;
        }
        QTextStream out2(&jsonfile2);
        out2 << QJsonDocument(velObject).toJson();
        jsonfile2.close();
        // delete the txt file
        file2.remove();
        file2.close();

        // do the same for the displacement
        QFile file3(outputDir->getDirName() + QDir::separator() + "Results/DISP.txt");
        if (!file3.open(QIODevice::ReadOnly | QIODevice::Text)) {
            errorMessage("Could not open the file");
            return;
        }
        QTextStream in3(&file3);
        QJsonArray disp_x;
        QJsonArray disp_y;
        QJsonArray disp_z;
        while (!in3.atEnd()) {
            QString line = in3.readLine();
            QStringList parts = line.split(" ");
            disp_x.append(parts[1].toDouble());
            disp_y.append(parts[2].toDouble());
            disp_z.append(parts[3].toDouble());
        }
        QJsonObject dispObject;
        dispObject["unit"] = "m";
        dispObject["type"] = "displacement";
        dispObject["data"] = "Time histroy of the displacement from DRM model";
        dispObject["name"] = "DRM Model Displacement";
        dispObject["time"] = time;
        dispObject["disp_x"] = disp_x;
        dispObject["disp_y"] = disp_y;
        dispObject["disp_z"] = disp_z;

        // write the json object to a file
        QFile jsonfile3(outputDir->getDirName() + QDir::separator() + "Results/Displacement.json");
        if (!jsonfile3.open(QIODevice::WriteOnly | QIODevice::Text)) {
            errorMessage("Could not open the file");
            return;
        }
        QTextStream out3(&jsonfile3);
        out3 << QJsonDocument(dispObject).toJson();
        jsonfile3.close();
        // delete the txt file
        file3.remove();
        file3.close();

        statusMessage("Job Results are downloaded");
        statusMessage("Please check the Results folder in the choosen output directory");



        
        // get the job results
    });


    

    // ================================================================
    // right widget
    // ================================================================
    // view an emty page
    // view->setUrl(QUrl("about:blank"));
    // rightLayout->addWidget(view);
    // add tabs to the right widget
    QTabWidget *tabWidget = new QTabWidget();
    rightLayout->addWidget(tabWidget);

    // add a web view to the tab widget
    TotalModelView = new QWebEngineView();
    TotalModelView->setUrl(QUrl("about:blank"));
    tabWidget->addTab(TotalModelView, "Total Model");

    SoilLayerView = new QWebEngineView();
    SoilLayerView->setUrl(QUrl("about:blank"));
    tabWidget->addTab(SoilLayerView, "Soil Layer");

    DRMLayerView = new QWebEngineView();
    DRMLayerView->setUrl(QUrl("about:blank"));
    tabWidget->addTab(DRMLayerView, "DRM Layer");

    AbsorbingLayerView = new QWebEngineView();
    AbsorbingLayerView->setUrl(QUrl("about:blank"));
    tabWidget->addTab(AbsorbingLayerView, "Absorbing Layer");

    PartitionesView = new QWebEngineView();
    PartitionesView->setUrl(QUrl("about:blank"));
    tabWidget->addTab(PartitionesView, "Partitions");




    // ========================================================================

    // adding soil properties to the layout
    leftLayout->addWidget(SoilProperties);
    leftLayout->addWidget(DRMBox);
    leftLayout->addWidget(AbsorbingBox);
    leftLayout->addWidget(PartitionesBox);
    leftLayout->addWidget(AnalysisBox);
    leftLayout->addWidget(JobBox);


    // make the left widget 1/8 of the right widget
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 7);

}

DRM_Model::~DRM_Model()
{
}

bool DRM_Model::outputToJSON(QJsonObject &rvObject)
{
    return true;
}

bool DRM_Model::inputFromJSON(QJsonObject &rvObject)
{
    return true;
}

bool DRM_Model::outputAppDataToJSON(QJsonObject &jsonObject)
{
    return true;
}

bool DRM_Model::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    return true;
}

bool DRM_Model::copyFiles(QString &dirName)
{
    return true;
}

void DRM_Model::createModel() {
    // creating the args to pass to the python script
    QStringList args;

    // check that soil width is not empty
    if (soilwidth_x->text().isEmpty() || soilwidth_y->text().isEmpty() || soilwidth_z->text().isEmpty()) {
        errorMessage("Soil width in x, y, and z directions should not be empty");
        return;
    }
    if (soilNumEle_x->text().isEmpty() || soilNumEle_y->text().isEmpty() || soilNumEle_z->text().isEmpty()) {
        errorMessage("Number of elements in x, y, and z directions should not be empty");
        return;
    }
    if (Vs->text().isEmpty() || nu->text().isEmpty() || rho->text().isEmpty()) {
        errorMessage("Vs, nu, and rho should not be empty");
        return;
    }
    if (DRM_filePath->text().isEmpty()) {
        errorMessage("DRM file path should not be empty");
        return;
    }
    if (DRM_numLayers->text().isEmpty() || DRM_laodFactor->text().isEmpty() || DRM_crdScale->text().isEmpty() || DRM_tolernace->text().isEmpty()) {
        errorMessage("DRM number of layers, load factor, coordinate scale, and tolerance should not be empty");
        return;
    }
    if (DRM_T00->text().isEmpty() || DRM_T01->text().isEmpty() || DRM_T02->text().isEmpty() || DRM_T10->text().isEmpty() || DRM_T11->text().isEmpty() || DRM_T12->text().isEmpty() || DRM_T20->text().isEmpty() || DRM_T21->text().isEmpty() || DRM_T22->text().isEmpty()) {
        errorMessage("DRM transformation matrix should not be empty");
        return;
    }
    if (DRM_originX->text().isEmpty() || DRM_originY->text().isEmpty() || DRM_originZ->text().isEmpty()) {
        errorMessage("DRM origin should not be empty");
        return;
    }

    // check if the drm file exists
    if (DRM_Location->currentText() == "Local") {
        if (!QFile::exists(DRM_filePath->text())) {
            errorMessage("DRM file does not exist");
            return;
        }
    }


    args << "--soilwidth_x" << soilwidth_x->text();
    args << "--soilwidth_y" << soilwidth_y->text();
    args << "--soilwidth_z" << soilwidth_z->text();
    args << "--soilNumEle_x" << soilNumEle_x->text();
    args << "--soilNumEle_y" << soilNumEle_y->text();
    args << "--soilNumEle_z" << soilNumEle_z->text();
    args << "--Vs" << Vs->text();
    args << "--nu" << nu->text();
    args << "--rho" << rho->text();
    args << "--DRM_filePath" << DRM_filePath->text();
    args << "--DRM_numLayers" << DRM_numLayers->text();
    args << "--DRM_loadFactor" << DRM_laodFactor->text();
    args << "--DRM_crdScale" << DRM_crdScale->text();
    args << "--DRM_tolerance" << DRM_tolernace->text();

    if (DRM_CoordinateTransformation->isChecked()) {
        args << "--DRM_T00" << DRM_T00->text();
        args << "--DRM_T01" << DRM_T01->text();
        args << "--DRM_T02" << DRM_T02->text();
        args << "--DRM_T10" << DRM_T10->text();
        args << "--DRM_T11" << DRM_T11->text();
        args << "--DRM_T12" << DRM_T12->text();
        args << "--DRM_T20" << DRM_T20->text();
        args << "--DRM_T21" << DRM_T21->text();
        args << "--DRM_T22" << DRM_T22->text();
        args << "--DRM_originX" << DRM_originX->text();
        args << "--DRM_originY" << DRM_originY->text();
        args << "--DRM_originZ" << DRM_originZ->text();
    } else {
        args << "--DRM_T00" <<"1.0";
        args << "--DRM_T01" <<"0.0";
        args << "--DRM_T02" <<"0.0";
        args << "--DRM_T10" <<"0.0";
        args << "--DRM_T11" <<"1.0";
        args << "--DRM_T12" <<"0.0";
        args << "--DRM_T20" <<"0.0";
        args << "--DRM_T21" <<"0.0";
        args << "--DRM_T22" <<"1.0";
        args << "--DRM_originX" <<"0.0";
        args << "--DRM_originY" <<"0.0";
        args << "--DRM_originZ" <<"0.0";
    }
    args << "--DRM_Software" << DRM_Sofrware->currentText();
    args << "--DRM_Location" << DRM_Location->currentText();



    if (Absorb_HaveAbsorbingElements->isChecked()) {
        args << "--Absorb_HaveAbsorbingElements" << "YES";
    } else {
        args << "--Absorb_HaveAbsorbingElements" << "NO";
    }

    // check that the number of absorbing elements is not empty
    if (Absorb_NumAbsorbingElements->text().isEmpty()) {
        errorMessage("Number of absorbing elements should not be empty");
        return;
    }

    args << "--Absorb_NumAbsorbingElements" << Absorb_NumAbsorbingElements->text();
    if (Absorb_type->currentText() == "Perfectly Matched Layer") {
        args << "--Absorb_type" << "PML";
        args << "--Absorb_rayleighAlpha" << "0.0";
        args << "--Absorb_rayleighBeta" << "0.0";
    } else if (Absorb_type->currentText() == "ASDA Absorbing Elements") {
        args << "--Absorb_type" << "ASDA";
        args << "--Absorb_rayleighAlpha" << "0.0";
        args << "--Absorb_rayleighBeta" << "0.0";
    } else { 
        //check that the rayleigh alpha and beta are not empty
        if (Absorb_rayleighAlpha->text().isEmpty() || Absorb_rayleighBeta->text().isEmpty()) {
            errorMessage("Rayleigh alpha and beta should not be empty");
            return;
        }
        args << "--Absorb_type" << "Rayleigh";
        args << "--Absorb_rayleighAlpha" << Absorb_rayleighAlpha->text();
        args << "--Absorb_rayleighBeta" << Absorb_rayleighBeta->text();
    }
        
    args << "--dt" << dt->text();
    args << "--t_final" << t_final->text();
    args << "--recording_dt" << recording_dt->text();
    args << "--outputDir" << outputDir->getDirName();

    if (partitioningAlgorithm->currentText() == "Kd-Tree") {
        args << "--PartitionAlgorithm" << "kd-tree";
        // errorMessage("Kd-Tree is not supported yet");
    } else {
        args << "--PartitionAlgorithm" << "metis";
    }
    args <<"--soilcores" << numSoilPartitions->text();
    args <<"--drmcores" << numDRMPartitions->text();
    args <<"--absorbingcores" << numAbsorbingPartitions->text();





    statusMessage("Creating the model ...");
    // create the python script
    QString appDir = SimCenterPreferences::getInstance()->getAppDir() + QDir::separator() + "applications" + QDir::separator() + "tools" + QDir::separator() + "DRM";
    QString runScript = appDir + QDir::separator() + "modelCreator.py";

    // create a new process to run the job
    RunPythonInThread *thePythonProcess = new RunPythonInThread(runScript, args, appDir);
    QEventLoop loop;
    // Connect the processFinished signal to the loop's quit slot
    connect(thePythonProcess, &RunPythonInThread::processFinished, &loop, &QEventLoop::quit);
    // Start the process
    thePythonProcess->runProcess();
    // wait for the process to finish
    loop.exec();

    statusMessage("Model creation is finished");



    // load the meshplots in the web view
    QString destDir = outputDir->getDirName() + QDir::separator() + "meshplots" + QDir::separator();
    QString htmlFile1 = destDir +  "DRM.html";
    QString htmlFile2 = destDir +  "Regular.html";
    QString htmlFile3 = destDir +  "PML.html";
    QString htmlFile4 = destDir +  "Total_domain.html";
    QString htmlFile5 = destDir +  "Total_partitioned.html";
    

    QUrl url;
    url = QUrl::fromLocalFile(htmlFile1);
    DRMLayerView->setUrl(url);

    url = QUrl::fromLocalFile(htmlFile2);
    SoilLayerView->setUrl(url);

    url = QUrl::fromLocalFile(htmlFile3);
    AbsorbingLayerView->setUrl(url);

    url = QUrl::fromLocalFile(htmlFile4);
    TotalModelView->setUrl(url);

    url = QUrl::fromLocalFile(htmlFile5);
    PartitionesView->setUrl(url);

}



void DRM_Model::get_credentials(QString &username, QString &password) {
    // pop up a window to ask for username and password
    QDialog *dialog = new QDialog();
    dialog->setWindowTitle("Login to Tapis");
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    QLineEdit *usernameEdit = new QLineEdit(dialog);
    usernameEdit->setPlaceholderText("Username");
    layout->addWidget(usernameEdit);
    QLineEdit *passwordEdit = new QLineEdit(dialog);
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(passwordEdit);
    QPushButton *loginButton = new QPushButton("Login", dialog);
    layout->addWidget(loginButton);
    dialog->setLayout(layout);
    connect(loginButton, &QPushButton::clicked, [=]() {
        // check if the username and password are not empty
        if (usernameEdit->text().isEmpty() || passwordEdit->text().isEmpty()) {
            errorMessage("Error: Username and password cannot be empty");
            return;
        }
        // close the dialog
        dialog->accept();

    });
    // make the dialog appear in the center of the screen i nstead of the top left corner
    // keep the dialog dimensions the same
    QScreen *screen = qApp->primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int x = (screenGeometry.width() - dialog->width()) / 2;
    int y = (screenGeometry.height() - dialog->height()) / 2;
    dialog->move(x, y);
    dialog->exec();
    // save the username and password
    username = usernameEdit->text();
    password = passwordEdit->text();
}

QStringList DRM_Model::getfilelist(QString &rootDir) {
    if (tapisUsername.isEmpty() || tapisPassword.isEmpty()) {
        this->get_credentials(tapisUsername, tapisPassword);
    }
    bool status =the_DesignSafe_Storage->login(tapisUsername, tapisPassword);
    QJsonObject jsonObject = the_DesignSafe_Storage->getFilesList(rootDir);
    // create stringlist from the json object
    QStringList filesList;
    QJsonArray jsonArray = jsonObject["files"].toArray();

    for (int i = 0; i < jsonArray.size(); i++) {
        QJsonObject obj = jsonArray[i].toObject();
        QString path = obj["path"].toString();
        filesList << path;
    }

    return filesList;
}





void 
DRM_Model::submitJob() {
    bool status;
    if (!loggedIn) {
        this->get_credentials(tapisUsername, tapisPassword);
        status =the_DesignSafe_Storage->login(tapisUsername, tapisPassword);
        if (!status) {
            errorMessage("Login Failed!");
            loggedIn = false;
            return;
        } else {
            loggedIn = true;
        }
    }

    // check that the output directory is not empty
    this->createModel();

    status = the_DesignSafe_Storage->mkdir(QString("DRM_Model"), QString(tapisUsername));
    // if (!status) {
    //     errorMessage("Creating the directory failed!");
    // } else {
    //     errorMessage("Creating the directory is finished");
    // }

    // remove the directory if it exists
    statusMessage("Removing the directory ...");
    status = the_DesignSafe_Storage->removeDirectory(QString(tapisUsername + "/DRM_Model"));
    if (!status) {
        errorMessage("Removing the directory failed!");
    } else {
        statusMessage("Removing the directory is finished");
    }


    statusMessage("Uploading the files ...");
    // upload the files to the DesignSafe
    QString destDir  = tapisUsername + "/DRM_Model"; 

    QString localdir = outputDir->getDirName() + QDir::separator() + "Mesh/";
    status = the_DesignSafe_Storage->uploadDirectory(localdir, destDir);

    if (!status) {
        errorMessage("Upload files failed!");
    } 

    localdir = outputDir->getDirName() + QDir::separator() + "Results/";
    destDir = tapisUsername + "/DRM_Model";
    status = the_DesignSafe_Storage->uploadDirectory(localdir, destDir);

    if (!status) {
        errorMessage("Upload files failed!");
    } else {
        statusMessage("Upload files is finished");
    }

    // job = {
    //     "name":"DRM_Model-frontera",
    //     "appId":"simcenter-opensees-drm-frontera_amnp95",
    //     "appVersion":"1.0.0",
    //     "execSystemLogicalQueue":"development",
    //     "execSystemExecDir":"${JobWorkingDir}/Mesh",
    //     "execSystemInputDir":"${JobWorkingDir}/Mesh",
    //     "execSystemOutputDir":"${JobWorkingDir}/Results/",
    //     "fileInputs":[
    //     {
    //         "envKey":"inputDirectory",
    //         "sourceUrl":"tapis://designsafe.storage.default/amnp95/DRM_Model/Mesh",
    //         "targetPath": "*"
    //     },
    //     {
    //         "envKey":"drmInput",
    //         "sourceUrl":"tapis://designsafe.storage.default/amnp95/tapis-jobs-archive/EEUQ_DesignSafe_amnp95_2024_09_19_23_17_40/DRMLoad.h5drm",
    //         "targetPath": "*"
    //     }
        
    //     ],
    //     "maxMinutes":5,
    //     "memoryMB":1000,
    //     "parameterSet":{
    //     "schedulerOptions": [
    //         { "arg": "-A DesignSafe-SimCenter" }
    //     ],
    //     "envVariables": [
    //         {
    //         "key":"inputScript",
    //         "value":"model.tcl"
    //         },
    //         {
    //         "key":"openSeesExecutable",
    //         "value":"OpenSeesMP"
    //         },	    
    //         {
    //         "key":"openSeesVersion",
    //         "value":"v3.6.0"
    //         }
    //     ]
    //     },
    //     "nodeCount":1,
    //     "coresPerNode":56
    // }

    // create QJsonObject to hold the job information

 

    QString day = QDateTime::currentDateTime().toString("yyyy_MM_dd");
    QString time = QDateTime::currentDateTime().toString("HH_mm_ss");
    QString jobname = "EEUQ_DRMModel_" + tapisUsername + "_" + day + "_" + time;
    QString archivepath = tapisUsername + "/tapis-jobs-archive/" + jobname +"/${JobUUID}"; 



    QJsonObject jobObject;
    jobObject["name"] = jobname;
    jobObject["appId"] = "simcenter-opensees-frontera";
    jobObject["appVersion"] = "1.0.0";
    jobObject["execSystemLogicalQueue"] = Job_Queue->currentText();
    jobObject["execSystemExecDir"] = "${JobWorkingDir}";
    jobObject["execSystemInputDir"] = "${JobWorkingDir}";
    jobObject["execSystemOutputDir"] = "${JobWorkingDir}/Results/";
    jobObject["archiveSystemDir"] = archivepath;
    
    QJsonArray fileInputs;
    QJsonObject fileInput1;
    fileInput1["envKey"] = "inputDirectory";
    fileInput1["sourceUrl"] = "tapis://designsafe.storage.default/" + tapisUsername + "/DRM_Model/Mesh";
    fileInput1["targetPath"] = "*";
    fileInputs.append(fileInput1);

    QJsonObject fileInput2;
    fileInput2["envKey"] = "drmInput";
    QString drmFile = DRM_filePath->text();
    fileInput2["sourceUrl"] = "tapis://designsafe.storage.default/" + drmFile ;
    fileInput2["targetPath"] = "*";

    fileInputs.append(fileInput2);

    jobObject["fileInputs"] = fileInputs;

    int maxMinutes = Job_maxRunTime->time().hour() * 60 + Job_maxRunTime->time().minute();
    jobObject["maxMinutes"] = maxMinutes;
    jobObject["memoryMB"] = 100000;

    QJsonObject parameterSet;
    QJsonArray schedulerOptions;
    QJsonObject schedulerOption;
    schedulerOption["arg"] = "-A DesignSafe-SimCenter";
    schedulerOptions.append(schedulerOption);
    parameterSet["schedulerOptions"] = schedulerOptions;

    QJsonArray envVariables;
    QJsonObject envVariable1;
    envVariable1["key"] = "inputScript";
    envVariable1["value"] = "model.tcl";
    envVariables.append(envVariable1);

    QJsonObject envVariable2;
    envVariable2["key"] = "openSeesExecutable";
    envVariable2["value"] = "OpenSeesMP";
    envVariables.append(envVariable2);

    QJsonObject envVariable3;
    envVariable3["key"] = "openSeesVersion";
    envVariable3["value"] = "v3.6.0";
    envVariables.append(envVariable3);

    parameterSet["envVariables"] = envVariables;
    jobObject["parameterSet"] = parameterSet;

    jobObject["nodeCount"] = 1;
    
    if (Absorb_HaveAbsorbingElements->isChecked()) {
        int totalCores = numSoilPartitions->text().toInt() + numDRMPartitions->text().toInt() + numAbsorbingPartitions->text().toInt();
        if (Job_System->currentText() == "frontera") {
            int numnodes = totalCores / 56;
            int coresPerNode = totalCores / (numnodes + 1);
            jobObject["nodeCount"] = numnodes;
            jobObject["coresPerNode"] = coresPerNode;
        } 
        if (Job_System->currentText() == "stampede3") {
            errorMessage("stampede3 is not supported yet");
        }
    }

    // submit the job
    statusMessage("Submitting the job ...");
    QString res = the_DesignSafe_Storage->startJob(jobObject);
    statusMessage("Job is submitted");
    statusMessage("The job id is: " + res);
    JobUUID->setText(res);
}