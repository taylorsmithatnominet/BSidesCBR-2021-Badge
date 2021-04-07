#include "BSidesMessaging.h" 
#include <string.h>
#include <Crc32.h>

// Monotonically-increasing sequence number
static uint8_t monotonicSequenceNumber;

bool prepareMessage(SerialMessage &msg, MessageType type, uint8_t seqNo, const void *data, size_t datalen) {
    if ((datalen > SERIALBRIDGE_MESSAGE_DATA_SIZE_MAX)
            || (datalen > 0 && data == NULL)
            || (datalen == 0 && data != NULL)
       ) {
        return false;
    }
    memset((void*)&msg, 0, sizeof(msg));
    msg.header.magic = SERIALBRIDGE_MESSAGE_MAGIC;
    msg.header.seqNo = seqNo;
    msg.header.type = type;
    msg.header.version = SERIALBRIDGE_MESSAGE_CURRENT_VERSION;
    if (datalen > 0) {
        memcpy((void*)(&msg.data.payload), data, datalen);
    }
    uint32_t crc32 = crc32_bitwise((void*)&msg, SERIALBRIDGE_MESSAGE_SIZE, 0);
    msg.header.crc32 = crc32;
    return true;
}

bool validateMessage(const SerialMessage &msg) {
    if ((msg.header.magic != SERIALBRIDGE_MESSAGE_MAGIC)
            || (msg.header.version > SERIALBRIDGE_MESSAGE_CURRENT_VERSION)
            || (msg.header.type < SERIALBRIDGE_MESSAGE_TYPE_MIN)
            || (msg.header.type >= SERIALBRIDGE_MESSAGE_TYPE_MAX)
       ) {
        return false;
    }
    // validate CRC32
    SerialMessage crcCheck = msg;
    // Set crc to zero for calculation
    crcCheck.header.crc32 = 0;
    uint32_t crc32_to_compare = msg.header.crc32;
    uint32_t crc32_calculated = crc32_bitwise((void*)&crcCheck, SERIALBRIDGE_MESSAGE_SIZE, 0);
    return crc32_to_compare == crc32_calculated;
}

uint8_t nextSequenceNumber(void) {
    return monotonicSequenceNumber++;
}

bool enqueueMessage(const SerialMessage &msg, SerialMessageQueue &queue) {
    if (!validateMessage(msg)) {
        return false;
    }
    queue.push(msg);
    return true;
}

bool dequeueMessage(SerialMessage *msgOut, SerialMessageQueue &queue) {
    if (queue.isEmpty() || (msgOut == NULL)) {
        return false;
    }
    // Consume the first message
    SerialMessage msg = queue.pop();
    if (!validateMessage(msg)) {
        return false;
    }
    memcpy(msgOut, &msg, SERIALBRIDGE_MESSAGE_SIZE);
    return true;
}

const char* messageType(MessageType type) {
    switch (type) {
        case VBATRequest: return "VBATRequest";
        case VBATResponse: return "VBATResponse";
        case IRdASend: return "IRdASend";
        case IRdARecv: return "IRdARecv";
        case IRdACheck: return "IRdACheck";
        case IMUEvent: return "IMUEvent";
        case AudioEvent: return "AudioEvent";
        case WiFiEvent: return "WiFiEvent";
        case BluetoothEvent: return "BluetoothEvent";
        case SDEvent: return "SDEvent";
        case SAOWireSend: return "SAOWireSend";
        case SAOWireRequest: return "SAOWireRequest";
        case SAOGPIOGet: return "SAOGPIOGet";
        case SAOGPIOSet: return "SAOGPIOSet";
        case SAOGPIOVal: return "SAOGPIOVal";
        case LEDSet: return "LEDSet";
        case Capsense: return "Capsense";
        case CapsenseFeedbackEnable: return "Capsense feedback enable";
        case DPad: return "DPad";
        case SAMD_Suspend: return "Suspend";
        case Diagnostics_PING: return "PING";
        case Diagnostics_PONG: return "PONG";
        case Message_ACK: return "ACK";
        case Message_NACK: return "NACK";
        default: return "UNKNOWN";
    }
}
