#include "WifiManager.h"
#include <esp_wifi.h>
#include "defines.h"

constexpr auto LOG_TAG = LOG_TAG_WIFI;

namespace WifiManager {
    void connectToWifi(const char *ssid, const char *password, const char *hostname) {
        WiFi.mode(WIFI_STA);
        WiFi.setTxPower(WIFI_POWER_19_5dBm);
        esp_wifi_set_ps(WIFI_PS_NONE);
        tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, hostname);
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
        WiFi.setHostname(hostname);
        WiFi.setAutoReconnect(false);

        LOGD(LOG_TAG, "Connecting to WiFi \"%s\"", ssid);

        WiFi.begin(ssid, password);

        const auto startConnectTime = millis();

        while (!WiFi.isConnected()) {
            LOGD(LOG_TAG, "Connecting...");
            delay(1000);

            if (millis() - startConnectTime > WIFI_MAXIMUM_CONNECTION_TIME_BEFORE_REBOOT) {
                LOGD(LOG_TAG, "Failed to connect to WiFi!");
                LOGD(LOG_TAG, "Restarting ESP in second...");
                delay(1000);
                ESP.restart();
            }
        }

        delay(250);

        WiFi.onEvent(onWiFiEvent, ARDUINO_EVENT_WIFI_STA_CONNECTED);
        WiFi.onEvent(onWiFiEvent, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

        LOGD(LOG_TAG, "Successfully connected to WiFi!");

        printWifiInfo();
    }

    void printWifiInfo() {
        const auto rssi = WiFi.RSSI();
        const auto hostname = WiFi.getHostname();
        const auto mac = WiFi.macAddress();
        const auto ip = WiFi.localIP();
        const auto gateway = WiFi.gatewayIP();
        const auto subnet = WiFi.subnetMask();
        const auto dns = WiFi.dnsIP();

        LOGD(LOG_TAG, "Information:");
        LOGD(LOG_TAG, "- RSSI: %d", rssi);
        LOGD(LOG_TAG, "- Hostname: %s", hostname);
        LOGD(LOG_TAG, "- MAC address: %s", mac.c_str());
        LOGD(LOG_TAG, "- IP address: %s", ip.toString().c_str());
        LOGD(LOG_TAG, "- Gateway: %s", gateway.toString().c_str());
        LOGD(LOG_TAG, "- Subnet: %s", subnet.toString().c_str());
        LOGD(LOG_TAG, "- DNS: %s", dns.toString().c_str());
    }

    void onWiFiEvent(const arduino_event_id_t event) {
        switch (event) {
            case ARDUINO_EVENT_WIFI_STA_CONNECTED:
                LOGD(LOG_TAG, "Successfully connected to WiFi! (again)");
                printWifiInfo();
                break;

            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED: {
                LOGD(LOG_TAG, "Unexpectedly disconnected from WiFi!");
                LOGD(LOG_TAG, "Reconnecting...");

                WiFi.disconnect();
                WiFi.begin();

                const auto startReconnectTime = millis();

                while (!WiFi.isConnected()) {
                    LOGD(LOG_TAG, "Reconnecting...");
                    delay(1000);

                    if (millis() - startReconnectTime > WIFI_MAXIMUM_CONNECTION_TIME_BEFORE_REBOOT) {
                        LOGD(LOG_TAG, "Failed to reconnect to WiFi!");
                        LOGD(LOG_TAG, "Restarting ESP in second...");
                        delay(1000);
                        ESP.restart();
                    }
                }

                break;
            }

            default:
                break;
        }
    }
}
