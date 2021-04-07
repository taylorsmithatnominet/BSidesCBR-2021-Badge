#include <Arduino.h>
#include <BadgeOs.h>
#ifdef ESP32
#include <esp32/rom/rtc.h>
#endif
#include <about.h>
#include <display.h>
#include <keyboard.h>
#include <bootanimation.h>
#include <device_info.h>
#include <menu.h>
#include <diagnostics.h>
#include <torch.h>
#include <helloworld.h>
#include <pacman.h>
#include <snake.h>
#include <tetris.h>
#include <simonsays.h>
#include <sketch.h>
#include <serial.h>
#include <serialbridge.h>
#include <bluetooth.h>
#include <wifiap.h>
#include <sdcard.h>
#include <flashman.h>
#ifdef  HAVE_IMU
#include <imu.h>
#include <imu_post.h>
#endif//HAVE_IMU
#ifdef  HAVE_AUDIO_CODEC
#  include <audio.h>
#endif//HAVE_AUDIO_CODEC
#ifdef HAVE_SUSPEND
#include <suspend.h>
#endif // HAVE_SUSPEND

void setup()
{
    bool shouldSkip = false;
#ifdef HAVE_SUSPEND
    suspend_init();
#endif // HAVE_SUSPEND

    // Apps and services
    keyboard_begin();
    sdcard_begin();
    serial_begin();
    flashman_init();
    display_begin();
    serialbridge_begin();
#ifdef HAVE_AUDIO_CODEC
    audio_begin();
#endif
    bluetooth_begin();
    diagnostics_begin();
    helloworld_begin();
#ifdef HAVE_IMU
    imu_begin();
#endif
    torch_begin();
    wifiap_begin();

    // Games
    pacman_begin();
    snake_begin();
    simonsays_begin();
    sketch_begin();
    tetris_begin();

    // Menus and special apps
    menu_begin();
    bootanimation_begin();
    device_info_begin();
    about_begin();

    badge_os.begin();

#ifdef ESP32
    if (rtc_get_reset_reason(0) == 5 /* DEEPSLEEP_RESET */) {
        shouldSkip = true;
    }
#endif

    /* The main menu is always our uppermost application unless something
     * decides to call `set_app()` to clear the stack.
     */
    badge_os.push_app(main_menu_app);

    if (!shouldSkip) {
        /* We also push the bootanimation preceded by any interstitials so that the
         * bootanimation has the ability to skip those as well.
         */
        badge_os.push_app(display_2020_interstitial);
        badge_os.push_app(bootanimation_app);
    } else {
        // Clear visual artifacts
        display_draw_background(BG_BLACK);
    }
}

void loop()
{
    badge_os.loop();
}

extern "C" {

void app_main()
{
#ifdef ESP32
    initArduino();
#endif
    setup();
    while(1)
    {
        loop();
        delay(10);
        yield();
    }
}

#ifdef __unix__
/* The host build uses `yield()` to handle X events */
void yield(void)
{
    x11_window.handle_event_on_yield();
}
/* And we need to call `app_main()` from `main()` */
int main(int argc, char *argv[]) { app_main(); return EXIT_SUCCESS; }
#endif //__unix__

};
