#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <WString.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <Utils.h> // Used in derived classes

#pragma pack(push, 1)
struct ProtocolParams {
    uint16_t teLong{0};
    uint16_t teShort{0};
    uint16_t teDelta{0};
    uint8_t minCountBit{0};
};

struct ProtocolDecoder {
    uint8_t step{0};
    uint32_t teLast{0};
    uint8_t bitCount{0};
    uint64_t data{0};
};

struct ProtocolResult {
    uint8_t bitCount{0};
    uint64_t data{0};
};
#pragma pack(pop)

class Protocol {
protected:
    ProtocolParams pParams;
    ProtocolDecoder pDecoder;
    ProtocolResult pResult;

    String mqttTopic;
    String mqttData;
    bool mqttDataReady{false};

    Protocol() = default;

    ~Protocol() = default;

public:
    virtual void processInterrupt(const uint32_t &duration, const bool &level) = 0;

    bool isMQTTDataReady() const;

    const String &getMQTTTopic() const;

    const String &getMQTTData() const;

    void resetMQTTData();
};

#endif //PROTOCOL_H
