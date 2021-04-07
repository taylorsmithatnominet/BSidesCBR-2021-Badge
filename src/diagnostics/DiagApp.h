#ifndef _H_DIAG_APP_H_
#define _H_DIAG_APP_H_

#include <string>

#include <BadgeApp.h>

// The status of the test
enum class DiagAppStatus
{
    kPending = 0,
    kPass,
    kFail,
    kSkipped,
};

//
// App for performing a test of a component
//
// How to use:
// a) Create subclass and override: begin; loop; end
// b) Create an instance and set functions to call when events happen using
//     - the extended constructor, or
//     - the function delegate setters defined for `BadgeApp`
//
class DiagApp : public BadgeApp
{
    public:
        DiagApp(std::string name_) { init(name_); }
        DiagApp(
            std::string name_
            , std::function<void(void)> begin
            , std::function<void(void)> loop
            , std::function<void(void)> end
            , std::function<void(uint8_t, bool)> keyh
        ) : BadgeApp(begin, loop, end, keyh) {
            init(name_);
        }

        // Returns the name of the test app
        const std::string& get_name(void) { return this->name; }
        // The status of the test - defaults to pending
        DiagAppStatus status = DiagAppStatus::kPending;
        // Mark the test as completed with a given status
        void test_complete(DiagAppStatus status);

    private:
        // The name of the diagnostics app
        std::string name;
        void init(std::string name_) { this->name = name_; }
};

#endif // _H_DIAG_APP_H_
