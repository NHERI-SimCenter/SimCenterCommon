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
#include <SC_ResultsWidget.h>

SC_ResultsWidget::SC_ResultsWidget(QWidget *parent)
  :SimCenterWidget(parent)
{
}

SC_ResultsWidget::~SC_ResultsWidget()
{

}

int
SC_ResultsWidget::processResults(QString &outputFile, QString &dirName){
  return 0;
}

void
SC_ResultsWidget::clear(void) {

}


#ifdef _R2D
SC_ResultsWidget::SC_ResultsWidget(QWidget *parent, QWidget *resWidget, QMap<QString, QList<QString>> assetTypeToType)
    :SimCenterWidget(parent)
{
  R2DresWidget = dynamic_cast<ResultsWidget*>(resWidget);

  if (R2DresWidget)
  {
    theAssetTypeToType = assetTypeToType;
    theVizWidget = R2DresWidget->getVisualizationWidget();
  } else {
    qDebug() << "Can not cast resWidet to ResultWidget";
  }
}

int SC_ResultsWidget::addResultTab(QString tabName, QString &dirName){
  return 0;
}


int SC_ResultsWidget::addResultSubtab(QString name, QWidget* existTab, QString &dirName){
  return 0;
}





void SC_ResultsWidget::restoreUI(void){
}


int SC_ResultsWidget::addResults(SC_ResultsWidget* resultsTab, QString &outputFile, QString &dirName,
                                 QString &assetType, QList<QString> typesInAssetType){
  return 0;
}



QWidget* SC_ResultsWidget::getVizWidget(){
  return theVizWidget;
}
QMainWindow* SC_ResultsWidget::getMainWindow(){
  return mainWindow;
}
QByteArray SC_ResultsWidget::getUiState(){
  return uiState;
}

void SC_ResultsWidget::setUiState(QByteArray newState){
  uiState = newState;
}
std::shared_ptr<QList<QDockWidget*>> SC_ResultsWidget::getDockList(){
  return dockList;
}
std::shared_ptr<SimCenterMapcanvasWidget> SC_ResultsWidget::getMapViewSubWidget(){
  return mapViewSubWidget;
}
QMenu* SC_ResultsWidget::getViewMenu(){
  return viewMenu;
}
std::shared_ptr<QList<QgsMapLayer*>> SC_ResultsWidget::getNeededLayers(){
  return neededLayers;
}
#endif

