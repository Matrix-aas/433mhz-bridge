#include "Infactory.h"

#include <defines.h>

constexpr auto LOG_TAG = LOG_TAG_INFACTORY;

Infactory::Infactory() {
    this->pParams.teLong = 2000;
    this->pParams.teShort = 500;
    this->pParams.teDelta = 150;
    this->pParams.minCountBit = 40;

    this->mqttTopic = "weather/infactory";
}

void Infactory::processInterrupt(const uint32_t &duration, const bool &level) {
    switch (this->pDecoder.step) {
        case InfactoryDecoderStepReset:
            this->resetDecoder(duration, level);
            break;

        case InfactoryDecoderStepCheckPreambule:
            this->checkPreambule(duration, level);
            break;

        case InfactoryDecoderStepSaveDuration:
            this->saveDuration(duration, level);
            break;

        case InfactoryDecoderStepCheckDuration:
            this->checkDuration(duration, level);
            break;

        default:
            break;
    }
}

void Infactory::resetDecoder(const uint32_t &duration, const bool &level) {
    if (level && DURATION_DIFF(duration, this->pParams.teShort * 2) < this->pParams.teDelta * 2) {
        this->pDecoder.step = InfactoryDecoderStepCheckPreambule;
        this->pDecoder.teLast = duration;
        this->headerCount = 0;
    }
}

void Infactory::checkPreambule(const uint32_t &duration, const bool &level) {
    if (level) {
        this->pDecoder.teLast = duration;
    } else {
        if (DURATION_DIFF(this->pDecoder.teLast, this->pParams.teShort * 2) < this->pParams.teDelta * 2 &&
            DURATION_DIFF(duration, this->pParams.teShort * 2) < this->pParams.teDelta * 2) {
            this->headerCount++;
        } else if (DURATION_DIFF(this->pDecoder.teLast, this->pParams.teShort) < this->pParams.teDelta &&
                   DURATION_DIFF(duration, this->pParams.teShort * 16) < this->pParams.teDelta * 8) {
            if (this->headerCount > 3) {
                this->pDecoder.step = InfactoryDecoderStepSaveDuration;
                this->pDecoder.data = 0;
                this->pDecoder.bitCount = 0;
            }
        } else {
            this->pDecoder.step = InfactoryDecoderStepReset;
        }
    }
}

void Infactory::saveDuration(const uint32_t &duration, const bool &level) {
    if (level) {
        this->pDecoder.teLast = duration;
        this->pDecoder.step = InfactoryDecoderStepCheckDuration;
    } else {
        this->pDecoder.step = InfactoryDecoderStepReset;
    }
}

// TODO: Implement CRC4 algorithm correctly - NOW IT'S NOT WORKING
bool Infactory::checkCrc(const uint64_t &data) {
    const uint8_t msg[] = {
        static_cast<uint8_t>(data >> 32),
        static_cast<uint8_t>(data >> 24 & 0x0F | (data & 0x0F) << 4),
        static_cast<uint8_t>(data >> 16),
        static_cast<uint8_t>(data >> 8),
        static_cast<uint8_t>(data)
    };

    auto crc = Utils::crc4(msg, 4, 0x13, 0);
    crc ^= msg[4] >> 4;

    const auto packetCRC = data >> 28 & 0x0F;

    LOGD(LOG_TAG, "Data: %X, CRC: %X, Packet CRC: %X, Result: %s",
             data, crc, packetCRC, crc == packetCRC ? "OK" : "FAIL"); // TODO: Remove this line

    return crc == packetCRC;
}

void Infactory::checkDuration(const uint32_t &duration, const bool &level) {
    if (!level) {
        if (duration >= this->pParams.teShort * 30) {
            if (this->pDecoder.bitCount == this->pParams.minCountBit
                //&& this->checkCrc() - TODO: Return back when CRC4 will be implemented correctly
                && this->pResult.data != this->pDecoder.data) {
                this->pResult.data = this->pDecoder.data;
                this->pResult.bitCount = this->pDecoder.bitCount;
                this->onDecode();
            }

            this->pDecoder.data = 0;
            this->pDecoder.bitCount = 0;
            this->pDecoder.step = InfactoryDecoderStepReset;
        } else if (DURATION_DIFF(this->pDecoder.teLast, this->pParams.teShort) < this->pParams.teDelta &&
                   DURATION_DIFF(duration, this->pParams.teLong) < this->pParams.teDelta * 2) {
            ADD_BIT(this->pDecoder.data, 0);
            ++this->pDecoder.bitCount;
            this->pDecoder.step = InfactoryDecoderStepSaveDuration;
        } else if (DURATION_DIFF(this->pDecoder.teLast, this->pParams.teShort) < this->pParams.teDelta &&
                   DURATION_DIFF(duration, this->pParams.teLong * 2) < this->pParams.teDelta * 4) {
            ADD_BIT(this->pDecoder.data, 1);
            ++this->pDecoder.bitCount;
            this->pDecoder.step = InfactoryDecoderStepSaveDuration;
        } else {
            this->pDecoder.step = InfactoryDecoderStepReset;
        }
    } else {
        this->pDecoder.step = InfactoryDecoderStepReset;
    }
}

void Infactory::onDecode() {
    const auto id = this->pResult.data >> 32;
    const auto batteryLow = (this->pResult.data >> 26) & 1;
    const auto temp = Utils::fahrenheitToCelsius((static_cast<float>(this->pResult.data >> 12 & 0x0FFF) - 900.0f) / 10.0f);
    const auto humidity = ((this->pResult.data >> 8 & 0x0F) * 10) + ((this->pResult.data >> 4 & 0x0F));
    const auto channel = this->pResult.data & 0x03;

    LOGD(LOG_TAG, "ID: %X, Battery: %s, Temp: %.2f°C, Humidity: %d%%, Channel: #%d",
             id, batteryLow ? "Low" : "OK", temp, humidity, channel);

    checkCrc(this->pResult.data); // TODO: Remove this line

    this->mqttData = R"({"id":)" + String(id, 16)
                     + R"(,"battery_low":)" + String(batteryLow)
                     + R"(,"temperature":)" + String(temp)
                     + R"(,"humidity":)" + String(humidity)
                     + R"(,"channel":)" + String(channel) + R"(})";
    this->mqttDataReady = true;
}

Infactory pInfactory;
