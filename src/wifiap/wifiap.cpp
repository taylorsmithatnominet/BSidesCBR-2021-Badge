#include "wifiap.h"

#include <BadgeOs.h>
#include <BadgeService.h>
#include <BadgeLog.h>
#include <display.h>
#include <menu.h>
#ifdef ESP32
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <esp_wifi.h>
#endif
#include <BadgeInfo.h>
#include <BadgeAssets.h>
#include <asset/wifi_ap_on_bmp.h>
#include <asset/wifi_ap_connected_bmp.h>
#include <asset/wifi_ap_data_transfer_bmp.h>
#include <vector>
#include <string>

BadgeApp        wifi_scan_app;
BadgeService    wifiap_service;
bool            wifiap_service_was_running = false;
#ifdef ESP32
WiFiServer      wifiap_server(80);
IPAddress       ap_local_IP(192,168,4,1);
IPAddress       ap_gateway(192,168,4,1);
IPAddress       ap_subnet(255,255,255,0);
static uint8_t  backup_display_mode;
#endif
static uint32_t g_last_update = 0;
bool            g_has_client = false;
bool            g_data_transfer = false;
uint32_t        g_last_data_transfer_ms = 0;

static void wifiap_set_has_client(bool has_client, bool data_transfer)
{
    bool update_needed = false;
    update_needed = update_needed ? true : (has_client != g_has_client);
    g_has_client = has_client;
    if ( has_client && data_transfer )
    {
        g_last_data_transfer_ms = (uint32_t)millis();
    }
    if ( ( (uint32_t)millis() - g_last_data_transfer_ms) < 2000 )
    {
        data_transfer = true;
    }
    update_needed = update_needed ? true : (data_transfer != g_data_transfer);
    g_data_transfer = data_transfer;
    if ( update_needed )
    {
        badge_os.update();
    }
}

void wifiap_begin(void)
{
    // add WiFi scan app to apps
    BadgeMenuItem *menu_item = app_menu.create_menu_item();
    menu_item->set_name("WiFi scan");
    menu_item->set_select_event([](){
        badge_os.push_app(wifi_scan_app); 
    });

    // WiFi scan app
    wifi_scan_app.set_begin([](){
        wifiap_service_was_running = wifiap_service.running();
        wifiap_service.stop();
#ifdef ESP32
        backup_display_mode = display.getMode();
        display.setMode(Adafruit_SSD1675BX::kModeFastestBlackWhite);
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);
#endif
        g_last_update = 0;
    });
    wifi_scan_app.set_loop([](){
        uint32_t tick = (uint32_t)millis();
        if ( g_last_update == 0 )
        {
            // first render
            display.fillScreen(DISPLAY_COLOR_BLACK);
            size_t y = display_draw_background(BG_DEFAULT);
            display.setTextColor(DISPLAY_COLOR_RED, DISPLAY_COLOR_BLACK);
            display.setCursor(8, 0);
            display.setTextSize(2, 2);
            display.print("WiFi scan");
            display.display();
        }
        else
        {
            if ( (tick - g_last_update) < 5000 )
            {
                // too quick
                return;
            }
        }
        g_last_update = tick;

        // populate found devices
        std::vector<std::string> name_list;
        std::vector<std::string> address_list;
#ifdef ESP32
        // real scan
        int found_devices_count = WiFi.scanNetworks();
        for ( int i = 0; i < found_devices_count; i++ )
        {
            name_list.push_back(std::string(WiFi.SSID(i).c_str()));
            address_list.push_back(std::string(WiFi.BSSIDstr(i).c_str()));
        }
        log(LOG_INFO, "Devices found: %d", found_devices_count);
        log(LOG_INFO, "Scan done!");
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
        display.print("WiFi scan");
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
    wifi_scan_app.set_end([](){
#ifdef ESP32
        display.setMode(backup_display_mode);
#endif
        if ( wifiap_service_was_running )
        {
            wifiap_service.start();
        }
    });
    wifi_scan_app.set_key_event([](uint8_t key, bool key_down){
        if ( ((key == BADGE_KEY_LEFT) || (key == BADGE_KEY_ENTER)) && (key_down) )
        {
            badge_os.pop_app();
        }
    });

    // WiFi AP service
    wifiap_service.set_name("WiFiAP");
    wifiap_service.set_draw_icon([](uint16_t x, uint16_t y, uint16_t w, uint16_t h){
        if ( g_data_transfer )
        {
            x += (w / 2) - (ASSET_WIDTH(wifi_ap_data_transfer_bmp) / 2);
            y += (h / 2) - (ASSET_HEIGHT(wifi_ap_data_transfer_bmp) / 2);
            DRAW_ASSET(x, y, wifi_ap_data_transfer_bmp, display);
        }
        else if ( g_has_client )
        {
            x += (w / 2) - (ASSET_WIDTH(wifi_ap_connected_bmp) / 2);
            y += (h / 2) - (ASSET_HEIGHT(wifi_ap_connected_bmp) / 2);
            DRAW_ASSET(x, y, wifi_ap_connected_bmp, display);
        }
        else
        {
            x += (w / 2) - (ASSET_WIDTH(wifi_ap_on_bmp) / 2);
            y += (h / 2) - (ASSET_HEIGHT(wifi_ap_on_bmp) / 2);
            DRAW_ASSET(x, y, wifi_ap_on_bmp, display);
        }
    });
#ifdef ESP32
    wifiap_service.set_begin([](){
        char ap_name[64];
        char ap_passwd[64];
        device_name(ap_name, sizeof(ap_name));
        wifi_passwd(ap_passwd, sizeof(ap_passwd));
        WiFi.softAP(ap_name, ap_passwd);
        WiFi.softAPConfig(ap_local_IP, ap_gateway, ap_subnet);
        IPAddress myIP = WiFi.softAPIP();
        log(LOG_INFO, "AP IP address: %s", myIP.toString().c_str());
        wifiap_server.begin();
    });
    wifiap_service.set_end([](){
        WiFi.softAPdisconnect(true);
    });
    wifiap_service.set_loop([](){
        WiFiClient client = wifiap_server.available();
#ifdef ESP32
        wifi_sta_list_t sta_list;
        memset(&sta_list, 0, sizeof(sta_list));
        esp_wifi_ap_get_sta_list(&sta_list);
        wifiap_set_has_client(sta_list.num > 0, client);
#endif
        if (client)
        {
            log(LOG_INFO, "New Client.");
            String request = "";
            while (client.connected()) {
                if (client.available()) {
                    char c = client.read();
                    if (c == '\n') {
                        if (request.length() == 0) {
                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-type:text/html");
                            client.println();
                            client.print("<h1>Badge 2021<h1>");
                            client.print("<i>Under construction<i>");
                            client.println();
                            break;
                        } else {
                            request = "";
                        }
                    } else if (c != '\r')
                    {
                        request += c;
                    }
                }
                delay(10);  // keep watchdog happy
            }
            client.stop();
            log(LOG_INFO, "Client Disconnected.");
        }
    });
#endif
    badge_os.add_service(wifiap_service);
}
