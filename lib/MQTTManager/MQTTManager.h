#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <WiFiClient.h>
#include <MQTT.h>
#include <vector>

#include "HADevice.h"

class MQTTManagerClass final {
private:
    String clientId;
    IPAddress host;
    uint16_t port{0};
    String username;
    String password;

    WiFiClient wifiClient;
    MQTTClient mqttClient;

    std::vector<HADevice> haDevices;

    volatile bool connected{false};

    void onConnected();

public:
    MQTTManagerClass();

    void connect(const String &host, const uint16_t &port, const String &username, const String &password);

    void connect();

    void loop();

    void onMessage(const String &topic, const String &payload);

    bool publish(const String &topic, const String &payload, const bool &retained = false, const uint8_t &qos = 0);

    bool publishRoot(const String &topic, const String &payload, const bool &retained = false, const uint8_t &qos = 0);

    volatile bool isConnected() const {
        return this->connected;
    }

    static void onPrincetonButtonAdd(const String &payload);

    static void onPrincetonButtonRemove(const String &payload);
};

extern MQTTManagerClass MQTTManager;

#endif //MQTT_MANAGER_H
