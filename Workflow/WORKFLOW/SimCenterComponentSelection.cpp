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

#include <SimCenterComponentSelection.h>
#include <QHBoxLayout>
#include <QTreeView>
#include <QTreeView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <CustomizedItemModel.h>
#include <QModelIndex>
#include "AnimatedStackedWidget.h"
#include <QDebug>
#include <SimCenterAppWidget.h>
#include <QApplication>
#include <QTimer>

#include <QFile>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QOpenGLWidget>
#include <QTableView>
#include <QTableWidget>
#include <QTreeView>
#include <QComboBox>
#include <QListView>
#include <QLineEdit>
#include <QScrollArea>
#include <QPushButton>
#include <QCoreApplication>
#include <QTableView>
#include <QTabWidget>
#include <QTabBar>
#include <QTreeView>
#include <QListView>
#include <QTableWidget>
#include "SectionTitle.h"

SimCenterComponentSelection::SimCenterComponentSelection(QWidget *parent)
    :SimCenterAppWidget(parent)
{

  QHBoxLayout *horizontalLayout = new QHBoxLayout();

  //horizontalLayout->setMargin(0);
  horizontalLayout->setContentsMargins(0,5,0,5);
  horizontalLayout->setSpacing(0);

  // Create a TreeView widget
  treeView = new QTreeView();
  standardModel = new CustomizedItemModel; 
  rootNode = standardModel->invisibleRootItem();

  infoItemIdx = rootNode->index();

  // Register the model
  treeView->setModel(standardModel);
  treeView->expandAll();
  treeView->setHeaderHidden(true);
  treeView->setMinimumWidth(100);
  treeView->setMaximumWidth(100);
  treeView->setEditTriggers(QTreeView::EditTrigger::NoEditTriggers); // Disable Edit


  // Customize the apperance of the menu on the left
  treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff ); // hide the horizontal scroll bar               
  treeView->setObjectName("treeViewOnTheLeft");
  treeView->setIndentation(0);
  treeView->setWordWrap(true);

  treeView->setSelectionMode(QAbstractItemView::SingleSelection);
  treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
  treeView->setDragEnabled(false);
  treeView->setDragDropMode(QAbstractItemView::NoDragDrop);
  
  // react to current index changes (fires while mouse moves with button pressed)
  QItemSelectionModel *selectionModel= treeView->selectionModel();
  connect(selectionModel, &QItemSelectionModel::currentChanged, 
    this, &SimCenterComponentSelection::currentChangedSlot);

  // preview/select as the mouse *enters* items. previously it was bugging when clicking and dragging on side-bar
  treeView->setMouseTracking(true);
  connect(treeView, &QTreeView::entered, this, [this](const QModelIndex& idx){
  if (!idx.isValid()) return;
  if (!(QApplication::mouseButtons() & Qt::LeftButton)) return; // <-- require mouse1
  treeView->selectionModel()->setCurrentIndex(
    idx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
  });

  // Add the TreeView widget to the layout
  horizontalLayout->addWidget(treeView);

  theStackedWidget = new AnimatedStackedWidget(); // this instead of new QStackedWidget() for animated
  static_cast<AnimatedStackedWidget*>(theStackedWidget)->setDuration(300);
  static_cast<AnimatedStackedWidget*>(theStackedWidget)->setEasing(QEasingCurve::OutCubic);
  horizontalLayout->addWidget(theStackedWidget);

  this->setLayout(horizontalLayout);
}

// Used for showing background decal in the apps. E.g., waves in HydroUQ
// Called from the workflowapp file for HydroUQ after creating the component selection
void SimCenterComponentSelection::makeChildrenTransparent()
{
    Q_ASSERT(theStackedWidget);

    // // 1) stacked widget viewport must be transparent (the scrolled content sits inside it)
    theStackedWidget->setAttribute(Qt::WA_StyledBackground, true);
    theStackedWidget->setAutoFillBackground(false);
    theStackedWidget->setObjectName("myStackedWidget");
    theStackedWidget->setStyleSheet("#myStackedWidget {background: transparent;}");

    // The content root:
    QWidget* root = theStackedWidget; // same as theStackedWidget

    auto makeContainerTransparent = [](QWidget* w, QWidget* root = nullptr) {
        // Don’t touch inputs/item-views; we only want containers
        if (qobject_cast<QAbstractItemView*>(w) ||  // QTableView/QTreeView/QListView/QTableWidget...
        qobject_cast<QComboBox*>(w) ||
        qobject_cast<QLineEdit*>(w) ||
        qobject_cast<QTextEdit*>(w) ||
        qobject_cast<QPlainTextEdit*>(w) ||
        qobject_cast<QOpenGLWidget*>(w) ||
        qobject_cast<QPushButton*>(w) ||
        qobject_cast<QTabBar*>(w)) {
            return;
        }
        w->setObjectName("rootWidget");
        w->setAttribute(Qt::WA_StyledBackground, true);
        w->setAutoFillBackground(false);
        w->setStyleSheet(
            "#rootWidget {background: transparent;}"
        );
    };
    
    // 2) Make the stacked widget itself transparent
    if (root) makeContainerTransparent(root);

    // 3) go down into some descendants
    for (auto *w : root->findChildren<QWidget*>()) {
        if (w->parent() == root 
        || w->parent()->parent() == root
        || w->parent()->parent()->parent() == root
        || w->parent()->parent()->parent()->parent() == root
        || w->parent()->parent()->parent()->parent()->parent() == root
    ) {
            // allow great-great-grandchildren
            // anymore and you get a lot of transparent widgets
            makeContainerTransparent(w, root);
        }
    }
}


SimCenterComponentSelection::~SimCenterComponentSelection()
{
 QLayout *layout = this->layout();
 layout->removeWidget(theStackedWidget);
 theStackedWidget->setParent(NULL);
}


bool
SimCenterComponentSelection::addComponent(QString text, QWidget *theWidget)
{
    if (textIndices.indexOf(text) == -1) {
        QStandardItem *theItem = new QStandardItem(text);
        rootNode->appendRow(theItem);
        QModelIndex modelIndex = theItem->index();
        theStackedWidget->addWidget(theWidget);
        textIndices.append(text);
        modelIndices.append(modelIndex);
        theWidget->setObjectName(text);
        return true;
    } else
        qDebug() << "ComponentSelection: text: " << text << " option already exists";

    return false;
}

void SimCenterComponentSelection::currentChangedSlot(const QModelIndex &current,
                                                     const QModelIndex & /*previous*/)
{
    // Only transition if left mouse button is currently pressed.
    // (ignores hover, keyboard navigation, and programmatic changes)
    if (!(QApplication::mouseButtons() & Qt::LeftButton))
        return;

    if (!current.isValid()) return;

    const QString selectedText = current.data(Qt::DisplayRole).toString();
    const int stackIndex = textIndices.lastIndexOf(selectedText);
    if (stackIndex == -1) return;

    if (auto *currW = qobject_cast<SimCenterAppWidget*>(theStackedWidget->currentWidget()))
        currW->setCurrentlyViewable(false);

    if (auto *anim = qobject_cast<AnimatedStackedWidget*>(theStackedWidget)) {
        const int dir = (stackIndex > anim->currentIndex()) ? -1 : +1;
        anim->setCurrentIndexAnimated(stackIndex, dir);
    } else {
        theStackedWidget->setCurrentIndex(stackIndex);
    }

    if (auto *newW = qobject_cast<SimCenterAppWidget*>(theStackedWidget->currentWidget()))
        newW->setCurrentlyViewable(true);
}

void
SimCenterComponentSelection::selectionChangedSlot(const QItemSelection &, const QItemSelection &)
{
    //
    // get the text of the selected item
    //

    const QModelIndex index = treeView->selectionModel()->currentIndex();
    QString selectedText = index.data(Qt::DisplayRole).toString();

    //
    // find text in list
    //

    int stackIndex = textIndices.lastIndexOf(selectedText);

    //
    // get stacked widget to display current if of course it exists
    //

    if (stackIndex != -1) {
        if (auto *animStack = qobject_cast<AnimatedStackedWidget*>(theStackedWidget)) {
            // determine slide direction by comparing indices
            int dir = (stackIndex > theStackedWidget->currentIndex()) ? -1 : +1;

            // mark old widget not viewable
            if (auto *curr = qobject_cast<SimCenterAppWidget*>(theStackedWidget->currentWidget()))
                curr->setCurrentlyViewable(false);

            animStack->setCurrentIndexAnimated(stackIndex, dir);

            // mark new widget viewable after slide animation completes
            // (use a single-shot to ensure it's after currentIndex changes)
            QTimer::singleShot(animStack->property("durationMs").toInt(), this, [=](){
                if (auto *nw = qobject_cast<SimCenterAppWidget*>(theStackedWidget->currentWidget()))
                    nw->setCurrentlyViewable(true);
            });
        } else {
            // fallback (shouldn’t happen)
            theStackedWidget->setCurrentIndex(stackIndex);
        }
    }
}

QWidget *
SimCenterComponentSelection::swapComponent(QString text, QWidget *theWidget)
{
    QWidget *theRes = NULL;

    //
    // find text iin list
    //

    int index = textIndices.indexOf(text);
    //
    // get stacked widget to display current if of course it exists
    //

    if (index != -1) {
        theRes=theStackedWidget->widget(index);
        if (theRes != NULL) {
            theStackedWidget->removeWidget(theRes);
        }
       theStackedWidget->insertWidget(index, theWidget);
    }
    return theRes;
}


QString
SimCenterComponentSelection::selectedComponentText(void)
{
    auto currIndex = theStackedWidget->currentIndex();

    if (currIndex < 0 || currIndex > textIndices.size() -1)
        return QString();

    return textIndices.at(currIndex);
}


bool
SimCenterComponentSelection::displayComponent(QString text)
{
    //
    // find index of text in list and display corresponding widget if index found
    //

    int index = textIndices.indexOf(text);

    if (index != -1) {

        QModelIndex index1 = modelIndices.at(index);
        treeView->setCurrentIndex(index1);
        theStackedWidget->setCurrentIndex(index);
        return true;
    }

    return false;
}

bool
SimCenterComponentSelection::displayComponent(int index)
{
    //
    // find index of text in list and display corresponding widget if index found
    //

    if (index >= 0 && index <= modelIndices.size()-1) {

        QModelIndex index1 = modelIndices.at(index);
        treeView->setCurrentIndex(index1);
        theStackedWidget->setCurrentIndex(index);
        return true;
    }

    return false;
}

void
SimCenterComponentSelection::setWidth(const int width)
{
    treeView->setMaximumWidth(width);
    treeView->setMinimumWidth(width);
}

void
SimCenterComponentSelection::setItemWidthHeight(const int width, const int height)
{
    auto customModel = dynamic_cast<CustomizedItemModel*>(standardModel);

    if(customModel)
    {
        customModel->setItemWidthHeight(width,height);
    }
}
