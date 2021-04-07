#include "CRSerialBridge.h"
#include <Arduino.h>
#include <ArduinoLowPower.h>
#include <BadgeLog.h>
#include <SerialBridge.h>
#include <BSidesMessaging.h>
#include <RgbDriver.h>

// Serial bridge to ESP32
static SerialBridge esp32;

// Serial bridge message queue
SerialMessageQueue serialBridgeQueue;

// Coroutine queues
extern SerialMessageQueue rgbLedQueue;
extern SerialMessageQueue saoQueue;
extern SerialMessageQueue irdaQueue;
extern SerialMessageQueue vbatQueue;
extern SerialMessageQueue capsenseQueue;

// RGB driver to turn off
extern RgbDriver rgbDriver;

// Read and process a single message per coroutine loop
static SerialMessage msgToRecv;
// Write a single message per coroutine loop
static SerialMessage msgToSend;
// We wait until it's been acknowledged before dequeuing the next message
static bool lastMessageAcked = true;
#define SAMD_MAX_RETRIES 3
#define SAMD_RETRY_DELAY 250
#define SAMD_SUSPEND_GRACE 1500
// Suspend SAMD after 60s of receiving no PING requests
// It is likely the ESP has suspended but we missed a message
#define SAMD_SUSPEND_PING_TIMEOUT 60000

static uint8_t retryAttempts = 0;
static int lastRetry = 0;
static int timeSinceLastPing = 0;

void wakeup(void) {
    log(LOG_INFO, "Woken from suspend");
    log_dump();
    // Reset
    NVIC_SystemReset();
}

void CRSerialBridge::setup(void) {
    esp32.connect();
    this->setupCoroutine(F("serialbridge"));
}

void handleSuspend(void) {
    log_dump();
    // Turn off LEDs
    digitalWrite(LED6, LOW);
    digitalWrite(LED7, LOW);
    digitalWrite(LED8, LOW);
    // Turn off RGB LEDs
    rgbDriver.selectStandbyMode();
    // Set IRdA module into standby
    digitalWrite(IR_TX, LOW);
    digitalWrite(IR_STANDBY, LOW);
    // Disconnect UART and reconfigure for interrupt use
    esp32.disconnect();
    Serial.end();
    DebugSerial.end();
    USBDevice.detach();
    // Wait for a grace period for UART to stop flapping
    delay(SAMD_SUSPEND_GRACE);
    pinMode(UART_RXD, INPUT_PULLDOWN);
    LowPower.attachInterruptWakeup(UART_RXD, wakeup, RISING);
    // Suspend SAMD core
    LowPower.sleep();
}

void handleAckNack(const SerialMessage &msg) {
    if (msg.header.seqNo > msgToSend.header.seqNo) {
        log(LOG_WARN, "Sequence number (n)acknowledged has advanced: expected %d received %d",
                msgToSend.header.seqNo, msg.header.seqNo);
    }
    if (((msg.header.type == Message_ACK)
                && (msg.header.seqNo == msgToSend.header.seqNo))
            || ((msg.header.type == Message_NACK)
                && (retryAttempts >= SAMD_MAX_RETRIES))
       ) {
        lastMessageAcked = true;
        retryAttempts = 0;
    }
}

void sendPong(void) {
    SerialMessage msg;
    timeSinceLastPing = millis();
    if (!prepareMessage(msg, Diagnostics_PONG, nextSequenceNumber(), NULL, 0)) {
        log(LOG_ERROR, "Failed to prepare PONG message");
    }
    if (!enqueueMessage(msg, serialBridgeQueue)) {
        log(LOG_ERROR, "Failed to enqueue %s message", messageType(msg.header.type));
    }
}

void handleMessage(const SerialMessage &msg) {
    switch (msg.header.type) {
        case Message_ACK:
        case Message_NACK:
            handleAckNack(msg);
            break;
        case VBATRequest:
            if (!enqueueMessage(msg, vbatQueue)) {
                log(LOG_ERROR, "Failed to enqueue message for vBAT");
            }
            break;
        case IRdACheck:
        case IRdASend:
        case IRdARecv:
            if (!enqueueMessage(msg, irdaQueue)) {
                log(LOG_ERROR, "Failed to enqueue message for IRdA");
            }
            break;
        case SAOWireSend:
        case SAOWireRequest:
        case SAOGPIOGet:
        case SAOGPIOSet:
            if (!enqueueMessage(msg, saoQueue)) {
                log(LOG_ERROR, "Failed to enqueue message for SAO");
            }
            break;
        case LEDSet:
            if (!enqueueMessage(msg, rgbLedQueue)) {
                log(LOG_ERROR, "Failed to enqueue message for LEDset");
            }
            break;
        case CapsenseFeedbackEnable:
            if (!enqueueMessage(msg, capsenseQueue)) {
                log(LOG_ERROR, "Failed to enqueue message for capsense");
            }
            break;
        case Diagnostics_PING:
            sendPong();
            break;
        case SAMD_Suspend:
            log(LOG_INFO, "Suspend message received");
            handleSuspend();
            break;
        default:
            log(LOG_WARN, "Unhandled message from ESP32: %s", messageType(msg.header.type));
    }
}

int CRSerialBridge::runCoroutine(void) {
    COROUTINE_LOOP() {
        // Check timeouts
        if ((millis() - timeSinceLastPing) > SAMD_SUSPEND_PING_TIMEOUT) {
            log(LOG_INFO, "No ping received in a while, suspending operations");
            handleSuspend();
        }
        // Handle receiving
        memset(&msgToRecv, 0, sizeof(SerialMessage));
        if (esp32.recvMessage(&msgToRecv)) {
            log(LOG_DEBUG, "Received %s message", messageType(msgToRecv.header.type));
                handleMessage(msgToRecv);
        }
        COROUTINE_YIELD();
        // Handle sending
        if (lastMessageAcked) {
            if (dequeueMessage(&msgToSend, serialBridgeQueue)) {
                lastMessageAcked = false;
                retryAttempts = 0;
                lastRetry = millis();
                log(LOG_DEBUG, "Sending a %s message to ESP32", messageType(msgToSend.header.type));
                if (!esp32.sendMessage(msgToSend)) {
                    log(LOG_ERROR, "Failed to send %s message", messageType(msgToSend.header.type));
                }
            }
        } else {
            if (millis() - lastRetry >= SAMD_RETRY_DELAY) {
                retryAttempts++;
                if (retryAttempts >= SAMD_MAX_RETRIES) {
                    log(LOG_WARN, "Retries exceeded, dropping %s message", messageType(msgToSend.header.type));
                    retryAttempts = 0;
                    lastMessageAcked = true;
                } else {
                    log(LOG_DEBUG, "(RETRY) Sending a %s message to ESP32", messageType(msgToSend.header.type));
                    if (!esp32.sendMessage(msgToSend)) {
                        log(LOG_ERROR, "(RETRY) Failed to send message");
                    }
                }
            }
        }
        COROUTINE_YIELD();
    }
}
