#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

#include "display_task.h"
#include "input_task.h"

#ifndef QBIT_I2C_SDA_PIN
#define QBIT_I2C_SDA_PIN 20
#endif

#ifndef QBIT_I2C_SCL_PIN
#define QBIT_I2C_SCL_PIN 21
#endif

U8G2_SSD1327_MIDAS_128X128_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(false);

    Wire.begin(QBIT_I2C_SDA_PIN, QBIT_I2C_SCL_PIN);
    u8g2.setBusClock(400000);
    u8g2.begin();
    u8g2.setContrast(0x80);

    if (!inputTaskInit() || !displayTaskInit(u8g2)) {
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
