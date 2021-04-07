#ifndef __CR_SERIAL_H
#define __CR_SERIAL_H

#include <AceRoutine.h>
using namespace ace_routine;

class CRSerial: public Coroutine {
    public:
	    void setup();
	    int runCoroutine() override;
};

#endif // __CR_SERIAL_H
