#ifndef __CR_SAO_H
#define __CR_SAO_H

#include <AceRoutine.h>
using namespace ace_routine;

class CRSAO: public Coroutine {
    public:
        void setup(void);
        int runCoroutine() override;
};

#endif // __CR_SAO_H
