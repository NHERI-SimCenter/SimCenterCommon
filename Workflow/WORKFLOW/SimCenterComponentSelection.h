#ifndef SIMCENTER_COMPONENT_SELECTION_H
#define SIMCENTER_COMPONENT_SELECTION_H

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
 *  @date    09/2018
 *  @version 1.0
 *
 *  @section DESCRIPTION
 *
 * The purpose of this class is to define interface for SimCenterComponentSelection. These are
 * selection widgets that provide the user a choice of options and display a window based on that * choice. They are used in workflow applications. Functionality used in many SimCenter clases.
 */

#include <QWidget>
#include <QModelIndex>
#include <QList>
#include <QString>

class QTreeView;
class QStackedWidget;
class QItemSelection;
class QStandardItemModel;
class QStandardItem;



class SimCenterComponentSelection : public QWidget
{
    Q_OBJECT
public:
    explicit SimCenterComponentSelection(QWidget *parent = 0);
    virtual ~SimCenterComponentSelection();

    /** 
     *   @brief addComponent method to add component to selection options.
     *   @param QString text to appear in selection
     *   @param SimCenterAppWidget * the SimCenterAppWidget to appear when selection made
     *   @return bool true or false indicating success or failure to add
     */  

    virtual bool addComponent(QString text, QWidget *theWidget);

    /** 
     *   @brief swapComponent method to add component to selection options.
     *   @param QString text associated with widget to be swapped
     *   @param SimCenterAppWidget * the SimCenterAppWidget to appear when selection made
     *   @return QWidget * - old widget or NULL if not found
     */  

    virtual QWidget *swapComponent(QString text, QWidget *theWidget);

    /** 
     *   @brief swapComponent method to add component to selection options.
     *   @param QString text associated with widget to be swapped
     *   @param SimCenterAppWidget * the SimCenterAppWidget to appear when selection made
     *   @return QWidget * - old widget or NULL if not found
     */  

    virtual bool displayComponent(QString text);

    /** 
     *   @brief displayComponent method to display a component
     *   @param QString text associated with widget to be displayed
     *   @return bool true or false indicating success or failure to add
     */  

signals:

public slots:

    /** 
     *   @brief selectionChangedSlot slot invoked when slection is changed.
     *   @param QItemSelection new item selected
     */  
    void selectionChangedSlot(const QItemSelection &, const QItemSelection &);
private:

    QTreeView *treeView;
    QStandardItemModel *standardModel;
    QStandardItem *rootNode;
    QStackedWidget *theStackedWidget;
    QModelIndex infoItemIdx;
    QList<QString> textIndices;
    QList<QModelIndex> modelIndices;
};

#endif // SIMCENTER_COMPONENT_SELECTION_H
