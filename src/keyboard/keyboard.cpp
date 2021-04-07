#include "keyboard.h"

#include <BadgeLog.h>
#include <BadgeOs.h>
#include <BadgeService.h>
#include <BSidesBadge2020.h>
#include <BSidesMessaging.h>

BadgeService keyboard_service;
// Last key event, exposed for other apps/services to read
int latestKeyEvent = 0;

const static uint8_t pin_map[5] = {
    BADGE_ENTER_GPIO,
    BADGE_LEFT_GPIO,
    BADGE_UP_GPIO,
    BADGE_RIGHT_GPIO,
    BADGE_DOWN_GPIO,
};

// Message queue, non-static so it can be externed elsewhere
SerialMessageQueue keyboardQueue;

void keyboard_begin(void)
{
    keyboard_service.set_begin([](){
        pinMode(pin_map[0], INPUT);
        pinMode(pin_map[1], INPUT);
        pinMode(pin_map[2], INPUT);
        pinMode(pin_map[3], INPUT);
        pinMode(pin_map[4], INPUT);
    });
    keyboard_service.set_loop([](){
        static uint8_t prev_pin_value[5] = {0xffU, 0xffU, 0xffU, 0xffU, 0xffU};
        static SerialMessage capsenseMsg;
        static CapsenseState touchValues = {false, false, false, false, false, false, false, false};
        // Scan D-pad
        for ( uint8_t pin = 0; pin < sizeof(prev_pin_value); pin++ )
        {
            uint8_t value = digitalRead(pin_map[pin]);
            if ( value != prev_pin_value[pin] )
            {
                // Update time of latest key event
                latestKeyEvent = millis();
                prev_pin_value[pin] = value;
                bool key_down = ( HIGH != value );
                log(LOG_DEBUG, "Key event: %s KEY_%s",
                    BADGE_KEY_NAMES[pin], key_down ? "DOWN" : "UP");
                switch (pin)
                {
                    case 0:
                        badge_os.key_event(BADGE_KEY_ENTER, key_down);
                        break;
                    case 1:
                        badge_os.key_event(BADGE_KEY_LEFT, key_down);
                        break;
                    case 2:
                        badge_os.key_event(BADGE_KEY_UP, key_down);
                        break;
                    case 3:
                        badge_os.key_event(BADGE_KEY_RIGHT, key_down);
                        break;
                    case 4:
                        badge_os.key_event(BADGE_KEY_DOWN, key_down);
                        break;
                    default:
                        break;
                }
            }
        }
        // Handle capsense message
        if (dequeueMessage(&capsenseMsg, keyboardQueue)) {
           CapsenseState newValues = capsenseMsg.data.payload.capsense;
           for (uint8_t pad = 0; pad < 8; pad++) {
               if (newValues.pads[pad] != touchValues.pads[pad]) {
                   // Update time of latest key event
                   latestKeyEvent = millis();
                   uint8_t padOffset = BADGE_CAPSENSE_BASE + pad;
                   log(LOG_DEBUG, "Key event: %s KEY_%s",
                           BADGE_KEY_NAMES[padOffset], newValues.pads[pad] ? "DOWN" : "UP");
                   // Key event
                   badge_os.key_event(padOffset, newValues.pads[pad]);
                   // Update pad value
                   touchValues.pads[pad] = newValues.pads[pad];
               }
           }
        }
    });
    badge_os.add_service(keyboard_service);
    keyboard_service.start();
}
