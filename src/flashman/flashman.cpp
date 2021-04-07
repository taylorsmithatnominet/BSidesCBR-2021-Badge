#include "flashman.h"

#include <BadgeLog.h>
#include <SPIFlash.h>

#ifdef ESP32
SPIClass SPI3_FLASH(HSPI);
SPIFlash flash(GPIO_NUM_15, &SPI3_FLASH);
#else
SPIFlash flash(-1, NULL);
#endif

void flashman_init(void) {
    int err = 0;
    if ( !flash.begin() ) {
        log(LOG_ERROR, "Failed to initialise the SPI flash: %d", flash.last_error());
    } else {
        uint32_t address = 0;
        if ( flash.size() > 0x1000 )
        {
            address = flash.size() - 0x1000;
        }
        log(LOG_INFO, "SPI flash initialised successfully! ");
        log(LOG_INFO, "Testing R/W at ADDR: 0x%08x", address);
        char buf[128] = {0};
        /* Read */
        err = flash.read(buf, address, sizeof(buf)) ? 0 : flash.last_error();
        log(LOG_INFO, "Flash at ADDR: 0x%08x '%s' (retcode %d)", address, buf, err);
        /* Erase */
        err = flash.erase(address, 4096) ? 0 : flash.last_error();
        log(LOG_INFO, "Flash erased at ADDR: 0x%08x (retcode %d)", address, err);
        /* Write */
        snprintf(buf, sizeof(buf), "HELLO WORLD - %i", millis());
        err = flash.write(buf, address, sizeof(buf)) ? 0 : flash.last_error();
        log(LOG_INFO, "Flash written at ADDR: 0x%08x '%s' (retcode %d)", address, buf, err);
        /* Read */
        err = flash.read(buf, address, sizeof(buf)) ? 0 : flash.last_error();
        log(LOG_INFO, "Flash at ADDR: 0x%08x '%s' (retcode %d)", address, buf, err);
    }
}
