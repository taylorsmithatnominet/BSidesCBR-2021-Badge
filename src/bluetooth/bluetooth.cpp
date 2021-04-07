#include "bluetooth.h"

#include <BadgeOs.h>
#include <BadgeApp.h>
#include <BadgeService.h>
#include <BadgeLog.h>
#include <display.h>
#include <menu.h>
#ifdef ESP32
#include <BluetoothSerial.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#endif
#include <BadgeInfo.h>
#include <BadgeAssets.h>
#include <asset/bt_on_bmp.h>
#include <asset/bt_connected_bmp.h>
#include <vector>
#include <string>

BadgeApp            bluetooth_app;
BadgeService        bluetooth_service;
bool                bluetooth_service_was_running = false;
#ifdef ESP32
BluetoothSerial     bluetooth_serial;
int                 ble_scan_time = 5; //In seconds
BLEScan*            ble_scan;
static uint8_t      backup_display_mode;
#endif
uint32_t            last_update = 0;

#ifdef ESP32
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        log(LOG_INFO, "Advertised Device: %s \n", advertisedDevice.toString().c_str());
    }
};
#endif

void bluetooth_begin(void)
{
    // create new scan (only need once)
#ifdef ESP32
    ble_scan = BLEDevice::getScan();
    ble_scan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
#endif

    // add bluetooth app to apps
    BadgeMenuItem *menu_item = app_menu.create_menu_item();
    menu_item->set_name("BT scan");
    menu_item->set_select_event([](){
        badge_os.push_app(bluetooth_app); 
    });

    // bluetooth app (for scans)
    bluetooth_app.set_begin([](){
        bluetooth_service_was_running = bluetooth_service.running();
        bluetooth_service.stop();
#ifdef ESP32
        backup_display_mode = display.getMode();
        display.setMode(Adafruit_SSD1675BX::kModeFastestBlackWhite);
        BLEDevice::init("");
        ble_scan->setActiveScan(true); //active scan uses more power, but get results faster
        ble_scan->setInterval(100);
        ble_scan->setWindow(99);  // less or equal setInterval value
#endif
        last_update = 0;
    });
    bluetooth_app.set_loop([](){
        uint32_t tick = (uint32_t)millis();
        if ( last_update == 0 )
        {
            // first render
            display.fillScreen(DISPLAY_COLOR_BLACK);
            size_t y = display_draw_background(BG_DEFAULT);
            display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
            display.setCursor(8, 0);
            display.setTextSize(2, 2);
            display.print("BT scan");
            display.display();
        }
        else
        {
            if ( (tick - last_update) < 2000 )
            {
                // too quick
                return;
            }
        }
        last_update = tick;

        // populate found devices
        std::vector<std::string> name_list;
        std::vector<std::string> address_list;
#ifdef ESP32
        // real scan
        BLEScanResults found_devices = ble_scan->start(ble_scan_time, false);
        for ( int i = 0; i < found_devices.getCount(); i++ )
        {
            BLEAdvertisedDevice device = found_devices.getDevice(i);
            BLEAddress address = device.getAddress();
            name_list.push_back(device.getName());
            address_list.push_back(address.toString());
        }
        log(LOG_INFO, "Devices found: %d", found_devices.getCount());
        log(LOG_INFO, "Scan done!");
        ble_scan->clearResults();
#else
        // test data
        name_list.push_back("Test's iPhone");
        address_list.push_back("aa:bb:cc:dd:ee:ff");
        name_list.push_back("Android");
        address_list.push_back("00:11:22:33:44:55");
#endif
        // display found devices
        display.fillScreen(DISPLAY_COLOR_BLACK);
        size_t y = display_draw_background(BG_DEFAULT);
        display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
        display.setCursor(8, 0);
        display.setTextSize(2, 2);
        display.print("BT scan");
        display.setCursor(8, y + 8);
        display.setTextSize(1, 1);
        for ( size_t i = 0; i < name_list.size(); i++ )
        {
            if ( name_list[i].size() > 0 )
            {
                display.println(name_list[i].c_str());
            }
            else
            {
                display.println("(no name)");
            }
            display.setCursor(8, display.getCursorY());
            display.println(address_list[i].c_str());
            display.setCursor(8, display.getCursorY());
        }
        display.display();
    });
    bluetooth_app.set_end([](){
#ifdef ESP32
        BLEDevice::deinit(false);
        display.setMode(backup_display_mode);
#endif
        if ( bluetooth_service_was_running )
        {
            bluetooth_service.start();
        }
    });
    bluetooth_app.set_key_event([](uint8_t key, bool key_down){
        if ( ((key == BADGE_KEY_LEFT) || (key == BADGE_KEY_ENTER)) && (key_down) )
        {
            badge_os.pop_app();
        }
    });

    // bluetooth service (for remote control)
    bluetooth_service.set_name("BT");
    bluetooth_service.set_draw_icon([](uint16_t x, uint16_t y, uint16_t w, uint16_t h){
#ifdef ESP32
        bool bt_connected = bluetooth_serial.hasClient();
#else
        bool bt_connected = false;
#endif
        if ( bt_connected )
        {
            x += (w / 2) - (ASSET_WIDTH(bt_connected_bmp) / 2);
            y += (h / 2) - (ASSET_HEIGHT(bt_connected_bmp) / 2);
            DRAW_ASSET(x, y, bt_connected_bmp, display);
        }
        else
        {
            x += (w / 2) - (ASSET_WIDTH(bt_on_bmp) / 2);
            y += (h / 2) - (ASSET_HEIGHT(bt_on_bmp) / 2);
            DRAW_ASSET(x, y, bt_on_bmp, display);
        }
    });
#ifdef ESP32
    bluetooth_service.set_begin([](){
        char bt_name[128];
        device_name(bt_name, sizeof(bt_name));
        bluetooth_serial.begin(bt_name);
    });
    bluetooth_service.set_end([](){
        bluetooth_serial.end();
    });
    bluetooth_service.set_loop([](){
        static char command[128] = {0};
        static size_t command_size = 0;
        static bool bt_connect_prev = false;
        static uint8_t last_key = 0;
        static bool last_key_down = false;
        if ( bt_connect_prev != bluetooth_serial.hasClient() )
        {
            badge_os.update();
            bt_connect_prev = bluetooth_serial.hasClient();
        }
        if ( last_key_down )
        {
            last_key_down = false;
            badge_os.key_event(last_key, false);
        }
        while (bluetooth_serial.available())
        {
            char value = (char)bluetooth_serial.read();
            bluetooth_serial.write(value);
            value = ( value == 0x0d ? 0x0a : value );
            if ( (value == 0x0a) && (command_size > 0) )
            {
                command[command_size] = '\0';
                log(LOG_INFO, "BT:'%s'\n", command);
                if ( 0 == strcmp(command, "left") )
                {
                    last_key = BADGE_KEY_LEFT;
                    last_key_down = true;
                    badge_os.key_event(last_key, last_key_down);
                }
                if ( 0 == strcmp(command, "up") )
                {
                    last_key = BADGE_KEY_UP;
                    last_key_down = true;
                    badge_os.key_event(last_key, last_key_down);
                }
                if ( 0 == strcmp(command, "right") )
                {
                    last_key = BADGE_KEY_RIGHT;
                    last_key_down = true;
                    badge_os.key_event(last_key, last_key_down);
                }
                if ( 0 == strcmp(command, "down") )
                {
                    last_key = BADGE_KEY_DOWN;
                    last_key_down = true;
                    badge_os.key_event(last_key, last_key_down);
                }
                if ( 0 == strcmp(command, "enter") )
                {
                    last_key = BADGE_KEY_ENTER;
                    last_key_down = true;
                    badge_os.key_event(last_key, last_key_down);
                }
                command_size = 0;
            }
            else if ( (value != 0x0a) && (command_size < (sizeof(command)-1) ) )
            {
                command[command_size] = value;
                command_size++;
            }
        }
    });
#endif
    badge_os.add_service(bluetooth_service);
}
