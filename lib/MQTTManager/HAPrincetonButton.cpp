#include "HAPrincetonButton.h"

HAPrincetonButton::HAPrincetonButton() {
    this->haType = "tag";
    this->deviceId = "princeton_button";

    this->noUniqId = true;
    this->availability = false;

    this->topic = Utils::buildMQTTRootTopic("princeton_button");

    this->valueTemplate = "{{ value_json.code }}";
}

HAPrincetonButton haPrincetonButton;
