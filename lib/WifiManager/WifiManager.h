#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

namespace WifiManager {
    void connectToWifi(const char *ssid, const char *password, const char *hostname);

    void printWifiInfo();

    void onWiFiEvent(arduino_event_id_t event);
}

#endif //WIFI_MANAGER_H
