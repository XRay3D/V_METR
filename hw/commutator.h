#ifndef COMMUTATOR_H
#define COMMUTATOR_H

#include "common_interfaces.h"
#include <QtSerialPort>

//#pragma pack(push, 1)
//typedef struct raw_t {
//    uint8_t dummy1;
//    uint8_t stat;
//    float v1;
//    float v2;
//    float v3;
//} raw_t;
//#pragma pack(pop)

class Port;

class Commutator : public CommonInterface {
    Q_OBJECT

public:
    Commutator();
    ~Commutator();

    // CommonInterface interface
    bool checkConnection() override;

public:
    int getDev();

    bool setChannel(int channel);
    bool setAuto();

    void getVal();
    void getRaw();

private:
    QMutex m_mutex;

    int dev = 0;
    int address = 2;
};

#endif // COMMUTATOR_H
