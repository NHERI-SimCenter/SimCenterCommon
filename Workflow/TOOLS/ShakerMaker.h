#ifndef SHAKER_MAKER_WIDGET_H
#define SHAKER_MAKER_WIDGET_H

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

/**
 *  @author  fmckenna
 *  @date    03/2024
 *  @version 1.0
 *
 *  @section DESCRIPTION
 *
 * The purpose of this class is to define interface for Istanbul ground motions
 */

#include <SimCenterAppWidget.h>
#include <SC_ResultsWidget.h>


#include <QWebEngineView>
#include <QStackedWidget>
#include <QTableWidget>
#include <QLabel>
#include <vector>
#include <QNetworkAccessManager>
#include <QTimeEdit>
#include <QTime>


class SC_DoubleLineEdit;
class SC_IntLineEdit;
class SC_DirEdit;
class SC_ComboBox;
class QPushButton;
class SC_CheckBox;
class QComboBox;


class ShakerMaker : public SimCenterAppWidget
{
    Q_OBJECT
public:
    ShakerMaker();
    virtual ~ShakerMaker();
    bool outputToJSON(QJsonObject &jsonObject);
    bool inputFromJSON(QJsonObject &jsonObject);  
    bool outputAppDataToJSON(QJsonObject &jsonObject);
    bool inputAppDataFromJSON(QJsonObject &jsonObject);
    bool copyFiles(QString &destDir);
    virtual void clear(void);

    virtual bool outputCitation(QJsonObject &jsonObject); 

    QWidget* createDRMBoxWidget(void);
    QWidget* createSingleStationWidget(void);
    QWidget* createMaterialLayerbox(void);
    void reomveMaterialLayer(int row);
    void Visualize();
    void SubmitJob();
    int createMetaData();
    void runJob();
    void loadModel(QString );


signals:

public slots:
  void RunModel(void);
  void stationTypeChanged(const QString &arg1);
  void CheckCenter(void);
  
private:
  SC_DoubleLineEdit   *dt;
  SC_IntLineEdit      *nfft;
  SC_DoubleLineEdit   *dk;
  SC_DoubleLineEdit   *tmin;
  SC_DoubleLineEdit   *tmax;
  SC_IntLineEdit      *numRealizations;
  SC_DirEdit          *tmpLocation;
  SC_ComboBox         *gridType;
  QWebEngineView      *webView1;
  QComboBox           *stationType;
  QStackedWidget      *StationStackedWidget;
  QTableWidget        *materialLayers;
  QTableWidget        *SourcePoints;
  QTableWidget        *SingleSatationPoints;
  int count;
  SC_DoubleLineEdit *centerXs;
  SC_DoubleLineEdit *centerYs;
  SC_DoubleLineEdit *centerZs;
  SC_DoubleLineEdit *centerXd;
  SC_DoubleLineEdit *centerYd;
  SC_DoubleLineEdit *centerZd;
  SC_DoubleLineEdit *widthX;
  SC_DoubleLineEdit *widthY;
  SC_DoubleLineEdit *widthZ;
  SC_DoubleLineEdit *meshSizeX;
  SC_DoubleLineEdit *meshSizeY;
  SC_DoubleLineEdit *meshSizeZ;
  QLabel *responsefig;
  SC_DoubleLineEdit   *sourceLatitude;
  SC_DoubleLineEdit   *sourceLongitude;
  QWebEngineView* simulationWebView;
  SC_CheckBox *viewStations;
  SC_CheckBox *viewCrustLayers;
  QLineEdit *crustfileName;
  QComboBox* system;
  QComboBox* queue;
  SC_IntLineEdit* numNodes;
  SC_IntLineEdit* coresPerNode;
  QTimeEdit* maxRunTime;
  SC_DoubleLineEdit* delta_h;
  SC_DoubleLineEdit* delta_v_rec;
  SC_DoubleLineEdit* delta_v_src;
  QWebEngineView *mapWebView;
  QComboBox *countryComboBox;
  QComboBox *faultComboBox;
  QComboBox *magnitudeComboBox;
  QComboBox *faultTypeComboBox;
  QComboBox *realizationComboBox;
  QPushButton *loaddtabaseButton;



};
#endif 
