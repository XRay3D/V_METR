#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <hw/mi.h>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName("XrSoft");
    a.setApplicationName("V_METR");
    mi mi_;
    Q_UNUSED(mi_)

    MainWindow w;
    w.show();

    return a.exec();
}
