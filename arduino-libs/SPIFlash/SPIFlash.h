/*
 * SPIFlash.h - Library for controlling the flash chip on the BSides Badge 2020
 * Reference: 
 * Created by Peter Rankin, 9 January 2021
*/

#ifndef SPI_FLASH_H_
#define SPI_FLASH_H_

#include <Arduino.h>
#include <SPI.h>

#ifdef ESP32
#include <driver/gpio.h>
#include <driver/spi_common.h>
#include <esp_err.h>
// because the ESP-IDF is not perfect
#ifdef __cplusplus
extern "C" {
#endif
#include <esp_flash_spi_init.h>
#ifdef __cplusplus
}
#endif
#include <esp_flash.h>
#include <hal/spi_types.h>
#endif  // ESP32

class SPIFlash
{
    public:
        // Set the SPIClass
        SPIFlash(int8_t CS, SPIClass *spi = &SPI);
        // Initialize connection to flash chip
        bool begin();
        // Read
        bool read(void *buffer, uint32_t address, uint32_t length);
        // Write
        bool write(const void *buffer, uint32_t address, uint32_t length);
        // Erase
        bool erase(uint32_t start, uint32_t length);
        bool erase() { return erase(0x00000000, size()); }
        // Flash size
        uint32_t size();
        // Last error code
        int last_error() { return last_error_; }
    private:
        int last_error_;
        int8_t cs_;
        SPIClass *spi_;
        bool ready_;
#ifdef ESP32
        esp_flash_spi_device_config_t spi_flash_cfg_;
        esp_flash_t *spi_flash_;
#endif
};

#endif // SPI_FLASH_H_
