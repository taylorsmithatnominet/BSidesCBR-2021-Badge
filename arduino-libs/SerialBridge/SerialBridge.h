#ifndef _SERIAL_BRIDGE_2020_
#define _SERIAL_BRIDGE_2020_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "BSidesBadge2020.h"
#include "BSidesMessaging.h"

#ifdef __AVR__
#ifndef ssize_t
#define ssize_t int
#endif
#endif

#define WOULD_BLOCK     ((ssize_t)-100)
#ifdef SAMD21
extern Uart DebugSerial;
#endif

typedef enum BridgeState {
    disconnected,
    pending,
    live,
} BridgeState;

class SerialBridgeLink {
    public:
        SerialBridgeLink();
        void begin(unsigned long baud=9600);
        void end(void);
        int available(void);
        int read(void);
        void flush(void);
        size_t write(uint8_t);
};

class SerialBridge {
    public:
        SerialBridge();
        void connect(void);
        void disconnect(void);
        int available(void);
        bool sendMessage(const SerialMessage& msg);
        bool recvMessage(SerialMessage* msg);
        ssize_t send(const void *buf, size_t buf_size);
        ssize_t recv(void *buf, size_t buf_size);
    private:
        SerialBridgeLink link;
        void sendAckMessage(uint8_t seqNo);
        void sendNackMessage(uint8_t seqNo);
};

#endif // _SERIAL_BRIDGE_2020_
