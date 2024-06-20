#include "HAInfactoryTemperature.h"

HAInfactoryTemperature::HAInfactoryTemperature() {
    this->haType = "sensor";
    this->deviceId = "weather_infactory_temperature";

    this->name = "Температура (Infactory)";

    this->deviceClass = "temperature";
    this->stateClass = "measurement";

    this->stateTopic = Utils::buildMQTTRootTopic("weather/infactory");

    this->unitOfMeasurement = "°C";

    this->valueTemplate = "{{ value_json.temperature }}";
}

HAInfactoryTemperature haInfactoryTemperature;
