#include <QtGui>

#include "Cell.h"
#include "SpreadsheetWidget.h"
#include <QMessageBox>
#include <QApplication>

SpreadsheetWidget::SpreadsheetWidget(int colCount, int rowCount, QStringList head, QList<int>types, QWidget *parent)
    : QTableWidget(parent), numRow(rowCount), numCol(colCount), theHeadings(head), dataTypes(types)
{
    clear();

    this->setRowCount(rowCount);
    this->setColumnCount(colCount);
    this->setHorizontalHeaderLabels(head);

    autoRecalc = true;

    setItemPrototype(new Cell);
    setSelectionMode(ContiguousSelection);

    connect(this, SIGNAL(itemChanged(QTableWidgetItem *)),
            this, SLOT(somethingChanged()));

    this->setHorizontalHeaderLabels(head);
}

QString SpreadsheetWidget::currentLocation() const
{
    return QChar('A' + currentColumn())
           + QString::number(currentRow() + 1);
}

QString SpreadsheetWidget::currentFormula() const
{
    return formula(currentRow(), currentColumn());
}

QTableWidgetSelectionRange SpreadsheetWidget::selectedRange() const
{
    QList<QTableWidgetSelectionRange> ranges = selectedRanges();
    if (ranges.isEmpty())
        return QTableWidgetSelectionRange();

    qDebug() << "ranges size: " + QString::number(ranges.count()) ;
    return ranges.first();
}

void SpreadsheetWidget::clear()
{
    this->clearContents();
    setCurrentCell(0, 0);
}

bool SpreadsheetWidget::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("SpreadsheetWidget"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_3);

    quint32 magic;
    in >> magic;
    if (magic != MagicNumber) {
        QMessageBox::warning(this, tr("SpreadsheetWidget"),
                             tr("The file is not a SpreadsheetWidget file."));
        return false;
    }

    clear();

    quint16 row;
    quint16 column;
    QString str;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    while (!in.atEnd()) {
        in >> row >> column >> str;
        setFormula(row, column, str);
    }
    QApplication::restoreOverrideCursor();
    return true;
}

bool SpreadsheetWidget::writeFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("SpreadsheetWidget"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_3);

    out << quint32(MagicNumber);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (int row = 0; row < RowCount; ++row) {
        for (int column = 0; column < ColumnCount; ++column) {
            QString str = formula(row, column);
            if (!str.isEmpty())
                out << quint16(row) << quint16(column) << str;
        }
    }
    QApplication::restoreOverrideCursor();
    return true;
}

void SpreadsheetWidget::sort(const SpreadsheetCompare &compare)
{
    QList<QStringList> rows;
    QTableWidgetSelectionRange range = selectedRange();
    int i;

    for (i = 0; i < range.rowCount(); ++i) {
        QStringList row;
        for (int j = 0; j < range.columnCount(); ++j)
            row.append(formula(range.topRow() + i,
                               range.leftColumn() + j));
        rows.append(row);
    }

    qStableSort(rows.begin(), rows.end(), compare);

    for (i = 0; i < range.rowCount(); ++i) {
        for (int j = 0; j < range.columnCount(); ++j)
            setFormula(range.topRow() + i, range.leftColumn() + j,
                       rows[i][j]);
    }

    clearSelection();
    somethingChanged();
}

void SpreadsheetWidget::cut()
{
    copy();
    del();
}

void SpreadsheetWidget::copy()
{
    qDebug() << "";
    qDebug() << "";
    qDebug() << "--------- NEW COPY ------------";
    QTableWidgetSelectionRange range = selectedRange();
    QString str;

    qDebug() << "range rows: " + QString::number(range.rowCount()) + "cols: " + QString::number(range.columnCount()) ;

    for (int i = 0; i < range.rowCount(); ++i) {
        if (i > 0)
            str += "\n";
        for (int j = 0; j < range.columnCount(); ++j) {
            if (j > 0)
                str += "\t";
            str += formula(range.topRow() + i, range.leftColumn() + j);
        }
    }
    qDebug() << "copied: " + str;
    QApplication::clipboard()->setText(str);
}

void SpreadsheetWidget::paste()
{
    qDebug() << "--------------------------";
    QTableWidgetSelectionRange range = selectedRange();


    qDebug() << "paste range rows: " + QString::number(range.rowCount()) + "cols: " + QString::number(range.columnCount()) ;

    QString str = QApplication::clipboard()->text();
    qDebug() << "paste : " + str;



    QStringList rows = str.split('\n');
    int numRows = rows.count();
    int numColumns = rows.first().count('\t') + 1;

    qDebug() << "clipboard rows: " + QString::number(numRows) + "cols: " + QString::number(numColumns) ;

    if (range.rowCount() * range.columnCount() != 1
            && (range.rowCount() != numRows
                || range.columnCount() != numColumns)) {
        QMessageBox::information(this, tr("SpreadsheetWidget"),
                tr("The information cannot be pasted because the copy "
                   "and paste areas aren't the same size."));
        return;
    }

    for (int i = 0; i < numRows; ++i) {
        QStringList columns = rows[i].split('\t');
        for (int j = 0; j < numColumns; ++j) {
            int row = range.topRow() + i;
            int column = range.leftColumn() + j;
            if (row < RowCount && column < ColumnCount)
                setFormula(row, column, columns[j]);
        }
    }
    somethingChanged();
}

void SpreadsheetWidget::del()
{
    QList<QTableWidgetItem *> items = selectedItems();
    if (!items.isEmpty()) {
        foreach (QTableWidgetItem *item, items)
            delete item;
        somethingChanged();
    }
}

void SpreadsheetWidget::selectCurrentRow()
{
    selectRow(currentRow());
}

void SpreadsheetWidget::selectCurrentColumn()
{
    selectColumn(currentColumn());
}

void SpreadsheetWidget::recalculate()
{
    for (int row = 0; row < RowCount; ++row) {
        for (int column = 0; column < ColumnCount; ++column) {
            if (cell(row, column))
                cell(row, column)->setDirty();
        }
    }
    viewport()->update();
}

void SpreadsheetWidget::setAutoRecalculate(bool recalc)
{
    autoRecalc = recalc;
    if (autoRecalc)
        recalculate();
}

void SpreadsheetWidget::findNext(const QString &str, Qt::CaseSensitivity cs)
{
    int row = currentRow();
    int column = currentColumn() + 1;

    while (row < RowCount) {
        while (column < ColumnCount) {
            if (text(row, column).contains(str, cs)) {
                clearSelection();
                setCurrentCell(row, column);
                activateWindow();
                return;
            }
            ++column;
        }
        column = 0;
        ++row;
    }
    QApplication::beep();
}

void SpreadsheetWidget::findPrevious(const QString &str,
                               Qt::CaseSensitivity cs)
{
    int row = currentRow();
    int column = currentColumn() - 1;

    while (row >= 0) {
        while (column >= 0) {
            if (text(row, column).contains(str, cs)) {
                clearSelection();
                setCurrentCell(row, column);
                activateWindow();
                return;
            }
            --column;
        }
        column = ColumnCount - 1;
        --row;
    }
    QApplication::beep();
}

void SpreadsheetWidget::somethingChanged()
{
    if (autoRecalc)
        recalculate();
    emit modified();
}

Cell *SpreadsheetWidget::cell(int row, int column) const
{
    return static_cast<Cell *>(item(row, column));
}

void SpreadsheetWidget::setFormula(int row, int column,
                             const QString &formula)
{
    Cell *c = cell(row, column);
    if (!c) {
        c = new Cell;
        setItem(row, column, c);
    }
    c->setFormula(formula);
}

QString SpreadsheetWidget::formula(int row, int column) const
{
    Cell *c = cell(row, column);
    if (c) {
        return c->formula();
    } else {
        return "";
    }
}

QString SpreadsheetWidget::text(int row, int column) const
{
    Cell *c = cell(row, column);
    if (c) {
        return c->text();
    } else {
        return "";
    }
}


int  SpreadsheetWidget::getNumRows(){
  return RowCount;
}
int SpreadsheetWidget::getNumColumns(){
  return ColumnCount;
}





bool SpreadsheetWidget::getString(int row, int col, QString &res){

  //QTableWidgetItem *theItem = this->item(row,col)->clone();
  Cell *theItem = cell(row, col);
  if (theItem == 0)
    return false;
  res = theItem->text();
  return true;
}

bool SpreadsheetWidget::getDouble(int row, int col, double &res){

  //QTableWidgetItem *theItem = this->item(row,col)->clone();
  Cell *theItem = cell(row, col);
  if (theItem == 0)
    return false;
  QString textData = theItem->text();
  res = textData.toDouble();
  return true;
}

bool SpreadsheetWidget::getInt(int row, int col, int &res){
  //QTableWidgetItem *theItem = this->item(row,col)->clone();
  Cell *theCell = cell(row, col);
  if (theCell == 0)
    return false;
  QString textData = theCell->text();
  res = textData.toInt();
  return true;
}

int  SpreadsheetWidget::setString(int row,int col, QString &data)
{
    //QTableWidgetItem *theCell = this->item(row, col)->clone();
    Cell *theCell = cell(row, col);
    if(!theCell)
    {
        theCell = new Cell;
        this->setItem(row, col, theCell);
    }
    theCell->setText(data);
    return 0;
}

int  SpreadsheetWidget::setDouble(int row, int col, double data)
{
    //QTableWidgetItem *theCell = this->item(row, col)->clone();
    Cell *theCell = cell(row, col);
    if(!theCell)
    {
        theCell = new Cell;
        this->setItem(row, col, theCell);
    }
    theCell->setText(QString::number(data));
    return 0;
}

int  SpreadsheetWidget::setInt(int row, int col, int data)
{
    //QTableWidgetItem *theCell = this->item(row, col)->clone();
    Cell *theCell = cell(row, col);
    if(!theCell)
    {
        theCell = new Cell;
        this->setItem(row, col, theCell);
    }
    theCell->setText(QString::number(data));
    return 0;
}



void SpreadsheetWidget::outputToJSON(QJsonArray &rvArray)
{
   // QJsonArray rvArray;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (int row = 0; row < numRow; ++row) {
        //QTableWidgetItem *firstItem= this->item(row, 0)->clone();
        Cell *firstItem = cell(row, 0);
        //QModelIndex = this->it
        if (firstItem != 0) {
            QString firstItemString =firstItem->text();
            qDebug() << firstItemString;
        if (!firstItemString.isEmpty()) {
          QJsonObject obj;
          for (int column = 0; column < numCol; ++column) {
               //QTableWidgetItem *theItem = this->item(row,column)->clone();
              Cell *theItem = cell(row, column);
               if (theItem == 0)
                   break;
               QString textData = theItem->text();
               if (dataTypes.at(column) == SIMPLESPREADSHEET_QString)
                    obj[theHeadings.at(column)]=textData;
               else if (dataTypes.at(column) == SIMPLESPREADSHEET_QDouble) {
                   // QString textPrecision = QString("%1").arg(textData, 0, 'g', 13);
                    obj[theHeadings.at(column)]=textData.toDouble();
               } else if (dataTypes.at(column) == SIMPLESPREADSHEET_QInt)
                       obj[theHeadings.at(column)]=textData.toInt();

           }
           rvArray.append(obj);
        }

        }
    }
    QApplication::restoreOverrideCursor();
 }

void SpreadsheetWidget::inputFromJSON(QJsonArray &rvArray){

}



bool SpreadsheetCompare::operator()(const QStringList &row1,
                                    const QStringList &row2) const
{
    for (int i = 0; i < KeyCount; ++i) {
        int column = keys[i];
        if (column != -1) {
            if (row1[column] != row2[column]) {
                if (ascending[i]) {
                    return row1[column] < row2[column];
                } else {
                    return row1[column] > row2[column];
                }
            }
        }
    }
    return false;
}
