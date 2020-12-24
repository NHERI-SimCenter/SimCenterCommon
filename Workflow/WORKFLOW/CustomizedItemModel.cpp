#include "CustomizedItemModel.h"



// this is a wrapper class to customize the appearance of the treeView

CustomizedItemModel::CustomizedItemModel()
{
    itemWidth = 20;
    itemHeight = 50;
}

QVariant
CustomizedItemModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    } else if (role == Qt::SizeHintRole) {
        return QSize(itemWidth, itemHeight);
    }else
    {
        return QStandardItemModel::data(index, role);
    }
}

void
CustomizedItemModel::setItemWidthHeight(const int width, const int height)
{
    itemWidth = width;
    itemHeight = height;
}
