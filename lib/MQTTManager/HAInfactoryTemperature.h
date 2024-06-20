#ifndef HAINFACTORYTEMPERATURE_H
#define HAINFACTORYTEMPERATURE_H

#include "HADevice.h"

class HAInfactoryTemperature final : public HADevice {
public:
    HAInfactoryTemperature();
};

extern HAInfactoryTemperature haInfactoryTemperature;

#endif //HAINFACTORYTEMPERATURE_H
