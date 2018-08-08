#ifndef INTERFACE_H
#define INTERFACE_H

#include <QByteArray>
#include <QString>

QString ControlSum(QString str)
{
    QString tmp;
    int ks = 65535, ns;
    for (int n = 1; n < str.size(); n++) {
        tmp = str.mid(n, 1);
        ns = (int)*str.mid(n, 1).toLocal8Bit().data(); //(int)*tmp.toAscii().data();
        ks = ks ^ ns;
        for (int i = 1; i < 9; i++) {
            if ((ks & (-2)) != ks)
                ks = (ks >> 1) ^ 40961;
            else
                ks >>= 1;
        }
    }
    return tmp.setNum(ks) + '\r';
}

bool ChekCom(QString& str)
{
    QStringList AR;
    QString tmp;
    if (str.mid(0, 2) == "я!")
        str = str.mid(1, str.size());
    else if (str.mid(0, 1) != "!")
        return false;
    if ((str.count(';') > 0) && (str.count('!') > 0)) {
        AR = str.split(";");
        str.clear();
        for (int i = 0; i < AR.size() - 1; str += AR[i++] + ";")
            ;
        tmp = AR[AR.size() - 1];
        if (tmp.toLong() == ControlSum(str).toLong()) {
            str = AR[1];
            if (str.count("$1") > 0) {
                return false;
                str = "0";
            } else {
                return true;
            }
        } else
            return false;
    } else
        return false;
    return false;
}

#endif // INTERFACE_H
