#include "commutator.h"

#include "commutator.h"
#include "irt.h"

Commutator::Commutator()
{
}

Commutator::~Commutator()
{
}

bool Commutator::checkConnection()
{
    return getDev() == 16;
}

int Commutator::getDev()
{
    dev = 0;
    emit Write(createParcel({ address, 0 }));
    if (m_semaphore.tryAcquire(1, 1000))
        dev = m_v[1].toInt();
    return dev;
}

bool Commutator::setChannel(int channel)
{
    PortControl pc(this);

    emit Write(createParcel({ address, 4, --channel }));
    if (m_semaphore.tryAcquire(1, 1000))
        if (m_v.at(1) == "$0")
            return true;

    return false;
}

bool Commutator::setAuto()
{
    PortControl pc(this);

    emit Write(createParcel({ address, 3, 1 }));
    if (m_semaphore.tryAcquire(1, 1000))
        if (m_v.at(1) == "$0")
            return true;

    return false;
}
