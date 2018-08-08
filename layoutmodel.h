#ifndef LAYOUTMODEL_H
#define LAYOUTMODEL_H

#include "mytablemodel.h"

#include <QAbstractTableModel>

class LayoutModel : public QAbstractTableModel {
public:
    LayoutModel(MyTableModel* m, QObject* parent = Q_NULLPTR);
    ~LayoutModel();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;

private:
    MyTableModel* m_m;
};

#endif // LAYOUTMODEL_H
