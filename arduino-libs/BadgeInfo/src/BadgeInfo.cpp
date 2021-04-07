#include <cstddef>
#include <cstring>

#include <BadgeLog.h>
#include <BadgeService.h>
#include <BSidesMessaging.h>
#include <HackerNames.h>

#include "BadgeInfo.h"

void wifi_mac_addr(uint8_t *mac)
{
#if defined(ESP32)
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
#elif defined(SAMD21)
    // TODO: use serial bridge to get it from the esp32
#else
    memcpy(mac, "\x11\x22\x33\x44\x55\x66", 6);
#endif
}

void bt_mac_addr(uint8_t *mac)
{
#if defined(ESP32)
    esp_read_mac(mac, ESP_MAC_BT);
#elif defined(SAMD21)
    // TODO: use serial bridge to get it from the esp32
#else
    memcpy(mac, "\x77\x88\x99\xaa\xbb\xcc", 6);
#endif
}

uint32_t device_id(void)
{
    uint8_t wifi_mac[6];
    uint8_t bt_mac[6];
    uint32_t id = 0;

    wifi_mac_addr(wifi_mac);
    bt_mac_addr(bt_mac);

    id |= (((uint32_t)(bt_mac[2] | wifi_mac[5])) << 24 ) & 0xff000000;
    id |= (((uint32_t)(bt_mac[3] | wifi_mac[4])) << 16 ) & 0x00ff0000;
    id |= (((uint32_t)(bt_mac[4] | wifi_mac[3])) <<  8 ) & 0x0000ff00;
    id |= (((uint32_t)(bt_mac[5] | wifi_mac[2])) <<  0 ) & 0x000000ff;

    id ^= 0xcafebabe;

    return id;
}

void wifi_mac_addr_string(char *out, size_t out_size)
{
    uint8_t wifi_mac[6];
    wifi_mac_addr(wifi_mac);
    memset(out, 0, out_size);
    snprintf(out, out_size, "%02x:%02x:%02x:%02x:%02x:%02x", wifi_mac[0], wifi_mac[1], wifi_mac[2], wifi_mac[3], wifi_mac[4], wifi_mac[5]);
    if ( out_size > 0 )
    {
        out[out_size-1];
    }
}

void bt_mac_addr_string(char *out, size_t out_size)
{
    uint8_t bt_mac[6];
    bt_mac_addr(bt_mac);
    memset(out, 0, out_size);
    snprintf(out, out_size, "%02x:%02x:%02x:%02x:%02x:%02x", bt_mac[0], bt_mac[1], bt_mac[2], bt_mac[3], bt_mac[4], bt_mac[5]);
    if ( out_size > 0 )
    {
        out[out_size-1];
    }
}

static void device_id_string(char *out, size_t out_size)
{
    memset(out, 0, out_size);
    snprintf(out, out_size, "%08x", device_id());
    if ( out_size > 0 )
    {
        out[out_size-1];
    }
}

void device_name(char *out, size_t out_size)
{
    hacker_name((size_t)device_id(), out, out_size);
}

void wifi_passwd(char *out, size_t out_size)
{
    memset(out, 0, out_size);
    if ( out_size == 0 )
    {
        return;
    }
    size_t copy_size = sizeof("password")-1;
    if ( copy_size > (out_size - 1) )
    {
        copy_size = (out_size - 1);
    }
    memcpy(out, "password", copy_size);
}

// non-static so it can be referenced for drawing battery status
VBATValue _vbat = { 0 };
void device_battery(char *out, size_t out_size)
{
    snprintf(out, out_size, "%.02fV", _vbat.value);
}

void wifi_ip(char *out, size_t out_size)
{
    memset(out, 0, out_size);
    if ( out_size == 0 )
    {
        return;
    }
    size_t copy_size = sizeof("192.168.4.1")-1;
    if ( copy_size > (out_size - 1) )
    {
        copy_size = (out_size - 1);
    }
    memcpy(out, "192.168.4.1", copy_size);
}

struct key_value {
    const char *name;
    void (*to_string)(char *out, size_t out_size);
};

const struct key_value info[] = {
    { "Device ID", device_id_string },
    { "Device Name", device_name },
    { "WiFi Address", wifi_mac_addr_string },
    { "Bluetooth Address", bt_mac_addr_string },
    { "WiFi Password", wifi_passwd },
    { "WiFi Web Server", wifi_ip },
    { "Battery Voltage", device_battery },
};

size_t device_info_count(void)
{
    return sizeof(info)/sizeof(info[0]);
}

void device_info_name(size_t index, char *out, size_t out_size)
{
    memset(out, 0, out_size);
    if ( index >= device_info_count() )
    {
        return;
    }
    if ( out_size == 0 )
    {
        return;
    }
    size_t copy_size = strlen(info[index].name);
    if ( copy_size > (out_size - 1) )
    {
        copy_size = out_size - 1;
    }
    memcpy(out, info[index].name, copy_size);
}

void device_info_value(size_t index, char *out, size_t out_size)
{
    memset(out, 0, out_size);
    if ( index >= device_info_count() )
    {
        return;
    }
    info[index].to_string(out, out_size);
}

/* Check for messages every loop and poll for status data every 60. This
 * service self-regulates its period to 1 second when it is started rather than
 * respecting whatever the code which configured and started us might have
 * chosen.
 */
#define _SVC_PERIOD         (1000U)
#define _SVC_POLL_MULT      (60U)
#define _SVC_POLL_MULT_FAST (3U)

extern SerialMessageQueue serialBridgeQueue;
SerialMessageQueue deviceInfoQueue;
static void _svc_begin(void) {
    log(LOG_INFO, "Starting device info svc");
    device_info_svc.set_period_ms(_SVC_PERIOD);
}
static void sendMessage(void) {
    /* This needs to remain available since it's passed by reference and it's
     * not clear if the queue actually makes a copy or takes true ownership of
     * it (ie. there is no delete). We have to assume that we won't overwrite
     * this via `prepareMessage()` while it's sitting in a queue. Since our TX
     * period is quite long, this should be safe enough although not ideal :(
     */
    static SerialMessage mtx;
    if (
        prepareMessage(mtx, VBATRequest, nextSequenceNumber(), NULL, 0)
        && enqueueMessage(mtx, serialBridgeQueue)
    ) {
        log(LOG_DEBUG, "Sent %s message", messageType(mtx.header.type));
    } else {
        log(LOG_WARN, "Failed to send battery status request");
    }
}
static void _svc_loop(void) {
    static size_t i = 0;    /* Neglect any wrapping - who cares */
    static bool received = false;

    /* We'll re-use the same message object repeatedly for received messages */
    auto mrx = new SerialMessage;
    while (dequeueMessage(mrx, deviceInfoQueue)) {
        /* If this fails we'll just move on without noting the fact */
        log(LOG_DEBUG, "Received %s message", messageType(mrx->header.type));
        switch (mrx->header.type) {
        case VBATResponse:
            _vbat = mrx->data.payload.vbat;
            received = true;
            break;
        }
    }
    delete mrx;

    if (i++ % _SVC_POLL_MULT == 0) {
        received = false;
        sendMessage();
    }
    // If we haven't received a message within a set time, send at a faster rate
    else if (i % _SVC_POLL_MULT_FAST == 0 && !received) {
        sendMessage();
    }
}
BadgeService device_info_svc(_svc_begin, _svc_loop, nullptr);
