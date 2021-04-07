#ifdef __unix__
#define HAS_X11_SUPPORT
#endif

#ifdef HAS_X11_SUPPORT
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Arduino.h>

#include "Adafruit_X11.h"

struct _priv {
    bool        initialised;
    size_t      data_size;
    char        *data;
    size_t      width;
    size_t      height;
    bool        needs_flush;

#ifdef HAS_X11_SUPPORT
    Display*    dpy;
    Window      win;
    Atom        WM_DELETE_WINDOW;
    Visual      *visual;
    XImage      *img;
    int         screen;
#endif
};
#define priv(x)    ((struct _priv*)((x)->data_))

Adafruit_X11::Adafruit_X11(int16_t width, int16_t height) : Adafruit_GFX(width, height)
{
    memset(this->data_, 0, sizeof(this->data_));
    priv(this)->initialised = false;
    priv(this)->needs_flush = false;
    priv(this)->width = (size_t)width;
    priv(this)->height = (size_t)height;

#ifdef HAS_X11_SUPPORT
    priv(this)->data_size = priv(this)->width * priv(this)->height * sizeof(uint32_t);
    priv(this)->data = (char*)malloc(priv(this)->data_size);
    if ( !priv(this)->data )
    {
        return;
    }

    priv(this)->dpy = XOpenDisplay(NULL);
    if (priv(this)->dpy == NULL) 
    {
        free(priv(this)->data);
        return;
    }

    priv(this)->screen = DefaultScreen(priv(this)->dpy);
    priv(this)->visual = DefaultVisual(priv(this)->dpy, priv(this)->screen);

    priv(this)->img = XCreateImage(priv(this)->dpy,
                                   priv(this)->visual,
                                   DefaultDepth(priv(this)->dpy, priv(this)->screen),
                                   ZPixmap,
                                   0,
                                   priv(this)->data,
                                   priv(this)->width,
                                   priv(this)->height,
                                   32,
                                   0);
    if ( !priv(this)->img )
    {
        XCloseDisplay(priv(this)->dpy);
        free(priv(this)->data);
        return;
    }

    priv(this)->win = XCreateSimpleWindow(priv(this)->dpy,
                                          RootWindow(priv(this)->dpy, priv(this)->screen),
                                          0,
                                          0,
                                          priv(this)->width,
                                          priv(this)->height,
                                          1,
                                          BlackPixel(priv(this)->dpy, priv(this)->screen),
                                          WhitePixel(priv(this)->dpy, priv(this)->screen));
    XSelectInput(priv(this)->dpy, priv(this)->win, ExposureMask | KeyPressMask | KeyReleaseMask);
    XMapWindow(priv(this)->dpy, priv(this)->win);

    XStoreName(priv(this)->dpy, priv(this)->win, "Adafruit X11");

    priv(this)->WM_DELETE_WINDOW = XInternAtom(priv(this)->dpy, "WM_DELETE_WINDOW", False); 
    XSetWMProtocols(priv(this)->dpy, priv(this)->win, &(priv(this)->WM_DELETE_WINDOW), 1);

    priv(this)->initialised = true;
#endif
}

void Adafruit_X11::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    uint8_t r, g, b;
    uint32_t colour32;

    if ( x < 0 || x > priv(this)->width )
    {
        return;
    }
    if ( y < 0 || y > priv(this)->height )
    {
        return;
    }

    r = ( color >> (11 - 3) ) & 0xf8;
    if ( r == 0xf8 )
    {
        r = 0xff;
    }

    g = ( color >> (5 - 2) ) & 0xfc;
    if ( g == 0xfc )
    {
        g = 0xff;
    }

    b = ( color << 3 ) & 0xf8;
    if ( b == 0xf8 )
    {
        b = 0xff;
    }

    colour32 = (uint32_t)r << 16 | (uint32_t)g << 8 | (uint32_t)b;

    uint32_t *pixel = (uint32_t *)(priv(this)->data);
    pixel[( y * priv(this)->width ) + x] = colour32;
    priv(this)->needs_flush = true;
}

void Adafruit_X11::setRotation(uint8_t r)
{
    Adafruit_GFX::setRotation(r);

    priv(this)->width = (size_t) width();
    priv(this)->height = (size_t) height();

#ifdef HAS_X11_SUPPORT
    // free existing image structure and pixel buffer
    XDestroyImage(priv(this)->img);
    priv(this)->data = NULL;  // XDestroyImage frees the pointer in data.

    // allocate a new buffer (because XDestroyImage freed the one we had).
    priv(this)->data_size = priv(this)->width * priv(this)->height * sizeof(uint32_t);
    priv(this)->data = (char*)malloc(priv(this)->data_size);
    if ( !priv(this)->data )
    {
        return;
    }

    // Create new image
    priv(this)->img = XCreateImage(priv(this)->dpy,
                                   priv(this)->visual,
                                   DefaultDepth(priv(this)->dpy, priv(this)->screen),
                                   ZPixmap,
                                   0,
                                   priv(this)->data,
                                   priv(this)->width,
                                   priv(this)->height,
                                   32,
                                   0);
    if ( !priv(this)->img )
    {
        XCloseDisplay(priv(this)->dpy);
        free(priv(this)->data);
        return;
    }

    // Resize window
    XResizeWindow(priv(this)->dpy,
                  priv(this)->win,
                  priv(this)->width,
                  priv(this)->height);
#endif
}

void Adafruit_X11::flush(void)
{
#ifdef HAS_X11_SUPPORT
    XPutImage(priv(this)->dpy,
              priv(this)->win,
              DefaultGC(priv(this)->dpy, priv(this)->screen),
              priv(this)->img,
              0,
              0,
              0,
              0,
              priv(this)->width,
              priv(this)->height);
#endif
}

#ifdef HAS_X11_SUPPORT
static Bool predicate_cb(Display *display, XEvent *event, XPointer arg)
{
    return True;
}
#endif

void Adafruit_X11::handle_event_on_yield(void)
{
    if (!priv(this)->initialised)
    {
        return;
    }

    if ( priv(this)->needs_flush )
    {
        this->flush();
        priv(this)->needs_flush = false;
    }

#ifdef HAS_X11_SUPPORT
    XEvent e;

    if (!XCheckIfEvent(priv(this)->dpy, &e, predicate_cb, NULL))
    {
        return;
    }

    if (e.type == Expose)
    {
        this->flush();
    }

    if (e.type == KeyPress)
    {
        if ( e.xkey.keycode == 0x09 ) // ESC
        {
            XDestroyWindow(priv(this)->dpy, priv(this)->win);
            XCloseDisplay(priv(this)->dpy);
            exit(0);
        }
        if ( e.xkey.keycode == 0x24 ||  e.xkey.keycode == 0x13) // ENTER or 0
        {
            digitalWrite(0, LOW);
        }
        if ( e.xkey.keycode == 0x64 ||  e.xkey.keycode == 0x0a) // LEFT or 1
        {
            digitalWrite(1, LOW);
        }
        if ( e.xkey.keycode == 0x62 ||  e.xkey.keycode == 0x0b) // UP or 2
        {
            digitalWrite(2, LOW);
        }
        if ( e.xkey.keycode == 0x66 ||  e.xkey.keycode == 0x0c) // RIGHT or 3
        {
            digitalWrite(3, LOW);
        }
        if ( e.xkey.keycode == 0x68 ||  e.xkey.keycode == 0x0d) // DOWN or 4
        {
            digitalWrite(4, LOW);
        }
    }

    if (e.type == KeyRelease)
    {
        if ( e.xkey.keycode == 0x24 ||  e.xkey.keycode == 0x13) // ENTER or 0
        {
            digitalWrite(0, HIGH);
        }
        if ( e.xkey.keycode == 0x64 ||  e.xkey.keycode == 0x0a) // LEFT or 1
        {
            digitalWrite(1, HIGH);
        }
        if ( e.xkey.keycode == 0x62 ||  e.xkey.keycode == 0x0b) // UP or 2
        {
            digitalWrite(2, HIGH);
        }
        if ( e.xkey.keycode == 0x66 ||  e.xkey.keycode == 0x0c) // RIGHT or 3
        {
            digitalWrite(3, HIGH);
        }
        if ( e.xkey.keycode == 0x68 ||  e.xkey.keycode == 0x0d) // DOWN or 4
        {
            digitalWrite(4, HIGH);
        }
    }

    if ((e.type == ClientMessage) && 
        (static_cast<unsigned int>(e.xclient.data.l[0]) == priv(this)->WM_DELETE_WINDOW))
    {
        XDestroyWindow(priv(this)->dpy, priv(this)->win);
        XCloseDisplay(priv(this)->dpy);
        exit(0);
    }
#endif
}
