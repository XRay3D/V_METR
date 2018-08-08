#include "layoutmodel.h"
#include <QDebug>

LayoutModel::LayoutModel(MyTableModel* m, QObject* parent)
    : m_m(m)
    , QAbstractTableModel(parent)
{
}

LayoutModel::~LayoutModel()
{
}

int LayoutModel::rowCount(const QModelIndex& /*parent*/) const
{
    return 5;
}

int LayoutModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 12;
}

QVariant LayoutModel::data(const QModelIndex& index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        return m_m->getData(2, index.column()  + index.row()*12);
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    default:
        break;
    }
    return QVariant();
}
