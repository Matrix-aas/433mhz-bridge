#include "HABridgeConnectionState.h"

HABridgeConnectionState::HABridgeConnectionState() {
    this->haType = "binary_sensor";
    this->deviceId = "connection_state";

    this->name = "Connection State";

    this->deviceClass = "connectivity";
    this->entityCategory = "diagnostic";

    this->availability = false;
    this->availabilityMode = "";

    this->stateTopic = Utils::buildMQTTRootTopic("bridge/state");
    this->payloadOff = "offline";
    this->payloadOn = "online";

    this->valueTemplate = "{{ value_json.state }}";
}

HABridgeConnectionState habridgeConnectionState;
