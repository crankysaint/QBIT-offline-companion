#ifndef INPUT_TASK_H
#define INPUT_TASK_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

enum class InputEventType : uint8_t {
    Tap,
    LongPress
};

struct InputEvent {
    InputEventType type;
    uint32_t timestampMs;
};

extern QueueHandle_t g_inputEventQueue;

bool inputTaskInit();
void inputTask(void *param);

#endif

