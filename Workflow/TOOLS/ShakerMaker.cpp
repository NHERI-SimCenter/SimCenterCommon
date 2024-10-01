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
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

#include "ShakerMaker.h"
#include <SC_DoubleLineEdit.h>
#include <SC_IntLineEdit.h>
#include <SC_DirEdit.h>
#include <SC_ComboBox.h>
#include <SC_CheckBox.h>
#include <QTableWidget>
#include <QEventLoop>
#include <QJsonObject>
#include <QDir>
#include <QDebug>
#include <QPushButton>
#include <QMessageBox>
#include <QGroupBox>
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QSplitter>
#include <QProcess>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QStringList>
#include <SimCenterPreferences.h>
#include <ModularPython.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QComboBox>
#include <QHeaderView>
#include <RunPythonInThread.h>
#include <QPixmap>
#include <QFileDialog>
#include <QNetworkReply>
#include <QNetworkAccessManager>


  
ShakerMaker::ShakerMaker(): SimCenterAppWidget()
{

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


    // ============================================================
    // left side
    // ============================================================
    // create left widget
    QWidget *leftWidget = new QWidget();
    QGridLayout *leftLayout = new QGridLayout(leftWidget);
    leftLayout->setAlignment(Qt::AlignTop);

    // ==========================
    // Analysis Information
    // ==========================

    QGroupBox *AnalysisGroupBox = new QGroupBox("Analysis Information");
    QGridLayout *AnalysisLayout = new QGridLayout(AnalysisGroupBox);

    // set alignment of the group box to left
    AnalysisGroupBox->setAlignment(Qt::AlignLeft);

    // dt
    AnalysisLayout->addWidget(new QLabel("dt"), 0, 0);
    dt = new SC_DoubleLineEdit("Timestep for output dataset",0.005);
    AnalysisLayout->addWidget(dt, 0, 1);
    AnalysisLayout->addWidget(new QLabel("Timestep for ouput dataset"), 0, 2);

    // nfft
    AnalysisLayout->addWidget(new QLabel("nfft"), 1, 0);
    nfft = new SC_IntLineEdit("nfft", 2048);
    AnalysisLayout->addWidget(nfft, 1, 1);
    AnalysisLayout->addWidget(new QLabel("Number of samples (need power of 2)"), 1, 2);

    // dk
    AnalysisLayout->addWidget(new QLabel("dk"), 2, 0);
    dk = new SC_DoubleLineEdit("dk", 0.2);
    AnalysisLayout->addWidget(dk, 2, 1);
    AnalysisLayout->addWidget(new QLabel("Wavelength discretization"), 2, 2);

    // tmin
    AnalysisLayout->addWidget(new QLabel("tmin"), 3, 0);
    tmin = new SC_DoubleLineEdit("tmin", 0.0);
    AnalysisLayout->addWidget(tmin, 3, 1);
    AnalysisLayout->addWidget(new QLabel("Start time for simulation"), 3, 2);

    // tmax
    AnalysisLayout->addWidget(new QLabel("tmax"), 4, 0);
    tmax = new SC_DoubleLineEdit("tmax", 60.0);
    AnalysisLayout->addWidget(tmax, 4, 1);
    AnalysisLayout->addWidget(new QLabel("End time for simulation (must be contained in nfft window)"), 4, 2);


    //delta_h
    AnalysisLayout->addWidget(new QLabel("dh (m)"), 5, 0);
    delta_h = new SC_DoubleLineEdit("delta_h", 40.0);
    AnalysisLayout->addWidget(delta_h, 5, 1);
    AnalysisLayout->addWidget(new QLabel("Horizontal distance criteria for database creation"), 5, 2);

    // delta_v_rec
    AnalysisLayout->addWidget(new QLabel("dv_rec (m)"), 6, 0);
    delta_v_rec = new SC_DoubleLineEdit("delta_v_rec", 5.0);
    AnalysisLayout->addWidget(delta_v_rec, 6, 1);
    AnalysisLayout->addWidget(new QLabel("Vertical distance criteria for receiver points in database"), 6, 2);

    // delta_v_src
    AnalysisLayout->addWidget(new QLabel("dv_src (m)"), 7, 0);
    delta_v_src = new SC_DoubleLineEdit("delta_v_src", 200.0);
    AnalysisLayout->addWidget(delta_v_src, 7, 1);
    AnalysisLayout->addWidget(new QLabel("Vertical distance criteria for source points in database"), 7, 2);


    // tmp directory
    QString dirPath = SimCenterPreferences::getInstance()->getLocalWorkDir() + QDir::separator() + "ShakerMaker";
    AnalysisLayout->addWidget(new QLabel("tmp Directory"),8,0);  
    tmpLocation = new SC_DirEdit("tmpLocation");
    tmpLocation->setDirName(dirPath);
    AnalysisLayout->addWidget(tmpLocation,8,1,1,2);




    // ==========================
    // Station
    // ==========================
    QGroupBox *StationGroupBox = new QGroupBox("Station Information");
    QGridLayout *StationLayout = new QGridLayout(StationGroupBox);

    QLabel *label=new QLabel();
    label->setMinimumWidth(250);
    label->setText(QString("Station Type"));

    stationType = new QComboBox();
    stationType->setObjectName("StationTypeCombox");
    stationType->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    stationType->addItem(tr("Single Station"));
    stationType->addItem(tr("DRM Box"));

    StationLayout->addWidget(stationType);

    StationStackedWidget = new QStackedWidget();
    StationStackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    StationLayout->addWidget(StationStackedWidget,2,0);

    // Adding Single Station
    StationStackedWidget->addWidget(createSingleStationWidget());


    // Adding DRM Box
    StationStackedWidget->addWidget(createDRMBoxWidget());

    connect(stationType,SIGNAL(currentIndexChanged(QString)),this,SLOT(stationTypeChanged(QString)));



    // ==================================
    // job submission
    // ===================================
    // submit job Gropu Box


    QGroupBox *Job_Visualization = new QGroupBox("Job and Visualization");
    QGroupBox *viulizationGroupBox = new QGroupBox("Visualization Options");
    QGroupBox *submitJobGroupBox = new QGroupBox("Submit Job");

    // devide job_Visualization into left and right and add boxes to those
    QGridLayout *jobVisualizationLayout = new QGridLayout(Job_Visualization);
    jobVisualizationLayout->addWidget(submitJobGroupBox, 0, 0);
    jobVisualizationLayout->addWidget(viulizationGroupBox, 0, 1);

    // make the columns widdth equal
    jobVisualizationLayout->setColumnStretch(0, 2);
    jobVisualizationLayout->setColumnStretch(1, 1);



    // create visualization layout
    QGridLayout *submitJobLayout = new QGridLayout(submitJobGroupBox);
    submitJobLayout->setAlignment(Qt::AlignTop);


    // make the columns widdth equal
    submitJobLayout->setColumnStretch(0, 1);
    submitJobLayout->setColumnStretch(1, 1);
    submitJobLayout->setColumnStretch(2, 1);
    submitJobLayout->setColumnStretch(3, 1);



    //  system selection could be frontera or stampede3
    system = new QComboBox();
    system->addItem("Frontera");
    system->addItem("Stampede3");
    submitJobLayout->addWidget(new QLabel("System"), 0, 0);
    submitJobLayout->addWidget(system, 0, 1, 1, 3);

    // Queue selection could be developmnet, small, normal, large
    queue = new QComboBox();
    queue->addItem("development");
    queue->addItem("small");
    queue->addItem("normal");
    queue->addItem("large");
    submitJobLayout->addWidget(new QLabel("Queue"), 1, 0);
    submitJobLayout->addWidget(queue, 1, 1, 1, 3);

    // number of nodes
    submitJobLayout->addWidget(new QLabel("Number of Nodes"), 2, 0);
    numNodes = new SC_IntLineEdit("Number of Nodes", 1);
    submitJobLayout->addWidget(numNodes, 2, 1);

    // cores per node
    submitJobLayout->addWidget(new QLabel("Cores per Node"), 2, 2);
    coresPerNode = new SC_IntLineEdit("Cores per Node", 56);
    submitJobLayout->addWidget(coresPerNode, 2, 3);


    // maximum run time
    submitJobLayout->addWidget(new QLabel("Max Run Time (hh:mm:ss)"), 3, 0);
    // maxRunTime = new QLineEdit("2:00:00");
    // submitJobLayout->addWidget(maxRunTime, 3, 1);

    maxRunTime = new QTimeEdit();
    maxRunTime->setDisplayFormat("hh:mm:ss"); // Set the desired format
    maxRunTime->setTime(QTime(2, 0, 0));
    submitJobLayout->addWidget(maxRunTime, 3, 1);
    // minum run time is 1 hour
    maxRunTime->setMinimumTime(QTime(1, 0, 0));

    // add group box to the last two columns of the second row
    // visualization options
    QGridLayout *visualizationLayout = new QGridLayout(viulizationGroupBox);
    visualizationLayout->addWidget(new QLabel("Show Stations"), 0, 0);
    viewStations = new SC_CheckBox("Visualize Stations", false);
    visualizationLayout->addWidget(viewStations, 0, 1);
    visualizationLayout->addWidget(new QLabel("Show Crust Layers"), 1, 0);
    viewCrustLayers = new SC_CheckBox("Visualize Crust Layers", false);
    visualizationLayout->addWidget(viewCrustLayers, 1, 1);
    


    // visualization button
    // the height of the button is row span of 2
    QPushButton *visualizeButton = new QPushButton("Visualize");
    // make the visualization button bigger
    // visualizeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    visualizationLayout->addWidget(visualizeButton, 2, 0, 1, 2);

    QPushButton *LoadModelButton = new QPushButton("Load Model");
    visualizationLayout->addWidget(LoadModelButton, 3, 0, 1, 2);

    // pop up a window to load the model
    connect(LoadModelButton, &QPushButton::clicked, [=](){
        QString filename = QFileDialog::getOpenFileName(this, tr("Open Model"), "", tr("All Files (*)"));
        // load the model
        loadModel(filename);
        // sourcefilejson->setText(filename);
    });





    // submit button to a whole row
    QPushButton *submitButton = new QPushButton("Submit Job");
    submitJobLayout->addWidget(submitButton, 3, 2, 1, 2);

    // connect the submit button to the submit function
    connect(submitButton, &QPushButton::clicked, this, &ShakerMaker::runJob );

    // make the rows of the submit job layout align with the rows of the visualization layout
    submitJobLayout->setRowStretch(0, 1);
    submitJobLayout->setRowStretch(1, 1);
    submitJobLayout->setRowStretch(2, 1);
    submitJobLayout->setRowStretch(3, 1);
    
    visualizationLayout->setRowStretch(0, 1);
    visualizationLayout->setRowStretch(1, 1);
    visualizationLayout->setRowStretch(2, 1);
    visualizationLayout->setRowStretch(3, 1);



    // connect the visulaize button to the visualize function
    connect(visualizeButton, &QPushButton::clicked, this, &ShakerMaker::Visualize);

    // ========================
    // material layers
    // ========================
    // material layers
    QWidget* MaterialLayersGroupBox = createMaterialLayerbox();


    // ========================
    // fault information
    // ========================
    // add the source group box to the layout
    QGroupBox *sourceGroupBox = new QGroupBox("Source Information");
    QGridLayout *sourceLayout = new QGridLayout(sourceGroupBox);

    // bring the third column to the right of the second column
    sourceLayout->setColumnStretch(0, 1);
    sourceLayout->setColumnStretch(1, 1);
    sourceLayout->setColumnStretch(2, 1);
    sourceLayout->setColumnStretch(3, 1);   


    SourcePoints = new QTableWidget();
    SourcePoints->setColumnCount(2);
    SourcePoints->setRowCount(1);

    // expand the table
    SourcePoints->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList labels2;
    labels2 << "File Path" << "";
    SourcePoints->setHorizontalHeaderLabels(labels2);


    // add push button browse tot he last columns
    QPushButton *browseButton = new QPushButton("Browse");
    SourcePoints->setCellWidget(0, 1, browseButton);

    // if i push the browse button it will return the row and column of the button
    connect(browseButton, &QPushButton::clicked, [=](){
        // get the row and column of the button
        int row = SourcePoints->currentRow();
        int column = SourcePoints->currentColumn();
        //  double click the cell 
        SourcePoints->cellDoubleClicked(row, column);
    });


    // add gropu box to the last row
    QGroupBox *loadfromdatabase = new QGroupBox("Load from Database");
    // add layout to the group box
    QGridLayout *databaseLayout = new QGridLayout(loadfromdatabase);
    loadfromdatabase->setLayout(databaseLayout);

    // read the DatabaseMetadata.json and load the countries
    QString destDir = tmpLocation->getDirName();
    QString appDir = SimCenterPreferences::getInstance()->getAppDir();
    QString faultdatabsefile = appDir + QDir::separator() + "applications" + QDir::separator()+ "tools" + QDir::separator()+ "ShakerMaker" + QDir::separator() + "DatabaseMetadata.json";
    
    // // remove the file if it exists
    // QFile::remove(faultdatabsefile);

    // // get the url of the file
    // QString url3 = "https://raw.githubusercontent.com/amnp95/ShakerMakerFaultDatabase/main/DatabaseMetadata.json";
    // // download the file from github
    // QString command3;
    // // find the operating system
    // #ifdef Q_OS_WIN
    //     command3 = "curl.exe -L -o " + faultdatabsefile + " " + url3;
    // #else
    //     command3 = "curl -L -o " + faultdatabsefile + " "  + url3;
    // #endif

    // // download the file
    // QProcess process3;
    // process3.start(command3);

    // // wait for the process to finish
    // process3.waitForFinished();

    // check if the file exists
    if (!QFile::exists(faultdatabsefile)) {
        QMessageBox::warning(this, "Error", "Could not download the database metadata file");
        return;
    }

    // load the json file
    QFile file(faultdatabsefile);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open file for reading";
        return;
    }
    QJsonObject databaseMetadata = QJsonDocument::fromJson(file.readAll()).object();
    file.close();

    // get the countries from the json file which is a list
    QJsonArray contries = databaseMetadata["Countries"].toArray();

    // add the countries to the combo box
    countryComboBox = new QComboBox();
    for (int i = 0; i < contries.size(); i++) {
        countryComboBox->addItem(contries[i].toString());
    }
    databaseLayout->addWidget(new QLabel("Country"), 0, 0);
    databaseLayout->addWidget(countryComboBox, 0, 1);

    // ad the Qcombo box for the faults
    faultComboBox = new QComboBox();
    
    // connect the country combo box to load the faults
    connect(countryComboBox, &QComboBox::currentTextChanged, [=](const QString &country){
        faultComboBox->clear();
        // get the faults for the selected country
        QJsonArray faults = databaseMetadata[country]["Faults"].toArray();
        for (int i = 0; i < faults.size(); i++) {
            faultComboBox->addItem(faults[i].toString());
        }
    });
    databaseLayout->addWidget(new QLabel("Fault"), 0, 2);
    databaseLayout->addWidget(faultComboBox, 0, 3);

    // add the Magnitude combo box to the layout
    magnitudeComboBox = new QComboBox();
    databaseLayout->addWidget(new QLabel("Magnitude"), 1, 0);

    // connect the fault combo box to load the magnitudes
    connect(faultComboBox, &QComboBox::currentTextChanged, [=](const QString &fault){
        magnitudeComboBox->clear();
        // get the magnitudes for the selected fault
        QJsonArray magnitudes = databaseMetadata[countryComboBox->currentText()][fault]["Magnitudes"].toArray();
        for (int i = 0; i < magnitudes.size(); i++) {
            magnitudeComboBox->addItem(magnitudes[i].toString());
        }
    });
    databaseLayout->addWidget(magnitudeComboBox, 1, 1);

    // add the faulttype combo box to the layout
    faultTypeComboBox = new QComboBox();
    databaseLayout->addWidget(new QLabel("Fault Type"), 1, 2);

    // connect the magnitude combo box to load the fault types
    connect(magnitudeComboBox, &QComboBox::currentTextChanged, [=](const QString &magnitude){
        faultTypeComboBox->clear();
        // get the fault types for the selected fault
        QJsonArray faultTypes = databaseMetadata[countryComboBox->currentText()][faultComboBox->currentText()][magnitude]["Types"].toArray();
        for (int i = 0; i < faultTypes.size(); i++) {
            faultTypeComboBox->addItem(faultTypes[i].toString());
        }
    });
    databaseLayout->addWidget(faultTypeComboBox, 1, 3);

    // add the reallization combo box to the layout
    realizationComboBox = new QComboBox();
    databaseLayout->addWidget(new QLabel("Realization"), 2, 0);

    // connect the fault type combo box to load the realizations
    connect(faultTypeComboBox, &QComboBox::currentTextChanged, [=](const QString &faultType){
        realizationComboBox->clear();
        // get the realizations for the selected fault type
        QJsonArray realizations = databaseMetadata[countryComboBox->currentText()][faultComboBox->currentText()][magnitudeComboBox->currentText()][faultType]["Realizations"].toArray();
        for (int i = 0; i < realizations.size(); i++) {
            realizationComboBox->addItem(realizations[i].toString());
        }
    });
    databaseLayout->addWidget(realizationComboBox, 2, 1);

    // send a fake signal to load to the contries combo box to update the faults
    emit countryComboBox->currentTextChanged(countryComboBox->currentText());
    
    // put a load button 
    loaddtabaseButton = new QPushButton("Load Database");
    databaseLayout->addWidget(loaddtabaseButton, 2, 2, 1, 2);
    // connect the load button to the load function
    connect(loaddtabaseButton, &QPushButton::clicked, [=](){
        // get the selected values
        QString country = countryComboBox->currentText();
        QString fault = faultComboBox->currentText();
        QString magnitude = magnitudeComboBox->currentText();
        QString faultType = faultTypeComboBox->currentText();
        QString realization = realizationComboBox->currentText();


        // download the file from github
        // https://github.com/amnp95/ShakerMakerFaultDatabase/blob/main/Chile/Sanramon/M_6.7_type_bl_number_1/faultInfo.json
        // create the url
        QString url = "https://raw.githubusercontent.com/amnp95/ShakerMakerFaultDatabase/main/"
                        + country + "/" + fault + "/M_" + magnitude + "_type_" + faultType + "_number_" + realization + "/faultInfo.json";

        // run a python code to download the file
        
        QString destDir = tmpLocation->getDirName() + QDir::separator() + "fault";

        QDir destinationDirectory(destDir);
        if (!destinationDirectory.exists()) {
            destinationDirectory.mkpath(destDir);
        }

        // delete everything in the destination directory
        QDir dir(destDir);
        for (auto &file : dir.entryList(QDir::Files)) {
            QFile::remove(dir.filePath(file));
        }

        // using curl to download the file
        QProcess *process = new QProcess();
        // see if we are on windows or linux or mac
        QString command;
        // find the operating system
        #ifdef Q_OS_WIN
            command = "curl.exe -L -o " + destDir + QDir::separator() + "faultInfo.json " + url;
        #else
            command = "curl -L -o " + destDir + QDir::separator() + "faultInfo.json " + url;
        #endif
        process->start(command);
        process->waitForFinished();

        // check if the file exists
        QFile file(destDir + QDir::separator() + "faultInfo.json");
        if (!file.exists()) {
            errorMessage("Error: File does not exist");
            return;
        }

        // read the json file to  get the other files to download
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString val = file.readAll();
        file.close();
        // val is dictionary of files
        QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
        QJsonObject obj = doc.object();
        QJsonArray filenames = obj["Faultfilenames"].toArray();
        // download the files
        for (int i = 0; i < filenames.size(); i++) {
            QString filename = filenames[i].toString();
            QString fileurl = "https://raw.githubusercontent.com/amnp95/ShakerMakerFaultDatabase/main/"
                              + country + "/" + fault + "/M_" + magnitude + "_type_" + faultType + "_number_" + realization + "/" + filename;
            // find the operating system
            #ifdef Q_OS_WIN
                command = "curl.exe -L -o " + destDir + QDir::separator() + filename + " " + fileurl;
            #else
                command = "curl -L -o " + destDir + QDir::separator() + filename + " " + fileurl;
            #endif
            process->start(command);
            process->waitForFinished();
        }        

        // download the SourceTimeFunction.py
        QString sourceTimeFunctionUrl = "https://raw.githubusercontent.com/amnp95/ShakerMakerFaultDatabase/main/"
                                          + country + "/" + fault + "/M_" + magnitude + "_type_" + faultType + "_number_" + realization + "/SourceTimeFunction.py";
        #ifdef Q_OS_WIN
            command = "curl.exe -L -o " + destDir + QDir::separator() + "SourceTimeFunction.py " + sourceTimeFunctionUrl;
        #else
            command = "curl -L -o " + destDir + QDir::separator() + "SourceTimeFunction.py " + sourceTimeFunctionUrl;
        #endif
        process->start(command);
        process->waitForFinished();

        // check if the SourceTimeFunction.py exists
        QFile sourceTimeFunctionFile(destDir + QDir::separator() + "SourceTimeFunction.py");
        if (!sourceTimeFunctionFile.exists()) {
            errorMessage("Error: SourceTimeFunction.py does not exist");
            return;
        }

        // empty the SourcePoints table
        int numrows = SourcePoints->rowCount();
        for (int i = 0; i < numrows; i++) {
            SourcePoints->removeRow(0);
        }
        // add the files to the SourcePoints table
        for (int i = 0; i < filenames.size(); i++) {
            SourcePoints->insertRow(i);
            QString filename = filenames[i].toString();
            SourcePoints->setItem(i, 0, new QTableWidgetItem(destDir + QDir::separator() + filename));
            QPushButton *browseButton = new QPushButton("Browse");
            SourcePoints->setCellWidget(i, 1, browseButton);
            // connect the browse button to the file dialog
            connect(browseButton, &QPushButton::clicked, [=](){
                // get the row and column of the button
                int row = SourcePoints->currentRow();
                int column = SourcePoints->currentColumn();
                //  double click the cell 
                SourcePoints->cellDoubleClicked(row, column);
            });
        };
        //read the latitude and longitude from the faultInfo.json
        sourceLatitude->setText(QString::number(obj["latitude"].toDouble()));
        sourceLongitude->setText(QString::number(obj["longitude"].toDouble()));
    });

    sourceLayout->addWidget(loadfromdatabase, 4, 0, 1, 4);
    // double click to open a file dialog
    connect(SourcePoints, &QTableWidget::cellDoubleClicked, [=](int row, int column){
        if (column == 1) {
        
            QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All Files (*)"));
            // set the string to the table without using new QTableWidgetItem
            SourcePoints->setItem(row, 0, new QTableWidgetItem(fileName));
        }    
    });
    


    // adding sourcelattitude and sourcelongitude to the source layout
    sourceLayout->addWidget(new QLabel("Fault Latitude"), 0, 0);
    sourceLatitude = new SC_DoubleLineEdit("Source Latitude", 0.0);
    sourceLayout->addWidget(sourceLatitude, 0, 1);
    sourceLayout->addWidget(new QLabel("Latitude of the Epicenter"), 0, 2);


    sourceLayout->addWidget(new QLabel("Fault Longitude"), 1, 0);
    sourceLongitude = new SC_DoubleLineEdit("Source Longitude", 0.0);
    sourceLayout->addWidget(sourceLongitude, 1, 1);
    sourceLayout->addWidget(new QLabel("Longitude of the Epicenter"), 1, 2);

    
    // add add row button
    QPushButton *addPointButton = new QPushButton("Add  Fault");
    sourceLayout->addWidget(addPointButton, 0, 3);

    // add remove row button
    QPushButton *removePointButton = new QPushButton("Remove Fault");
    sourceLayout->addWidget(removePointButton, 1, 3);


    sourceLayout->addWidget(SourcePoints, 2, 0, 1, 4);




    // connect add row button to the first row
    connect(addPointButton, &QPushButton::clicked, [=](){
        SourcePoints->insertRow(0);
        // add the browse button to the last column
        QPushButton *browseButton = new QPushButton("Browse");
        SourcePoints->setCellWidget(0, 1, browseButton);
        // connect the browse button to the file dialog
        connect(browseButton, &QPushButton::clicked, [=](){
            // get the row and column of the button
            int row = SourcePoints->currentRow();
            int column = SourcePoints->currentColumn();
            //  double click the cell 
            SourcePoints->cellDoubleClicked(row, column);
        });
    });

    // connect remove row button to the first row
    connect(removePointButton, &QPushButton::clicked, [=](){
        SourcePoints->removeRow(0);
    });



    // ====================================================================================
    // right side
    // ====================================================================================
    // create right widget
    QWidget *rightWidget = new QWidget();
    QGridLayout *rightLayout = new QGridLayout(rightWidget);
    rightLayout->setAlignment(Qt::AlignTop);
    QGroupBox *SimulationGroupBox = new QGroupBox("Fault Visualization");
    QGridLayout *SimulationLayout = new QGridLayout(SimulationGroupBox);

    // difernet tabs for the simulation Group Box
    // create a tab widget (map tab and plot tab)
    QTabWidget *tabWidget = new QTabWidget();
    SimulationLayout->addWidget(tabWidget, 0, 0);
    // add the tabs
    QWidget *mapTab = new QWidget();
    QWidget *plotTab = new QWidget();
    tabWidget->addTab(plotTab, "Plot");
    tabWidget->addTab(mapTab, "Map");


    // simulation layout is a web view to load html file in the plot tab
    simulationWebView = new QWebEngineView();
    simulationWebView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    plotTab->setLayout(new QVBoxLayout());
    plotTab->layout()->addWidget(simulationWebView);


    // map layout is a web view to load html file in the map tab
    mapWebView = new QWebEngineView();
    mapWebView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mapTab->setLayout(new QVBoxLayout());
    mapTab->layout()->addWidget(mapWebView);



    simulationWebView->setUrl(QUrl("about:blank"));
    mapWebView->setUrl(QUrl("about:blank"));



    // // simulation layout is a web view to load html file 
    // simulationWebView = new QWebEngineView();
    // simulationWebView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // SimulationLayout->addWidget(simulationWebView, 0, 0);


    rightLayout->addWidget(SimulationGroupBox, 0, 0);
    rightWidget->setLayout(rightLayout);







    


    // add the group box to the left layout
    leftWidget->setMinimumSize(50, 0); // Set the smallest size for left widget
    rightWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    

    leftLayout->addWidget(AnalysisGroupBox);
    leftLayout->addWidget(sourceGroupBox);
    leftLayout->addWidget(MaterialLayersGroupBox);
    leftLayout->addWidget(StationGroupBox);
    leftLayout->addWidget(Job_Visualization);
    mainSplitter->addWidget(leftWidget);
    mainSplitter->addWidget(rightWidget);
    // // make the left widget smallest possible
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 2);


}


ShakerMaker::~ShakerMaker()
{

  
  
  
}

void 
ShakerMaker::reomveMaterialLayer(int row) {
    // first check wat the current row count is not zero
    int rowCount = materialLayers->rowCount();
    if (rowCount > 1) {
        materialLayers->removeRow(row);
    }

}


void
ShakerMaker::RunModel(void)
{
  
}

void ShakerMaker::SubmitJob(void)
{




}

void
ShakerMaker::CheckCenter(void)
{
    QStringList args;
    args << "--dt" << QString::number(dt->getDouble());
    args << "--nfft" << QString::number(nfft->getInt());
    args << "--dk" << QString::number(dk->getDouble());
    args << "--tmin" << QString::number(tmin->getDouble());
    args << "--tmax" << QString::number(tmax->getDouble());
    args << "--tmpLocation" << tmpLocation->getDirName();
    args << "--stationType" << stationType->currentText();
    // args << "--centerX" << QString::number(centerXs->getDouble());
    // args << "--centerY" << QString::number(centerYs->getDouble());
    // args << "--centerZ" << QString::number(centerZs->getDouble());
    // check if the station type is single station
    if (stationType->currentText() == "Single Station") {
        args << "--stationLatitude"  << QString::number(centerXs->getDouble());
        args << "--stationLongitude" << QString::number(centerYs->getDouble());
        args << "--stationDepth"     << QString::number(centerZs->getDouble());
    } else if (stationType->currentText() == "DRM Box") {
        args << "--DRMstationLatitude"  << QString::number(centerXd->getDouble());
        args << "--DRMstationLongitude" << QString::number(centerYd->getDouble());
        args << "--DRMstationDepth"     << QString::number(centerZd->getDouble());
    }

    
    args << "--numLayers" << QString::number(materialLayers->rowCount()-1);

    // loop over the material layers
    QString Materialproperties = "";
    for (int i = 0; i < materialLayers->rowCount()-1; i++) {
        Materialproperties += materialLayers->item(i, 1)->text();
        Materialproperties += ":";
        Materialproperties += materialLayers->item(i, 2)->text();
        Materialproperties += ":";
        Materialproperties += materialLayers->item(i, 3)->text();
        Materialproperties += ":";
        Materialproperties += materialLayers->item(i, 4)->text();
        Materialproperties += ":";
        Materialproperties += materialLayers->item(i, 5)->text();
        Materialproperties += ":";
        Materialproperties += materialLayers->item(i, 6)->text();
        if (i != materialLayers->rowCount()-2)
            Materialproperties += ",";

    }
    args << "--materialProperties" << Materialproperties;

    // loop over the last row of the material layers
    args << "--halfSpaceLayerName" << materialLayers->item(materialLayers->rowCount()-1, 0)->text();
    args << "--halfSpaceThickness" << materialLayers->item(materialLayers->rowCount()-1, 1)->text();
    args << "--halfSpaceVp" << materialLayers->item(materialLayers->rowCount()-1, 2)->text();
    args << "--halfSpaceVs" << materialLayers->item(materialLayers->rowCount()-1, 3)->text();
    args << "--halfSpaceDensity" << materialLayers->item(materialLayers->rowCount()-1, 4)->text();
    args << "--halfSpaceQp" << materialLayers->item(materialLayers->rowCount()-1, 5)->text();
    args << "--halfSpaceQs" << materialLayers->item(materialLayers->rowCount()-1, 6)->text();


    //     
    qDebug() << "Show model is running";
    errorMessage("Checking the repsonse of the center");

    QString destDir = tmpLocation->getDirName();
    QString appDir = SimCenterPreferences::getInstance()->getAppDir();
    QString pyvistaScript = appDir + QDir::separator() + "ShakerMaker" + QDir::separator() + "ShakerMaker.py";

    QDir destinationDirectory(destDir);
    if (!destinationDirectory.exists()) {
        destinationDirectory.mkpath(destDir);
    }



    // delete the centerStation.png if it exists
    QFile file(destDir + QDir::separator() + "centerStation.png");
    if (file.exists()) {
        file.remove();
    }



    RunPythonInThread *thePythonProcess = new RunPythonInThread(pyvistaScript, args, destDir);
    thePythonProcess->runProcess();

    // check if the centerStation.png exists else wait for the file to be created
    while (!file.exists()) {
        QThread::msleep(1000);
    }

    QPixmap pixmap(destDir + QDir::separator() + "centerStation.png");
    responsefig->setPixmap(pixmap);
    
}


bool
ShakerMaker::outputAppDataToJSON(QJsonObject &jsonObject)
{
  qDebug() << "ShakerMaker::outputAppDataToJSON() - should not be called";
  Q_UNUSED(jsonObject);
  return false;
}

bool
ShakerMaker::inputAppDataFromJSON(QJsonObject &jsonObject)
{
  qDebug() << "ShakerMaker::inputAppDataFromJSON() - should not be called";
  Q_UNUSED(jsonObject);
  return false;  
}

bool
ShakerMaker::outputToJSON(QJsonObject &jsonObject)
{

    
    return true;
}

bool
ShakerMaker::inputFromJSON(QJsonObject &jsonObject)
{

    
    return true;
}


bool
ShakerMaker::copyFiles(QString &destDir)
{
  return true;
  
}

void
ShakerMaker::clear(void)
{

}

bool
ShakerMaker::outputCitation(QJsonObject &jsonObject)
{
  jsonObject.insert("citation", QString("Zhang, W., J. Crempien, K. Zhong, P. Chen, P. Arduino, E. Taciroglu. (2023) A suite of 57 broadband physics-based ground motion simulations for the Istanbul region, in Regional-scale physics-based ground motion simulation for Istanbul, Turkey. DesignSafe-CI. https://doi.org/10.17603/ds2-e7nq-8d52 v1"));
  jsonObject.insert("description", QString("A suite of 57 broadband physics-based ground motion simulations for the Istanbul region"));
  return true;
}

void ShakerMaker::Visualize() {
    // check that faults are loaded
    if (SourcePoints->rowCount() == 0) {
        errorMessage("Error: No faults loaded");
        return;
    }
    // check that fault latitude and longitude are not empty
    if (sourceLatitude->getDouble() == 0.0 || sourceLongitude->getDouble() == 0.0) {
        errorMessage("Error: Fault latitude and longitude cannot be empty");
        return;
    }

    // check that the first column of the table is not empty
    for (int i = 0; i < SourcePoints->rowCount(); i++) {
        if (SourcePoints->item(i, 0) == nullptr) {
            errorMessage("Error: Fault file cannot be empty");
            return;
        }
        if (SourcePoints->item(i, 0)->text().isEmpty() || SourcePoints->item(i, 0)->text() == "") {
            errorMessage("Error: Fault file cannot be empty");
            return;
        }
        
    }

    //check that the view stations is checked
    if (viewStations->isChecked()) {
        // check that the station is DRM Box or Single Station
        if (stationType->currentText() == "Single Station") {
            if (SingleSatationPoints->rowCount() == 0) {
                errorMessage("Error: No stations loaded");
                return;
            }
            // check that the latitude, longitude and depth are not empty
            for (int i = 0; i < SingleSatationPoints->rowCount(); i++) {
                if (SingleSatationPoints->item(i, 0) == nullptr || SingleSatationPoints->item(i, 1) == nullptr || SingleSatationPoints->item(i, 2) == nullptr) {
                    errorMessage("Error: Station latitude, longitude and depth cannot be empty");
                    return;
                }
                // check that the latitude, longitude and depth are numbers
                bool ok;
                double latitude = SingleSatationPoints->item(i, 0)->text().toDouble(&ok);
                if (!ok) {
                    errorMessage("Error: Station latitude must be a number");
                    return;
                }
                double longitude = SingleSatationPoints->item(i, 1)->text().toDouble(&ok);
                if (!ok) {
                    errorMessage("Error: Station longitude must be a number");
                    return;
                }
                double depth = SingleSatationPoints->item(i, 2)->text().toDouble(&ok);
                if (!ok) {
                    errorMessage("Error: Station depth must be a number");
                    return;
                }
            }
        } 

        // if DRM Box is selected, check that the coordinates are not empty
        if (stationType->currentText() == "DRM Box") {
            if (centerXd->text().isEmpty() || centerYd->text().isEmpty()) {
                errorMessage("Error: DRM Box coordinates cannot be empty");
                return;
            }
        }
    }



    // check if that the view crust layers is checked
    if (viewCrustLayers->isChecked()) {
        if (materialLayers->rowCount() == 0) {
            errorMessage("Error: No material layers loaded");
            return;
        }
        // check that the material thickness is not empty for each layer
        for (int i = 0; i < materialLayers->rowCount(); i++) {
            if (materialLayers->item(i, 1) == nullptr || materialLayers->item(i, 1)->text().isEmpty()) {
                errorMessage("Error: Material thickness cannot be empty");
                return;
            }
            // check that the thickness is a number
            bool ok;
            double thickness = materialLayers->item(i, 1)->text().toDouble(&ok);
            if (!ok) {
                errorMessage("Error: Material thickness must be a number");
                return;
            }
        }
    }
        


    QStringList args;

    // creating material layers info
    QString plotCrustLayers = "False";
    QString thickness = "";
    if (viewCrustLayers->isChecked()) {
        plotCrustLayers = "True";
        for (int i = 0; i < materialLayers->rowCount(); i++) {
            thickness += materialLayers->item(i, 1)->text();
            thickness += ";";
        }
    }
    
    // creating station coordinates
    QString plotStation = "False";
    QString stationCoordinates = " ";
    int numstations = 0;
    if (viewStations->isChecked()) {
        plotStation = "True";
        if (stationType->currentText() == "Single Station") {
            for (int i = 0; i < SingleSatationPoints->rowCount(); i++) {
                stationCoordinates += SingleSatationPoints->item(i, 0)->text();
                stationCoordinates += ",";
                stationCoordinates += SingleSatationPoints->item(i, 1)->text();
                stationCoordinates += ",";
                stationCoordinates += SingleSatationPoints->item(i, 2)->text();
                stationCoordinates += ";";
            }
            numstations = SingleSatationPoints->rowCount();
        } else if (stationType->currentText() == "DRM Box") {
            stationCoordinates += QString::number(centerXd->getDouble());
            stationCoordinates += ",";
            stationCoordinates += QString::number(centerYd->getDouble());
            stationCoordinates += ",0.0;";
            numstations = 1;
        }
    }


    // check that fault files are exist
    QString faultfile = tmpLocation->getDirName() + QDir::separator() + "fault" + QDir::separator() + "faultInfo.json";
    // check if the file exists
    QFile file(faultfile);
    if (!file.exists()) {
        errorMessage("Error: Fault information file does not exist");
        return;
    }
    faultfile = tmpLocation->getDirName() + QDir::separator() + "fault" + QDir::separator() + "SourceTimeFunction.py";
    // check if the file exists
    file.setFileName(faultfile);
    if (!file.exists()) {
        errorMessage("Error: SourceTimeFunction.py file does not exist");
        return;
    }



    args << "--tmplocation" << tmpLocation->getDirName(); 
    args << "--plotlayers" << plotCrustLayers;
    args << "--plotstations" << plotStation;
    args << "--numsataions" << QString::number(numstations);
    args << "--thickness" << thickness;
    // print thickness
    if (plotStation == "True") {
        args << "--stationcoordinates" << stationCoordinates;
    }

    




    //     
    qDebug() << "Show model is running";
    errorMessage("Visualizing the fault");

    QString destDir = tmpLocation->getDirName();
    QString appDir = SimCenterPreferences::getInstance()->getAppDir();
    QString pyvistaScript = appDir + QDir::separator() + "applications" + QDir::separator() + "tools" + QDir::separator() + "ShakerMaker" + QDir::separator() + "faultplotter.py";

    QDir destinationDirectory(destDir);
    if (!destinationDirectory.exists()) {
        destinationDirectory.mkpath(destDir);
    }



    // // delete the centerStation.png if it exists
    QFile file2(destDir + QDir::separator() + "faults_map.html");
    if (file2.exists()) {
        file2.remove();
    }



    RunPythonInThread *thePythonProcess = new RunPythonInThread(pyvistaScript, args, destDir);
    thePythonProcess->runProcess();


    QEventLoop loop;

    // Connect the processFinished signal to the loop's quit slot
    connect(thePythonProcess, &RunPythonInThread::processFinished, &loop, &QEventLoop::quit);

    // Start the process
    thePythonProcess->runProcess();

    // Start the event loop and wait for the process to finish
    loop.exec();

    // Now the process is finished, check if the file exists
    if (file2.exists()) {
        // load the file in the web view
        // enable webgl for the web view
        simulationWebView->settings()->setAttribute(QWebEngineSettings::WebGLEnabled, true);
        simulationWebView->load(QUrl::fromLocalFile(destDir + QDir::separator() + "faults.html"));
        simulationWebView->show();
        mapWebView->load(QUrl::fromLocalFile(destDir + QDir::separator() + "faults_map.html"));
        mapWebView->show();
    } else {
        errorMessage("Error: Faults_map.html file not found");
    }

    errorMessage("Faults visualization is done");

}

QWidget* ShakerMaker::createSingleStationWidget(void) {
    QWidget *theWidget = new QWidget();
    QGridLayout *theLayout = new QGridLayout(theWidget);
    QWebEngineView *webView = new QWebEngineView();

    SingleSatationPoints = new QTableWidget();
    SingleSatationPoints->setColumnCount(3);
    SingleSatationPoints->setRowCount(1);

    // expand the table
    SingleSatationPoints->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList labels;
    labels << "Latitude (deg)" << "Longitude (deg)" << "Depth (km)";
    



    SingleSatationPoints->setHorizontalHeaderLabels(labels);




    

    // adding the table to the layout
    theLayout->addWidget(SingleSatationPoints, 0, 0, 1, 3);

    // add add row station button
    QPushButton *addRowButton = new QPushButton("Add Station");
    theLayout->addWidget(addRowButton, 1, 0);

    // add remove row station button
    QPushButton *removeRowButton = new QPushButton("Remove Station");
    theLayout->addWidget(removeRowButton, 1, 1);


    // add google Map button
    QPushButton *MapButton = new QPushButton("Open Map");
    theLayout->addWidget(MapButton, 1, 2);



    // connect add row button to the first row
    connect(addRowButton, &QPushButton::clicked, [=](){
        SingleSatationPoints->insertRow(0);

    });

    // connect remove row button to the first row
    connect(removeRowButton, &QPushButton::clicked, [=](){
        // check if the row count is greater than 1
        if (SingleSatationPoints->rowCount() > 1)
            SingleSatationPoints->removeRow(0);
    });

    // connect open street google map
    connect(MapButton, &QPushButton::clicked, [=](){
        QString url = "https://www.google.com/maps";
        webView->load(QUrl(url));
        webView->show();
    });




    return theWidget;
}

QWidget* ShakerMaker::createDRMBoxWidget(void) {
    QWidget *theWidget = new QWidget();
    QGridLayout *theLayout = new QGridLayout(theWidget);

    theLayout->addWidget(new QLabel("Latitude"), 0, 0);
    centerXd = new SC_DoubleLineEdit("DRM Station Latitude");
    theLayout->addWidget(centerXd, 0, 1);
    theLayout->addWidget(new QLabel("Latitude of the DRM Station"), 0, 2);

    theLayout->addWidget(new QLabel("Longitude"), 1, 0);
    centerYd = new SC_DoubleLineEdit("Longitude");
    theLayout->addWidget(centerYd, 1, 1);
    theLayout->addWidget(new QLabel("Latitude of the DRM Station"), 1, 2);

    // theLayout->addWidget(new QLabel("Z coordinate (m)"), 2, 0);
    // centerZd = new SC_DoubleLineEdit("Center Z", 0.0);
    // theLayout->addWidget(centerZd, 2, 1);
    // theLayout->addWidget(new QLabel("Z coordinate of the center DRM Box (Vertical Depth Direction)"), 2, 2);
    // // lock the depth of the DRM Box to be 0
    // centerZd->setEnabled(false);


    theLayout->addWidget(new QLabel("Width X (m)"), 2, 0);
    widthX = new SC_DoubleLineEdit("Width X", 100);
    theLayout->addWidget(widthX, 2, 1);
    theLayout->addWidget(new QLabel("Width of the DRM Box in X direction (North-South Direction)"), 2, 2);

    theLayout->addWidget(new QLabel("Width Y (m)"), 3, 0);
    widthY = new SC_DoubleLineEdit("Width Y", 100);
    theLayout->addWidget(widthY, 3, 1);
    theLayout->addWidget(new QLabel("Width of the DRM Box in Y direction (East-West Direction)"), 3, 2);

    theLayout->addWidget(new QLabel("Depth (m)"), 4, 0);
    widthZ = new SC_DoubleLineEdit("Width Z", 50);
    theLayout->addWidget(widthZ, 4, 1);
    theLayout->addWidget(new QLabel("Width of the DRM Box in Z direction (Vertical Depth Direction)"), 4, 2);

    theLayout->addWidget(new QLabel("Mesh Size X (m)"), 2, 3, Qt::AlignRight);
    meshSizeX = new SC_DoubleLineEdit("Mesh Size X", 5.0);
    theLayout->addWidget(meshSizeX, 2,4 );
    theLayout->addWidget(new QLabel("Mesh Size in X direction (North-South Direction)"), 2, 5);

    theLayout->addWidget(new QLabel("Mesh Size Y (m)"), 3, 3, Qt::AlignRight);
    meshSizeY = new SC_DoubleLineEdit("Mesh Size Y", 5.0);
    theLayout->addWidget(meshSizeY, 3, 4);
    theLayout->addWidget(new QLabel("Mesh Size in Y direction (East-West Direction)"), 3, 5);

    theLayout->addWidget(new QLabel("Mesh Size Z (m)"), 4, 3, Qt::AlignRight);
    meshSizeZ = new SC_DoubleLineEdit("Mesh Size Z", 5.0);
    theLayout->addWidget(meshSizeZ, 4, 4);
    theLayout->addWidget(new QLabel("Mesh Size in Z direction (Vertical Depth Direction)"), 4, 5);

    // make the columns width equal
    theLayout->setColumnStretch(0, 1);
    theLayout->setColumnStretch(1, 1);
    theLayout->setColumnStretch(2, 1);
    theLayout->setColumnStretch(3, 1);
    theLayout->setColumnStretch(4, 1);
    theLayout->setColumnStretch(5, 1);

    // make the 4th column align tthe layout alighn to the right
    
    


    return theWidget;
}


void ShakerMaker::stationTypeChanged(const QString &arg1) {
    qDebug() << "Station Type Changed";
    if (arg1 == "Single Station") {
        StationStackedWidget->setCurrentIndex(0);
    } else if (arg1 == "DRM Box") {
        StationStackedWidget->setCurrentIndex(1);
    }
}

QWidget* ShakerMaker::createMaterialLayerbox(void) {
    QGroupBox *materialLayersGroupBox = new QGroupBox("Material Layers");
    QGridLayout *materialLayersLayout = new QGridLayout(materialLayersGroupBox);
    materialLayers = new QTableWidget();
    materialLayers->setColumnCount(7);
    materialLayers->setRowCount(1);

    // make columns 2, 3, 4, 5, 6 are double and column 1 is string
 






    // expand the table
    materialLayers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    QStringList labels;
    labels << "Layer name"<<"Thickness (km)" << "Vp (km/s)" << "Vs (km/s)" << "Density (g/cm^3)" << "Qp" << "Qs";
    materialLayers->setHorizontalHeaderLabels(labels);

    // set the properties of the table of o the first and second row


    materialLayers->setItem(0,0,new QTableWidgetItem("Half Space Layer"));
    materialLayers->setItem(0,1,new QTableWidgetItem("0"));
    materialLayers->setItem(0,2,new QTableWidgetItem("3.46"));
    materialLayers->setItem(0,3,new QTableWidgetItem("2.0"));
    materialLayers->setItem(0,4,new QTableWidgetItem("2.0"));
    materialLayers->setItem(0,5,new QTableWidgetItem("1000.0"));
    materialLayers->setItem(0,6,new QTableWidgetItem("1000.0"));


    // lock the thickness of the half space
    materialLayers->item(0,1)->setFlags(materialLayers->item(0,1)->flags() & ~Qt::ItemIsEditable);



    materialLayersLayout->addWidget(materialLayers,3,0,1,8);

    // add add row button
    QPushButton *addRowButton = new QPushButton("Add Layer");
    materialLayersLayout->addWidget(addRowButton, 4, 0);

    // add remove row button
    QPushButton *removeRowButton = new QPushButton("Remove Layer");
    materialLayersLayout->addWidget(removeRowButton, 4, 1);

    // connect add row button to the first row
    connect(addRowButton, &QPushButton::clicked, [=](){
        materialLayers->insertRow(0);
    });

    // connect remove row button to the first row
    connect(removeRowButton, &QPushButton::clicked, [=](){
        reomveMaterialLayer(0);
    });
    

    // add file path label
    materialLayersLayout->addWidget(new QLabel("Load form File"), 5, 0);

    // load file name path 
    crustfileName = new QLineEdit();
    materialLayersLayout->addWidget(crustfileName, 5, 1,1,4);


    // add browse button
    QPushButton *browseButton = new QPushButton("Browse");
    materialLayersLayout->addWidget(browseButton, 5, 6);

    // connect the browse button to the file dialog
    connect(browseButton, &QPushButton::clicked, [=](){
        QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All Files (*)"));
        // set the string to the FileName 
        crustfileName->setText(filename);
    });

    // add load button
    QPushButton *loadButton = new QPushButton("Load");
    materialLayersLayout->addWidget(loadButton, 5, 7);

    // connect the load button to the file dialog
    connect(loadButton, &QPushButton::clicked, [=](){
        QString filename = crustfileName->text();
        // load a json file
        QFile file(filename);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString val = file.readAll();

        QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
        QJsonArray obj = doc.array();

    

        // delete all the rows in the table
        int numrows = materialLayers->rowCount();
        for (int i = 0; i < numrows; i++) {
            materialLayers->removeRow(0);
        }



        // loop over the json array
        for (int i = 0; i < obj.size(); i++) {
            // add a new row to the end of the table
            materialLayers->insertRow(i);
            int row = i;
            QJsonObject layer = obj[i].toObject();
            materialLayers->setItem(row, 0, new QTableWidgetItem(layer["name"].toString()));

            // check the thickness of the last layer to be zero else give an error
            if (i == obj.size()-1) {
                double thick = layer["thick"].toDouble();
                if (thick != 0) {
                    // print thickness of the half space layer is not zero so we made it zero
                    errorMessage("Error: Thickness of the Half Space Layer (last layer) is not zero");
                    errorMessage("Setting the thickness of the Half Space Layer to zero automatically");
                    layer["thick"] = 0;
                }
            }
            materialLayers->setItem(row, 1, new QTableWidgetItem(QString::number(layer["thick"].toDouble())));
            materialLayers->setItem(row, 2, new QTableWidgetItem(QString::number(layer["vp"].toDouble())));
            materialLayers->setItem(row, 3, new QTableWidgetItem(QString::number(layer["vs"].toDouble())));
            materialLayers->setItem(row, 4, new QTableWidgetItem(QString::number(layer["rho"].toDouble())));
            materialLayers->setItem(row, 5, new QTableWidgetItem(QString::number(layer["Qa"].toDouble())));
            materialLayers->setItem(row, 6, new QTableWidgetItem(QString::number(layer["Qb"].toDouble())));
        }

        file.close();
    });






    return materialLayersGroupBox;
}



int ShakerMaker::createMetaData() {
    

    // open file with name metadata.json to write metadata 
    // in the tmpLocation
    QString destDir = tmpLocation->getDirName();
    // check if the directory exists if not create it
    QDir dir(destDir);
    if (!dir.exists()) {
        dir.mkpath(destDir);
    }



    QFile file(destDir + QDir::separator() + "metadata.json");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    // check if the file is open
    if (!file.isOpen()) {
        errorMessage("Error: Could not open metadata.json for writing");
        return -1;
    }

    // Create a JSON object to hold the metadata
    QJsonObject metadata;
    // metadata["crustdata"] is list of dictonaries
    QJsonArray crustdataArray;
    for (int i = 0; i < materialLayers->rowCount(); i++) {
        QJsonObject layer;

        // check if the thickness is not empty
        for (int j = 0; j < 7; j++) {
            if (materialLayers->item(i, j) == nullptr || materialLayers->item(i, j)->text().isEmpty()) {
                errorMessage("Error: Material layer data is incomplete");
                file.close();
                return -1;
            }
            if (j>0 ) {
                // check if the thickness is a number
                bool ok;
                double value = materialLayers->item(i, j)->text().toDouble(&ok);
                if (!ok) {
                    errorMessage("Error: Material layer properties must be numbers");
                    file.close();
                    return -1;
                }
            }
 
        }

        layer["name"] = materialLayers->item(i, 0)->text();
        layer["thick"] = materialLayers->item(i, 1)->text().toDouble();
        layer["vp"] = materialLayers->item(i, 2)->text().toDouble();
        layer["vs"] = materialLayers->item(i, 3)->text().toDouble();
        layer["rho"] = materialLayers->item(i, 4)->text().toDouble();
        layer["Qa"] = materialLayers->item(i, 5)->text().toDouble();
        layer["Qb"] = materialLayers->item(i, 6)->text().toDouble();
        crustdataArray.append(layer);
    }
    metadata["crustdata"] = crustdataArray;



    // add station data
    // station data is a dictionary
    QJsonObject stationData;
    stationData["stationType"] = stationType->currentText();
    stationData["name"] = "Station provided by user in EE-UQ";
    if (stationType->currentText() == "Single Station") {
        stationData["stationType"] = "single";
        // check that the table is not empty
        if (SingleSatationPoints->rowCount() == 0) {
            errorMessage("Error: No stations provided");
            file.close();
            return -1;
        }

        


        // create an array of stations
        QJsonArray stationsArray;
        for (int i = 0; i < SingleSatationPoints->rowCount(); i++) {
            QJsonObject station;
            // check if the latitude, longitude and depth are not empty
            if (SingleSatationPoints->item(i, 0) == nullptr || SingleSatationPoints->item(i, 1) == nullptr || SingleSatationPoints->item(i, 2) == nullptr) {
                errorMessage("Error: Station data is incomplete");
                file.close();
                return -1;
            }
            // check if the latitude, longitude and depth are numbers
            bool ok;
            double latitude = SingleSatationPoints->item(i, 0)->text().toDouble(&ok);
            if (!ok) {
                errorMessage("Error: Station latitude must be a number");
                file.close();
                return -1;
            }
            double longitude = SingleSatationPoints->item(i, 1)->text().toDouble(&ok);
            if (!ok) {
                errorMessage("Error: Station longitude must be a number");
                file.close();
                return -1;
            }
            double depth = SingleSatationPoints->item(i, 2)->text().toDouble(&ok);
            if (!ok) {
                errorMessage("Error: Station depth must be a number");
                file.close();
                return -1;
            }


            station["latitude"] = latitude;
            station["longitude"] = longitude;
            station["depth"] = depth;

            QJsonObject singleStationMetadata;
            singleStationMetadata["name"] = "Station " + QString::number(i+1);
            singleStationMetadata["filter_results"] = false;
            QJsonObject filterParameters;
            filterParameters["fmax"] = 10.0;
            singleStationMetadata["filter_parameters"] = filterParameters;
            station["metadata"] = singleStationMetadata;
            stationsArray.append(station);
        }
        stationData["Singlestations"] = stationsArray;
    } else if (stationType->currentText() == "DRM Box") {
        stationData["stationType"] = "DRM";
        QJsonObject drmBox;
        // check that  latitude, longitude and are not empty
        if (centerXd->text().isEmpty() || centerYd->text().isEmpty() || widthX->text().isEmpty()|| widthY->text().isEmpty() || widthZ->text().isEmpty() || meshSizeX->text().isEmpty() || meshSizeY->text().isEmpty() || meshSizeZ->text().isEmpty()) {
            errorMessage("Error: DRM information can not be empty");
            file.close();
            return -1;
        }


        drmBox["latitude"] = centerXd->getDouble();
        drmBox["longitude"] = centerYd->getDouble();
        drmBox["name"] = "DRMStation";
        drmBox["Width X"] = widthX->getDouble();
        drmBox["Width Y"] = widthY->getDouble();
        drmBox["Depth"]   = widthZ->getDouble();
        drmBox["Mesh Size X"] = meshSizeX->getDouble();
        drmBox["Mesh Size Y"] = meshSizeY->getDouble();
        drmBox["Mesh Size Z"] = meshSizeZ->getDouble();
        stationData["DRMbox"] = drmBox;
    }
    metadata["stationdata"] = stationData;


    // dt, nfft, dk, tmin, tmax, dh
    // check if the values are not empty
    if (dt->text().isEmpty() || nfft->text().isEmpty() || dk->text().isEmpty() || tmin->text().isEmpty() || tmax->text().isEmpty() || delta_h->text().isEmpty() || delta_v_rec->text().isEmpty() || delta_v_src->text().isEmpty()) {
        errorMessage("Error: Analysis information parameters can not be empty");
        file.close();
        return -1;
    }


    // create analysisData object
    QJsonObject analysisData;
    analysisData["dt"] = dt->getDouble();
    analysisData["nfft"] = nfft->getInt();
    analysisData["dk"] = dk->getDouble();
    analysisData["tmin"] = tmin->getDouble();
    analysisData["tmax"] = tmax->getDouble();
    analysisData["dh"] = delta_h->getDouble();
    analysisData["delta_v_rec"] = delta_v_rec->getDouble();
    analysisData["delta_v_src"] = delta_v_src->getDouble();
    metadata["analysisdata"] = analysisData;




    // open the the other faultinfo.json file in the same directory
    QFile file2(destDir + QDir::separator() + "fault" + QDir::separator() + "faultInfo.json");
    file2.open(QIODevice::ReadOnly | QIODevice::Text);
    // check if the file is open
    if (!file2.isOpen()) {
        errorMessage("Error: Could not open faultInfo.json for reading");
        return -1;
    }

    // read the metadata from the file
    QString val = file2.readAll();
    file2.close();
    QJsonDocument doc2 = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject faultInfo = doc2.object();


    // check that the fault latitude and longitude are not empty
    if (sourceLatitude->text().isEmpty() || sourceLongitude->text().isEmpty()) {
        errorMessage("Error: Fault latitude and longitude cannot be empty");
        file2.close();
        file.close();
        return -1;
    }

    // edit the meta data based on the user input
    faultInfo["latitude"] = sourceLatitude->getDouble();
    faultInfo["longitude"] = sourceLongitude->getDouble();

    // read the filenames from the user input table
    QJsonArray filenamesArray;
    for (int i = 0; i < SourcePoints->rowCount(); i++) {
        if (SourcePoints->item(i, 0) == nullptr || SourcePoints->item(i, 0)->text().isEmpty()) {
            errorMessage("Error: Fault file cannot be empty");
            file2.close();
            file.close();
            return -1;
        }
        filenamesArray.append(SourcePoints->item(i, 0)->text());
    }
    faultInfo["Faultfilenames"] = filenamesArray;

    // add the fault info to the metadata
    metadata["faultdata"] = faultInfo;



    // add the jobdata 
    QJsonObject jobData;
    jobData["system"] = system->currentText();
    jobData["queue"] = queue->currentText();
    jobData["numNodes"] = numNodes->getInt();
    jobData["numCores"] = coresPerNode->getInt();
    if (maxRunTime != nullptr) {
        jobData["maxruntime"] = maxRunTime->time().toString("hh:mm:ss");
    } else {
        errorMessage("Error: Max runtime cannot be empty");
    }
    metadata["jobdata"] = jobData;

    // write the metadata to the file
    QJsonDocument doc(metadata);
    file.write(doc.toJson());
    file.close();
    // errorMessage("Metadata written to metadata.json");
    return 0;
}




void ShakerMaker::runJob(void) {

    // pop up window to get the username and password in one dialog
    // create a new dialog
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Login to Tapis");
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    QLineEdit *username = new QLineEdit(dialog);
    username->setPlaceholderText("Username");
    layout->addWidget(username);
    QLineEdit *password = new QLineEdit(dialog);
    password->setPlaceholderText("Password");
    password->setEchoMode(QLineEdit::Password);
    layout->addWidget(password);
    QPushButton *loginButton = new QPushButton("Login", dialog);
    layout->addWidget(loginButton);
    dialog->setLayout(layout);
    connect(loginButton, &QPushButton::clicked, [=]() {
        // check if the username and password are not empty
        if (username->text().isEmpty() || password->text().isEmpty()) {
            errorMessage("Error: Username and password cannot be empty");
            return;
        }
        // close the dialog
        dialog->accept();
    });
    // print the username and password
    dialog->exec();
    QString user = username->text();
    QString pass = password->text();




    // create metadata
    int res = createMetaData();
    if (res < 0) {
        return;
    }
    // check if the metadata file exists
    QString destDir = tmpLocation->getDirName();
    QString metaDataPath = destDir + QDir::separator();
    QFile file(metaDataPath);
    if (!file.exists()) {
        errorMessage("Error: Metadata file does not exist");
        return;
    }

    QString appDir = SimCenterPreferences::getInstance()->getAppDir() + QDir::separator() + "applications" + QDir::separator() + "tools" + QDir::separator() + "ShakerMaker";
    QString runScript   = appDir + QDir::separator() + "ShakerMakersubmitjob.py";
    QString tapisfolder = appDir + QDir::separator() + "TapisFiles";
    QStringList args;
    args << "--metadata" << metaDataPath;
    args << "--tapisfolder" << tapisfolder;
    args << "--username" << user;
    args << "--password" << pass;

    // create a new process to run the job
    ModularPython *thePythonApp = new ModularPython(tmpLocation->getDirName());
    errorMessage("STARTING PYTHON");
    thePythonApp->run(runScript,args);
    // print tthe output of the python script every 5 seconds
    errorMessage("PYTHON DONE");  
    delete thePythonApp;

}


void ShakerMaker::loadModel(QString modelPath) {
    // load the model from the given path
    QFile file(modelPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMessage("Error: Could not open model file");
        return;
    }
    // read the json file
    QString val = file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject model = doc.object();


    // load the parameters
    //  check that if the keys exist in the model
    if (!model.contains("dt") || !model.contains("nfft") || !model.contains("dk") || !model.contains("tmin") || !model.contains("tmax") || !model.contains("dh") || !model.contains("dv_rec") || !model.contains("dv_src")) {
        errorMessage("Error: Model file is missing parameters or has incorrect names");
        errorMessage("need keys: dt, nfft, dk, tmin, tmax, dh, delta_v_rec, delta_v_src");    
        return;
    }

    // chekc that the values are of the correct type
    if (!model["dt"].isDouble() || !model["nfft"].isDouble() || !model["dk"].isDouble() || !model["tmin"].isDouble() || !model["tmax"].isDouble() || !model["dh"].isDouble() || !model["dv_rec"].isDouble() || !model["dv_rec"].isDouble()) {
        errorMessage("Error: Model file has incorrect types for parameters");
        return;
    }
    
    dt->setText(QString::number(model["dt"].toDouble()));
    nfft->setText(QString::number(model["nfft"].toInt()));
    dk->setText(QString::number(model["dk"].toDouble()));
    tmin->setText(QString::number(model["tmin"].toDouble()));
    tmax->setText(QString::number(model["tmax"].toDouble()));
    delta_h->setText(QString::number(model["dh"].toDouble()));
    delta_v_rec->setText(QString::number(model["dv_rec"].toDouble()));
    delta_v_src->setText(QString::number(model["dv_src"].toDouble()));



    // load the job data
    // check that the keys exist in the model
    if (!model.contains("System") || !model.contains("Queue") || !model.contains("Number of Nodes") || !model.contains("Cores per Node") || !model.contains("Max Run Time")) {
        errorMessage("Error: Model file is missing job parameters or has incorrect names");
        errorMessage("need keys: system, queue, numNodes, numCores, maxruntime");    
        return;
    }
    // check that the values are of the correct type
    if (!model["System"].isString() || !model["Queue"].isString() || !model["Number of Nodes"].isDouble() || !model["Cores per Node"].isDouble() || !model["Max Run Time"].isString()) {
        errorMessage("Error: Model file has incorrect types for job parameters");
        return;
    }

    // check the model[system] and model[queue] are in the combobox
    bool find = false;
    for (int i = 0; i < system->count(); i++) {
        if (system->itemText(i) == model["System"].toString()) {
            system->setCurrentIndex(i);
            find = true;
            break;
        }
    }
    if (!find) {
        errorMessage("Error: System not supported");
        return;
    } 

    find = false;
    for (int i = 0; i < queue->count(); i++) {
        if (queue->itemText(i) == model["Queue"].toString()) {
            queue->setCurrentIndex(i);
            find = true;
            break;
        }
    }
    if (!find) {
        errorMessage("Error: Queue not supported");
        return;
    }

    numNodes->setText(QString::number(model["Number of Nodes"].toInt()));
    coresPerNode->setText(QString::number(model["Cores per Node"].toInt()));
    if (model.contains("Max Run Time")) {
        QString maxRuntime = model["Max Run Time"].toString();
        QTime time = QTime::fromString(maxRuntime, "hh:mm:ss");
        maxRunTime->setTime(time);
    }


    // check that model has the fault type key
    QString station_type = model["station_type"].toString();
    // make the faultType lowercase
    station_type = station_type.toLower();
    if (model.contains("station_type")) {
        // check that the fault type is either single or DRM
        if (station_type != "single" && station_type != "drm") {
            errorMessage("Error: Station type must be either single or DRM");
            return;
        }
    } else {
        errorMessage("Error: Model file is missing station_type key");
        return;
    }


    // DRM
    if (station_type == "drm") {
        // check that the model 
        QJsonObject drmBox = model["station_info"].toObject();

        // check that the keys exist in the model
        if (!drmBox.contains("Latitude") || !drmBox.contains("Longitude") || !drmBox.contains("Width X") || !drmBox.contains("Width Y") || !drmBox.contains("Depth") || !drmBox.contains("Mesh Size X") || !drmBox.contains("Mesh Size Y") || !drmBox.contains("Mesh Size Z")) {
            errorMessage("Error: DRM Box is missing parameters or has incorrect names");
            errorMessage("need keys: Latitude, Longitude, Width X, Width Y, Depth, Mesh Size X, Mesh Size Y, Mesh Size Z");
            return;
        }

        // check that the values are of the correct type
        if (!drmBox["Latitude"].isDouble() || !drmBox["Longitude"].isDouble() || !drmBox["Width X"].isDouble() || !drmBox["Width Y"].isDouble() || !drmBox["Depth"].isDouble() || !drmBox["Mesh Size X"].isDouble() || !drmBox["Mesh Size Y"].isDouble() || !drmBox["Mesh Size Z"].isDouble()) {
            errorMessage("Error: DRM Box has incorrect types for parameters");
            return;
        }

        // set the values
        centerXd->setText(QString::number(drmBox["Latitude"].toDouble()));
        centerYd->setText(QString::number(drmBox["Longitude"].toDouble()));
        widthX->setText(QString::number(drmBox["Width X"].toDouble()));
        widthY->setText(QString::number(drmBox["Width Y"].toDouble()));
        widthZ->setText(QString::number(drmBox["Depth"].toDouble()));
        meshSizeX->setText(QString::number(drmBox["Mesh Size X"].toDouble()));
        meshSizeY->setText(QString::number(drmBox["Mesh Size Y"].toDouble()));
        meshSizeZ->setText(QString::number(drmBox["Mesh Size Z"].toDouble()));


        // make the current index of the station type to DRM
        stationType->setCurrentIndex(1);

    } else {
        // model["station_info"] is list of dictonaries
        QJsonArray stationsArray = model["station_info"].toArray();
        // clear the whole table
        SingleSatationPoints->setRowCount(0);


        for (int i = 0; i < stationsArray.size(); i++) {
            QJsonObject station = stationsArray[i].toObject();
            // check that the keys exist in the model
            if (!station.contains("Latitude") || !station.contains("Longitude") || !station.contains("Depth")) {
                errorMessage("Error: Station is missing parameters or has incorrect names");
                errorMessage("need keys: Latitude, Longitude, Depth for each station");
                return;
            }
            // check that the values are of the correct type
            if (!station["Latitude"].isDouble() || !station["Longitude"].isDouble() || !station["Depth"].isDouble()) {
                errorMessage("Error: Station has incorrect types for parameters");
                return;
            }

            // clear the whole table
            
            SingleSatationPoints->insertRow(i);
            SingleSatationPoints->setItem(i, 0, new QTableWidgetItem(QString::number(station["Latitude"].toDouble())));
            SingleSatationPoints->setItem(i, 1, new QTableWidgetItem(QString::number(station["Longitude"].toDouble())));
            SingleSatationPoints->setItem(i, 2, new QTableWidgetItem(QString::number(station["Depth"].toDouble())));
        }       

        // make the current index of the station type to single
        stationType->setCurrentIndex(0);  
    }

    // load the layers data
    // check that the model has the layers key
    if (!model.contains("layers")) {
        errorMessage("Error: Model file is missing layers array");
        return;
    }
    QJsonArray layersArray = model["layers"].toArray();

    // clear the whole table
    materialLayers->setRowCount(0);

    // loop over the layers array
    for (int i = 0; i < layersArray.size(); i++) {
        QJsonObject layer = layersArray[i].toObject();
        // check that the keys exist in the model
        // {"Layer Name":"Layer1",          "Thickness":10, "Vp":2000, "Vs":1000, "Density":2000, "Qp": 100, "Qs": 50},
        if (!layer.contains("Layer Name") || !layer.contains("Thickness") || !layer.contains("Vp") || !layer.contains("Vs") || !layer.contains("Density") || !layer.contains("Qp") || !layer.contains("Qs")) {
            errorMessage("Error: Layer is missing parameters or has incorrect names");
            errorMessage("need keys: Layer Name, Thickness, Vp, Vs, Density, Qp, Qs for each layer");
            return;
        }
        // check that the values are of the correct type
        if (!layer["Layer Name"].isString() || !layer["Thickness"].isDouble() || !layer["Vp"].isDouble() || !layer["Vs"].isDouble() || !layer["Density"].isDouble() || !layer["Qp"].isDouble() || !layer["Qs"].isDouble()) {
            errorMessage("Error: Layer has incorrect types for parameters");
            return;
        }

        // append row to the end of the table
        materialLayers->insertRow(i);

        double thick = layer["Thickness"].toDouble();
        // make the thickness of the last layer to be zero
        if (i == layersArray.size()-1) {
            thick = 0;
        }   

        materialLayers->setItem(i, 0, new QTableWidgetItem(layer["Layer Name"].toString()));
        materialLayers->setItem(i, 1, new QTableWidgetItem(QString::number(thick)));
        materialLayers->setItem(i, 2, new QTableWidgetItem(QString::number(layer["Vp"].toDouble())));
        materialLayers->setItem(i, 3, new QTableWidgetItem(QString::number(layer["Vs"].toDouble())));
        materialLayers->setItem(i, 4, new QTableWidgetItem(QString::number(layer["Density"].toDouble())));
        materialLayers->setItem(i, 5, new QTableWidgetItem(QString::number(layer["Qp"].toDouble())));
        materialLayers->setItem(i, 6, new QTableWidgetItem(QString::number(layer["Qs"].toDouble())));
    }



    // load fault data
    QString fault_type = model["fault_type"].toString();
    fault_type = fault_type.toLower();

    if (model.contains("fault_type")) {
        // check that the fault type is database or custom 
        if (fault_type != "database" && fault_type != "custom") {
            errorMessage("Error: Fault type must be either database or custom");
            return;
        }
    } else {
        errorMessage("Error: Model file is missing fault_type key");
        return;
    }

    // load the fault data
    if (fault_type == "database") {
        // check that the model has the fault info key
        if (!model.contains("fault_info")) {
            errorMessage("Error: Model file is missing fault_info key");            
            return;
        }
        QJsonObject faultInfo = model["fault_info"].toObject();

        // check that the keys exist in the model
        if (!faultInfo.contains("Country") || !faultInfo.contains("Fault") || !faultInfo.contains("Magnitude") || !faultInfo.contains("Fault Type") || !faultInfo.contains("Realization")) {
            errorMessage("Error: Fault info is missing parameters or has incorrect names");
            errorMessage("need keys: Country, Fault, Magnitude, Type, Realization");
            return;
        }

        // check that the values are of the correct type
        if (!faultInfo["Country"].isString() || !faultInfo["Fault"].isString() || !faultInfo["Magnitude"].isString() || !faultInfo["Fault Type"].isString() || !faultInfo["Realization"].isString()) {
            errorMessage("Error: Fault info has incorrect types for parameters");
            return;
        }
    
        // set the values
        // check that the values are in the combobox
        find = false;
        for (int i = 0; i < countryComboBox->count(); i++) {
            if (countryComboBox->itemText(i) == faultInfo["Country"].toString()) {
                countryComboBox->setCurrentIndex(i);
                find = true;
                break;
            }
        }
        if (!find) {
            errorMessage("Error: Country not supported");
            return;
        }

        find = false;
        for (int i = 0; i < faultComboBox->count(); i++) {
            if (faultComboBox->itemText(i) == faultInfo["Fault"].toString()) {
                faultComboBox->setCurrentIndex(i);
                find = true;
                break;
            }
        }
        if (!find) {
            errorMessage("Error: Fault not supported");
            return;
        }

        find = false;
        for (int i = 0; i < magnitudeComboBox->count(); i++) {
            if (magnitudeComboBox->itemText(i) == faultInfo["Magnitude"].toString()) {
                magnitudeComboBox->setCurrentIndex(i);
                find = true;
                break;
            }
        }
        if (!find) {
            errorMessage("Error: Magnitude not supported");
            return;
        }

        find = false;
        for (int i = 0; i < faultTypeComboBox->count(); i++) {
            if (faultTypeComboBox->itemText(i) == faultInfo["Fault Type"].toString()) {
                faultTypeComboBox->setCurrentIndex(i);
                find = true;
                break;
            }
        }
        if (!find) {
            errorMessage("Error: Fault Type not supported");
            return;
        }

        find = false;
        for (int i = 0; i < realizationComboBox->count(); i++) {
            if (realizationComboBox->itemText(i) == faultInfo["Realization"].toString()) {
                realizationComboBox->setCurrentIndex(i);
                find = true;
                break;
            }
        }
        if (!find) {
            errorMessage("Error: Realization not supported");
            return;
        }


        // press the load button  to load the database
        loaddtabaseButton->click();

    } else if (fault_type == "custom") {

        errorMessage("Error: Custom fault not implemented yet");
        return;

    }   



}




