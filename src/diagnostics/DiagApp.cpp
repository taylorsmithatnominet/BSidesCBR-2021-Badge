#include "DiagApp.h"

#include <BadgeOs.h>

// A diag app is actually a very thin wrapper around a `BadgeApp` - the only
// non-trivial functionality we need to define is how to mark a test as
// complete with some enumerated status
void DiagApp::test_complete(DiagAppStatus status)
{
    this->status = status;
    badge_os.pop_app();
}
