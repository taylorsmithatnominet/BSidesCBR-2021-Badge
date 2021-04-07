#ifndef __CR_VBAT_H
#define __CR_VBAT_H

#include <AceRoutine.h>
using namespace ace_routine;

class CRvBAT: public Coroutine {
    public:
        void setup(void);
        int runCoroutine() override;
};

#endif // __CR_VBAT_H
