#include "mi.h"
#include <QObject>

QThread thread;
QSemaphore semafore;

Irt* mi::irt = nullptr;
Commutator* mi::com = nullptr;

mi::mi()
{
    if (!semafore.available()) {
        irt = new Irt;
        com = new Commutator;

        irt->moveToThread(&thread);
        com->moveToThread(&thread);

        thread.connect(&thread, &QThread::finished, irt, &QObject::deleteLater);
        thread.connect(&thread, &QThread::finished, com, &QObject::deleteLater);

        thread.start(QThread::NormalPriority);
    }
    semafore.release();
}

mi::~mi()
{
    semafore.acquire();
    if (!semafore.available()) {
        thread.quit();
        thread.wait();
        irt = nullptr;
        com = nullptr;
    }
}
