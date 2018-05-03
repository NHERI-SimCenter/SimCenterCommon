#ifndef CELL_H
#define CELL_H

#include <QTableWidgetItem>

// Abstraction class for a spreadsheet cell.
// Implemented to support formulas

class Cell : public QTableWidgetItem
{
public:
    Cell();

    QTableWidgetItem *clone() const;
    void setData(int role, const QVariant &value);
    QVariant data(int role) const;
    void setFormula(const QString &formula);
    QString formula() const;
    void setDirty();

private:
    QVariant value() const;

    mutable QVariant cachedValue;
    mutable bool cacheIsDirty;
};

#endif // CELL_H
