#ifndef _ADAFRUIT_X11_
#define _ADAFRUIT_X11_

#include <stdint.h>

#include <Adafruit_GFX.h>

class Adafruit_X11 : public Adafruit_GFX  {
    public:
        Adafruit_X11(int16_t width, int16_t height);

        // Adafruit_GFX provides all other drawing APIs from this
        virtual void drawPixel(int16_t x, int16_t y, uint16_t color);

        // Window needs special handling of rotation.
        void setRotation(uint8_t r);

        // call on yield
        void handle_event_on_yield(void);

        // used internally to flush pixels to the screen
        void flush(void);

    private:
        uint8_t     data_[128];
};

#endif // _ADAFRUIT_X11_
