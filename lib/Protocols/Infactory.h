#ifndef INFACTORY_H
#define INFACTORY_H

#include "Protocol.h"

enum InfactoryDecoderStep {
    InfactoryDecoderStepReset = 0,
    InfactoryDecoderStepCheckPreambule,
    InfactoryDecoderStepSaveDuration,
    InfactoryDecoderStepCheckDuration,
};

class Infactory final : public Protocol {
private:
    uint16_t headerCount{0};

    void resetDecoder(const uint32_t &duration, const bool &level);

    void checkPreambule(const uint32_t &duration, const bool &level);

    void saveDuration(const uint32_t &duration, const bool &level);

    void checkDuration(const uint32_t &duration, const bool &level);

    static bool checkCrc(const uint64_t &data);

    void onDecode();

public:
    Infactory();

    ~Infactory() = default;

    void processInterrupt(const uint32_t &duration, const bool &level) override;
};

extern Infactory pInfactory;

#endif //INFACTORY_H
