#ifndef CUSTOMIZEDITEMMODEL_H
#define CUSTOMIZEDITEMMODEL_H

#include <QStandardItemModel>

class CustomizedItemModel : public QStandardItemModel
{
    Q_OBJECT
public:
    CustomizedItemModel();
    QVariant data(const QModelIndex &index, int role) const;

    void setItemWidthHeight(const int width, const int height);

signals:

public slots:

private:

    int itemWidth;
    int itemHeight;
};

#endif // CUSTOMIZEDITEMMODEL_H
