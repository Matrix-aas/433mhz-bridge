#include <Arduino.h>
#include <esp_task_wdt.h>

#include <CC1101.h>
#include <ProtocolsManager.h>
#include <WifiManager.h>
#include <MQTTManager.h>
#include "defines.h"
#include "PrincetonKeys.h"

CC1101 cc1101(CC1101_SCLK, CC1101_MISO, CC1101_MOSI, CC1101_CS, CC1101_GDO0);

void taskMqttProcessor(void *) {
    LOGD(LOG_TAG_MQTT, "Task MQTT Processor started!");

    MQTTManager.connect(MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);

    while (true) {
        if (!MQTTManager.isConnected() || !WiFi.isConnected()) {
            ProtocolsManager.resetAllMQTTData();
            delay(100);
            continue;
        }

        const auto mqttDatas = ProtocolsManager.getMQTTDatas();

        if (!mqttDatas.empty()) {
            for (const auto &mqttData: mqttDatas) {
                MQTTManager.publishRoot(mqttData.topic, mqttData.payload);
            }
        }

        MQTTManager.loop();
    }
}

void setup() {
    delay(1000);

    LOGGER_SETUP_SERIAL();

    LOGD(LOG_TAG_CC1101, "Initializing...");

    if (cc1101.init()) {
        LOGD(LOG_TAG_CC1101, "Successfully initialized!");
    } else {
        LOGD(LOG_TAG_CC1101, "Failed to initialize!");
        LOGD(LOG_TAG_CC1101, "Please check your connections!");
        LOGD(LOG_TAG_CC1101, "Restarting ESP in 10 seconds...");
        delay(10000);
        ESP.restart();
        return;
    }

    ProtocolsManager.setCC1101(&cc1101);

    WifiManager::connectToWifi(WIFI_SSID, WIFI_PASSWORD, WIFI_HOSTNAME);

    PrincetonKeys.loadKeys();

    esp_task_wdt_init(WDT_TIMEOUT, true);
    esp_task_wdt_add(nullptr);

    xTaskCreatePinnedToCore(taskMqttProcessor, "taskMqttProcessor", 4096, nullptr, 0, nullptr, 0);
}

void loop() {
    esp_task_wdt_reset();

    if (!MQTTManager.isConnected() || !WiFi.isConnected()) {
        return;
    }

    ProtocolsManager.loop();
}
