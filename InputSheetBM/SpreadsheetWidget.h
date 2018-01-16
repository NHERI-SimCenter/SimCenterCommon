#ifndef SPREADSHEETWIDGET_H
#define SPREADSHEETWIDGET_H

#include <QTableWidget>


#define SIMPLESPREADSHEET_QString 0
#define SIMPLESPREADSHEET_QDouble 1
#define SIMPLESPREADSHEET_QInt    2

class Cell;
class SpreadsheetCompare;

class SpreadsheetWidget : public QTableWidget
{
    Q_OBJECT

public:
    SpreadsheetWidget(int colCount, int rowCount, QStringList, QList<int>, QWidget *parent = 0);

    bool autoRecalculate() const { return autoRecalc; }
    QString currentLocation() const;
    QString currentFormula() const;
    QTableWidgetSelectionRange selectedRange() const;
    void clear();
    bool readFile(const QString &fileName);
    bool writeFile(const QString &fileName);
    void sort(const SpreadsheetCompare &compare);

    void outputToJSON(QJsonArray &rvArray);
    void inputFromJSON(QJsonArray &rvArray);

    int getNumRows();
    int getNumColumns();
    bool getString(int row, int col, QString &);
    bool getDouble(int row, int col, double &);
    bool getInt(int row, int col, int &);
    int  setString(int row,int col, QString &);
    int  setDouble(int row, int col, double);
    int  setInt(int row, int col, int);


public slots:
    void cut();
    void copy();
    void paste();
    void del();
    void selectCurrentRow();
    void selectCurrentColumn();
    void recalculate();
    void setAutoRecalculate(bool recalc);
    void findNext(const QString &str, Qt::CaseSensitivity cs);
    void findPrevious(const QString &str, Qt::CaseSensitivity cs);

signals:
    void modified();

private slots:
    void somethingChanged();

private:
    enum { MagicNumber = 0x7F51C883, RowCount = 999, ColumnCount = 26 };

    int numRow;
    int numCol;

    QStringList theHeadings;
    QList<int>  dataTypes;

    Cell *cell(int row, int column) const;
    QString text(int row, int column) const;
    QString formula(int row, int column) const;
    void setFormula(int row, int column, const QString &formula);

    bool autoRecalc;
};

class SpreadsheetCompare
{
public:
    bool operator()(const QStringList &row1,
                    const QStringList &row2) const;

    enum { KeyCount = 3 };
    int keys[KeyCount];
    bool ascending[KeyCount];
};

#endif // SPREADSHEETWIDGET_H
