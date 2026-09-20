#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1327.h>
#include <Wire.h>

#include "display_task.h"
#include "input_task.h"

#ifndef QBIT_I2C_SDA_PIN
#define QBIT_I2C_SDA_PIN 20
#endif

#ifndef QBIT_I2C_SCL_PIN
#define QBIT_I2C_SCL_PIN 21
#endif

#ifndef QBIT_I2C_ADDRESS
#define QBIT_I2C_ADDRESS 0x3D
#endif

Adafruit_SSD1327 display(128, 128, &Wire, -1);

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(false);

    Wire.begin(QBIT_I2C_SDA_PIN, QBIT_I2C_SCL_PIN);
    Wire.setClock(400000);

    if (!display.begin(QBIT_I2C_ADDRESS)) {
        for (;;) {
            delay(1000);
        }
    }
    display.clearDisplay();
    display.display();

    if (!inputTaskInit() || !displayTaskInit(display)) {
        for (;;) {
            delay(1000);
        }
    }

    const BaseType_t displayTaskOk = xTaskCreate(displayTask, "display", 6144, nullptr, 2, nullptr);
    const BaseType_t inputTaskOk = xTaskCreate(inputTask, "input", 2048, nullptr, 2, nullptr);

    if (displayTaskOk != pdPASS || inputTaskOk != pdPASS) {
        for (;;) {
            delay(1000);
        }
    }
}

void loop() {
    vTaskDelete(nullptr);
}
