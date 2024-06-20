#include "HADevice.h"

bool isNotEmpty(const String &str) {
    return str.length() > 0;
}

String HADevice::getPayload() const {
    String result = "{";

    if (!this->noUniqId)
        result += R"("unique_id": ")" + Utils::getMQTTRoot() + "_" + this->deviceId + R"(",)";

    if (isNotEmpty(this->name))
        result += R"("name": ")" + this->name + R"(",)";

    if (this->availability) {
        result += R"("availability": [)";
        result += R"({"topic": ")" + Utils::buildMQTTRootTopic("bridge/state") + R"(", "value_template": "{{ value_json.state }}"})";
        result += R"(],)";

        if (isNotEmpty(this->availabilityMode))
            result += R"("availability_mode": ")" + this->availabilityMode + R"(",)";
    }

    if (isNotEmpty(this->topic))
        result += R"("topic": ")" + this->topic + R"(",)";

    if (isNotEmpty(this->deviceClass))
        result += R"("device_class": ")" + this->deviceClass + R"(",)";
    if (isNotEmpty(this->entityCategory))
        result += R"("entity_category": ")" + this->entityCategory + R"(",)";

    if (isNotEmpty(this->stateTopic))
        result += R"("state_topic": ")" + this->stateTopic + R"(",)";
    if (isNotEmpty(this->stateClass))
        result += R"("state_class": ")" + this->stateClass + R"(",)";
    if (isNotEmpty(this->unitOfMeasurement))
        result += R"("unit_of_measurement": ")" + this->unitOfMeasurement + R"(",)";

    if (isNotEmpty(this->commandTopic))
        result += R"("command_topic": ")" + this->commandTopic + R"(",)";
    if (isNotEmpty(this->payloadOff))
        result += R"("payload_off": ")" + this->payloadOff + R"(",)";
    if (isNotEmpty(this->payloadOn))
        result += R"("payload_on": ")" + this->payloadOn + R"(",)";
    if (isNotEmpty(this->automationType))
        result += R"("automation_type": ")" + this->automationType + R"(",)";

    result += R"("enabled_by_default": )" + String(this->enabledByDefault ? "true" : "false") + R"(,)";


    if (isNotEmpty(this->valueTemplate))
        result += R"("value_template": ")" + this->valueTemplate + R"(",)";

    // Device info
    result += R"("device": {)";
    result += R"("identifiers": [")" + Utils::getMQTTRoot() + R"("],)";
    if (isNotEmpty(this->deviceInfo.name))
        result += R"("name": ")" + this->deviceInfo.name + R"(",)";
    if (isNotEmpty(this->deviceInfo.model))
        result += R"("model": ")" + this->deviceInfo.model + R"(",)";
    if (isNotEmpty(this->deviceInfo.manufacturer))
        result += R"("manufacturer": ")" + this->deviceInfo.manufacturer + R"(",)";
    if (isNotEmpty(this->deviceInfo.swVersion))
        result += R"("sw_version": ")" + this->deviceInfo.swVersion + R"(",)";
    if (isNotEmpty(this->deviceInfo.hwVersion))
        result += R"("hw_version": ")" + this->deviceInfo.hwVersion + R"(")";
    result += "}";

    result += "}";

    return result;
}

String HADevice::getTopic() const {
    if (this->haType.length() == 0 || this->deviceId.length() == 0)
        return "";

    return "homeassistant/" + this->haType + "/" + Utils::getMQTTRoot() + "/" + this->deviceId + "/config";
}
