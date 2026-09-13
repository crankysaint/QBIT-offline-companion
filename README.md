# QBIT-offline-companion

Offline desktop companion firmware for an ESP32-C3 SuperMini with a Waveshare 1.5" SSD1327 128x128 OLED.

## What it does

- boots into a short splash animation
- switches into offline face playback only
- single tap advances to the next face
- long press cycles animation speed modes
- uses I2C OLED defaults of SDA=20 and SCL=21

## What it does not include

- WiFi
- MQTT
- WebSockets
- buzzer or sound output
- web dashboard
- weather, games, poke, claim, friend, or camera features

## Build

This repository now contains a minimal PlatformIO firmware at the repository root:

```ini
[env:esp32-c3-supermini]
```

If your button or display pins differ, override the `QBIT_BUTTON_PIN`, `QBIT_I2C_SDA_PIN`, and `QBIT_I2C_SCL_PIN` build flags in the repository-root `platformio.ini`.
