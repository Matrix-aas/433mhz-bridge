#include "CC1101.h"

CC1101 *CC1101::_instance = nullptr;

CC1101::CC1101(const uint8_t &sclk, const uint8_t &miso, const uint8_t &mosi, const uint8_t &cs, const uint8_t &gdo0)
    : pinSCLK(sclk),
      pinMISO(miso),
      pinMOSI(mosi),
      pinCS(cs),
      pinGDO0(gdo0) {
    _instance = this;
}

bool CC1101::init() const {
    ELECHOUSE_cc1101.setSpiPin(this->pinSCLK, this->pinMISO, this->pinMOSI, this->pinCS);

    if (!isCC1101Connected()) {
        return false;
    }

    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.setRxBW(650);
    ELECHOUSE_cc1101.setModulation(2);
    ELECHOUSE_cc1101.setMHZ(433.92);

    const auto interruptPin = digitalPinToInterrupt(this->pinGDO0);
    attachInterrupt(interruptPin, handleCC1101ReceiveInterrupt, CHANGE);

    ELECHOUSE_cc1101.SetRx();

    return true;
}

bool CC1101::isCC1101Connected() {
    return ELECHOUSE_cc1101.getCC1101();
}

void CC1101::bufferToFlush() {
    if (this->bufferIndex == 0) {
        return;
    }

    this->flushBufferLength = this->bufferIndex;

    for (uint32_t i = 0; i < this->flushBufferLength; ++i) {
        this->flushBuffer[i].duration = this->buffer[i].duration;
        this->flushBuffer[i].level = this->buffer[i].level;
    }

    this->bufferIndex = 0;
    this->flushFlag = true;
}

void IRAM_ATTR CC1101::handleInterrupt() {
    const auto time = micros();
    const auto duration = time - this->lastReceivedTime;

    if (duration < 50) {
        this->lastReceivedTime = time;
        return;
    }

    this->buffer[this->bufferIndex].duration = duration;
    this->buffer[this->bufferIndex].level = !digitalRead(pinGDO0);

    if (this->bufferIndex + 1 >= CC1101_RECEIVE_BUFFER_LENGTH) {
        this->bufferToFlush();
    } else {
        ++this->bufferIndex;
    }

    this->lastReceivedTime = time;
}

void IRAM_ATTR handleCC1101ReceiveInterrupt() {
    CC1101::_instance->handleInterrupt();
}
