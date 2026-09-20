# QBIT-offline-companion

Offline desktop companion firmware for an ESP32-C3 SuperMini with a Waveshare 1.5" SSD1327 128x128 OLED.

This firmware uses the FluxGarage RoboEyes library for smooth animated robot-eye expressions and stays fully offline at runtime.

## What it does

- boots into a short splash animation
- switches into offline RoboEyes face playback only
- single tap cycles RoboEyes moods (default, happy, tired, angry)
- long press triggers a one-shot RoboEyes animation (blink, confused, laugh)
- keeps automatic idle eye movement and blinking on-device only
- uses the Adafruit SSD1327 + Adafruit GFX stack for the 128x128 grayscale OLED
- uses I2C OLED defaults of SDA=20, SCL=21, and address 0x3D

## What it does not include

- WiFi
- MQTT
- WebSockets
- buzzer or sound output
- web dashboard
- weather, games, poke, claim, friend, or camera features

## Architecture

- `src/main.cpp` initializes I2C, the Adafruit SSD1327 display, and the two RTOS tasks.
- `src/display_task.*` keeps the boot splash and dispatches button events.
- `src/gif_player.*` now wraps FluxGarage RoboEyes and applies the offline mood/animation mapping.
- `src/input_task.*` remains a single-button offline input queue with tap and long-press events.

## Build

This repository now contains a minimal PlatformIO firmware at the repository root:

```ini
[env:esp32-c3-supermini]
```

Dependencies are pinned in `platformio.ini` for:

- Adafruit BusIO
- Adafruit GFX Library
- Adafruit SSD1327
- FluxGarage RoboEyes

If your button or display wiring differs, override the `QBIT_BUTTON_PIN`, `QBIT_I2C_SDA_PIN`, `QBIT_I2C_SCL_PIN`, and `QBIT_I2C_ADDRESS` build flags in the repository-root `platformio.ini`.

## License

FluxGarage RoboEyes is GPL-3.0 licensed. Because this firmware links against RoboEyes, this repository is distributed under the GPL-3.0-or-later terms in `LICENSE`.
