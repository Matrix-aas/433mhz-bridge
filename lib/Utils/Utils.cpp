#include "Utils.h"

#include <defines.h>
#include <WiFi.h>

String MQTTRootStr;

namespace Utils {
    uint8_t crc4(const uint8_t *data, const uint8_t &length, const uint8_t &poly, const uint8_t &init) {
        uint8_t crc = init;

        for (uint32_t i = 0; i < length; i++) {
            crc ^= data[i];
            for (uint32_t j = 0; j < 4; j++) {
                if (crc & 0x80) {
                    crc = (crc << 1) ^ poly;
                } else {
                    crc <<= 1;
                }
            }
        }

        return crc;
    }

    float fahrenheitToCelsius(const float &farenheit) {
        return (farenheit - 32) * 5 / 9;
    }

    String getMQTTRoot() {
        if (MQTTRootStr.length() == 0) {
            auto mac = WiFi.macAddress();
            mac.replace(":", "");
            MQTTRootStr = String(MQTT_BASE_TOPIC) + "_" + mac;
        }
        return MQTTRootStr;
    }

    String buildMQTTRootTopic(const String &topic) {
        return getMQTTRoot() + "/" + topic;
    }
}
