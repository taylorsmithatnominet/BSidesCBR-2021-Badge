#ifndef __CR_CAPSENSE_H
#define __CR_CAPSENSE_H

#include <AceRoutine.h>
using namespace ace_routine;

class CRCapsense: public Coroutine {
    public:
        void setup(void);
        int runCoroutine() override;
};

#endif // __CR_CAPSENSE_H
