#ifndef _BADGE_LOG_H_
#define _BADGE_LOG_H_

#include <stdbool.h>

/* Log level definition and config helpers */
typedef enum {
    LOG_NOTSET,
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_CRITICAL,
    LOG_PANIC,
    _LOG_MAX_LV,
} log_level_e;

void set_log_level(const log_level_e);

typedef enum {
    LOG_DECORATED = true,
    LOG_UNDECORATED = false,
    _LOG_MAX_DEC,
} log_decoration_e;

void set_log_decoration(const log_decoration_e);

/* All logging calls are simple, short voided functions in order to reduce the
 * overhead on the caller - both in coding and calling. The caller can assume
 * that a call to the logging subsystem will return quickly and that data
 * passed by reference is then safe to deallocate.
 */
void __attribute__ ((format (printf, 2, 3)))
log(const log_level_e, const char * const, ...);
void __attribute__ ((format (printf, 3, 4)))
log(const log_level_e, const log_decoration_e, const char * const, ...);

void log_dump();

#endif//_BADGE_LOG_H_
