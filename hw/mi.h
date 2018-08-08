#ifndef MEASURINGINTERFACE_H
#define MEASURINGINTERFACE_H

#include "irt.h"
#include "commutator.h"

class mi {
public:
    mi();
    ~mi();
    static Irt* irt;
    static Commutator* com;
};

#endif // MEASURINGINTERFACE_H
