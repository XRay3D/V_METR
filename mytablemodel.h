#ifndef MYTABLEMODEL_H
#define MYTABLEMODEL_H

#include <QAbstractTableModel>

class MyTableModel : public QAbstractTableModel {
public:
    MyTableModel(QObject* parent = Q_NULLPTR);
    ~MyTableModel();

    //    void setColumnCount(int columnCount);
    double getData(int num, int ch);
    void setData(int num, int ch, double val);
    void setTemp1(double val);
    void setTemp2(double val);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    int m_columnCount = 1;
    double m_data[3][64];
    QPair<double, double> m_temperature;
    void calcPpm(int ch);
};

#endif // MYTABLEMODEL_H
