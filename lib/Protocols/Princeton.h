#ifndef AKHAN_H
#define AKHAN_H
#include "Protocol.h"

#define PRINCETON_GUARD_TIME_DEFAULT 30

enum PrincetonDecoderStep {
    PrincetonDecoderStepReset = 0,
    PrincetonDecoderStepSaveDuration,
    PrincetonDecoderStepCheckDuration,
};

class Princeton final : public Protocol {
private:
    uint32_t te{0};
    uint32_t guardTime{PRINCETON_GUARD_TIME_DEFAULT};
    uint64_t lastData{0};

    uint64_t lastDecodedData{0};
    uint64_t lastDecodedDataTime{0};

    void resetDecoder(const uint32_t &duration, const bool &level);

    void saveDuration(const uint32_t &duration, const bool &level);

    void checkDuration(const uint32_t &duration, const bool &level);

    void onDecode();

public:
    Princeton();

    virtual ~Princeton() = default;

    void processInterrupt(const uint32_t &duration, const bool &level) override;
};

extern Princeton pPrinceton;

#endif //AKHAN_H
