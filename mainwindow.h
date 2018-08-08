#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include <QAxObject>
#include <QDate>
#include <QEvent>
#include <QKeyEvent>
#include <QMainWindow>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QTimer>
namespace Ui {
class MainWindow;
}

class MyTableModel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QMainWindow* parent = 0);
    ~MainWindow();
    //    QAxObject* excel;
    //    QAxObject* workbooks;
    //    QAxObject* workbook;
    //    QAxObject* worksheet;
    //    QAxObject* range;
    //    QAxObject* cells;
    //    void Cells(int Row, int Col, QVariant Val);

private:
    MyTableModel* model();
    QTimer m_timer;
    QTimer m_resizeTimer;
    Ui::MainWindow* ui;
    void ReadSettings();
    void WriteSettings();
    void setValue(double val);
    int m_ch = 0;
    QTime m_time;

protected:
    void resizeEvent(QResizeEvent* event);


    // QWidget interface
protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
private slots:
    void on_pbLayout_clicked();
};

#endif // MAINWINDOW_H
