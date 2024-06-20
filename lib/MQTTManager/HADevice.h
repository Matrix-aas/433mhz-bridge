#ifndef HADEVICE_H
#define HADEVICE_H

#include <defines.h>
#include "Utils.h"

struct MQTTDeviceInfo {
    String name{HA_DEVICE_NAME};
    String model{HA_DEVICE_MODEL};
    String manufacturer{HA_DEVICE_MANUFACTURER};
    String swVersion{HA_DEVICE_SW_VERSION};
    String hwVersion{HA_DEVICE_HW_VERSION};
};

class HADevice {
protected:
    String haType{""};
    String deviceId{""};

    String name{""};

    bool availability{true};
    String availabilityMode{"all"};

    String deviceClass{""};
    String entityCategory{""};

    String topic{""};

    String stateTopic{""};
    String stateClass{""};
    String unitOfMeasurement{""};

    String valueTemplate{""};

    String commandTopic{""};
    String payloadOff{""};
    String payloadOn{""};
    String automationType{""};

    bool enabledByDefault{true};

    bool noUniqId{false};

    MQTTDeviceInfo deviceInfo;

public:
    String getPayload() const;

    String getTopic() const;
};

#endif //HADEVICE_H
