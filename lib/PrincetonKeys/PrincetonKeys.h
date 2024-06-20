#ifndef PRINCETONKEYS_H
#define PRINCETONKEYS_H

#include <cstdint>
#include <vector>

#include "defines.h"

#define PRINCETON_KEYS_EEPROM_SIZE (PRINCETON_KEYS_MAXIMUM_COUNT * sizeof(uint32_t) + sizeof(uint16_t))
#define PRINCETON_KEYS_MARKER ((uint16_t)0x5052)

class PrincetonKeysClass final {
private:
    uint32_t keys[PRINCETON_KEYS_MAXIMUM_COUNT]{};

    int32_t findFirstEmptyIndex() const;

public:
    PrincetonKeysClass();

    int32_t indexOf(const uint32_t &key) const;

    bool hasKey(const uint32_t &key) const;

    bool addKey(const uint32_t &key);

    bool removeKey(const uint32_t &key);

    void loadKeys();

    uint8_t count() const;

    std::vector<uint32_t> getKeys() const;
};

extern PrincetonKeysClass PrincetonKeys;

#endif //PRINCETONKEYS_H
