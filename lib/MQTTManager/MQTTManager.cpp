#include "MQTTManager.h"

#include <WiFi.h>

#include "HABridgeConnectionState.h"
#include "HAPrincetonButton.h"
#include "HAInfactoryHumidity.h"
#include "HAInfactoryTemperature.h"
#include "PrincetonKeys.h"

constexpr auto LOG_TAG = LOG_TAG_MQTT;

MQTTManagerClass::MQTTManagerClass() {
    this->mqttClient.setKeepAlive(MQTT_KEEP_ALIVE);

    this->haDevices.push_back(habridgeConnectionState);

    this->haDevices.push_back(haInfactoryTemperature);
    this->haDevices.push_back(haInfactoryHumidity);

    this->haDevices.push_back(haPrincetonButton);
}

void MQTTManagerClass::connect(const String &host, const uint16_t &port, const String &username, const String &password) {
    this->clientId = Utils::getMQTTRoot();
    this->port = port;
    this->username = username;
    this->password = password;

    if (!this->host.fromString(host)) {
        LOGD(LOG_TAG, "Invalid host! Stuck in a loop, use reset button to reboot.");
        while (true);
    }

    this->connect();
}

void MQTTManagerClass::connect() {
    this->connected = false;

    if (!WiFi.isConnected()) {
        return;
    }

    this->mqttClient.begin(this->host, this->port, this->wifiClient);
    this->mqttClient.setWill(Utils::buildMQTTRootTopic("bridge/state").c_str(), R"({"state": "offline"})", true, 0);
    this->mqttClient.onMessage([this](const String &topic, const String &payload) {
        this->onMessage(topic, payload);
    });

    uint8_t retries = 0;

    while (!this->mqttClient.connected()) {
        ++retries;

        LOGD(LOG_TAG, "Connection attempt #%d to %s:%d as \"%s\"...", retries, this->host.toString().c_str(), this->port, this->clientId.c_str());

        if (this->mqttClient.connect(this->clientId.c_str(), this->username.c_str(), this->password.c_str())) {
            this->onConnected();
            return;
        }

        LOGD(LOG_TAG, "Failed to connect! Connect return code: %d", this->mqttClient.lastError());

        if (retries == MQTT_MAX_CONNECTION_ATTEMPTS_BEFORE_REBOOT) {
            LOGD(LOG_TAG, "Reaching maximum connection attempts! Rebooting in second...");
            delay(1000);
            ESP.restart();
        }

        LOGD(LOG_TAG, "Retrying in %d milliseconds...", MQTT_CONNECTION_ATTEMPT_INTERVAL);

        delay(MQTT_CONNECTION_ATTEMPT_INTERVAL);
    }
}

void MQTTManagerClass::loop() {
    if (!this->mqttClient.connected()) {
        this->connect();
    } else {
        this->mqttClient.loop();
    }
}

void MQTTManagerClass::onConnected() {
    if (this->connected) {
        return;
    }

    this->connected = true;

    LOGD(LOG_TAG, "Successfully connected!");

    for (const auto &haDevice: this->haDevices) {
        const auto haTopic = haDevice.getTopic();

        if (haTopic.length() <= 0) {
            continue;
        }

        this->mqttClient.publish(haDevice.getTopic().c_str(), haDevice.getPayload().c_str(), true, 0);

        LOGD(LOG_TAG, "Published HA device to \"%s\"", haDevice.getTopic().c_str());
    }

    this->mqttClient.publish(Utils::buildMQTTRootTopic("bridge/state"), R"({"state": "online"})", true, 0);

    LOGD(LOG_TAG, "Published bridge online state");

    this->mqttClient.subscribe(Utils::buildMQTTRootTopic("bridge/reboot"));
    this->mqttClient.subscribe(Utils::buildMQTTRootTopic("princeton_button/+"));
}

void MQTTManagerClass::onMessage(const String &topic, const String &payload) {
    LOGD(LOG_TAG, "Received message on \"%s\": %s", topic.c_str(), payload.c_str());

    if (topic == Utils::buildMQTTRootTopic("princeton_button/add")) {
        onPrincetonButtonAdd(payload);
    } else if (topic == Utils::buildMQTTRootTopic("princeton_button/remove")) {
        onPrincetonButtonRemove(payload);
    } else if (topic == Utils::buildMQTTRootTopic("bridge/reboot")) {
        LOGD(LOG_TAG, "Rebooting in second...");
        delay(1000);
        ESP.restart();
    }
}

bool MQTTManagerClass::publish(const String &topic, const String &payload, const bool &retained, const uint8_t &qos) {
    LOGD(LOG_TAG, "Publishing to \"%s\": %s", topic.c_str(), payload.c_str());
    return this->mqttClient.publish(topic.c_str(), payload.c_str(), retained, qos);
}

bool MQTTManagerClass::publishRoot(const String &topic, const String &payload, const bool &retained, const uint8_t &qos) {
    return this->publish(Utils::buildMQTTRootTopic(topic).c_str(), payload.c_str(), retained, qos);
}

void MQTTManagerClass::onPrincetonButtonAdd(const String &payload) {
    const uint32_t code = strtoul(payload.c_str(), nullptr, 16);

    if (code == 0 || code == ULONG_MAX) {
        return;
    }

    PrincetonKeys.addKey(code);
}

void MQTTManagerClass::onPrincetonButtonRemove(const String &payload) {
    const uint32_t code = strtoul(payload.c_str(), nullptr, 16);

    if (code == 0 || code == ULONG_MAX) {
        return;
    }

    PrincetonKeys.removeKey(code);
}

MQTTManagerClass MQTTManager;
