#ifndef __CR_IRDA_H
#define __CR_IRDA_H

#include <AceRoutine.h>
using namespace ace_routine;

class CRIRdA: public Coroutine {
    public:
        void setup(void);
        int runCoroutine() override;
};

#endif // __CR_IRDA_H
