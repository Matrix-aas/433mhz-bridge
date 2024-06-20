#include "Protocol.h"

bool Protocol::isMQTTDataReady() const {
    return this->mqttDataReady
           && this->mqttTopic.length() > 0
           && this->mqttData.length() > 0;
}

const String &Protocol::getMQTTTopic() const {
    return this->mqttTopic;
}

const String &Protocol::getMQTTData() const {
    return this->mqttData;
}

void Protocol::resetMQTTData() {
    this->mqttData = "";
    this->mqttDataReady = false;
}
