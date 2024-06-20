#ifndef PROTOCOLS_MANAGER_H
#define PROTOCOLS_MANAGER_H

#include <vector>

#include "CC1101.h"
#include "Protocol.h"

struct MQTTData {
    String topic;
    String payload;
};

class ProtocolsManagerClass final {
private:
    std::vector<Protocol *> protocols;

    SemaphoreHandle_t mutex;

    CC1101 *cc1101{nullptr};

    void registerProtocols();

    void addProtocol(Protocol *protocol);

    void processInterrupt(const uint32_t &duration, const bool &level) const;

public:
    ProtocolsManagerClass();

    void loop() const;

    std::vector<MQTTData> getMQTTDatas() const;

    void resetAllMQTTData() const;

    void setCC1101(CC1101 *cc1101) {
        this->cc1101 = cc1101;
    }
};

extern ProtocolsManagerClass ProtocolsManager;

#endif //PROTOCOLS_MANAGER_H
