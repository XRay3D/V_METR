#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QApplication>
#include <QItemDelegate>
#include <QPainter>
#include <QTableView>

class CellWidget : public QItemDelegate {
    Q_OBJECT
public:
    CellWidget() {}
    virtual ~CellWidget() {}

    // QAbstractItemDelegate interface
public:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        painter->save();

        QRect r(option.rect);
        r -= QMargins(1, 1, 1, 1);

        QFont font;
        painter->setBrush(Qt::NoBrush);
        if (option.state & QStyle::State_Selected) {
            painter->setPen(QPen(Qt::red, 2, Qt::SolidLine, Qt::SquareCap));
            painter->drawRect(r);
            font.setPointSize(18);
            font.setBold(true);
        } else {
            painter->setPen(Qt::black);
            font.setPointSize(15);
        }
        QString str;
        double val = index.data().toDouble();
        for (int i = -60; i < 60; i += 10) {
            if (i < val && val < i + 10) {
                str = QString("%1...%2").arg(i).arg(i + 10);
                break;
            }
        }

        painter->setFont(font);

        if (str.isEmpty())
            str = "OOR";

        if (val < 0)
            painter->setPen(Qt::blue);
        else
            painter->setPen(Qt::red);

        painter->drawText(r, str, QTextOption(Qt::AlignTop | Qt::AlignHCenter));

        painter->restore();
        str = QString::number((index.column() + index.row() * 12) + 1);
        painter->drawText(r, str, QTextOption(Qt::AlignCenter));

        painter->drawText(r, QString::number(index.data().toDouble()), QTextOption(Qt::AlignBottom | Qt::AlignHCenter));

        //        else

        //        QStyleOption;
        //        //        if (index.column() == 1) {
        //        int progress = index.data().toDouble();
        //        QStyleOptionProgressBar progressBarOption;
        //        progressBarOption.rect = option.rect;
        //        progressBarOption.minimum = 0;
        //        progressBarOption.maximum = 100;
        //        progressBarOption.progress = progress;
        //        progressBarOption.text = QString::number(progress) + "%";
        //        progressBarOption.textVisible = true;
        //        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
        //        //        } else
        //        //        QStyledItemDelegate::paint(painter, option, index);
    }
};

class TableView : public QTableView {
    Q_OBJECT
public:
    explicit TableView(QWidget* parent = nullptr);

signals:

public slots:

    // QWidget interface
protected:
    void hideEvent(QHideEvent* event) override;
    //    void showEvent(QShowEvent* event) override;
};

#endif // TABLEVIEW_H
