#include <cstdbool>
#include <cstdint>
#include <memory>

#include <Arduino.h>
#include <esp_err.h>

#include <BadgeApp.h>
#include <BadgeKey.h>
#include <BadgeLog.h>
#include <BadgeMenu.h>
#include <BadgeOs.h>
#include <BSidesBadge2020.h>
#include <menu.h>
#include <display.h>

#include <audiobit.h>
#include <driver/i2s.h>
#include <dr_wav.h>

/* This is a WAV file which has been run through `xxd`.
 * XXX: This needs to be generated as part of the build process.
 */
#include "tones.h"

/* We play the audio sample every `AUDIO_LOOP_PERIOD` milliseconds */
#define AUDIO_LOOP_PERIOD (3000U)
/* We read WAV data in chunks of this many samples */
#define SAMPLES_PER_CHUNK (512U)

/* Display definitions */
#define FONT_SIZE       (1U)
#define FONT_SIZE_HDR   (2U * FONT_SIZE)
#define LINE_HEIGHT     (8U * FONT_SIZE)
#define INDENT_PX       (8U)

/* Casting this everywhere is annoying */
#define I2S_NUM_ ((i2s_port_t) I2S_NUM)


/* Audio codec setup logic */
static bool inited = false;

static esp_err_t _audio_setup(void) {
    esp_err_t err;
    /* GPIO2 controls the buffer which passes MCLK from the ESP to DAC */
    const gpio_config_t _audio_io_cfg = {
        .pin_bit_mask = 1ULL << I2S_MCLK_BUFFER_EN,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    /* We simply install all the necessary drivers here. For the I2S driver
     * this will start it so we attempt to stop it immediately rather than
     * leave it running in the background when we're not actually enabling MCLK
     * to reach the DAC.
     */
    if (gpio_config(&_audio_io_cfg) == ESP_OK) {
        if (audiobit_i2s_init() == ESP_OK) {
            (void) i2s_stop(I2S_NUM_);  /* Best effort */
            audiobit_i2c_init();        /* Returns void */
        } else {
            log(LOG_ERROR, "Failed to set up the I2S driver");
        }
    } else {
        log(LOG_ERROR, "Failed to enable audio codec MCLK buffer");
    }

    inited = (ESP_OK == err);
    return err;
}
static void _audio_teardown(void) {
    if (!inited)
        log(LOG_WARN, "Called audio shutdown while not initialised");
    i2s_zero_dma_buffer(I2S_NUM_);
    if (i2s_driver_uninstall(I2S_NUM_) != ESP_OK)
        log(LOG_ERROR, "Failed to uninstall the I2S driver");
}
extern void audio_teardown(void) { _audio_teardown(); }

/* We leave the I2C up but it won't actually work while MCLK is stopped */
static esp_err_t _audio_start(void) {
    esp_err_t err;

    log(LOG_DEBUG, "Starting audio driver");
    err = gpio_set_level(I2S_MCLK_BUFFER_EN, HIGH);
    if (err == ESP_OK) {
        err = i2s_start(I2S_NUM_);
        if (err == ESP_OK) {
            err = audiobit_playback_init();
            if (err == ESP_OK) {
                log(LOG_INFO, "I2S and I2C setup is completed for playback");
                (void) i2s_zero_dma_buffer(I2S_NUM_);
            } else {
                log(LOG_ERROR, "Failed to initialise I2S playback");
            }
        } else {
            log(LOG_ERROR, "Failed to start I2S driver");
        }
    } else {
        log(LOG_ERROR, "Failed to enable I2S clock output buffer");
    }

    return err;
}
static void _audio_stop(void) {
    log(LOG_DEBUG, "Stopping audio driver");
    if (i2s_stop(I2S_NUM_) != ESP_OK)
        log(LOG_ERROR, "Failed to stop I2S driver");
    if (gpio_set_level(I2S_MCLK_BUFFER_EN, LOW) != ESP_OK)
        log(LOG_ERROR, "Failed to disable I2S clock output buffer");
}

/* Play the hardcoded WAV sample from memory */
static esp_err_t _audio_play_tones(void) {
    esp_err_t err = ESP_FAIL;
    drwav wav;
    drwav_int16 *samples;
    drwav_uint64 frames_to_read, frames_read;
    size_t alloc_sz, bytes_written = 0;

    log(LOG_INFO, "Playing tones!");

    if (drwav_init_memory(&wav, tones_wav, tones_wav_len, NULL)) {
        log(LOG_DEBUG, "Read WAV from memory");
        log(
            LOG_DEBUG,
            "Read WAV from memory" " - "
            "channels: %hu" " - "
            "sample rate: %lu" " - "
            "bits per sample: %hu",
            wav.channels, wav.sampleRate, wav.bitsPerSample
        );
        frames_to_read = wav.totalPCMFrameCount;
        alloc_sz = (wav.bitsPerSample / 8) * wav.channels * SAMPLES_PER_CHUNK;
        samples = (drwav_int16 *) calloc(alloc_sz, 1);
        i2s_set_clk(
            I2S_NUM_,
            wav.sampleRate, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO
        );
        while (frames_to_read > 0) {
            frames_read = drwav_read_pcm_frames_s16(
                &wav, SAMPLES_PER_CHUNK, samples
            );
            err = i2s_write(I2S_NUM_, samples, alloc_sz, &bytes_written, 100);
            if (err != ESP_OK) {
                log(LOG_ERROR, "I2S write error!");
                break;
            }
            if (frames_to_read >= frames_read) {
                frames_to_read -= frames_read;
            } else {
                log(
                    LOG_WARN,
                    "Read more samples than expected (%llu > %llu)",
                    frames_read, frames_to_read
                );
                break;
            }
        }
        free(samples);
        log(LOG_DEBUG, "Finished reading WAV from memory");
    } else {
        log(LOG_ERROR, "Failed to read WAV from memory");
    }
    return err;
}

/* Display logic */
static size_t _audio_draw_frame(void) {
    size_t y;

    display.setMode(Adafruit_SSD1675BX::kModeFasterBlackWhite);
    display.fillScreen(DISPLAY_COLOR_BLACK);
    display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
    display.setCursor(INDENT_PX, 0);
    display.setTextSize(FONT_SIZE_HDR, FONT_SIZE_HDR);
    display.print("AUDIO");
    y = display_draw_background(BG_DEFAULT);

    display.setTextSize(FONT_SIZE, FONT_SIZE);
    display.setCursor(INDENT_PX, y);
    return y;
}
static void _audio_draw_error(const char* msg) {
    /* Draw a flashy (literally) error wherever the cursor is at the moment */
    display.setMode(Adafruit_SSD1675BX::kModeBlackWhiteRed);
    display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
    display.println(msg);
    display.println("Closing app");
}
/* Application implementation */
static void _audio_begin(void) {
    (void) _audio_draw_frame();

    if (ESP_OK != _audio_start()) {
        log(LOG_ERROR, "Audio init failed, closing app!");
        _audio_draw_error("Audio init failed");
        /* We pop ourselves here but that still lets us fall through */
        badge_os.pop_app();
    } else {
        display.println("C H O O N Z");
        display.println("graphic design is my passion");
    }
    display.display();
}
static void _audio_exit(void) { _audio_stop(); }

static void _audio_loop(void) {
    static uint32_t last = 0;
    uint32_t delta, now = millis();

    if (now < last) {
        delta = UINT32_MAX - (last - now); /* assume single wrap */
    } else {
        delta = now - last;
    }
    if (delta < AUDIO_LOOP_PERIOD) {
        /* Make sure we don't repeat whatever was left in the DMA buffer */
        i2s_zero_dma_buffer(I2S_NUM_);
        return;
    }
    last = now;
    if (ESP_OK != _audio_play_tones()) {
        (void) _audio_draw_frame();
        _audio_draw_error("Failed to play the sample");
    }
}

static void _audio_kh(uint8_t key, bool down) {
    if (down && key == BADGE_KEY_LEFT) badge_os.pop_app();
}

/* Menu logic */
static BadgeApp audio_app(_audio_begin, _audio_loop, _audio_exit, _audio_kh);
void audio_begin(void) {
    BadgeMenuItem *menu_item = app_menu.create_menu_item();
    menu_item->set_name("Audio Demo");
    menu_item->set_select_event([](){ badge_os.push_app(audio_app); });
    _audio_setup();
}

/* We also provide a diagnostic app here using the same functionality
 *
 * We re-use `_audio_exit()` here on purpose rather than dupe it.
 */
static void _audiag_begin(void);
static void _audiag_loop(void);
static void _audiag_kh(uint8_t, bool);
static DiagApp _audio_post(
    "Audio", _audiag_begin, _audiag_loop, _audio_exit, _audiag_kh
);
std::shared_ptr<DiagApp> audio_post{
    std::shared_ptr<DiagApp>{}, &_audio_post
};

static bool _audiag_proceed = false;
static bool _audiag_started = false;
#define AUDIAG_AUTOSTART_DELAY (3000U)

static size_t _audiag_draw_frame(void) {
    size_t y = display.height() / 2;

    display.fillScreen(DISPLAY_COLOR_BLACK);
    display.setTextColor(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
    display.setTextSize(FONT_SIZE, FONT_SIZE);
    display.setCursor(INDENT_PX, y); y += LINE_HEIGHT * 2;
    display.print("- Audio test -");

    return y;
}

static void _audiag_begin(void) {
    size_t y;
    _audiag_proceed = _audiag_started = false;

    /* Make all renders non-blocking */
    display.setMode(Adafruit_SSD1675BX::kModeFasterBlackWhite);

    y = _audiag_draw_frame();
    if (ESP_OK == _audio_start()) {
        display.setCursor(INDENT_PX, y); y += LINE_HEIGHT;
        display.print("RIGHT: Start test");
        display.setCursor(INDENT_PX, y); y += LINE_HEIGHT * 2;
        display.print("LEFT:  Fail test");
        display.setCursor(0, y); y += LINE_HEIGHT * 4;
        display.print(
            " If audio does not  \n"
            " play as expected,  \n"
            " hold left to fail  \n"
            " at any time"
        );
    } else {
        display.print(
            " TEST FAILED:       \n"
            " Unable to init the \n"
            " audio codec chip!  \n"
        );
        _audio_post.test_complete(DiagAppStatus::kFail);
    }
    display.display();
    log(LOG_INFO, "Ready to start audio POST");
}
static void _audiag_loop(void) {
    static uint32_t last = millis();
    size_t y;
    uint32_t delta, now = millis();

    if (now < last) {
        /* assume single wrap */
        delta = UINT32_MAX - (last - now);
    } else {
        delta = now - last;
    }
    /* If we haven't yet been allowed to proceed, check if the auto-start delay
     * has expired. If it has, set proceed to true and fall through to
     * immediately attempt to play the sample. Otherwise, just check if the
     * delay between loops has elapsed as usual.
     */
    if (!_audiag_proceed) {
        if (delta >= AUDIAG_AUTOSTART_DELAY) {
            log(LOG_INFO, "Auto-starting audio diagnostic sample loop");
            _audiag_proceed = true;
        }
    } else {
        if (delta < AUDIO_LOOP_PERIOD) {
            /* This is likely to cut off the sample but that's fine here */
            i2s_zero_dma_buffer(I2S_NUM_);
            return;
        }
    }

    if (_audiag_proceed && _audio_post.status == DiagAppStatus::kPending) {
        _audiag_started = true;
        last = now;

        y = _audiag_draw_frame();
        display.setCursor(INDENT_PX, y); y += LINE_HEIGHT;
        display.print("Playing sample...");
        display.setCursor(INDENT_PX, y); y += LINE_HEIGHT;
        display.print("RIGHT: Pass test");
        display.setCursor(INDENT_PX, y); y += LINE_HEIGHT;
        display.print("LEFT:  Fail test");
        display.display();

        if (ESP_OK != _audio_play_tones()) {
            display.print(
                " TEST FAILED:       \n"
                " Unable to play the \n"
                " audio sample data! \n"
            );
            display.display();
            log(LOG_WARN, "Failing audio diagnostic due to playback error");
            _audio_post.test_complete(DiagAppStatus::kFail);
            _audio_stop();
        }
    }
}
static void _audiag_kh(uint8_t key, bool down) {
    if (!down) return;
    switch (key) {
    case BADGE_KEY_RIGHT:
        if (!_audiag_proceed) {
            log(LOG_INFO, "Starting audio diagnostic sample loop");
            _audiag_proceed = true;
        } else if (_audiag_started) {
            log(LOG_WARN, "Passing audio diagnostic due to user input");
            _audio_post.test_complete(DiagAppStatus::kPass);
        }
        break;
    case BADGE_KEY_LEFT:
        log(LOG_WARN, "Failing audio diagnostic due to user input");
        _audio_post.test_complete(DiagAppStatus::kFail);
        break;
    }
    if (_audio_post.status != DiagAppStatus::kPending) _audio_stop();
}
