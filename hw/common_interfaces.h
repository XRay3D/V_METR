#ifndef COMMON_INTERFACES_H
#define COMMON_INTERFACES_H

#include <QDebug>
#include <QElapsedTimer>
#include <QMutexLocker>
#include <QSemaphore>
#include <QSerialPort>
#include <QString>
#include <QThread>
#include <elemer_ascii.h>

class PortControl;
class Port;

class CommonInterface : public QObject, public ElemerASCII {
    Q_OBJECT
    friend class PortControl;
    friend class Port;

public:
    CommonInterface();
    ~CommonInterface();

    bool ping(const QString& portName = QString());
    virtual bool checkConnection() = 0;
    virtual bool IsConnected() const;

signals:
    void Open(int mode);
    void Close();
    void Write(const QByteArray& data);

protected:
    Port* m_port;
    QList<QByteArray> m_v;
    static QMutex m_mutex;
    QSemaphore m_semaphore;
    bool m_connected = false;

private:
    QThread m_portThread;
};

class Port : public QSerialPort, public ElemerASCII {
    Q_OBJECT

public:
    Port(CommonInterface* manInterface);

    ~Port();

    void Open(int mode);
    void Close();
    void Write(const QByteArray& data);

    CommonInterface* m_device;

private:
    void ReadyRead();

    QByteArray m_data;
    QMutex m_mutex;
    int m_couter;
};

class PortControl {
public:
    PortControl(CommonInterface* manInterface)
        : m_device(manInterface)
    {
        qDebug() << "+PortControl" << m_device;
        m_timer.start();
        m_device->m_mutex.lock();
        emit m_device->Open(QIODevice::ReadWrite);
        if (m_device->m_semaphore.tryAcquire(1, 1000))
            return;
        m_device->m_semaphore.acquire(m_device->m_semaphore.available());
    }

    ~PortControl()
    {
        emit m_device->Close();
        if (!m_device->m_semaphore.tryAcquire(1, 1000))
            return;
        m_device->m_mutex.unlock();
        qDebug() << "-PortControl" << m_device << m_timer.elapsed();
    }

private:
    CommonInterface* m_device;
    QElapsedTimer m_timer;
};

#endif // COMMON_INTERFACES_H
