#include "PrincetonKeys.h"

#include <algorithm>

#include "Utils.h"

constexpr auto LOG_TAG = LOG_TAG_PRINCETON_KEYS;

PrincetonKeysClass::PrincetonKeysClass() {
    memset(this->keys, 0, sizeof(this->keys));
}

int32_t PrincetonKeysClass::indexOf(const uint32_t &key) const {
    const auto index = std::find(this->keys, this->keys + PRINCETON_KEYS_MAXIMUM_COUNT, key) - this->keys;
    return index < PRINCETON_KEYS_MAXIMUM_COUNT ? index : -1;
}

int32_t PrincetonKeysClass::findFirstEmptyIndex() const {
    return this->indexOf(0);
}

bool PrincetonKeysClass::hasKey(const uint32_t &key) const {
    return this->indexOf(key) != -1;
}

bool PrincetonKeysClass::addKey(const uint32_t &key) {
    const auto index = this->findFirstEmptyIndex();

    if (index == -1) {
        LOGD(LOG_TAG, "No free space for new key");
        return false;
    }

    this->keys[index] = key;

    Utils::eepromTransaction(PRINCETON_KEYS_EEPROM_SIZE, [this, index] {
        EEPROM.writeUInt(index * sizeof(uint32_t), this->keys[index]);
        return true;
    });

    LOGD(LOG_TAG, "Added key #0x%08X", key);

    return true;
}

bool PrincetonKeysClass::removeKey(const uint32_t &key) {
    const auto index = this->indexOf(key);

    if (index == -1) {
        LOGD(LOG_TAG, "Key #0x%08X not found!", key);
        return false;
    }

    this->keys[index] = 0;

    Utils::eepromTransaction(PRINCETON_KEYS_EEPROM_SIZE, [index] {
        EEPROM.writeUInt(index * sizeof(uint32_t), 0);
        return true;
    });

    LOGD(LOG_TAG, "Removed key #0x%08X", key);

    return true;
}

void PrincetonKeysClass::loadKeys() {
    LOGD(LOG_TAG, "Loading keys...");

    Utils::eepromTransaction(PRINCETON_KEYS_EEPROM_SIZE, [this] {
        const auto marker = EEPROM.readUShort(PRINCETON_KEYS_MAXIMUM_COUNT * sizeof(uint32_t));

        if (marker != PRINCETON_KEYS_MARKER) {
            LOGD(LOG_TAG, "Initializing EEPROM...");

            memset(this->keys, 0, sizeof(this->keys));

            EEPROM.writeUShort(PRINCETON_KEYS_MAXIMUM_COUNT * sizeof(uint32_t), PRINCETON_KEYS_MARKER);
            for (uint32_t index = 0; index < PRINCETON_KEYS_MAXIMUM_COUNT; ++index) {
                EEPROM.writeUInt(index * sizeof(uint32_t), 0);
            }

            return true;
        }

        for (uint32_t index = 0; index < PRINCETON_KEYS_MAXIMUM_COUNT; ++index) {
            this->keys[index] = EEPROM.readUInt(index * sizeof(uint32_t));
        }

        return false;
    });

    const auto keysList = this->getKeys();
    if (keysList.empty()) {
        LOGD(LOG_TAG, "No keys loaded!");
    } else {
        LOGD(LOG_TAG, "Loaded %d keys:", keysList.size());
        for (const auto key: keysList) {
            LOGD(LOG_TAG, "- 0x%08X", key);
        }
    }
}

uint8_t PrincetonKeysClass::count() const {
    return std::count_if(this->keys, this->keys + PRINCETON_KEYS_MAXIMUM_COUNT, [](const uint32_t &key) {
        return key != 0;
    });
}

std::vector<uint32_t> PrincetonKeysClass::getKeys() const {
    std::vector<uint32_t> result;

    std::copy_if(this->keys, this->keys + PRINCETON_KEYS_MAXIMUM_COUNT, std::back_inserter(result), [](const uint32_t &key) {
        return key != 0;
    });

    return result;
}

PrincetonKeysClass PrincetonKeys;
