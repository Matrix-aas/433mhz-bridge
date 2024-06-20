#ifndef CC1101_H
#define CC1101_H

#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include "defines.h"

#pragma pack(push, 1)
struct CC1101InterruptData {
    uint32_t duration{0};
    uint8_t level{0};
};
#pragma pack(pop)

class CC1101 final {
private:
    uint8_t pinSCLK;
    uint8_t pinMISO;
    uint8_t pinMOSI;
    uint8_t pinCS;
    uint8_t pinGDO0;

    volatile CC1101InterruptData buffer[CC1101_RECEIVE_BUFFER_LENGTH]{};
    volatile uint16_t bufferIndex = 0;

    volatile CC1101InterruptData flushBuffer[CC1101_RECEIVE_BUFFER_LENGTH]{};
    volatile uint16_t flushBufferLength = 0;

    volatile bool flushFlag = false;
    volatile uint32_t lastReceivedTime = 0UL;

public:
    static CC1101 *_instance;

    CC1101(const uint8_t &sclk, const uint8_t &miso, const uint8_t &mosi, const uint8_t &cs, const uint8_t &gdo0);

    bool init() const;

    static bool isCC1101Connected();

    void bufferToFlush();

    void IRAM_ATTR handleInterrupt();

    uint32_t getLastReceivedTime() const {
        return this->lastReceivedTime;
    }

    uint16_t getBufferIndex() const {
        return this->bufferIndex;
    }

    volatile CC1101InterruptData *getFlushBuffer() {
        return this->flushBuffer;
    }

    volatile uint16_t getFlushBufferLength() const {
        return this->flushBufferLength;
    }

    volatile bool isFlushFlag() const {
        return this->flushFlag;
    }

    void resetFlushFlag() {
        this->flushFlag = false;
    }
};

void IRAM_ATTR handleCC1101ReceiveInterrupt();

#endif //CC1101_H
