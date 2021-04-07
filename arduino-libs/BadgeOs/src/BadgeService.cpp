#include <Arduino.h>

#include "BadgeService.h"

#include <BadgeLog.h>

void BadgeService::begin(void)
{
    if ( !running_ )
    {
        if ( begin_ )
        {
            begin_();
        }
        running_ = true;
    }
    /* We leave the `last_tick_ms_` value alone to allow services which have
     * never run or which have been re-enabled after more time than their tick
     * period has elapsed to loop immediately. This saves services from having
     * to perform their first loop iteration in their `begin()` method.
     */
}

void BadgeService::loop(void)
{
    uint32_t delta_ms, now;

    /* Don't bother checking services with no loop or which aren't running */
    if ( !loop_ || !running_ ) return;

    now = (uint32_t) millis();
    /* If we haven't run before or we've wrapped the timer value, always run */
    if (last_tick_ms_ == 0 || now < last_tick_ms_) {
        delta_ms = period_ms_;
    } else {
        delta_ms = now - last_tick_ms_;
    }
    if ( delta_ms >= period_ms_ )
    {
        log(LOG_TRACE, "Calling service loop: %s", name_.c_str());
        loop_();
        /* Don't set this to `now` to avoid tight loops for greedy services */
        last_tick_ms_ = (uint32_t) millis();
    }
}
