#include <cstring>

#include "serialbridge.h"
#include <BadgeLog.h>
#include <BadgeOs.h>
#include <BadgeService.h>
#include <SerialBridge.h>
#include <BSidesMessaging.h>
#include <BSidesBadge2020.h>


static SerialBridge samd;
BadgeService        serial_bridge_service;
SerialMessageQueue  serialBridgeQueue;
BridgeState         bridgeState = disconnected;
#define SEND_INTERVAL_MS (0U)
#define PING_INTERVAL_MS (10000U)

// Read and process a single message per "coroutine" loop
static SerialMessage msgToRecv;
// Write a single message per "coroutine" loop
static SerialMessage msgToSend;
// We wait until it's been acknowledged before dequeuing the next message
static bool lastMessageAcked = true;
#define ESP_MAX_RETRIES 3
#define ESP_RETRY_DELAY 250
static uint8_t retryAttempts = 0;
static int lastRetry = 0;
static int lastPing = 0;

// Message forwarding queues
extern SerialMessageQueue deviceInfoQueue;
extern SerialMessageQueue keyboardQueue;
extern SerialMessageQueue irdaQueue;

void enqueuePing(void) {
    SerialMessage msg;
    log(LOG_DEBUG, "Sending ping");
    if (!prepareMessage(msg, Diagnostics_PING, nextSequenceNumber(), NULL, 0)) {
        log(LOG_ERROR, "Failed to set up ping");
        return;
    }
    if (!enqueueMessage(msg, serialBridgeQueue)) {
        log(LOG_ERROR, "Failed to enqueue ping");
        return;
    }
    bridgeState = pending;
    lastPing = millis();
}

void handleAckNack(const SerialMessage &msg) {
    if (msg.header.seqNo > msgToSend.header.seqNo) {
        log(LOG_WARN, "Sequence number (n)acknowledged has advanced: expected %d received %d",
                msgToSend.header.seqNo, msg.header.seqNo);
    }
    if (((msg.header.type == Message_ACK)
                && (msg.header.seqNo == msgToSend.header.seqNo))
            || ((msg.header.type == Message_NACK)
                && (retryAttempts >= ESP_MAX_RETRIES))
       ) {
        lastMessageAcked = true;
        retryAttempts = 0;
    }
}

void handleMessage(SerialMessage &msg) {
    switch (msg.header.type) {
        case Message_ACK:
        case Message_NACK:
            handleAckNack(msg);
            break;
        case VBATResponse:
            if (!enqueueMessage(msg, deviceInfoQueue)) {
                log(LOG_ERROR, "Failed to send VBAT response to info service");
            }
            break;
        case Capsense:
            if (!enqueueMessage(msg, keyboardQueue)) {
                log(LOG_ERROR, "Failed to enqueue capsense message for keyboard service");
            }
            break;
        case Diagnostics_PONG:
            bridgeState = live;
            break;
        case IRdACheck:
            if (!enqueueMessage(msg, irdaQueue)) {
                log(LOG_ERROR, "Failed to enqueue IRdACheck message for IR service");
            }
            break;
        default:
            log(LOG_WARN, "Unhandled message from SAMD: %s", messageType(msg.header.type));
            break;
    }
}

void serialbridge_begin(void)
{
    serial_bridge_service.set_begin([](){
        bridgeState = disconnected;
        samd.connect();
        lastPing = 0;
    });
    serial_bridge_service.set_loop([](){
        // Check if we need to send a PING
        if ((millis() - lastPing) > PING_INTERVAL_MS) {
            enqueuePing();
        }
        // Handle receiving
        memset(&msgToRecv, 0, sizeof(msgToRecv));
        if (samd.recvMessage(&msgToRecv)) {
            log(LOG_DEBUG, "Received %s message", messageType(msgToRecv.header.type));
                handleMessage(msgToRecv);
        }
        // Handle sending
        if (lastMessageAcked) {
            if (dequeueMessage(&msgToSend, serialBridgeQueue)) {
                lastMessageAcked = false;
                retryAttempts = 0;
                lastRetry = millis();
                log(LOG_DEBUG, "Sending a %s message to SAMD", messageType(msgToSend.header.type));
                if (!samd.sendMessage(msgToSend)) {
                    log(LOG_ERROR, "Failed to send message");
                }
            }
        } else {
            if (millis() - lastRetry >= ESP_RETRY_DELAY) {
                retryAttempts++;
                if (retryAttempts >= ESP_MAX_RETRIES) {
                    log(LOG_WARN, "Retries exceeded, dropping %s message", messageType(msgToSend.header.type));
                    retryAttempts = 0;
                    lastMessageAcked = true;
                    if (msgToSend.header.type == Diagnostics_PING) {
                        bridgeState = disconnected;
                    }
                } else {
                    log(LOG_DEBUG, "(RETRY) Sending a %s message to ESP32", messageType(msgToSend.header.type));
                    if (!samd.sendMessage(msgToSend)) {
                        log(LOG_ERROR, "(RETRY) Failed to send message");
                    }
                }
            }
        }
    });
    serial_bridge_service.set_period_ms(SEND_INTERVAL_MS);
    badge_os.add_service(serial_bridge_service);
    serial_bridge_service.start();
}
