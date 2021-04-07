#ifndef __CR_RGBLED_H
#define __CR_RGBLED_H

#include <AceRoutine.h>
using namespace ace_routine;

class CRRgbLed: public Coroutine {
    public:
        void setup(void);
        int runCoroutine() override;
};

#endif // __CR_RGBLED_H
