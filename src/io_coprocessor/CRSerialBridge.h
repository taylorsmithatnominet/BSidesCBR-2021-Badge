#ifndef __CR_SERIALBRIDGE_H
#define __CR_SERIALBRIDGE_H

#include <AceRoutine.h>
using namespace ace_routine;

class CRSerialBridge: public Coroutine {
    public:
        void setup(void);
        int runCoroutine(void) override;
};

#endif // __CR_SERIALBRIDGE_H
