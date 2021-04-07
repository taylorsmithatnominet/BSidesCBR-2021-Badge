#include "sdcard.h"
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#ifdef ESP32
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#endif

#include <BadgeOs.h>
#include <BadgeService.h>
#include <BSidesBadge2020.h>

#define SPI3_MISO 12
#define SPI3_MOSI 13
#define SPI3_CLK  14
#define SD_CS   4

BadgeService        sdcard_service;
const char* TAG = "SDCARD EXAMPLE";
void sdcard_begin(void)
{
#ifdef ESP32
    sdcard_service.set_begin([](){
        // mask other chips on SPI bus for testing
        gpio_set_direction(GPIO_NUM_15, GPIO_MODE_INPUT_OUTPUT);
        gpio_pullup_en(GPIO_NUM_15);
        gpio_set_level(GPIO_NUM_15, 1);
        gpio_set_direction(GPIO_NUM_27, GPIO_MODE_INPUT_OUTPUT);
        gpio_pullup_en(GPIO_NUM_27);
        gpio_set_level(GPIO_NUM_27, 1);
        ESP_LOGI(TAG, "GPIOs %d and %d set to: %d %d",GPIO_NUM_15, GPIO_NUM_27, gpio_get_level(GPIO_NUM_15), gpio_get_level(GPIO_NUM_27));

        printf("Initializing SPI bus for card...\n");

        sdmmc_host_t host = SDSPI_HOST_DEFAULT();
        sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
        slot_config.gpio_miso = (gpio_num_t)SPI3_MISO;
        slot_config.gpio_mosi = (gpio_num_t)SPI3_MOSI;
        slot_config.gpio_sck  = (gpio_num_t)SPI3_CLK;
        slot_config.gpio_cs   = (gpio_num_t)SD_CS;
        // This initializes the slot without card detect (CD) and write protect (WP) signals.
        // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.

        // Options for mounting the filesystem.
        // If format_if_mount_failed is set to true, SD card will be partitioned and
        // formatted in case when mounting fails.
        esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
        };
        sdmmc_card_t* card;
        esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

        if (ret != ESP_OK) {
            if (ret == ESP_FAIL) {
                ESP_LOGE(TAG, "Failed to mount filesystem. "
                        "If you want the card to be formatted, set format_if_mount_failed = true.");
            } else {
                ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                        "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
            }
            return;
        }

        // Card has been initialized, print its properties
        sdmmc_card_print_info(stdout, card);

        // Use POSIX and C standard library functions to work with files.
        // First create a file.
        ESP_LOGI(TAG, "Opening file");
        FILE* f = fopen("/sdcard/hello.txt", "w");
        if (f == NULL) {
            ESP_LOGE(TAG, "Failed to open file for writing");
            return;
        }
        fprintf(f, "Hello %s!\n", card->cid.name);
        fclose(f);
        ESP_LOGI(TAG, "File written");

        // Check if destination file exists before renaming
        struct stat st;
        if (stat("/sdcard/foo.txt", &st) == 0) {
            // Delete it if it exists
            unlink("/sdcard/foo.txt");
        }

        // Rename original file
        ESP_LOGI(TAG, "Renaming file");
        if (rename("/sdcard/hello.txt", "/sdcard/foo.txt") != 0) {
            ESP_LOGE(TAG, "Rename failed");
            return;
        }

        // Open renamed file for reading
        ESP_LOGI(TAG, "Reading file");
        f = fopen("/sdcard/foo.txt", "r");
        if (f == NULL) {
            ESP_LOGE(TAG, "Failed to open file for reading");
            return;
        }
        char line[64];
        fgets(line, sizeof(line), f);
        fclose(f);
        // strip newline
        char* pos = strchr(line, '\n');
        if (pos) {
            *pos = '\0';
        }
        ESP_LOGI(TAG, "Read from file: '%s'", line);

        // All done, unmount partition and disable SDMMC or SPI peripheral
        esp_vfs_fat_sdmmc_unmount();
        ESP_LOGI(TAG, "Card unmounted");



    });
    sdcard_service.set_loop([](){
    });
#endif // ESP32
    badge_os.add_service(sdcard_service);
    sdcard_service.start();
}
