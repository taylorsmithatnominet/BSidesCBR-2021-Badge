#ifndef _H_EXAMPLE_DIAG_APP_H_
#define _H_EXAMPLE_DIAG_APP_H_

#include <DiagApp.h>

//
// Example diagnostics app
//
class ExampleDiagApp : public DiagApp
{
    public:
        ExampleDiagApp(std::string name) : DiagApp(name) {}

    protected:
        void begin(void) override;
        void loop(void) override;
        void end(void) override;
        void key_event(uint8_t key, bool key_down) override;
};

#endif // _H_EXAMPLE_DIAG_APP_H_
