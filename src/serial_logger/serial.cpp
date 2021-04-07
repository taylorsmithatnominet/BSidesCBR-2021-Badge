#include <Arduino.h>
#include <BadgeLog.h>
#include <BadgeOs.h>
#include <BadgeService.h>

#include "serial.h"

BadgeService seriallogger_service;

void serial_begin(void) {
    seriallogger_service.set_name("syslog");
    seriallogger_service.set_begin([] { Serial.begin(115200); });
    seriallogger_service.set_loop([] { log_dump(); });
    seriallogger_service.set_period_ms(500);
    badge_os.add_service(seriallogger_service);
    seriallogger_service.start();
}
