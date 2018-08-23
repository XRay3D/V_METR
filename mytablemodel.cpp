#include "mytablemodel.h"
#include <QDebug>
#include <QFile>
#include <QTableView>

MyTableModel::MyTableModel(QObject* parent)
    : QAbstractTableModel(parent)
    , m_temperature{ 0.0, 0.0 }
{
    setObjectName("ModelCoefficients");

    memset(m_data, 0, sizeof(m_data));

    QFile file("data.bin");
    if (!file.open(QIODevice::ReadOnly))
        return;

    QByteArray d(file.readAll());

    memcpy(m_data, d.constData(), sizeof(m_data));
    //    for (int ch = 0; ch < 64; ++ch) {
    //        calcPpm(ch);
    //    }
}

MyTableModel::~MyTableModel()
{
    QFile file("data.bin");

    if (!file.open(QIODevice::WriteOnly))
        return;

    file.write(reinterpret_cast<const char*>(m_data), sizeof(m_data));
}

int MyTableModel::rowCount(const QModelIndex& /*parent*/) const
{
    return 64;
}

int MyTableModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 3; //m_columnCount;
}

QVariant MyTableModel::data(const QModelIndex& index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        return m_data[index.column()][index.row()];
    case Qt::BackgroundColorRole: {
        if (index.column() != 2)
            return QVariant();
        int h = 120 - m_data[2][index.row()];
        int s = 255;
        if (h < 0)
            h = 0;
        else if (h > 240)
            h = 240;
        return QColor::fromHsv(h, s, 255);
    }
    case Qt::TextColorRole:
        if (index.column() == 2)
            if (m_data[2][index.row()] < -80)
                return QColor(Qt::white);
        return QVariant();
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    default:
        break;
    }
    return QVariant();
}

//void MyTableModel::setColumnCount(int /*columnCount*/)
//{
//    if (m_columnCount == columnCount) {
//        return;
//    } else if (m_columnCount > columnCount) {
//        beginRemoveColumns(QModelIndex(), columnCount, m_columnCount - 1);
//        m_columnCount = columnCount;
//        endRemoveColumns();
//    } else {
//        beginInsertColumns(QModelIndex(), m_columnCount, columnCount - 1);
//        m_columnCount = columnCount;
//        endInsertColumns();
//    }
//}

double MyTableModel::getData(int num, int ch)
{
    return m_data[num][ch];
}

void MyTableModel::setData(int num, int ch, double val)
{
    const QModelIndex& index1 = createIndex(ch, num);
    const QModelIndex& index2 = createIndex(2, num);
    m_data[num][ch] = val;
    calcPpm(ch);
    dataChanged(index1, index2, { Qt::DisplayRole });
    reinterpret_cast<QTableView*>(parent())->scrollTo(index1);
}

void MyTableModel::setTemp1(double val)
{
    if (qFuzzyCompare(m_temperature.first, val))
        return;

    m_temperature.first = val;

    for (int ch = 0; ch < 64; ++ch) {
        calcPpm(ch);
    }

    const QModelIndex& index1 = createIndex(2, 0);
    const QModelIndex& index2 = createIndex(2, 63);
    dataChanged(index1, index2, { Qt::DisplayRole });
}

void MyTableModel::setTemp2(double val)
{
    if (qFuzzyCompare(m_temperature.second, val))
        return;

    m_temperature.second = val;

    for (int ch = 0; ch < 64; ++ch) {
        calcPpm(ch);
    }

    const QModelIndex& index1 = createIndex(2, 0);
    const QModelIndex& index2 = createIndex(2, 63);
    dataChanged(index1, index2, { Qt::DisplayRole });
}

QVariant MyTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        if (orientation == Qt::Horizontal)
            return QStringList({ "T1", "T2", "PPM" }).value(section);
        else
            return section + 1;
    default:
        break;
    }

    return QVariant();
}

void MyTableModel::calcPpm(int ch)
{
    if (!qFuzzyIsNull(m_data[0][ch])
        && !qFuzzyIsNull(m_data[1][ch])
        && !qFuzzyIsNull(m_temperature.first)
        && !qFuzzyIsNull(m_temperature.second)) {
        m_data[2][ch] = ((m_data[1][ch] - m_data[0][ch]) / (m_data[0][ch] * (m_temperature.second - m_temperature.first))) * 1000000.0;
    } else {
        m_data[2][ch] = 0.0;
    }
}
