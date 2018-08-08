#include "irt.h"

Irt::Irt()
{
}

Irt::~Irt()
{
}

bool Irt::checkConnection()
{
    return getDev() == 9;
}

//bool Irt::ping(const QString& portName)
//{
//}

int Irt::getDev()
{
    dev = 0;
    emit Write(createParcel({ address, 0 }));
    if (m_semaphore.tryAcquire(1, 1000))
        dev = m_v[1].toInt();
    return dev;
}

void Irt::getVal()
{
    PortControl pc(this);
    QByteArray data;
    m_semaphore.acquire(m_semaphore.available());
    do {
        emit Write(createParcel({ address, 5, 0 }));
        if (!m_semaphore.tryAcquire(1, 1000))
            break;
        emit Write(createParcel({ address, 6 }));
        if (!m_semaphore.tryAcquire(1, 1000))
            break;
        for (int i = 0; i < 5; ++i) {
            if (i == 4)
                emit Val(*reinterpret_cast<float*>(data.data()));
            else {
                emit Write(createParcel({ address, 6 }));
                if (!m_semaphore.tryAcquire(1, 1000))
                    break;
                data.append(m_v[1].toInt());
            }
        }
    } while (0);
}

void Irt::getRaw()
{
#pragma pack(push, 1)
    typedef struct raw_t {
        uint8_t dummy1;
        uint8_t stat;
        float v1;
        float v2;
        float v3;
    } raw_t;
#pragma pack(pop)

    if (m_port->isOpen() && m_mutex.tryLock()) {
        m_semaphore.acquire(m_semaphore.available());
        emit Write(createParcel({ address, 2, 255 }));
        if (m_semaphore.tryAcquire(1, 1000)) {
            raw_t* raw = reinterpret_cast<raw_t*>(QByteArray::fromHex(m_v[1]).data());
            emit Raw({ raw->stat, raw->v1, raw->v2, raw->v3 });
        }
        m_mutex.unlock();
    }
}
