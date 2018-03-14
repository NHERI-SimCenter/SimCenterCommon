#ifndef INPUTWIDGETSHEETBM_H
#define INPUTWIDGETSHEETBM_H

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

// Written: fmckenna

#include <QWidget>

#include <QItemSelection>
#include <QTreeView>
#include <QStandardItemModel>
#include <QHBoxLayout>
#include "MainWindow.h"

class GeneralInformationWidget;
class ClineInputWidget;
class FloorInputWidget;
class BeamInputWidget;
class ColumnInputWidget;
class BraceInputWidget;
class WallInputWidget;
class SteelInputWidget;
class ConcreteInputWidget;
class FramesectionInputWidget;
class SlabsectionInputWidget;
class WallsectionInputWidget;
class ConnectionInputWidget;
class PointInputWidget;
class RandomVariableInputWidget;
class SpreadsheetWidget;
class MainWindow;

class InputWidgetSheetBM : public QWidget
{
    Q_OBJECT
public:
    explicit InputWidgetSheetBM(QWidget *parent = 0);
    ~InputWidgetSheetBM();

    void outputToJSON(QJsonObject &rvObject);
    void inputFromJSON(QJsonObject &rvObject);
    void clear(void);

    void setMainWindow(MainWindow* window);

    const SpreadsheetWidget * getActiveSpreadsheet();

signals:

public slots:  
    void selectionChangedSlot(const QItemSelection &, const QItemSelection &);

private:
    void outputGeneralInformationToJSON(QJsonObject &rvObject);

    MainWindow* window;

    QHBoxLayout *horizontalLayout;
    QTreeView *treeView;
    QStandardItemModel *standardModel;

    GeneralInformationWidget *theGeneralInformationInput;
    ClineInputWidget *theClineInput;
    FloorInputWidget *theFloorInput;
    BeamInputWidget *theBeamInput;
    ColumnInputWidget *theColumnInput;
    BraceInputWidget *theBraceInput;
    WallInputWidget *theWallInput;
    SteelInputWidget *theSteelInput;
    ConcreteInputWidget *theConcreteInput;
    QStringList theFramesectionTypes;
    QMap<QString, FramesectionInputWidget*> theFramesectionInputs;
    FramesectionInputWidget *theConcreteRectColFSInput;
    FramesectionInputWidget *theConcreteBoxColFSInput;
    FramesectionInputWidget *theConcreteCircColFSInput;
    FramesectionInputWidget *theConcretePipeColFSInput;
    SlabsectionInputWidget *theSlabsectionInput;
    WallsectionInputWidget *theWallsectionInput;
    ConnectionInputWidget *theConnectionInput;
    PointInputWidget *thePointInput;
    RandomVariableInputWidget *theRVs;

    QModelIndex infoItemIdx;
    SimCenterTableWidget  *currentWidget;

    QJsonObject *jsonObjOrig;
};

#endif // INPUTWIDGETBMSHEET_H
