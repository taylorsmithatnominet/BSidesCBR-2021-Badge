//
// BSides Badge Serial Bridge Message Format
//
#ifndef __SERIALBRIDGE_MESSAGES_H
#define __SERIALBRIDGE_MESSAGES_H
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <CircularBuffer.h>

using namespace std;

#define SERIALBRIDGE_MESSAGE_MAGIC 0xdeadcafe

#define SERIALBRIDGE_MESSAGE_SIZE 64 // bytes
#define SERIALBRIDGE_MESSAGE_V1 1
#define SERIALBRIDGE_MESSAGE_CURRENT_VERSION SERIALBRIDGE_MESSAGE_V1

#define SERIALBRIDGE_MESSAGE_QUEUE_MAX_SIZE 10

/**
 * Serial bridge message types
 */
typedef enum {
    SERIALBRIDGE_MESSAGE_TYPE_MIN = 0,
    // VBat request/response
    VBATRequest = SERIALBRIDGE_MESSAGE_TYPE_MIN,
    VBATResponse,
    // IRdA send/receive
    IRdASend,
    IRdACheck,
    IRdARecv,
    // IMU event
    IMUEvent,
    // Audio event
    AudioEvent,
    // Wi-Fi event
    WiFiEvent,
    // Bluetooth Event
    BluetoothEvent,
    // SD card event
    SDEvent,
    // SAO I2C send/receive
    SAOWireSend,
    SAOWireRequest,
    // SAO GPIO read/set
    SAOGPIOGet,
    SAOGPIOSet,
    SAOGPIOVal,
    // Set RGB LED state
    LEDSet,
    // Capsense pad state
    Capsense,
    // Enable/disable direct capsense feedback to LEDs
    CapsenseFeedbackEnable,
    // D-pad button state
    DPad,
    // Call to suspend the SAMD for power saving
    SAMD_Suspend,
    // Diagnostics ping-pong
    Diagnostics_PING,
    Diagnostics_PONG,
    // ACK/NACK of serial send
    Message_ACK,
    Message_NACK,
    SERIALBRIDGE_MESSAGE_TYPE_MAX,
} MessageType_Enum;

typedef uint8_t MessageType;

/**
 * Serial bridge message header
 */
typedef struct SerialMessageHeader {
    uint32_t magic;
    uint8_t seqNo;
    uint8_t version;
    MessageType type;
    uint32_t crc32;
} __attribute((packed)) SerialMessageHeader;

#define SERIALBRIDGE_MESSAGE_DATA_SIZE_MAX (SERIALBRIDGE_MESSAGE_SIZE - sizeof(SerialMessageHeader))

// Padding type to make all messages the same size
typedef struct padding {
    uint8_t padding[SERIALBRIDGE_MESSAGE_DATA_SIZE_MAX];
} __attribute((packed)) padding;

/*
 * VBAT battery value
 */
typedef struct VBATValue {
    // The value of the battery voltage
    float value;
} __attribute((packed)) VBATValue;

/**
 * IR Code
 */
typedef struct IRCode {
    uint8_t code[SERIALBRIDGE_MESSAGE_DATA_SIZE_MAX];
} __attribute((packed)) IRCode;


/**
 * IR Component Check
 */
typedef struct IRCheck {
    bool checkResult;
} __attribute((packed)) IRCheck;

/**
 * SAO I2C command and data
 * - note: this is a preliminary message type, we will want to change this.
 */
typedef struct SAOWireData {
    uint8_t cmd;
    uint8_t data[128];
} __attribute((packed)) SAOWireData;

/**
 * SAO GPIO Data state
 */
typedef struct SAOGPIOData {
    uint8_t pinmode[2];
    uint8_t pinstate[2];
} __attribute((packed)) SAOGPIOData;

/**
 * RGB LED State to set
 */
typedef struct RGBLEDState {
    uint8_t brightness[8];
    uint8_t red[8];
    uint8_t green[8];
    uint8_t blue[8];
} __attribute((packed)) RGBLEDState;

/**
 * Capsense pad state
 */
typedef struct CapsenseState {
    bool pads[8];
} __attribute((packed)) CapsenseState;

typedef struct CapsenseFeedback {
    bool enable;
} __attribute((packed)) CapsenseFeedback;

/**
 * DPad state
 */
typedef struct DPadState {
    bool up;
    bool down;
    bool left;
    bool right;
    bool enter;
} __attribute((packed)) DPadState;

/**
 * Serial bridge message data
 */
typedef struct SerialMessageData {
    union {
        padding none;
        VBATValue vbat;
        IRCode code;
        SAOWireData i2cData;
        SAOGPIOData gpio;
        RGBLEDState leds;
        CapsenseState capsense;
        CapsenseFeedback feedback;
        DPadState dpad;
        IRCheck ircheck;
    } __attribute((packed)) payload;
} __attribute((packed)) SerialMessageData;

/**
 * Serial bridge message structure
 */
typedef struct SerialMessage_v1 {
    SerialMessageHeader header;
    SerialMessageData data;
} __attribute((packed)) SerialMessage;

/**
 * Serial message queue convenience type
 */
typedef CircularBuffer<SerialMessage,SERIALBRIDGE_MESSAGE_QUEUE_MAX_SIZE> SerialMessageQueue;

// MARK: Messaging functionality

/**
 * Populate a message type correctly, setting its version and type fields appropriately
 * @internal populates the data section with the (non-typed) data structure
 * @return false if datalen too large to fit
 */
bool prepareMessage(SerialMessage &msg, MessageType type, uint8_t seqNo, const void *data, size_t datalen);
/**
 * Validate a message is a recognised version and message type
 * @return false if message version or type is unknown
 */
bool validateMessage(const SerialMessage &msg);

/**
 * Monotonically-increasing sequence number (not thread-safe)
 */
uint8_t nextSequenceNumber(void);

/**
 * Enqueue a message for processing
 * @return false if message not valid, or queue error
 */
bool enqueueMessage(const SerialMessage &msg, SerialMessageQueue &queue);

/**
 * Dequeue a message for proecssing
 * @internal populates msgOut section with the message
 * @note Will consume the first element of the queue regardless of whether it's a valid message or not
 * @note It's up to the caller to guarantee the msg pointee is large enough to hold a SerialMessage
 * @return false if message not valid, msgOut is NULL, or no messages available
 */
bool dequeueMessage(SerialMessage *msgOut, SerialMessageQueue &queue);

/**
 * Returns a string describing message type, or "unknown" if it's not a valid type enum
 */
const char* messageType(MessageType type);

#endif // __SERIALBRIDGE_MESSAGES_H
