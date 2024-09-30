#ifndef SC_RESULTS_WIDGET_H
#define SC_RESULTS_WIDGET_H

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

// Written: fmckenna

#include <SimCenterWidget.h>

#ifdef _R2D
#include "SimCenterMapcanvasWidget.h"
#include "ResultsWidget.h"
#include "VisualizationWidget.h"
#endif

class QMainWindow;
class QgsMapLayer;
class QDockWidget;
class QMenu;

class SC_ResultsWidget : public SimCenterWidget
{
    Q_OBJECT
  
public:
    explicit SC_ResultsWidget(QWidget *parent = 0);
    virtual ~SC_ResultsWidget();
    virtual int processResults(QString &outputFile, QString &dirName);
    virtual void clear(void);

#ifdef _R2D
    explicit SC_ResultsWidget(QWidget *parent, QWidget *resWidget, QMap<QString, QList<QString>> assetTypeToType);
    virtual int addResultTab(QString tabName, QString &dirName);
    virtual int addResultSubtab(QString name, QWidget* existTab, QString &dirName);

    virtual int addResults(SC_ResultsWidget* resultsTab, QString &outputFile, QString &dirName,
                           QString &assetType, QList<QString> typesInAssetType);


  // Interface function used by subclasses in R2D
    QWidget* getVizWidget();
    QMainWindow* getMainWindow();
    QByteArray getUiState();
    void setUiState(QByteArray newState);
    std::shared_ptr<QList<QDockWidget*>> getDockList();
    std::shared_ptr<SimCenterMapcanvasWidget> getMapViewSubWidget();
    QMenu* getViewMenu();
    std::shared_ptr<QList<QgsMapLayer*>> getNeededLayers();
#endif
  
signals:

public slots:

private slots:

#ifdef _R2D  
    void restoreUI(void);
#endif  

protected:

#ifdef _R2D    
    VisualizationWidget* theVizWidget;
    ResultsWidget* R2DresWidget;
    QMap<QString, QList<QString>> theAssetTypeToType;

    QMainWindow* mainWindow;
    QByteArray uiState;
    std::shared_ptr<QList<QDockWidget*>> dockList = std::shared_ptr<QList<QDockWidget*>>(new QList<QDockWidget*>());
//    QList<QDockWidget*>* dockList;
    std::shared_ptr<SimCenterMapcanvasWidget> mapViewSubWidget;
    QMenu* viewMenu;
//    QList<QgsMapLayer*>* neededLayers =  ;
    std::shared_ptr<QList<QgsMapLayer*>> neededLayers  = std::shared_ptr<QList<QgsMapLayer*>>(new QList<QgsMapLayer*>());
#endif
  
    void extractErrorMsg(QString workDir, QString errFileName, QString uqEngineName, QString &errMsg);

  
};


#endif // SC_RESULTS_WIDGET
