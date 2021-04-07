/*
 * SPIFlash.cpp - Library for controlling the flash chip on the BSides Badge 2020
 * Reference: 
 * Created by Peter Rankin, 9 January 2021
*/
#include "SPIFlash.h"

/* The chip is 4Mbit, ie. 512MiB */
#define SPI_FLASH_HARD_CODED_CAPACITY (4 << 10 << 10 >> 3)

SPIFlash::SPIFlash(int8_t CS, SPIClass *spi) {
    last_error_ = 0;
    cs_ = CS;
    spi_ = spi;
    ready_ = false;
#ifdef ESP32
    spi_flash_ = NULL;
    memset(&spi_flash_cfg_, 0, sizeof(spi_flash_cfg_));
#endif
}

#ifdef ESP32
struct spi_struct_t {
    spi_dev_t * dev;
#if !CONFIG_DISABLE_HAL_LOCKS
    xSemaphoreHandle lock;
#endif
    uint8_t num;
};
#endif

bool SPIFlash::begin() {
    ready_ = false;
#ifdef ESP32
    esp_err_t err = 1;
    if ( !spi_flash_ ) {
        if ( !spi_ )         {
            last_error_ = (int)ESP_ERR_INVALID_ARG;
            return false;
        }
        spi_->begin();
        struct spi_struct_t *spi_internal = (struct spi_struct_t*)(spi_->bus());
        if ( !spi_internal )         {
            last_error_ = (int)ESP_ERR_INVALID_ARG;
            return false;
        }
        spi_host_device_t host_id = HSPI_HOST;
        switch (spi_internal->num) 
        {
            case FSPI:
                host_id = SPI_HOST;
                break;
            case HSPI:
                host_id = HSPI_HOST;
                break;
            case VSPI:
                host_id = VSPI_HOST;
                break;
            default:
                last_error_ = (int)ESP_ERR_INVALID_ARG;
                return false;
        }
        spi_flash_cfg_.host_id = host_id,
        spi_flash_cfg_.cs_id = 0,                   /* !CS          */
        spi_flash_cfg_.cs_io_num = (int)cs_,        /* e.g GPIO 15  */
        spi_flash_cfg_.io_mode = SPI_FLASH_SLOWRD,  /* XXX: Keep it simple for now  */
        spi_flash_cfg_.speed = ESP_FLASH_40MHZ,     /* We can go up to 104MHz if we can drive the clock */
        spi_flash_cfg_.input_delay_ns = 0,          /* Unknown -> 0 */
        err = spi_bus_add_flash_device(&spi_flash_, &spi_flash_cfg_);
        if ( err ) {
            spi_flash_ = NULL;
            last_error_ = (int)err;
            return false;
        }
        if ( !spi_flash_ ) {
            last_error_ = (int)ESP_ERR_NOT_FOUND;
            return false;
        }
    }
    /* We specify the chip's capacity here rather than allowing it to
     * be automatically detected during the `esp_flash_init()` call
     * since ESP-IDF doesn't understand Adesto's new (?) format for
     * capacity in the device ID bytes. The chip does define a legacy
     * chip ID command but we'd need to modify the chip driver in IDF
     * to make it do issue that command if the usual one returns an ID
     * which IDF doesn't understand.
     */
    spi_flash_->size = SPI_FLASH_HARD_CODED_CAPACITY;
    /* Actually initialise the flash chip */
    err = esp_flash_init(spi_flash_);
    if ( err ) {
        last_error_ = (int)err;
        return false;
    }
    ready_ = true;
    return true;
#else  // ESP32 / NOT ESP32
    last_error_ = 1;
    return false;
#endif // NOT ESP32
}

bool SPIFlash::read(void *buffer, uint32_t address, uint32_t length) {
#ifdef ESP32
    if ( !spi_flash_ || !ready_ ) {
        last_error_ = (int)ESP_ERR_NOT_FOUND;
        return false;
    }
    esp_err_t err = esp_flash_read(spi_flash_, buffer, address, length);
    if ( err ) {
        last_error_ = (int)err;
        return false;
    }
#else
    last_error_ = 1;
    return false;
#endif
    return true;
}

bool SPIFlash::write(const void *buffer, uint32_t address, uint32_t length) {
#ifdef ESP32
    if ( !spi_flash_ || !ready_ ) {
        last_error_ = (int)ESP_ERR_NOT_FOUND;
        return false;
    }
    esp_err_t err = esp_flash_write(spi_flash_, buffer, address, length);
    if ( err ) {
        last_error_ = (int)err;
        return false;
    }
    return true;
#else
    last_error_ = 1;
    return false;
#endif
}

bool SPIFlash::erase(uint32_t start, uint32_t length) {
#ifdef ESP32
    if ( !spi_flash_ || !ready_ ) {
        last_error_ = (int)ESP_ERR_NOT_FOUND;
        return false;
    }
    esp_err_t err = esp_flash_erase_region(spi_flash_, start, length);
    if ( err ) {
        last_error_ = (int)err;
        return false;
    }
    return true;
#else
    last_error_ = 1;
    return false;
#endif
}

uint32_t SPIFlash::size() {
#ifdef ESP32
    if ( !spi_flash_ ) {
        last_error_ = (int)ESP_ERR_NOT_FOUND;
        return false;
    }
    return (uint32_t)(spi_flash_->size);
#else
    return 0;
#endif
}
