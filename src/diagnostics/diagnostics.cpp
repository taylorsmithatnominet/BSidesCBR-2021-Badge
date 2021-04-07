#include <vector>
#include <memory>

#include "diagnostics.h"

#include <BadgeOs.h>
#include <BadgeLog.h>
#include <BadgeMenu.h>
#include <BadgeService.h>
#include <BSidesBadge2020.h>
#include <display.h>
#include <serialbridge.h>
#include <menu.h>
#include <DiagApp.h>
#include <ExampleDiagApp.h>

#include <irblaster_post.h>

#ifdef  HAVE_IMU
#include <imu_post.h>
#endif//HAVE_IMU
#include <rgbled.h>

#ifdef  HAVE_AUDIO_CODEC
#include <audio.h>
#endif//HAVE_AUDIO_CODEC

// The diagnostics screen
BadgeApp diagnostics_app;
#ifdef ESP32
extern BadgeService suspend_service;
#endif // ESP32

// Indicator for whether the app is running
static bool running = false;
// Timeout for automatic progression to the next test
#define DIAGNOSTIC_AUTO_TIMEOUT (2000U)
static uint32_t timeout_base = 0;
// List of diagnostics apps
static std::vector<std::shared_ptr<DiagApp>> diag_apps;
// Index of current app ready for test
static std::size_t current_app = 0;
// Flag to indicated when all tests have been completed
static bool all_tests_completed = false;
// Requirements for multiple click events
#define START_KEY           (BADGE_KEY_ENTER)
#define CLICKS_TO_START     (1U)
#define ESCAPE_KEY          (BADGE_KEY_LEFT)
#define CLICKS_TO_ESCAPE    (3U)
#define SKIP_KEY            (BADGE_KEY_DOWN)
#define CLICKS_TO_SKIP      (2U)
/* We can't easily initialise this here because the badge key enum isn't
 * trivial enough for use as a designated initialiser :( We'll set it up in
 * `begin()` below. Anything we forget to set will behave like 1 minimum click.
 */
static uint8_t CLICKS_TO[BADGE_KEY_MAX] = { 0 };
static uint8_t click_counts[BADGE_KEY_MAX] = { 0 };
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

void diagnostics_begin(void)
{
    // Add to the "Apps" menu
    BadgeMenuItem *menu_item = app_menu.create_menu_item();
    menu_item->set_name("Diagnostic");
    menu_item->set_select_event([](){
        badge_os.push_app(diagnostics_app);
    });

    diagnostics_app.set_begin([](){
        log(LOG_INFO, "diagnostics: begin");
#ifdef ESP32
        // Given diagnostics is mostly hands-off, stop suspension from happening
        suspend_service.stop();
#endif // ESP32
        /* Set up the required number of clicks to do things */
        CLICKS_TO[START_KEY] = CLICKS_TO_START;
        CLICKS_TO[ESCAPE_KEY] = CLICKS_TO_ESCAPE;
        CLICKS_TO[SKIP_KEY] = CLICKS_TO_SKIP;
        memset(click_counts, 0, sizeof(click_counts));

#ifdef ESP32
            display.setMode(Adafruit_SSD1675BX::kModeFasterBlackWhite);
#endif

        // Setup all of the test apps.
        // We don't have a constructor to call this in, so we do it here.
        // It is cleaned up when end() is called.
        if(!running)
        {
            // Start running
            running = true;

#ifdef DEBUG
            // EXAMPLE
            std::shared_ptr<DiagApp> example_test (new ExampleDiagApp("EXAMPLE"));
            diag_apps.push_back(std::move(example_test));
#endif

            /* Diagnostics for components on all board variants */
            diag_apps.push_back(display_post);
            diag_apps.push_back(serialbridge_post);
            diag_apps.push_back(std::shared_ptr<DiagApp>(&rgbled_post, [](DiagApp*){}));

            /* Push a nullptr in as a spacer */
            diag_apps.push_back(nullptr);

            /* Diagnostics for optional components */
            diag_apps.push_back(irblaster_post);
#ifdef  HAVE_AUDIO_CODEC
            diag_apps.push_back(audio_post);
#endif//HAVE_AUDIO_CODEC
#ifdef  HAVE_IMU
            diag_apps.push_back(std::shared_ptr<DiagApp>(&imu_post, [](DiagApp*){}));
#endif//HAVE_IMU

            for(auto diag_app: diag_apps)
                if (diag_app) diag_app->status = DiagAppStatus::kPending;
        }

        // Find the next app to be tested
        if(!all_tests_completed)
        {
            size_t i;
            for (i = 0; i < diag_apps.size(); ++i) {
                auto diag_app = diag_apps[i];
                if (diag_app && diag_app->status == DiagAppStatus::kPending) {
                    current_app = i;
                    break;
                }
            }
            if (i >= diag_apps.size()) {
                /* This leaves `current_app` pointing at the last test */
                all_tests_completed = true;
            }
        }

        log(LOG_DEBUG, "diagnostics: render started");

        // Clear the screen
        display.fillScreen(DISPLAY_COLOR_BLACK);

        // Title
        display.setTextColor(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
        display.setTextSize(2, 2);
        display.setCursor(4, 0);
        display.print("DIAGNOSTIC");

        // List tests
        display.setTextColor(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
        display.setTextSize(1, 1);
        int16_t line = 0;
        for(auto diag_app : diag_apps)
        {
            if (diag_app) {
                int16_t line_position = 32 + 16 * line;
                display.setCursor(16, line_position);
                display.print(diag_app->get_name().c_str());
                display.setCursor(72, line_position);
                display.print("[");
                display.setCursor(84, line_position);
                switch (diag_app->status)
                {
                case DiagAppStatus::kPass:
                    display.print("PASS");
                    break;
                case DiagAppStatus::kFail:
                    display.print("FAIL");
                    break;
                case DiagAppStatus::kSkipped:
                    display.print("SKIP");
                    break;
                default:
                    break;
                }
                display.setCursor(112, line_position);
                display.print("]");
            }
            line++;
        }

        // Set progress marker
        display.setTextColor(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
        display.setTextSize(1, 1);
        if(!all_tests_completed)
        {
            int16_t progress_position = 32 + 16 * current_app;
            display.setCursor(8, progress_position);
            display.print(">");
        }
        else
        {
            int16_t complete_position = 32 + 16 * diag_apps.size();
            display.setCursor(16, complete_position);
            display.print("TEST COMPLETE");
        }

        // Button guides
        display.setTextColor(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
        display.setTextSize(1, 1);
        display.setCursor(0, display.height() - 8 * 4);
        display.println(" Enter:     Run test");
        display.println(" Down (x2): Skip test");
        display.println(" Left (x3): Exit");

        log(LOG_DEBUG, "diagnostics: render done");
        display.display();
        log(LOG_DEBUG, "diagnostics: display done");
        /* Start counting the timeout once we've rendered the status */
        timeout_base = millis();
    });

    diagnostics_app.set_loop([](){
        uint32_t delta, now;

        if (all_tests_completed) return;

        now = millis();
        if (now < timeout_base) {
            delta = UINT32_MAX - (timeout_base - now); /* assume single wrap */
        } else {
            delta = now - timeout_base;
        }
        if (delta >= DIAGNOSTIC_AUTO_TIMEOUT) {
            auto da = diag_apps[current_app];
            log(
                LOG_INFO, "diagnostics: Auto-starting '%s'",
                da->get_name().c_str()
            );
            badge_os.push_app(*da);
        }
    });

    diagnostics_app.set_end([](){
        log(LOG_INFO, "diagnostics: end");
#ifdef ESP32
        // Restart suspend service
        suspend_service.start();
#endif // ESP32
    });

    diagnostics_app.set_key_event([](uint8_t key, bool key_down){
        uint8_t clicks_seen;

        if (key_down && key < ARRAY_SIZE(click_counts)) {
            clicks_seen = ++click_counts[key];
            log(
                LOG_DEBUG, "Have %u of %u clicks for action %u",
                clicks_seen, CLICKS_TO[key], key
            );
            if (clicks_seen >= CLICKS_TO[key]) {
                click_counts[key] = 0;
                switch (key) {
                case START_KEY:
                    if (!all_tests_completed)
                        badge_os.push_app(*(diag_apps[current_app]));
                    break;
                case ESCAPE_KEY:
                    // No longer running
                    running = false;
                    diag_apps.clear();
                    current_app = 0;
                    all_tests_completed = false;
                    // Exit
                    badge_os.pop_app();
                    break;
                case SKIP_KEY:
                    diag_apps[current_app]->status = DiagAppStatus::kSkipped;
                    diagnostics_app.begin();
                    break;
                }
            }
        }
    });
}
