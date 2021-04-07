/*
 * bsidesbadge2020.h - BSides Badge 2020
 * Created by Jordan Johnson, 24 February 2020
 */
#ifndef BSIDES_BADGE_2020_H_
#define BSIDES_BADGE_2020_H_

// Device compiler detection
#ifdef ARDUINO_SAMD_MKRZERO
#ifndef SAMD21
#define SAMD21
#endif
#endif

// SAMD21
#ifdef SAMD21

// I2C CHANNEL
#define I2C_SCL         (12u) // PA09
#define I2C_SDA         (11u) // PA08
// UART BRIDGE
#define UART_RXD        (26u) // PA12
#define UART_TXD        (28u) // PA14
// UART DEBUG (Note TXD here corresponds to SAMD21 TX, and RXD to SAMD21 RX)
#define UART_SCI_TXD    (02u) // PA10
#define UART_SCI_RXD    (03u) // PA11
// SAMD USB
#define USB_SAMD_N      (22u) // PA24
#define USB_SAMD_P      (23u) // PA25
// SAO GPIO
#define SAO_GPIO0       (27u) // PA13
#define SAO_GPIO1       (29u) // PA15
// INFRA-RED TX/RX
#define IR_RX           (06u) // PA20
#define IR_TX           (07u) // PA21
#define IR_STANDBY      (24u) // PA18
// LEDS
#define LED6            (32u) // PB08
#define LED7            (30u) // PB27
#define LED8            (31u) // PB28
// CAPACITIVE TOUCH SENSORS
// These are in clockwise order when viewed from the
// front of the badge, starting at the top-left
#define CP1             (25u) // PA03
#define CP2             (18u) // PA04
#define CP3             (20u) // PA06
#define CP4             (17u) // PB03
#define CP5             (16u) // PB02
#define CP6             (15u) // PA02
#define CP7             (19u) // PA05
#define CP8             (21u) // PA07

// VBAT MEASURE
#define VBAT_MEAS       (33u) // PA09
// ARDUINO DIRECT CONNECT
#define AIN10           (16u) // PB02
#define AIN11           (17u) // PB03
#define TCC0_W4         (04u) // PB10
#define TCC0_W5         (05u) // PB11
#define S5_TX           (14u) // PB22
#define S5_RX           (13u) // PB03
#define AIN0            (15u) // PA02
#define VREFA           (25u) // PA03
#define AIN4            (18u) // PA04
#define AIN5            (19u) // PA05
#define AIN6            (20u) // PA06
#define AIN4            (21u) // PA07
#define S1_MOSI         (08u) // PA16
#define S1_SCK          (09u) // PA17
#define S1_MISO         (10u) // PA19
#define TC4_W0          (00u) // PA22
#define TC4_W1          (01u) // PA23

#endif // SAMD21

// ESP32
#ifdef ESP32

// UART BRIDGE
#define UART_RXD        (33u) // IO33
#define UART_TXD        (32u) // IO32

// EINK
#define EPD_SID   13 // SDA  (SPI4.MOSI)
#define EPD_SCLK  14 // SCL  (SPI3.CLK)
#define EPD_CS    27 // CS#  (SPI3.CS1)
#define EPD_DC    25 // D/C# (SPI3.WP)
#define EPD_SRAM_CS   -1 // NOT USED
#define EPD_RESET 26 // RES# (SPI3.HOLD)
#define EPD_MISO  12 // NOT USED
#define EPD_BUSY  35 // BUSY (DISP_BUSY)

// BUTTONS - DEV BOARD REV.1 (LEFT TO RIGHT)
#define BADGE_BUTTON_1_GPIO     (36u)
#define BADGE_BUTTON_2_GPIO     (37u)
#define BADGE_BUTTON_3_GPIO     (38u)
#define BADGE_BUTTON_4_GPIO     (39u)
#define BADGE_BUTTON_5_GPIO     (34u)

// BUTTONS
/* These are the old definitions for the dev board
 * Use the below uncommented values for the final board
#define BADGE_LEFT_GPIO         BADGE_BUTTON_1_GPIO
#define BADGE_UP_GPIO           BADGE_BUTTON_2_GPIO
#define BADGE_ENTER_GPIO        BADGE_BUTTON_3_GPIO
#define BADGE_DOWN_GPIO         BADGE_BUTTON_4_GPIO
#define BADGE_RIGHT_GPIO        BADGE_BUTTON_5_GPIO
*/
// D-Pad mapped buttons
#define BADGE_LEFT_GPIO         BADGE_BUTTON_2_GPIO
#define BADGE_UP_GPIO           BADGE_BUTTON_1_GPIO
#define BADGE_ENTER_GPIO        BADGE_BUTTON_5_GPIO
#define BADGE_DOWN_GPIO         BADGE_BUTTON_4_GPIO
#define BADGE_RIGHT_GPIO        BADGE_BUTTON_3_GPIO

// I2S
#define I2S_MCLK_BUFFER_EN      (GPIO_NUM_2)

#endif // ESP32

// DISPLAY
#define DISPLAY_WIDTH           ((int16_t)(296))
#define DISPLAY_HEIGHT          ((int16_t)(128))
#define DISPLAY_COLOR_BLACK     ((uint16_t)(0x0000))
#define DISPLAY_COLOR_WHITE     ((uint16_t)(0xffff))
#define DISPLAY_COLOR_RED       ((uint16_t)(0xf800))

// IMU
#define ESP32_I2C_IMU_ADDR      ((char)0x68)
#define ESP32_I2C_IMU_SDA       ((int)22)
#define ESP32_I2C_IMU_SCL       ((int)19)

// UNIX (HOST)
#ifdef __unix__

// BUTTONS
#define BADGE_LEFT_GPIO         (1u)
#define BADGE_UP_GPIO           (2u)
#define BADGE_ENTER_GPIO        (0u)
#define BADGE_DOWN_GPIO         (4u)
#define BADGE_RIGHT_GPIO        (3u)

#endif // __unix__

#endif // BSIDES_BADGE_2020_H_
