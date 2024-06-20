#ifndef LOGGER_H
#define LOGGER_H

#define LOGGER_SERIAL_BAUD 115200

#ifdef DEBUG
// Enable serial logging
#include <HardwareSerial.h>
#define LOGGER_SETUP_SERIAL() Serial.begin(LOGGER_SERIAL_BAUD);
#define LOGD(tag, text, ...) Serial.printf("[%s] " text "\n", tag, ##__VA_ARGS__);
#else
// Disable serial logging
#define LOGGER_SETUP_SERIAL()
#define LOGD(tag, text, ...)
#endif

#endif //LOGGER_H
