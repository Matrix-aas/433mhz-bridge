#ifndef HABRIDGE_CONNECTION_STATE_H
#define HABRIDGE_CONNECTION_STATE_H

#include "HADevice.h"

class HABridgeConnectionState final : public HADevice {
public:
    HABridgeConnectionState();
};

extern HABridgeConnectionState habridgeConnectionState;

#endif //HABRIDGE_CONNECTION_STATE_H
