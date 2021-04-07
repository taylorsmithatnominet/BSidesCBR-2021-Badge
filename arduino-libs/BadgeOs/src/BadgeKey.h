#ifndef _BADGE_KEY_
#define _BADGE_KEY_

#include <stdint.h>

/* We have to ensure there are no gaps so we can use these as trivial
 * designated initialisers for arrays elsewhere (e.g. key name map) :/ how dumb
 */
enum {
    /* D-Pad */
    BADGE_DPAD_BASE,
    BADGE_KEY_ENTER = BADGE_DPAD_BASE,
    BADGE_KEY_LEFT,
    BADGE_KEY_UP,
    BADGE_KEY_RIGHT,
    BADGE_KEY_DOWN,
    BADGE_DPAD_MAX = BADGE_KEY_DOWN,
    /* Capsense */
    BADGE_CAPSENSE_BASE,
    BADGE_CAPSENSE_1 = BADGE_CAPSENSE_BASE,
    BADGE_CAPSENSE_2,
    BADGE_CAPSENSE_3,
    BADGE_CAPSENSE_4,
    BADGE_CAPSENSE_5,
    BADGE_CAPSENSE_6,
    BADGE_CAPSENSE_7,
    BADGE_CAPSENSE_8,
    BADGE_CAPSENSE_MAX = BADGE_CAPSENSE_8,
    /* Virtual key events */
    BADGE_VIRTKEY_BASE,
    BADGE_KEY_UPDATE = BADGE_VIRTKEY_BASE,
    BADGE_VIRTKEY_MAX = BADGE_KEY_UPDATE,
    /* Max value */
    BADGE_KEY_MAX,
    _BADGE_KEY_INVALID = BADGE_KEY_MAX,
};

#define _BK_X(s) #s
#define _BK_S(s) _BK_X(s)
#define _BK_B(c) BADGE_ ## c ## _BASE
#define _BK_M(c) BADGE_ ## c ## _MAX
#define BADGE_KEY_IS(c, k) (_BK_B(c) <= k && k <= _BK_M(c))
#define BADGE_KEY_IS_DPAD(k)        BADGE_KEY_IS(DPAD,      k)
#define BADGE_KEY_IS_CAPSENSE(k)    BADGE_KEY_IS(CAPSENSE,  k)
#define BADGE_KEY_IS_VIRTUAL(k)     BADGE_KEY_IS(VIRTKEY,   k)

extern const char* BADGE_KEY_NAMES[BADGE_KEY_MAX];

#endif // _BADGE_KEY_
