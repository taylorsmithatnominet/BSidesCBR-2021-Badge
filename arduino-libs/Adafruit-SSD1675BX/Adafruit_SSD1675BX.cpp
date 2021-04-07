#include "Adafruit_SSD1675BX.h"

// We use a thread for display updates when built for ESP32
#ifdef ESP32
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#endif

/** specification of display update instruction */
struct badge_eink_lut_entry {
    /** the number of cycles the voltages are held; 0 = end of list */
    uint8_t length;

    /** bitmapped value containing voltages for every (old-bit, new-bit) pair:
     * - bits 0,1: from 0 to 0
     * - bits 2,3: from 0 to 1
     * - bits 4,5: from 1 to 0
     * - bits 6,7: from 1 to 1
     *
     * allowed values:
     * - 0: VSS
     * - 1: VSH
     * - 2: VSL
     */
    uint8_t voltages;
};

/** filters to use on a badge_eink_lut_entry structure */
enum badge_eink_lut_flags {
    LUT_FLAG_NONE     = 0,
    LUT_FLAG_FIRST    = 1, // do not depend on previous image
    LUT_FLAG_PARTIAL  = 2, // do not touch already correct pixels
    LUT_FLAG_WHITE    = 4, // white only
    LUT_FLAG_BLACK    = 8, // black only
    LUT_FLAG_RED      = 16, // red only
};

// full, includes inverting
const struct badge_eink_lut_entry badge_eink_lut_full[] = {
    { .length = 23, .voltages = 0x02, },
    { .length =  4, .voltages = 0x01, },
    { .length = 11, .voltages = 0x11, },
    { .length =  4, .voltages = 0x12, },
    { .length =  6, .voltages = 0x22, },
    { .length =  5, .voltages = 0x66, },
    { .length =  4, .voltages = 0x69, },
    { .length =  5, .voltages = 0x59, },
    { .length =  1, .voltages = 0x58, },
    { .length = 14, .voltages = 0x99, },
    { .length =  1, .voltages = 0x88, },
    { .length = 0 }
};

// full, no inversion
const struct badge_eink_lut_entry badge_eink_lut_normal[] = {
    { .length =  3, .voltages = 0x10, },
    { .length =  5, .voltages = 0x18, },
    { .length =  1, .voltages = 0x08, },
    { .length =  8, .voltages = 0x18, },
    { .length =  2, .voltages = 0x08, },
    { .length = 0 }
};

// full, no inversion, needs 2 updates for full update
const struct badge_eink_lut_entry badge_eink_lut_faster[] = {
    { .length =  1, .voltages = 0x10, },
    { .length =  8, .voltages = 0x18, },
    { .length =  1, .voltages = 0x08, },
    { .length = 0 }
};

// full, no inversion, needs 4 updates for full update
const struct badge_eink_lut_entry badge_eink_lut_fastest[] = {
    { .length =  1, .voltages = 0x10, },
    { .length =  3, .voltages = 0x18, },
    { .length =  1, .voltages = 0x08, },
    { .length = 0 }
};

static uint8_t
badge_eink_lut_conv(uint8_t voltages, enum badge_eink_lut_flags flags)
{
    if (flags & LUT_FLAG_FIRST)
    {
        voltages |= voltages >> 4;
        voltages &= 15;
        if ((voltages & 3) == 3) // reserved
            voltages ^= 2; // set to '1': VSH (black)
        if ((voltages & 12) == 12) // reserved
            voltages ^= 4; // set to '2': VSL (white)
        voltages |= voltages << 4;
    }

    if (flags & LUT_FLAG_PARTIAL)
        voltages &= 0x3c; // only keep 0->1 and 1->0

    if (flags & LUT_FLAG_WHITE)
        voltages &= 0xcc; // only keep 0->1 and 1->1

    if (flags & LUT_FLAG_BLACK)
        voltages &= 0x33; // only keep 0->0 and 1->0

    return voltages;
}

// DEPG0290B01
int
badge_eink_lut_generate_depg0290rws(const struct badge_eink_lut_entry *list, enum badge_eink_lut_flags flags, uint8_t *lut)
{
    //ESP_LOGD(TAG, "flags = %d.", flags);

    memset(lut, 0, 70);

    int pos = 0;
    int spos = 0;
    while (list->length != 0)
    {
        int len = list->length;
        if (pos == 7)
        {
            //ESP_LOGE(TAG, "lut overflow.");
            return -1; // full
        }
        uint8_t voltages = badge_eink_lut_conv(list->voltages, flags);

        lut[0*10 + pos] |= ((voltages >> 0) & 3) << ((3-spos)*2);
        lut[1*10 + pos] |= ((voltages >> 2) & 3) << ((3-spos)*2);
        lut[2*10 + pos] |= ((voltages >> 4) & 3) << ((3-spos)*2);
        lut[3*10 + pos] |= ((voltages >> 6) & 3) << ((3-spos)*2);
        lut[5*10 + pos*5 + spos] = len;
        lut[5*10 + pos*5 + spos] = len;

        spos++;
        if (spos == 2)
        {
            spos = 0;
            pos++;
        }

        list = &list[1];
    }

    return 70;
}

#define BUSY_WAIT               500
#define BUSY_WAIT_FAST_MODE     120

#define FAST_MODE_REFRESHES_NEEDED      8

const unsigned char LUT_DATA[] = {
    0xA0, 0x90, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x50, 0x90, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xA0, 0x90, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x50, 0x90, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x0F, 0x0F, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x03,
    0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x17, 0x41, 0x00, 0x32, 0x50, 0x2C, 0x0B,
};

/**************************************************************************/
/*!
    @brief constructor if using external SRAM chip and software SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param SID the SID pin to use
    @param SCLK the SCLK pin to use
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param SRCS the SRAM chip select pin to use
    @param MISO the MISO pin to use
    @param BUSY the busy pin to use
*/
/**************************************************************************/
Adafruit_SSD1675BX::Adafruit_SSD1675BX(int width, int height, int8_t SID,
                                       int8_t SCLK, int8_t DC, int8_t RST,
                                       int8_t CS, int8_t SRCS, int8_t MISO,
                                       int8_t BUSY)
    : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY) {

  mode_ = kModeBlackWhiteRed;
  bw_red_color_ = EPD_BLACK;

  if ((height % 8) != 0) {
    height += 8 - (height % 8);
  }

  buffer1_size = width * height / 8;
  buffer2_size = buffer1_size;

  if (SRCS >= 0) {
    use_sram = true;
    buffer1_addr = 0;
    buffer2_addr = buffer1_size;
    buffer1 = buffer2 = NULL;
  } else {
    buffer1 = (uint8_t *)malloc(buffer1_size);
    buffer2 = (uint8_t *)malloc(buffer2_size);
  }

  threadInit();
}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset

/**************************************************************************/
/*!
    @brief constructor if using on-chip RAM and hardware SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param SRCS the SRAM chip select pin to use
    @param BUSY the busy pin to use
*/
/**************************************************************************/
Adafruit_SSD1675BX::Adafruit_SSD1675BX(int width, int height, int8_t DC,
                                       int8_t RST, int8_t CS, int8_t SRCS,
                                       int8_t BUSY, SPIClass *spi)
    : Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY, spi) {

  mode_ = kModeBlackWhiteRed;
  bw_red_color_ = EPD_BLACK;

  if ((height % 8) != 0) {
    height += 8 - (height % 8);
  }

  buffer1_size = width * height / 8;
  buffer2_size = buffer1_size;

  if (SRCS >= 0) {
    use_sram = true;
    buffer1_addr = 0;
    buffer2_addr = buffer1_size;
    buffer1 = buffer2 = NULL;
  } else {
    buffer1 = (uint8_t *)malloc(buffer1_size);
    buffer2 = (uint8_t *)malloc(buffer2_size);
  }

  threadInit();
}

/**************************************************************************/
/*!
    @brief set display mode (sharp & colors VS fast black & white)
*/
/**************************************************************************/
void Adafruit_SSD1675BX::setMode(uint8_t mode)
{
    lock();
    refreshes_needed_ = 0;
    while ( upload_in_progress_ || display_in_progress_ )
    {
        unlock();
        delay(1);
        lock();
    }
    mode_ = mode;
    if ( fastMode() )
    {
        buffer2_size = 0;
    }
    else
    {
        buffer2_size = buffer1_size;
    }
    unlock();
}

/**************************************************************************/
/*!
    @brief wait for busy signal to end
*/
/**************************************************************************/
void Adafruit_SSD1675BX::busy_wait(void) {
    if (_busy_pin >= 0) {
        // start of busy wait
        uint32_t start = (uint32_t)millis();
        // use 1ms delays for the first 20ms delays
        for ( uint8_t i = 0; i < 20; i++ )
        {
            if ( !digitalRead(_busy_pin) )
            {
                return;
            }
            delay(1);
        }
        // wait for busy low
        while (digitalRead(_busy_pin)) {
            delay(10);
            if ( fastMode() && displayQueueFull() )
            {
                uint32_t tick = (uint32_t)millis();
                if ( (tick - start) > BUSY_WAIT_FAST_MODE )
                {
                    // fast mode - stop this wait and start with "newer" data
                    return;
                }
            }
        }
    } else {
        delay(BUSY_WAIT);
    }
}

/**************************************************************************/
/*!
    @brief begin communication with and set up the display.
    @param reset if true the reset pin will be toggled.
*/
/**************************************************************************/
void Adafruit_SSD1675BX::begin(bool reset) {
  Adafruit_EPD::begin(reset);
  setBlackBuffer(0, true); // black defaults to inverted
  setColorBuffer(1, false);

  powerDown();
}

void Adafruit_SSD1675BX::invertBuffers() {
    setBlackBuffer(0, !blackInverted);
    setColorBuffer(1, !colorInverted);
}

void Adafruit_SSD1675BX::swapBuffers() {
    uint8_t *tmp = buffer1;
    buffer1 = buffer2;
    buffer2 = tmp;
    uint16_t tmp_addr = buffer1_addr;
    buffer1_addr = buffer2_addr;
    buffer2_addr = tmp_addr;
    setBlackBuffer(0, blackInverted);
    setColorBuffer(1, colorInverted);
}

/**************************************************************************/
/*!
    @brief faster drawing methods
*/
/**************************************************************************/
uint16_t Adafruit_SSD1675BX::einkColor(uint16_t color)
{
    if ( ( color & (0x8000 | 0x0400 | 0x0010) ) == 0 )
    {
        // high bits of RGB are all 0s, treat it as black
        color = EPD_BLACK;
    }
    else if ( ( color & (0x8000 | 0x0400 | 0x0010) ) == (0x8000 | 0x0400 | 0x0010) )
    {
        // high bits of RGB are all 1s, treat it as white
        color = EPD_WHITE;
    }
    else
    {
        // everything else is red
        color = EPD_RED;
    }
    return color;
}

void Adafruit_SSD1675BX::fillScreen(uint16_t color)
{
    renderBegin();

    color = einkColor(color);
    if ( mode_ != kModeBlackWhiteRed ) {
        if ( color == EPD_BLACK ) {
            bw_red_color_ = EPD_WHITE;
        } else if ( color == EPD_WHITE ) {
            bw_red_color_ = EPD_BLACK;
        } else {
            bw_red_color_ = EPD_BLACK;
            color = EPD_WHITE;
        }
    }
    uint8_t black_byte = ( (color == EPD_BLACK) ? 0x00 : 0xff );
    if ( !fastMode() ) {
        // slow mode - memset both BLACK and RED buffers.
        uint8_t color_byte = ( (color == EPD_RED) ? 0xff : 0x00 );
        if (use_sram) {
            if (buffer1_size != 0) {
                sram.erase(buffer1_addr, buffer1_size, black_byte);
            }
            if (buffer2_size != 0) {
                sram.erase(buffer2_addr, buffer2_size, color_byte);
            }
        } else {
            if (buffer1 && (buffer1_size != 0)) {
                memset(buffer1, black_byte, buffer1_size);
            }
            if (buffer2 && (buffer2_size != 0)) {
                memset(buffer2, color_byte, buffer2_size);
            }
        }
    } else {
        // fast mode - render into buffer 2, display buffer 1, then swap etc.
        if (use_sram) {
            sram.erase(buffer2_addr, buffer1_size, black_byte);
        } else {
            memset(buffer2, black_byte, buffer1_size);
        }
    }
}

void Adafruit_SSD1675BX::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if ( !fastMode() )
    {
        // fix color
        color = einkColor(color);
        // what to do with 
        if ( mode_ != kModeBlackWhiteRed ) {
            color = ( color == EPD_RED ) ? bw_red_color_ : color;
        }
        // hack
        y = height() - 1 - y;
        // slow mode - use Adafruit_EPD to draw the pixel.
        if ( color == EPD_WHITE ) {
            invertBuffers();
            Adafruit_EPD::drawPixel(x, y, EPD_BLACK);
            Adafruit_EPD::drawPixel(x, y, EPD_RED);
            invertBuffers();
            return;
        }
        Adafruit_EPD::drawPixel(x, y, color);
    } else {
        // fast mode - almost a 1-to-1 copy of drawPixel from Adafruit_EPD - with some optimisations

        // check bounds.
        if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
            return;

        if ( ( color & (0x8000 | 0x0400 | 0x0010) ) == 0 )
        {
            // high bits of RGB are all 0s, treat it as black
            color = EPD_BLACK;
        }
        else if ( ( color & (0x8000 | 0x0400 | 0x0010) ) == (0x8000 | 0x0400 | 0x0010) )
        {
            // high bits of RGB are all 1s, treat it as white
            color = EPD_WHITE;
        }
        else
        {
            // everything else is red
            color = bw_red_color_;
        }

        uint8_t *pBuf;

        // check rotation, move pixel around if necessary
        switch (getRotation()) {
            case 0:
                y = height() - 1 - y;
                break;
            case 1:
                EPD_swap(x, y);
                break;
            case 2:
                y = height() - 1 - y;
                x = WIDTH - x - 1;
                y = HEIGHT - y - 1;
                break;
            case 3:
                EPD_swap(x, y);
                x = WIDTH - x - 1;
                y = HEIGHT - y - 1;
                break;
        }
        uint16_t addr = ((uint32_t)(WIDTH - 1 - x) * (uint32_t)HEIGHT + y) >> 3;
        uint8_t c;

        if (use_sram) {
            addr = buffer2_addr + addr;
            c = sram.read8(addr);
            pBuf = &c;
            if (color == EPD_BLACK) {
                *pBuf &= ~(1 << (7 - y & 0x7));
            } else {
                *pBuf |= (1 << (7 - y & 0x7));
            }
            sram.write8(addr, *pBuf);
        } else {
            pBuf = buffer2 + addr;
            if (color == EPD_BLACK) {
                *pBuf &= ~(1 << (7 - y & 0x7));
            } else {
                *pBuf |= (1 << (7 - y & 0x7));
            }
        }
    }
}

/**************************************************************************/
/*!
    @brief signal the display to update
*/
/**************************************************************************/
void Adafruit_SSD1675BX::update() {
    uint8_t mode_old = mode_;
    uploadEnd();
    if ( mode_old != mode_ )
    {
        // mode has changed, stop display.
        return;
    }
    displayBegin();

    uint8_t buf[1];

    uint8_t lut[70];
    memset(lut, 0, sizeof(lut));
    if ( mode_ != kModeBlackWhiteRed )
    {
        switch ( mode_ )
        {
            case kModeFullBlackWhite:
                badge_eink_lut_generate_depg0290rws(badge_eink_lut_full, LUT_FLAG_FIRST, lut);
                EPD_command(SSD1675BX_WRITE_LUT, lut, sizeof(lut));
                break;

            case kModeNormalBlackWhite:
                badge_eink_lut_generate_depg0290rws(badge_eink_lut_normal, LUT_FLAG_FIRST, lut);
                EPD_command(SSD1675BX_WRITE_LUT, lut, sizeof(lut));
                break;

            case kModeFasterBlackWhite:
                badge_eink_lut_generate_depg0290rws(badge_eink_lut_faster, LUT_FLAG_FIRST, lut);
                EPD_command(SSD1675BX_WRITE_LUT, lut, sizeof(lut));
                break;

            case kModeFastestBlackWhite:
                badge_eink_lut_generate_depg0290rws(badge_eink_lut_fastest, LUT_FLAG_FIRST, lut);
                EPD_command(SSD1675BX_WRITE_LUT, lut, sizeof(lut));
                break;

            default:
                break;
        }
    }

    // display update sequence
    buf[0] = 0xC7;
    EPD_command(SSD1675BX_DISP_CTRL2, buf, 1);

    EPD_command(SSD1675BX_MASTER_ACTIVATE);

    busy_wait();
    if (_busy_pin <= -1) {
        delay(3000);
    }

    displayEnd();
}

/**************************************************************************/
/*!
    @brief start up the display
*/
/**************************************************************************/
void Adafruit_SSD1675BX::powerUp() {
    uint8_t buf[5];

    hardwareReset();
    delay(1);
    busy_wait();

    // soft reset
    EPD_command(SSD1675BX_SW_RESET);
    busy_wait();

    // set analog block control
    buf[0] = 0x54;
    EPD_command(SSD1675BX_SET_ANALOGBLOCK, buf, 1);

    // set digital block control
    buf[0] = 0x3B;
    EPD_command(SSD1675BX_SET_DIGITALBLOCK, buf, 1);

    // driver output control
    buf[0] = WIDTH - 1;
    buf[1] = (WIDTH - 1) >> 8;
    buf[2] = 0x00;
    EPD_command(SSD1675BX_DRIVER_CONTROL, buf, 3);

    // Data entry sequence
    buf[0] = 0x01;
    EPD_command(SSD1675BX_DATA_MODE, buf, 1);

    // Set ram X start/end postion
    buf[0] = 0x00;
    buf[1] = (HEIGHT / 8) - 1;
    EPD_command(SSD1675BX_SET_RAMXPOS, buf, 2);

    // Set ram Y start/end postion
    buf[0] = WIDTH - 1;
    buf[1] = (WIDTH - 1) >> 8;
    buf[2] = 0x00;
    buf[3] = 0x00;
    EPD_command(SSD1675BX_SET_RAMYPOS, buf, 4);

    // border color
    buf[0] = 0x01;
    EPD_command(SSD1675BX_WRITE_BORDER, buf, 1);

    // Vcom Voltage
    buf[0] = 0x36;
    EPD_command(SSD1675BX_WRITE_VCOM, buf, 1);

    // Set gate voltage
    buf[0] = LUT_DATA[100];
    EPD_command(SSD1675BX_GATE_VOLTAGE, buf, 1);

    // Set source voltage
    buf[0] = LUT_DATA[101];
    buf[1] = LUT_DATA[102];
    buf[2] = LUT_DATA[103];
    EPD_command(SSD1675BX_SOURCE_VOLTAGE, buf, 3);

    // Set dummy line period
    buf[0] = LUT_DATA[105];
    EPD_command(SSD1675BX_WRITE_DUMMY, buf, 1);

    // Set gate line width
    buf[0] = LUT_DATA[106];
    EPD_command(SSD1675BX_WRITE_GATELINE, buf, 1);

    EPD_command(SSD1675BX_WRITE_LUT, LUT_DATA, 99);

    // Set internal temperature sensor
    buf[0] = 0x80;
    EPD_command(SSD1675BX_TEMP_SELECT, buf, 1);

    if ( !fastMode() )     {
        // slow mode - needed steps (but can be skipped - with artifacts - in fast mode).

        buf[0] = 0xB1;
        EPD_command(SSD1675BX_DISP_CTRL2, buf, 1);

        EPD_command(SSD1675BX_MASTER_ACTIVATE);
    }

    busy_wait();

    uploadBegin();
}

/**************************************************************************/
/*!
    @brief wind down the display
*/
/**************************************************************************/
void Adafruit_SSD1675BX::powerDown() {
  uint8_t buf[1];

  // Only deep sleep if we can get out of it
  if (_reset_pin >= 0) {
    // deep sleep
    buf[0] = 0x01;
    EPD_command(SSD1675BX_DEEP_SLEEP, buf, 1);
    delay(1);
  } else {
    EPD_command(SSD1675BX_SW_RESET);
    busy_wait();
  }
}

/**************************************************************************/
/*!
    @brief Send the specific command to start writing to EPD display RAM
    @param index The index for which buffer to write (0 or 1 or tri-color
   displays) Ignored for monochrome displays.
    @returns The byte that is read from SPI at the same time as sending the
   command
*/
/**************************************************************************/
uint8_t Adafruit_SSD1675BX::writeRAMCommand(uint8_t index) {
  if (index == 0) {
    return EPD_command(SSD1675_WRITE_RAM1, false);
  }
  if (index == 1) {
    return EPD_command(SSD1675_WRITE_RAM2, false);
  }
  return 0;
}

/**************************************************************************/
/*!
    @brief Some displays require setting the RAM address pointer
    @param x X address counter value
    @param y Y address counter value
*/
/**************************************************************************/
void Adafruit_SSD1675BX::setRAMAddress(uint16_t x, uint16_t y) {
  uint8_t buf[2];

  // Set RAM X address counter
  buf[0] = 0x00;
  EPD_command(SSD1675_SET_RAMXCOUNT, buf, 1);

  // Set RAM Y address counter
  buf[0] = WIDTH - 1;
  buf[1] = (WIDTH - 1) >> 8;
  EPD_command(SSD1675_SET_RAMYCOUNT, buf, 2);
}

#ifdef ESP32
void display_thread(void *arg) {
    reinterpret_cast<Adafruit_SSD1675BX*>(arg)->thread();
};
#endif

void Adafruit_SSD1675BX::threadInit() {
#ifdef ESP32
    lock_ = xSemaphoreCreateMutex();
    xTaskCreate(&display_thread, "display_thread", 8192, (void*)this, 5, NULL);
#endif
    render_in_progress_ = false;
    upload_in_progress_ = false;
    display_in_progress_ = false;
    refreshes_needed_ = 0;
}

void Adafruit_SSD1675BX::lock() {
#ifdef ESP32
    xSemaphoreTake(lock_, portMAX_DELAY);
#endif
}

void Adafruit_SSD1675BX::unlock() {
#ifdef ESP32
    xSemaphoreGive(lock_);
#endif
}

void Adafruit_SSD1675BX::thread() {
    while(1) {
        delay(10);
        if ( fastMode() && displayQueuePop() )
        {
            Adafruit_EPD::display();
        }
    }
}

void Adafruit_SSD1675BX::displayQueuePush() {
    lock();
    refreshes_needed_ = FAST_MODE_REFRESHES_NEEDED;
    unlock();
}

bool Adafruit_SSD1675BX::displayQueuePop() {
    uint8_t refreshes_needed = 0;
    lock();
    refreshes_needed = refreshes_needed_;
    refreshes_needed_ = refreshes_needed_ > 0 ? refreshes_needed_ - 1 : 0;
    unlock();
    return refreshes_needed > 0;
}

bool Adafruit_SSD1675BX::displayQueueFull() {
    uint8_t refreshes_needed = 0;
    lock();
    refreshes_needed = refreshes_needed_;
    unlock();
    return refreshes_needed == FAST_MODE_REFRESHES_NEEDED;
}

static uint32_t main_tick_ = 0;

void Adafruit_SSD1675BX::display(void) {
    renderEnd();
#ifdef ESP32
    if ( !fastMode() )
    {
        // slow mode - just display
        Adafruit_EPD::display();
    }
    else
    {
        // fast mode - queue display refresh
        displayQueuePush();
    }
#else
    // non-esp32, just display (on esp32, this is handled in a thread)
    Adafruit_EPD::display();
#endif
}

void Adafruit_SSD1675BX::renderBegin() {
    lock();
    if ( render_in_progress_ ) {
        unlock();
        return;
    }
    render_in_progress_ = true;
    unlock();
}

void Adafruit_SSD1675BX::renderEnd() {
    lock();
    if ( !render_in_progress_ ) {
        unlock();
        return;
    }
    render_in_progress_ = false;
    if ( fastMode() )
    {
        // fast mode - wait for upload to complete before swapping buffers.
        while ( upload_in_progress_ )
        {
            unlock();
            delay(1);
            lock();
        }
        swapBuffers();
    }
    unlock();
}

void Adafruit_SSD1675BX::uploadBegin() {
    lock();
    if ( upload_in_progress_ ) {
        unlock();
        return;
    }
    upload_in_progress_ = true;
    unlock();
}

void Adafruit_SSD1675BX::uploadEnd() {
    lock();
    if ( !upload_in_progress_ ) {
        unlock();
        return;
    }
    upload_in_progress_ = false;
    unlock();
}

void Adafruit_SSD1675BX::displayBegin() {
    lock();
    if ( display_in_progress_ ) {
        unlock();
        return;
    }
    display_in_progress_ = true;
    unlock();
}

void Adafruit_SSD1675BX::displayEnd() {
    lock();
    if ( !display_in_progress_ ) {
        unlock();
        return;
    }
    display_in_progress_ = false;
    unlock();
}
