#ifndef IRT_H
#define IRT_H

#include "common_interfaces.h"
#include <QtSerialPort>

class IrtPort;

class Irt : public CommonInterface {
    Q_OBJECT
    friend class IrtPort;

public:
    Irt();
    ~Irt();

    // CommonInterface interface
    bool checkConnection() override;

public:
    int getDev();

    void getVal();
    void getRaw();

signals:
    void Raw(const QList<QVariant>& data);
    void Val(double data);

private:
    int dev = 0;
    int address = 1;
};

#endif // IRT_H
