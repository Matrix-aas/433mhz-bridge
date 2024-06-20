#include "Princeton.h"
#include <cmath>

#include "PrincetonKeys.h"

constexpr auto LOG_TAG = LOG_TAG_PRINCETON;

Princeton::Princeton() {
    this->pParams.teLong = 1170;
    this->pParams.teShort = 390;
    this->pParams.teDelta = 300;
    this->pParams.minCountBit = 24;

    this->mqttTopic = "princeton_button";
}

void Princeton::processInterrupt(const uint32_t &duration, const bool &level) {
    switch (this->pDecoder.step) {
        case PrincetonDecoderStepReset:
            this->resetDecoder(duration, level);
            break;

        case PrincetonDecoderStepSaveDuration:
            this->saveDuration(duration, level);
            break;

        case PrincetonDecoderStepCheckDuration:
            this->checkDuration(duration, level);
            break;

        default:
            break;
    }
}

void Princeton::resetDecoder(const uint32_t &duration, const bool &level) {
    if (!level && DURATION_COMPARE(duration, this->pParams.teShort * 36, this->pParams.teDelta * 36)) {
        this->pDecoder.step = PrincetonDecoderStepSaveDuration;
        this->pDecoder.data = 0;
        this->pDecoder.bitCount = 0;
        this->pDecoder.teLast = 0;
        this->te = 0;
        this->guardTime = PRINCETON_GUARD_TIME_DEFAULT;
    }
}

void Princeton::saveDuration(const uint32_t &duration, const bool &level) {
    if (level) {
        this->pDecoder.teLast = duration;
        this->te += duration;
        this->pDecoder.step = PrincetonDecoderStepCheckDuration;
    }
}

void Princeton::checkDuration(const uint32_t &duration, const bool &level) {
    if (!level) {
        if (duration >= this->pParams.teLong * 2) {
            this->pDecoder.step = PrincetonDecoderStepSaveDuration;

            if (this->pDecoder.bitCount == this->pParams.minCountBit) {
                if (this->lastData == this->pDecoder.data && this->lastData) {
                    this->te /= (this->pDecoder.bitCount * 4 + 1);

                    this->pResult.data = this->pDecoder.data;
                    this->pResult.bitCount = this->pDecoder.bitCount;
                    this->guardTime = static_cast<uint32_t>(roundf(
                        static_cast<float>(duration) / static_cast<float>(this->te)
                    ));

                    this->onDecode();
                }

                this->lastData = this->pDecoder.data;
            }

            this->pDecoder.data = 0;
            this->pDecoder.bitCount = 0;
            this->te = 0;
        } else {
            this->te += duration;

            if (DURATION_COMPARE(this->pDecoder.teLast, this->pParams.teShort, this->pParams.teDelta) &&
                DURATION_COMPARE(duration, this->pParams.teLong, this->pParams.teDelta * 3)) {
                ADD_BIT(this->pDecoder.data, 0);
                ++this->pDecoder.bitCount;

                this->pDecoder.step = PrincetonDecoderStepSaveDuration;
            } else if (DURATION_COMPARE(this->pDecoder.teLast, this->pParams.teLong, this->pParams.teDelta * 3) &&
                       DURATION_COMPARE(duration, this->pParams.teShort, this->pParams.teDelta)) {
                ADD_BIT(this->pDecoder.data, 1);
                ++this->pDecoder.bitCount;

                this->pDecoder.step = PrincetonDecoderStepSaveDuration;
            } else {
                this->pDecoder.step = PrincetonDecoderStepReset;
            }
        }
    } else {
        this->pDecoder.step = PrincetonDecoderStepReset;
    }
}

void Princeton::onDecode() {
    const auto time = millis();

    const auto data = this->pResult.data;

    if (time - this->lastDecodedDataTime < 1000 && this->lastDecodedData == data) {
        this->lastDecodedDataTime = time;
        return;
    }

    this->lastDecodedDataTime = time;
    this->lastDecodedData = data;

    const auto serial = data >> 4;
    const auto btn = data & 0xF;

    const auto isKeyInBase = PrincetonKeys.hasKey(serial);

    LOGD(LOG_TAG, "Serial: %x, Button: %x, authorized: %s", serial, btn, isKeyInBase ? "yes" : "no");

    if (isKeyInBase) {
        this->mqttData = R"({"serial":")" + String(serial, 16)
                         + R"(","button":")" + String(btn, 16)
                         + R"(","code":")" + String(data, 16) + R"("})";
        this->mqttDataReady = true;
    }
}

Princeton pPrinceton;
