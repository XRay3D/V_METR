#include "tableview.h"

#include <QApplication>
#include <QHeaderView>
#include <QHideEvent>

TableView::TableView(QWidget* parent)
    : QTableView(parent)
{
    resize(1200, 300);
    setItemDelegate(new CellWidget);
}

void TableView::hideEvent(QHideEvent* event)
{
    event->accept();
    deleteLater();
}

void TableView::showEvent(QShowEvent* event)
{
    for (int i = 0; i < model()->columnCount(); ++i) {
        setColumnWidth(i, (width() - verticalHeader()->width()) / model()->columnCount());
    }
    for (int i = 0; i < model()->rowCount(); ++i) {
        setRowHeight(i, (height() - horizontalHeader()->height()) / model()->rowCount());
    }
}
