#include <cstdbool>
#include <cstddef>
#include <cstdint>

#ifdef ARDUINO_SAMD_MKRZERO
#ifndef SAMD21
#define SAMD21
#endif
#endif // ARDUINO_SAMD_MKRZERO

#ifdef ESP32
#define HAS_BRIDGELINK
#define BRIDGELINK_NEEDS_FLUSH    true
#include <HardwareSerial.h>
#endif
#ifdef SAMD21
#define HAS_BRIDGELINK
#define BRIDGELINK_NEEDS_FLUSH
#include <Arduino.h>
#include "wiring_private.h" // pinPeripheral() function
#endif
#ifdef __unix__
#include <SoftwareSerial.h>
#include <sys/types.h>
#endif

#include <BadgeLog.h>

#include "SerialBridge.h"
// Include pin assignments
#include "BSidesBadge2020.h"

#ifdef SAMD21
// SAMD21 Arduino pinouts for UART bridge use -- we set up a Serial object here on the relevant pins
// UART_RXD on SERCOM4, PAD[0]
// UART_TXD on SERCOM4, PAD[2];
Uart BridgeLink(&sercom4, UART_RXD, UART_TXD, SERCOM_RX_PAD_0, UART_TX_PAD_2);
Uart DebugSerial(&sercom0, UART_SCI_RXD, UART_SCI_TXD, SERCOM_RX_PAD_3, UART_TX_PAD_2);
void SERCOM4_Handler()
{
   BridgeLink.IrqHandler();
}
void SERCOM0_Handler()
{
    DebugSerial.IrqHandler();
}
#endif
#ifdef ESP32
// ESP32 pinouts don't define a serial bridge on these pins, so we make our own
// We take over UART #1
HardwareSerial BridgeLink(1);
#endif

SerialBridgeLink::SerialBridgeLink()
{
}

void SerialBridgeLink::begin(unsigned long baud)
{
log(LOG_INFO, "Connecting Serial Link");
#ifdef SAMD21
pinPeripheral(UART_RXD, PIO_SERCOM_ALT);
pinPeripheral(UART_TXD, PIO_SERCOM_ALT);
pinPeripheral(UART_SCI_RXD, PIO_SERCOM);
pinPeripheral(UART_SCI_TXD, PIO_SERCOM);
BridgeLink.begin(baud);
#endif
#ifdef ESP32
BridgeLink.begin(baud, SERIAL_8N1, UART_RXD, UART_TXD);
#endif
log(LOG_INFO, "Connected");
}

void SerialBridgeLink::end(void)
{
#ifdef HAS_BRIDGELINK
    return BridgeLink.end();
#endif
}

int SerialBridgeLink::available(void)
{
#ifdef HAS_BRIDGELINK
    return BridgeLink.available();
#else
    return 0;
#endif
}

int SerialBridgeLink::read(void)
{
#ifdef HAS_BRIDGELINK
    return BridgeLink.read();
#else
    return 0;
#endif
}

void SerialBridgeLink::flush(void)
{
#ifdef HAS_BRIDGELINK
    return BridgeLink.flush(BRIDGELINK_NEEDS_FLUSH);
#endif
}

size_t SerialBridgeLink::write(uint8_t value)
{
#ifdef HAS_BRIDGELINK
    return BridgeLink.write(value);
#else
    return 0;
#endif
}

SerialBridge::SerialBridge()
{

}

void SerialBridge::connect(void)
{
    this->link.begin();
}

void SerialBridge::disconnect(void)
{
    this->link.end();
}

int SerialBridge::available(void)
{
    return this->link.available();
}

/* These are intended to ensure we get always get the start and end of a message, but
 * we don't currently do anything to mark framing past this. Any application layer
 * protocol on to of this wire protocol can handle SYNACKs if need be.
 */
#define SERIALBRIDGE_SYNC       (0b11011001U)
#define SERIALBRIDGE_EOF        (0b00100110U)
// 1 SYNC transmits appears to be enough for a mostly reliable line during busy messaging
#define SERIALBRIDGE_SYNC_TX    (1U)

bool SerialBridge::sendMessage(const SerialMessage& msg) {
    // Send sync bytes
    for (int i = 0; i < SERIALBRIDGE_SYNC_TX; ++i) {
        if (this->link.write(SERIALBRIDGE_SYNC) != 1) {
            return false;
        }
    }
    ssize_t sent = this->send(&msg, SERIALBRIDGE_MESSAGE_SIZE);
    if (sent != SERIALBRIDGE_MESSAGE_SIZE) {
        return false;
    }
    // We send a single EOF
    if (this->link.write(SERIALBRIDGE_EOF) != 1) {
        return false;
    }
    return true;
}

void SerialBridge::sendAckMessage(uint8_t seqNo) {
    SerialMessage msg;
    prepareMessage(msg, Message_ACK, seqNo, NULL, 0);
    log(LOG_DEBUG, "ACK message %d", seqNo);
    this->sendMessage(msg);
}

void SerialBridge::sendNackMessage(uint8_t seqNo) {
    SerialMessage msg;
    prepareMessage(msg, Message_NACK, seqNo, NULL, 0);
    log(LOG_DEBUG, "NACK message %d", seqNo);
    this->sendMessage(msg);
}

bool SerialBridge::recvMessage(SerialMessage* msg) {
    if (this->link.available() < (SERIALBRIDGE_MESSAGE_SIZE + (SERIALBRIDGE_SYNC_TX))) {
        log(LOG_DEBUG, "Not enough bytes to be a minimally valid message");
        return false;
    }
    // Find a SYNC byte
    uint8_t sync = 0;
    while ((sync != SERIALBRIDGE_SYNC) && (this->link.available() > 0)) {
        sync = this->link.read();
    }
    // Consume SYNC bytes
    while ((sync == SERIALBRIDGE_SYNC) && (this->link.available() > 0)) {
        sync = this->link.read();
    }
    // Ensure we haven't just consumed SYNCs
    if (sync == SERIALBRIDGE_SYNC) {
        log(LOG_ERROR, "Failed to find message");
        return false;
    }
    // Put the first byte in and consume the rest of the message
    uint8_t *buf = (uint8_t*)msg;
    buf[0] = sync;
    buf++;
    ssize_t recvd = this->recv((void*)buf, SERIALBRIDGE_MESSAGE_SIZE - 1) + 1;
    if (recvd < (SERIALBRIDGE_MESSAGE_SIZE - 1)) {
        log(LOG_DEBUG, "Not a full message: received %d expected %d", recvd, SERIALBRIDGE_MESSAGE_SIZE);
    }
    // Consume until EOF byte found
    sync = 0;
    while ((sync != SERIALBRIDGE_EOF) && (this->link.available() > 0)) {
        sync = this->link.read();
    }
    if (!validateMessage(*msg)) {
        log(LOG_ERROR, "Invalid %s message", messageType(msg->header.type));
        // Don't (n)ACK a (n)ACK
        if ((msg->header.type != Message_ACK) 
                && (msg->header.type != Message_NACK)) {
            this->sendNackMessage(msg->header.seqNo);
        }
        return false;
    }
    // Don't (n)ACK a (n)ACK
    if ((msg->header.type != Message_ACK) 
            && (msg->header.type != Message_NACK)) {
        this->sendAckMessage(msg->header.seqNo);
    }
    return true;
}

ssize_t SerialBridge::send(const void *buf, size_t buf_size)
{
    size_t i = -1;
#ifdef HAS_BRIDGELINK
    uint8_t *p = (uint8_t*) buf;

    for ( i = 0; i < buf_size; i++ ) {
        if (this->link.write(p[i]) != 1) goto flush;
    }

flush:
    this->link.flush();
#endif
    return (ssize_t)i;
}

ssize_t SerialBridge::recv(void *buf, size_t buf_size)
{
    uint8_t *p = (uint8_t*)buf;
    int offset = -1;
#ifdef HAS_BRIDGELINK
    while (this->link.available() && offset < (int)buf_size) {
        ++offset;
        p[offset] = this->link.read();
    }
#endif
    return offset;
}
