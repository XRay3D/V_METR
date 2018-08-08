#include "mainwindow.h"
#include "interface.h"
#include "layoutmodel.h"
#include "mytablemodel.h"
#include "tableview.h"
#include "ui_mainwindow.h"
#include <QClipboard>
#include <QDebug>
#include <QSerialPortInfo>
#include <QSettings>
#include <hw/mi.h>

//const unsigned char rel[][2] = {
//    { 0x00, 0x00 }, /*00*/
//    { 0x7F, 0x7F }, /*01*/
//    { 0xBF, 0x7F }, /*02*/
//    { 0xDF, 0x7F }, /*03*/
//    { 0xEF, 0x7F }, /*04*/
//    { 0xF7, 0x7F }, /*05*/
//    { 0xFB, 0x7F }, /*06*/
//    { 0xFD, 0x7F }, /*07*/
//    { 0xFE, 0x7F }, /*08*/
//    { 0xFF, 0x3F }, /*09*/
//    { 0xFF, 0x5F }, /*10*/
//    { 0xFF, 0x6F }, /*11*/
//    { 0xFF, 0x77 }, /*12*/
//    { 0xFF, 0x7B }, /*13*/
//    { 0xFF, 0x7D }, /*14*/
//    { 0xFF, 0x7E }, /*15*/
//    { 0x80, 0x80 }, /*16*/
//    { 0x40, 0x80 }, /*17*/
//    { 0x20, 0x80 }, /*18*/
//    { 0x10, 0x80 }, /*19*/
//    { 0x08, 0x80 }, /*20*/
//    { 0x04, 0x80 }, /*21*/
//    { 0x02, 0x80 }, /*22*/
//    { 0x01, 0x80 }, /*23*/
//    { 0x00, 0xC0 }, /*24*/
//    { 0x00, 0xA0 }, /*25*/
//    { 0x00, 0x90 }, /*26*/
//    { 0x00, 0x88 }, /*27*/
//    { 0x00, 0x84 }, /*28*/
//    { 0x00, 0x82 }, /*29*/
//    { 0x7F, 0x7F }, /*01*/
//    { 0xBF, 0x7F }, /*02*/
//    { 0xDF, 0x7F }, /*03*/
//    { 0xEF, 0x7F }, /*04*/
//    { 0xF7, 0x7F }, /*05*/
//    { 0xFB, 0x7F }, /*06*/
//    { 0xFD, 0x7F }, /*07*/
//    { 0xFE, 0x7F }, /*08*/
//    { 0xFF, 0x3F }, /*09*/
//    { 0xFF, 0x5F }, /*10*/
//    { 0xFF, 0x6F }, /*11*/
//    { 0xFF, 0x77 }, /*12*/
//    { 0xFF, 0x7B }, /*13*/
//    { 0xFF, 0x7D }, /*14*/
//    { 0xFF, 0x7E }, /*15*/
//    { 0x80, 0x80 }, /*16*/
//    { 0x40, 0x80 }, /*17*/
//    { 0x20, 0x80 }, /*18*/
//    { 0x10, 0x80 }, /*19*/
//    { 0x08, 0x80 }, /*20*/
//    { 0x04, 0x80 }, /*21*/
//    { 0x02, 0x80 }, /*22*/
//    { 0x01, 0x80 }, /*23*/
//    { 0x00, 0xC0 }, /*24*/
//    { 0x00, 0xA0 }, /*25*/
//    { 0x00, 0x90 }, /*26*/
//    { 0x00, 0x88 }, /*27*/
//    { 0x00, 0x84 }, /*28*/
//    { 0x00, 0x82 }, /*29*/
//};

MainWindow::MainWindow(QMainWindow* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableView->setModel(new MyTableModel(ui->tableView));
    connect(&m_resizeTimer, &QTimer::timeout, [&] { ui->tableView->resizeRowsToContents(); });

    //    TiRead = new QTimer(this);
    //    TiChanel = new QTimer(this);
    //    connect(TiRead, SIGNAL(timeout()), SLOT(Read()));
    //    connect(TiChanel, SIGNAL(timeout()), SLOT(Cahel()));

    using dsbx = void (QDoubleSpinBox::*)(double);
    connect(ui->dsbxTemp1, static_cast<dsbx>(&QDoubleSpinBox::valueChanged), model(), &MyTableModel::setTemp1);
    connect(ui->dsbxTemp2, static_cast<dsbx>(&QDoubleSpinBox::valueChanged), model(), &MyTableModel::setTemp2);

    using sbx = void (QSpinBox::*)(int);
    connect(ui->sbxChannel, static_cast<sbx>(&QSpinBox::valueChanged), mi::com, &Commutator::setChannel);

    connect(&m_timer, &QTimer::timeout, mi::irt, &Irt::getVal);
    connect(mi::irt, &Irt::Val, this, &MainWindow::setValue);

    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts())
        ui->cbxPort->addItem(info.portName());

    connect(ui->pbPing, &QPushButton::clicked, [&] {
        m_timer.stop();
        bool ok = mi::irt->ping(ui->cbxPort->currentText()) && mi::com->ping(ui->cbxPort->currentText());
        ui->gbxMeasure->setEnabled(ok);
        if (ok)
            m_timer.start(500);
    });

    connect(ui->pbAuto, &QPushButton::toggled, [&](bool checked) {
        if (checked) {
            m_ch = ui->sbxFirstChanel->value();
            mi::com->setAuto();
            mi::com->setChannel(m_ch);
            ui->sbxChannel->setValue(m_ch);
            m_time.start();
        }
    });

    ReadSettings();

    //	excel = new QAxObject( "Excel.Application", this );
    //	excel->dynamicCall( "SetSheetsInNewWorkbook(int)", QVariant( 1 ) );
    //	excel->dynamicCall( "SetDisplayAlerts", QVariant( false ) );
    //	workbooks = excel->querySubObject( "Workbooks" );
    //	workbook = workbooks->querySubObject( "Add()" );
    //	worksheet = workbook->querySubObject( "ActiveSheet" );
    //	worksheet->dynamicCall( "Name", "Data sheeet" );
    //	excel->dynamicCall( "SetVisible(bool)", QVariant( true ) );
    //	Cells(11,4,"I, мА=");
    //	Cells(11,5,0.01);
    //	Cells(11,3,123);
}

MainWindow::~MainWindow()
{
    WriteSettings();
    delete ui;
}

//void MainWindow::Cells(int Row, int Col, QVariant Val)
//{
//    static QTextCodec* codec = QTextCodec::codecForName("Windows-1251");
//    QString C, R;
//    char* c = new char[2];
//    c[0] = Col + 'A' - 1;
//    R.setNum(Row);
//    C = codec->toUnicode(c, 1);
//    C = C + R + ':' + C + R;
//    range = worksheet->querySubObject("Range(const QVariant&)", QVariant(C));
//    range->dynamicCall("SetValue(const QVariant&)", Val);
//}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    m_resizeTimer.start(300);
    //    int i;
    //    i = (ui->Table->width() - 48) / 3;
    //    ui->Table->setColumnWidth(0, i);
    //    ui->Table->setColumnWidth(1, i);
    //    ui->Table->setColumnWidth(2, i);
}

void MainWindow::ReadSettings()
{
    QSettings settings;
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/state").toByteArray());
    ui->cbxPort->setCurrentIndex(settings.value("MainWindow/comboBox").toInt());
    ui->sbxFirstChanel->setValue(settings.value("MainWindow/sbxFirstChanel").toInt());
    ui->sbxLastChanel->setValue(settings.value("MainWindow/sbxLastChanel").toInt());
    ui->sbxTime->setValue(settings.value("MainWindow/sbxTime").toInt());
    ui->sbxCurrentMeasure->setValue(settings.value("MainWindow/sbxCurrentMeasure").toInt());
    ui->dsbxTemp1->setValue(settings.value("MainWindow/dsbxTemp1").toInt());
    ui->dsbxTemp2->setValue(settings.value("MainWindow/dsbxTemp2").toInt());
}

void MainWindow::WriteSettings()
{
    QSettings settings;
    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/state", saveState());
    settings.setValue("MainWindow/comboBox", ui->cbxPort->currentIndex());
    settings.setValue("MainWindow/sbxFirstChanel", ui->sbxFirstChanel->value());
    settings.setValue("MainWindow/sbxLastChanel", ui->sbxLastChanel->value());
    settings.setValue("MainWindow/sbxTime", ui->sbxTime->value());
    settings.setValue("MainWindow/sbxCurrentMeasure", ui->sbxCurrentMeasure->value());
    settings.setValue("MainWindow/dsbxTemp1", ui->dsbxTemp1->value());
    settings.setValue("MainWindow/dsbxTemp2", ui->dsbxTemp2->value());
}

void MainWindow::setValue(double val)
{
    ui->dsbxValue->setValue(val);

    if (ui->pbAuto->isChecked()) {
        if (m_time.elapsed() > ui->sbxTime->value() * 1000) {
            model()->setData(ui->sbxCurrentMeasure->value() - 1, m_ch - 1, ui->dsbxValue->value());
            if (++m_ch > ui->sbxLastChanel->value()) {
                ui->pbAuto->setChecked(false);
                return;
            } else {
                ui->sbxChannel->setValue(m_ch);
                m_time.restart();
            }
        }
    }
}

MyTableModel* MainWindow::model()
{
    return reinterpret_cast<MyTableModel*>(ui->tableView->model());
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu;
    QAction* at = menu.addAction(QIcon(), "Копировать данные", [&]() {
        QClipboard* m_clipboard{ QApplication ::clipboard() };
        QString str;
        for (int col = 0; col < model()->columnCount(QModelIndex()); ++col) {
            str.append(model()->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString() + '\t');
        }
        str.append("\r\n");
        for (int row = 0; row < model()->rowCount(QModelIndex()); ++row) {
            for (int col = 0; col < model()->columnCount(QModelIndex()); ++col) {
                str.append(QString::number(model()->getData(col, row)).replace('.',',') + '\t');
            }
            str.append("\r\n");
        }
        m_clipboard->setText(str);
    },
        QKeySequence(QKeySequence::Copy));
    menu.exec(event->globalPos(), at);
}

void MainWindow::on_pbLayout_clicked()
{
    TableView* v = new TableView();
    v->setModel(new LayoutModel(model(), v));
    v->show();
}
