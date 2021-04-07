#ifndef LIB_ADAFRUIT_SSD1675BX
#define LIB_ADAFRUIT_SSD1675BX

#include <Adafruit_EPD.h>
#include <Arduino.h>

#define EPD_RAM_BW 0x10
#define EPD_RAM_RED 0x13

#define SSD1675BX_DRIVER_CONTROL 0x01
#define SSD1675BX_GATE_VOLTAGE 0x03
#define SSD1675BX_SOURCE_VOLTAGE 0x04
#define SSD1675BX_DEEP_SLEEP 0x10
#define SSD1675BX_DATA_MODE 0x11
#define SSD1675BX_SW_RESET 0x12
#define SSD1675BX_HV_READY 0x14
#define SSD1675BX_VCI_READY 0x15
#define SSD1675BX_TEMP_SELECT 0x18
#define SSD1675BX_TEMP_WRITE 0x1A
#define SSD1675BX_MASTER_ACTIVATE 0x20
#define SSD1675BX_DISP_CTRL1 0x21
#define SSD1675BX_DISP_CTRL2 0x22
#define SSD1675BX_WRITE_RAM1 0x24
#define SSD1675BX_WRITE_RAM2 0x26
#define SSD1675BX_WRITE_VCOM 0x2C
#define SSD1675BX_READ_OTP 0x2D
#define SSD1675BX_READ_STATUS 0x2F
#define SSD1675BX_WRITE_LUT 0x32
#define SSD1675BX_WRITE_DUMMY 0x3A
#define SSD1675BX_WRITE_GATELINE 0x3B
#define SSD1675BX_WRITE_BORDER 0x3C
#define SSD1675BX_SET_RAMXPOS 0x44
#define SSD1675BX_SET_RAMYPOS 0x45
#define SSD1675BX_SET_RAMXCOUNT 0x4E
#define SSD1675BX_SET_RAMYCOUNT 0x4F
#define SSD1675BX_SET_ANALOGBLOCK 0x74
#define SSD1675BX_SET_DIGITALBLOCK 0x7E

/**************************************************************************/
/*!
    @brief  Class for interfacing with SSD1675 (2020 BSides Badge EINK) EPD drivers
*/
/**************************************************************************/
class Adafruit_SSD1675BX : public Adafruit_EPD {
public:
  Adafruit_SSD1675BX(int width, int height, int8_t SID, int8_t SCLK, int8_t DC,
                     int8_t RST, int8_t CS, int8_t SRCS, int8_t MISO,
                     int8_t BUSY = -1);
  Adafruit_SSD1675BX(int width, int height, int8_t DC, int8_t RST, int8_t CS,
                     int8_t SRCS, int8_t BUSY = -1, SPIClass *spi = &SPI);

  void begin(bool reset = true);
  void powerUp();
  void update();
  void powerDown();

  // faster drawing
  void fillScreen(uint16_t color);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void display();

  // added so we can write white pixels
  void invertBuffers();
  void swapBuffers();

  const static uint8_t kModeBlackWhiteRed = 0;
  const static uint8_t kModeFullBlackWhite = 1;
  const static uint8_t kModeNormalBlackWhite = 2;
  const static uint8_t kModeFasterBlackWhite = 3;
  const static uint8_t kModeFastestBlackWhite = 4;
  void setMode(uint8_t mode);
  uint8_t getMode() const { return mode_; };
  inline bool fastMode() { return ( mode_ == kModeFasterBlackWhite) || ( mode_ == kModeFastestBlackWhite); }

  void thread();

protected:
  uint8_t writeRAMCommand(uint8_t index);
  void setRAMAddress(uint16_t x, uint16_t y);
  void busy_wait();

private:
  uint16_t einkColor(uint16_t color);

  uint8_t mode_;
  uint16_t bw_red_color_;

  // threading for speed (wait in background for hardware update to complete).
  void threadInit();
  void lock();
  void unlock();
  void renderBegin();
  void renderEnd();
  void uploadBegin();
  void uploadEnd();
  void displayBegin();
  void displayEnd();
  void displayQueuePush();
  bool displayQueuePop();
  bool displayQueueFull();
  void *lock_;
  bool render_in_progress_;
  bool upload_in_progress_;
  bool display_in_progress_;
  uint8_t refreshes_needed_;
};

#endif
