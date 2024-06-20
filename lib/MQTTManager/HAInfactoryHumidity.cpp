#include "HAInfactoryHumidity.h"

HAInfactoryHumidity::HAInfactoryHumidity() {
    this->haType = "sensor";
    this->deviceId = "weather_infactory_humidity";

    this->name = "Влажность (Infactory)";

    this->deviceClass = "humidity";
    this->stateClass = "measurement";

    this->stateTopic = Utils::buildMQTTRootTopic("weather/infactory");

    this->unitOfMeasurement = "%";

    this->valueTemplate = "{{ value_json.humidity }}";
}

HAInfactoryHumidity haInfactoryHumidity;
