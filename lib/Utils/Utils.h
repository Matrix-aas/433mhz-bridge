#ifndef UITLS_H
#define UITLS_H

#include <cstdint>
#include <EEPROM.h>

#define DURATION_DIFF(a, b) ((a) > (b) ? (a) - (b) : (b) - (a))
#define DURATION_COMPARE(a, b, delta) (DURATION_DIFF(a, b) <= delta)
#define ADD_BIT(data, bit) ((data) <<= 1, (data) |= (bit))

namespace Utils {
    uint8_t crc4(const uint8_t *data, const uint8_t &length, const uint8_t &poly, const uint8_t &init);

    float fahrenheitToCelsius(const float &farenheit);

    String getMQTTRoot();

    String buildMQTTRootTopic(const String &topic);

    template<typename Transaction>
    void eepromTransaction(const size_t &size, Transaction transaction) {
        EEPROM.begin(size);

        if (transaction() == true) {
            EEPROM.commit();
        }

        EEPROM.end();
    }
}

#endif //UITLS_H
