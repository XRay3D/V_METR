#include "common_interfaces.h"

QMutex CommonInterface::m_mutex;

////////////////////////////////////
/// \brief CommonInterface::CommonInterface
///
CommonInterface::CommonInterface()
    : m_port(new Port(this))
{
    m_port->moveToThread(&m_portThread);
    connect(&m_portThread, &QThread::finished, m_port, &QObject::deleteLater);
    connect(this, &CommonInterface::Open, m_port, &Port::Open);
    connect(this, &CommonInterface::Close, m_port, &Port::Close);
    connect(this, &CommonInterface::Write, m_port, &Port::Write);
    m_portThread.start(QThread::NormalPriority);
}

CommonInterface::~CommonInterface()
{
    m_portThread.quit();
    m_portThread.wait();
}

bool CommonInterface::ping(const QString& portName)
{
    QMutexLocker locker(&m_mutex);
    bool ok = false;
    m_semaphore.acquire(m_semaphore.available());
    do {
        if (m_port->isOpen()) {
            emit Close();
            if (!m_semaphore.tryAcquire(1, 1000))
                break;
        }

        if (!portName.isEmpty())
            m_port->setPortName(portName);

        emit Open(QIODevice::ReadWrite);
        if (m_semaphore.tryAcquire(1, 1000))
            ok = checkConnection();

        emit Close();
        if (!m_semaphore.tryAcquire(1, 1000))
            break;

    } while (0);
    return ok;
}

bool CommonInterface::IsConnected() const
{
    return m_connected;
}

////////////////////////////////////
/// \brief Port::Port
/// \param manInterface
///
Port::Port(CommonInterface* manInterface)
    : m_device(manInterface)
{
    setBaudRate(Baud9600);
    setParity(NoParity);
    setDataBits(Data8);
    setFlowControl(NoFlowControl);
    connect(this, &QSerialPort::readyRead, this, &Port::ReadyRead);
}

Port::~Port() {}

void Port::Open(int mode)
{
    //qDebug() << portName() << "Open";
    if (open(static_cast<OpenMode>(mode))) {
        setRequestToSend(true);
        setDataTerminalReady(false);
        waitForReadyRead(10);
        m_device->m_semaphore.release();
    }
}

void Port::Close()
{
    //qDebug() << portName() << "Close";
    close();
    m_device->m_semaphore.release();
}

void Port::Write(const QByteArray& data)
{
    //qDebug() << portName() << "Write" << data;
    write(data);
}

void Port::ReadyRead()
{
    QMutexLocker locker(&m_mutex);
    m_data.append(readAll());
    if (m_data.endsWith('\r')) {
        //qDebug() << portName() << "Read " << m_data;
        if (checkParcel(m_data, m_device->m_v)) {
            m_data.clear();
            m_device->m_semaphore.release();
        }
    }
}
