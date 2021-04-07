/*
 * rgbDriver.h - Library for controlling the TI LT5024 RGB LED driver on the BSides Badge 2020
 * Reference: https://www.ti.com/lit/ds/symlink/lp5024.pdf
 * Created by Jordan Johnson, 23 February 2020
*/

#ifndef RGB_DRIVER_H_
#define RGB_DRIVER_H_

#include "Arduino.h"

class RgbDriver
{
    public:
        // Set the slave address of the LP5024 driver
        RgbDriver(char address);
        // Set Chip_EN=0 to enter STANDBY mode
        int selectStandbyMode();
        // Set Chip_EN=1 to enter normal mode
        int selectNormalMode();
        // Reset the LP5024
        int reset();
        // Set the LP5024 configuration
        int setDeviceConfig(bool Log_Scale_EN,
                            bool Power_Save_EN,
                            bool Auto_Incr_EN,
                            bool PWM_Dithering_EN,
                            bool Max_Current_Option,
                            bool LED_Global_Off);
        // Enable or disable individual LEDs
        int setLedConfig(bool LED7_Bank_EN,
                         bool LED6_Bank_EN,
                         bool LED5_Bank_EN,
                         bool LED4_Bank_EN,
                         bool LED3_Bank_EN,
                         bool LED2_Bank_EN,
                         bool LED1_Bank_EN,
                         bool LED0_Bank_EN);
        // Set the brightness for all LEDs
        int setBankBrightness(char bank_brightness);
        // Set the colour for all LEDs
        int setBankColour(char red, char green, char blue);
        // Set the brightness for an individual LED
        int setLedBrightness(char led, char brightness);
        // Set the colour for an individual LED
        int setLedColour(char led, char red, char green, char blue);
    private:
        // Transmit data to the LP5025 via I2C
        int transmit(char addr, char data);

        // Address registers
        const char DEVICE_CONFIG0 = 0x00;
        const char DEVICE_CONFIG1 = 0x01;
        const char LED_CONFIG0 = 0x02;
        const char BANK_BRIGHTNESS = 0x03;
        const char BANK_A_COLOR = 0x04;
        const char BANK_B_COLOR = 0x05;
        const char BANK_C_COLOR = 0x06;
        const char RESET = 0x027;
        // The address of the LP5024
        char slaveAddress;
};

#endif // RGB_DRIVER_H_
