#include <cstdarg>
#include <cstdbool>
#include <cstddef>
#include <cstdio>
#include <cstring>

#include <Arduino.h>
#include "BadgeLog.h"
#ifdef SAMD21
#include "SerialBridge.h"
#endif

/**
 * Internal definitions
 */
#ifndef LOG_RINGBUF_SZ
#define LOG_RINGBUF_SZ (4096U)
#endif//LOG_RINGBUF_SZ

/* Log entries are truncated to a maximum of `LOG_MAX_SZ - 1` characters */
#ifndef LOG_MAX_SZ
#define LOG_MAX_SZ (256U)
#endif//LOG_MAX_SZ$

#ifndef DEFAULT_BADGEOS_LOG_LEVEL
#define DEFAULT_BADGEOS_LOG_LEVEL (LOG_INFO)
#endif//DEFAULT_BADGEOS_LOG_LEVEL

#ifndef LOG_LEVEL
#  ifdef CONFIG_BADGEOS_LOG_LEVEL
#    define LOG_LEVEL (CONFIG_BADGEOS_LOG_LEVEL)
#  else
#    define LOG_LEVEL (DEFAULT_BADGEOS_LOG_LEVEL)
#  endif
#endif//LOG_LEVEL

#ifndef LOG_DECORATE
#define LOG_DECORATE (LOG_DECORATED)
#endif//LOG_DECORATE

/**
 * Logging ringbuffer
 */
static char ringbuf[LOG_RINGBUF_SZ];
static char *cur_r = ringbuf, *cur_w = ringbuf;
#define _RINGBUF_END (&ringbuf[sizeof(ringbuf)])

/* Cursor management */
static inline size_t _cur_w_sz(bool contig) {
    char *stop;
    ptrdiff_t sz;

    if (contig) {
        stop = cur_r > cur_w ? cur_r : _RINGBUF_END;
    } else {
        stop = cur_r + sizeof(ringbuf);
    }
    sz = stop - cur_w;
    return (sz >= 0) ? (size_t) sz : 0;
}
static inline void _cur_adv(char **cur_p, const size_t sz) {
    *cur_p += sz;
    while (*cur_p >= _RINGBUF_END) *cur_p -= LOG_RINGBUF_SZ;
}
/* R/W */
static inline size_t _ring_rd(char *buf, size_t buf_sz) {
    size_t i;
    for (i = 0; buf_sz-- >= 0; ++i, ++buf) {
        /* Immediately bail if we're about to copy from the write cursor */
        if (cur_r == cur_w) break;
        *buf = *cur_r;
        /* Check if we hit a NUL before advancing `cur_r` */
        if (*(cur_r++) == '\0') break;
        if (cur_r >= _RINGBUF_END) cur_r -= LOG_RINGBUF_SZ;
    }
    /* Ensure we always NUL terminate */
    *buf = '\0';
    return i;
}
static inline void _ring_wr(const char * buf, size_t buf_sz) {
    while (buf_sz-- >= 0 && *buf != '\0') {
        *(cur_w++) = *(buf++);
        if (cur_w >= _RINGBUF_END) cur_w -= LOG_RINGBUF_SZ;
    }
    *(cur_w++) = '\0';
}

/* Entry truncation logic */
static const char _trunc_s[] = "[trunc]";
static inline void _mark_trunc(char * const buf, const size_t buf_sz) {
    /* If we don't have room for the marker, do nothing */
    if (sizeof(_trunc_s) < buf_sz) {
        strncpy(buf + buf_sz - sizeof(_trunc_s), _trunc_s, sizeof(_trunc_s));
    }
}

/**
 * Actual logging
 */
static log_level_e _g_level = LOG_LEVEL;
void set_log_level(const log_level_e level) {
    _g_level = level;
}

static log_decoration_e _g_decorate = LOG_DECORATE;
void set_log_decoration(const log_decoration_e decorate) {
    _g_decorate = decorate;
}

static const char *_level_as[] = {
    [LOG_NOTSET]    = "NOT SET",
    [LOG_TRACE]     = "TRACE",
    [LOG_DEBUG]     = "DEBUG",
    [LOG_INFO]      = "INFO",
    [LOG_WARN]      = "WARN",
    [LOG_ERROR]     = "ERROR",
    [LOG_CRITICAL]  = "CRITICAL",
    [LOG_PANIC]     = "PANIC",
    [_LOG_MAX_LV]   = "UNKNOWN",
};

void _do_log(
    const log_level_e level, const log_decoration_e decoration,
    const char * const fmt, va_list ap
) {
    bool direct;
    char *ent, *p;
    int sprint_sz, ent_sz = 0;

    /* Bail early if this is below the current level */
    if (level < _g_level) return;

    /* Fastest path is to format directly into the ring buffer */
    if (_cur_w_sz(true) >= LOG_MAX_SZ) {
        direct = true;
        ent = cur_w;
    } else {
        direct = false;
        ent = (char *) alloca(LOG_MAX_SZ);
    }
    /* We have a pointer to our current cursor in the entry being formatted */
    p = ent;

    /* If we need to decorate the log entry, attempt to do so but never consume
     * more than a quarter of the buffer size (arbitrarily selected).
     */
    if (decoration == LOG_DECORATED) {
        const size_t dec_max = LOG_MAX_SZ >> 2;
        /* Find the appropriate log level string for decoration and fall back
         * to the maximum value for "UNKNOWN" if we are out of bounds or NULL.
         */
        const char *lv_s = _level_as[level < _LOG_MAX_LV ? level : _LOG_MAX_LV];
        if (lv_s  == NULL) lv_s = _level_as[_LOG_MAX_LV];
        /* If we fail here, we can't really recover. Bail immediately without
         * advancing the cursor (for direct writes) so that the next entry will
         * overwrite the decoration we just wrote, or allow it to fall out of
         * the stack once we return (for indirect).
         */
        sprint_sz = snprintf(p, dec_max, "[%s:%lu] ", lv_s, millis());
        if (sprint_sz < 0) return;
        /* Ensure that if the decoration was truncated we have a close brace */
        if (sprint_sz >= dec_max) {
            ent[dec_max - 1] = ']';
            sprint_sz = dec_max;    /* Advance to the trailing NUL */
        }
        /* Advance the entry cursor and size */
        p += sprint_sz;
        ent_sz += sprint_sz;
    }

    /* Perform the log string formatting */
    sprint_sz = vsnprintf(p, LOG_MAX_SZ - ent_sz, fmt, ap);

    /* If we didn't error out, we need to either advance the cursor for direct
     * writes or use the ringbuf helper to do a potentialy segmented write.
     * Otherwise we'll fall through without advancing the cursor/writing to the
     * ring buffer.
     */
    if (sprint_sz >= 0) {
        ent_sz += sprint_sz;
        /* If the sum of the prior `ent_sz` and the size returned by
         * `vsnprintf()` is equal to or greater than the maximum buffer size,
         * that indicates the formatted string was truncated and we filled the
         * space in the buffer we allowed ourselves to use (including a NUL).
         */
        if (ent_sz >= LOG_MAX_SZ) {
            _mark_trunc(ent, LOG_MAX_SZ);
            /* We only wrote this much, not including the trailing NUL which will
             * be accounted for below.
             */
            ent_sz = LOG_MAX_SZ - 1;
        }
        ent_sz += 1;  /* Account for the trailing NUL first */
        if (direct) _cur_adv(&cur_w, (size_t) ent_sz);
        else _ring_wr(ent, ent_sz);
    }
}

void log(const log_level_e level,
    const char * const fmt, ...
) {
    va_list ap;
    va_start(ap, fmt);
    _do_log(level, _g_decorate, fmt, ap);
    va_end(ap);
}

void log(const log_level_e level, const log_decoration_e decoration,
    const char * const fmt, ...
) {
    va_list ap;
    va_start(ap, fmt);
    _do_log(level, decoration, fmt, ap);
    va_end(ap);
}

/* Dump all pending log entries manually */
void log_dump(void) {
    char buf[LOG_MAX_SZ];
    while (_ring_rd(buf, sizeof(buf)) > 0) {
#ifndef __unix__
        Serial.println(buf);
#ifdef SAMD21
        DebugSerial.println(buf);
#endif
#else
        printf("%s\n", buf);
        fflush(NULL);
#endif
    }
}
