#include "input_task.h"

#ifndef QBIT_BUTTON_PIN
#define QBIT_BUTTON_PIN 1
#endif

#ifndef QBIT_BUTTON_ACTIVE_LOW
#define QBIT_BUTTON_ACTIVE_LOW 1
#endif

namespace {
constexpr uint32_t kLongPressMs = 800;
constexpr uint32_t kDebounceMs = 30;
constexpr uint32_t kPollDelayMs = 10;

bool isPressed() {
    const bool rawHigh = digitalRead(QBIT_BUTTON_PIN) == HIGH;
#if QBIT_BUTTON_ACTIVE_LOW
    return !rawHigh;
#else
    return rawHigh;
#endif
}
}  // namespace

QueueHandle_t g_inputEventQueue = nullptr;

bool inputTaskInit() {
    pinMode(QBIT_BUTTON_PIN, QBIT_BUTTON_ACTIVE_LOW ? INPUT_PULLUP : INPUT);
    if (g_inputEventQueue == nullptr) {
        g_inputEventQueue = xQueueCreate(4, sizeof(InputEvent));
    }
    return g_inputEventQueue != nullptr;
}

void inputTask(void *param) {
    (void)param;

    bool wasPressed = false;
    bool longPressSent = false;
    uint32_t pressedAt = 0;
    uint32_t releasedAt = 0;

    for (;;) {
        const uint32_t now = millis();
        const bool pressed = isPressed();

        if (pressed && !wasPressed && (now - releasedAt) >= kDebounceMs) {
            wasPressed = true;
            longPressSent = false;
            pressedAt = now;
        } else if (pressed && wasPressed && !longPressSent && (now - pressedAt) >= kLongPressMs) {
            const InputEvent event{InputEventType::LongPress, now};
            if (xQueueSend(g_inputEventQueue, &event, 0) == pdTRUE) {
                longPressSent = true;
            }
        } else if (!pressed && wasPressed) {
            wasPressed = false;
            releasedAt = now;
            if (!longPressSent && (now - pressedAt) >= kDebounceMs) {
                const InputEvent event{InputEventType::Tap, now};
                xQueueSend(g_inputEventQueue, &event, 0);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(kPollDelayMs));
    }
}
