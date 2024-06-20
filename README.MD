# ESP32 CC1101 Receiver for 433.92 MHz

## Overview

This project is an ESP32 firmware for receiving and decoding 433.92 MHz signals using the CC1101 transceiver.
It supports decoding signals from gate remote controls (Princeton) and weather station (Infactory).
The decoded data is then forwarded to Home Assistant via MQTT for smart home automation.
You can easily add new protocols by implementing the `Protocol` interface in the `protocols` folder. And register the new protocol in the `ProtocolsManager` class.

## Features

- **Protocol Decoding:**
    - **Princeton**: Decode signals from gate remote controls transmitting fixed keys.
    - **Infactory**: Decode weather station data transmitting outdoor temperature and humidity.
- **MQTT Integration**: Forward decoded data to Home Assistant via MQTT.
- **Home Assistant Auto-Discovery**: Automatically discover and configure sensors in Home Assistant.

## Get Started

1. Connect CC1101 to your ESP32.
2. Configure your settings in `defines.h`.
3. Create a `secrets.h` file in the `include` folder with your WiFi and MQTT credentials.
4. Flash the ESP32 with the provided firmware.
5. Enjoy real-time monitoring and control through Home Assistant.

## CC1101 Connection

Connect the CC1101 module to your ESP32 as follows:

```
ESP32  | CC1101
----------------
3.3V   | VCC
GND    | GND
GPIO19 | MISO
GPIO23 | MOSI
GPIO18 | SCK
GPIO5  | CSN
GPIO15 | GDO0
```

**Note:** The GPIO pins can be changed in the `defines.h` file.

## Support

For questions, contributions, or issues, please visit our [GitHub Issues page](https://github.com/Matrix-aas/433mhz-bridge/issues).

---

Give it a star if you find it helpful! 🌟