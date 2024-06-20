#include "ProtocolsManager.h"

#include <defines.h>

#include "Infactory.h"
#include "Princeton.h"

ProtocolsManagerClass::ProtocolsManagerClass() {
    this->mutex = xSemaphoreCreateMutex();
    this->registerProtocols();
}

void ProtocolsManagerClass::registerProtocols() {
    this->addProtocol(&pPrinceton);
    this->addProtocol(&pInfactory);
}

void ProtocolsManagerClass::addProtocol(Protocol *protocol) {
    this->protocols.push_back(protocol);
}

void ProtocolsManagerClass::processInterrupt(const uint32_t &duration, const bool &level) const {
    for (auto *const protocol: this->protocols) {
        try {
            protocol->processInterrupt(duration, level);
        } catch (const std::exception &e) {
            LOGD(LOG_TAG_GLOBAL, "Error processing interrupt: %s", e.what());
        }
    }
}

void ProtocolsManagerClass::loop() const {
    if (!this->cc1101) {
        return;
    }

    const auto time = micros();

    if (time - this->cc1101->getLastReceivedTime() > CC1101_FORCE_FLUSH_DURATION && cc1101->getBufferIndex() > 0) {
        cc1101->bufferToFlush();
    }

    if (cc1101->isFlushFlag()) {
        cc1101->resetFlushFlag();

        const auto flushBufferLength = cc1101->getFlushBufferLength();
        const auto flushBuffer = cc1101->getFlushBuffer();

        xSemaphoreTake(this->mutex, portMAX_DELAY);

        for (auto index = 0; index < flushBufferLength; ++index) {
            const auto duration = flushBuffer[index].duration;
            const auto level = flushBuffer[index].level;

            this->processInterrupt(duration, level);
        }

        xSemaphoreGive(this->mutex);
    }
}

std::vector<MQTTData> ProtocolsManagerClass::getMQTTDatas() const {
    std::vector<MQTTData> mqttDatas;

    xSemaphoreTake(this->mutex, portMAX_DELAY);

    for (auto *const protocol: this->protocols) {
        if (!protocol->isMQTTDataReady()) {
            continue;
        }

        const auto mqttTopic = protocol->getMQTTTopic();
        const auto mqttData = protocol->getMQTTData();

        mqttDatas.push_back({mqttTopic, mqttData});

        protocol->resetMQTTData();
    }

    xSemaphoreGive(this->mutex);

    return mqttDatas;
}

void ProtocolsManagerClass::resetAllMQTTData() const {
    xSemaphoreTake(this->mutex, portMAX_DELAY);

    for (auto *const protocol: this->protocols) {
        protocol->resetMQTTData();
    }

    xSemaphoreGive(this->mutex);
}

ProtocolsManagerClass ProtocolsManager;
