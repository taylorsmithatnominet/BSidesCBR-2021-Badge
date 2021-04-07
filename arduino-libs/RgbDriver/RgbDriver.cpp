/*
 * rgbDriver.h - Library for controlling the TI LT5024 RGB LED driver on the BSides Badge 2020
 * Reference: https://www.ti.com/lit/ds/symlink/lp5024.pdf
 * Created by Jordan Johnson, 23 February 2020
*/

#include "Arduino.h"
#include "Wire.h"
#include "RgbDriver.h"

// Set the slave address of the LP5024 driver
RgbDriver::RgbDriver(char address)
{
    slaveAddress = address;
}

// Public Methods

// Set Chip_EN=0 to enter STANDBY mode
int RgbDriver::selectStandbyMode() {
    return transmit(DEVICE_CONFIG0, 0x00);
}

// Set Chip_EN=1 to enter normal mode
int RgbDriver::selectNormalMode() {
    Wire.begin();
    return transmit(DEVICE_CONFIG0, 0x40);
}

// Reset the LP5024
int RgbDriver::reset() {
    int status = transmit(RESET, 0xFF);
    if (status == 0) {
        status = transmit(RESET, 0x00);
     }
    return status;
}

// Set the LP5024 configuration
int RgbDriver::setDeviceConfig(bool Log_Scale_EN,
                               bool Power_Save_EN,
                               bool Auto_Incr_EN,
                               bool PWM_Dithering_EN,
                               bool Max_Current_Option,
                               bool LED_Global_Off) {
    char config = Log_Scale_EN * 32 +
                  Power_Save_EN * 16 +
                  Auto_Incr_EN * 8 +
                  PWM_Dithering_EN * 4 +
                  Max_Current_Option * 2 +
                  LED_Global_Off;
    return transmit(DEVICE_CONFIG1, config);
}

// Enable or disable individual LEDs
int RgbDriver::setLedConfig(bool LED7_Bank_EN,
                            bool LED6_Bank_EN,
                            bool LED5_Bank_EN,
                            bool LED4_Bank_EN,
                            bool LED3_Bank_EN,
                            bool LED2_Bank_EN,
                            bool LED1_Bank_EN,
                            bool LED0_Bank_EN) {
    char config = LED7_Bank_EN * 128 +
                  LED6_Bank_EN * 64 +
                  LED5_Bank_EN * 32 +
                  LED4_Bank_EN * 16 +
                  LED3_Bank_EN * 8 +
                  LED2_Bank_EN * 4 +
                  LED1_Bank_EN * 2 +
                  LED0_Bank_EN;
    return transmit(LED_CONFIG0, config);
}

// Set the brightness for all LEDs
int RgbDriver::setBankBrightness(char bank_brightness) {
    return transmit(BANK_BRIGHTNESS, bank_brightness);
}

// Set the colour for all LEDs
int RgbDriver::setBankColour(char red, char green, char blue) {
    // RED
    int status = transmit(BANK_A_COLOR, red);
    // GREEN
    if (status == 0) {
        transmit(BANK_B_COLOR, green);
    }
    // BLUE
    if (status == 0) {
        transmit(BANK_C_COLOR, blue);
    }
    return status;
}

// Set the brightness for an individual LED
int RgbDriver::setLedBrightness(char led, char brightness) {
    char addr = led + 0x07;
    return transmit(addr, brightness);
}

// Set the colour for an individual LED
int RgbDriver::setLedColour(char led, char red, char green, char blue) {
    char redAddr = led * 3 + 0x0F;
    char greenAddr = led * 3 + 0x10;
    char blueAddr = led * 3 + 0x11;
    // RED
    int status = transmit(redAddr, red);
    // GREEN
    if (status == 0) {
        transmit(greenAddr, green);
    }
    // BLUE
    if (status == 0) {
        transmit(blueAddr, blue);
    }
    return status;
}

// Private Methods

// Transmit data to the LP5025 via I2C
int RgbDriver::transmit(char addr, char data) {
    Wire.beginTransmission(slaveAddress);
    Wire.write(addr);
    Wire.write(data);
    return Wire.endTransmission();
}
