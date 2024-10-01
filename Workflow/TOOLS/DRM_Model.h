#ifndef DRM_MODEL_H
#define DRM_MODEL_H

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
#include <TapisV3.h>
#include <QComboBox>
#include <QWebEngineView>
#include <QTimeEdit>
class SC_DoubleLineEdit;
class SC_IntLineEdit;
class SC_DirEdit;
class SC_CheckBox;

class DRM_Model : public SimCenterAppWidget
{
    Q_OBJECT
public:
    explicit DRM_Model(QWidget *parent = 0);
    ~DRM_Model();

    bool outputToJSON(QJsonObject &rvObject) override;
    bool inputFromJSON(QJsonObject &rvObject) override;
    bool outputAppDataToJSON(QJsonObject &jsonObject) override;
    bool inputAppDataFromJSON(QJsonObject &jsonObject) override;
    bool copyFiles(QString &dirName) override;
    void createModel();
    void get_credentials(QString &username, QString &password);
    QStringList getfilelist(QString &rootDir);
    void submitJob();

signals:


private:
    // soil params
    SC_DoubleLineEdit *soilwidth_x;
    SC_DoubleLineEdit *soilwidth_y;
    SC_DoubleLineEdit *soilwidth_z;
    SC_IntLineEdit    *soilNumEle_x;
    SC_IntLineEdit    *soilNumEle_y;
    SC_IntLineEdit    *soilNumEle_z;
    SC_DoubleLineEdit *Vs;
    SC_DoubleLineEdit *nu;
    SC_DoubleLineEdit *rho;

    // DRM params
    QLineEdit   *DRM_filePath;
    SC_IntLineEdit *DRM_numLayers;
    SC_DoubleLineEdit *DRM_laodFactor;
    SC_DoubleLineEdit *DRM_crdScale;
    SC_DoubleLineEdit *DRM_tolernace;
    SC_DoubleLineEdit *DRM_T00;
    SC_DoubleLineEdit *DRM_T01;
    SC_DoubleLineEdit *DRM_T02;
    SC_DoubleLineEdit *DRM_T10;
    SC_DoubleLineEdit *DRM_T11;
    SC_DoubleLineEdit *DRM_T12;
    SC_DoubleLineEdit *DRM_T20;
    SC_DoubleLineEdit *DRM_T21;
    SC_DoubleLineEdit *DRM_T22;
    SC_DoubleLineEdit *DRM_originX;
    SC_DoubleLineEdit *DRM_originY;
    SC_DoubleLineEdit *DRM_originZ;    
    QComboBox *DRM_Sofrware;
    SC_CheckBox *DRM_CoordinateTransformation;
    QComboBox *DRM_Location;



    // absorbing layer
    SC_CheckBox *Absorb_HaveAbsorbingElements;
    SC_DoubleLineEdit *Absorb_NumAbsorbingElements;
    SC_DoubleLineEdit *Absorb_rayleighAlpha;
    SC_DoubleLineEdit *Absorb_rayleighBeta;
    QComboBox *Absorb_type;



    // analysis information
    SC_DoubleLineEdit *dt;
    SC_DoubleLineEdit *t_final;
    SC_DoubleLineEdit *recording_dt;
    SC_DirEdit *outputDir;


    // partitioning information
    QComboBox *partitioningAlgorithm;
    SC_IntLineEdit *numSoilPartitions;
    SC_IntLineEdit *numAbsorbingPartitions;
    SC_IntLineEdit *numDRMPartitions;
    


    // web views
    QWebEngineView *DRMLayerView;
    QWebEngineView *SoilLayerView;
    QWebEngineView *AbsorbingLayerView;
    QWebEngineView *TotalModelView;
    QWebEngineView *PartitionesView;


    // tapis information
    QString tapisUsername = "";
    QString tapisPassword = "";
    TapisV3 *the_DesignSafe_Storage;
    bool loggedIn = false;


    // job information
    QComboBox *Job_System;
    QComboBox *Job_Queue;
    QTimeEdit* Job_maxRunTime;
    QLineEdit *JobUUID;





};
#endif // DRM_MODEL_H