#include "display_task.h"

#include <Arduino.h>
#include <cstdio>

#include "gif_player.h"
#include "input_task.h"

namespace {
enum class AppState : uint8_t {
    BootAnim,
    GifPlayback
};

constexpr uint16_t kBackgroundColor = 0x00;
constexpr uint16_t kMainColor = 0x0F;

Adafruit_SSD1327 *g_display = nullptr;
AppState g_state = AppState::BootAnim;
uint32_t g_stateStartedAt = 0;
uint32_t g_overlayUntilMs = 0;
char g_overlayText[48] = {0};

constexpr uint32_t kBootDurationMs = 1800;
constexpr uint32_t kFrameDelayMs = 20;
constexpr uint32_t kOverlayDurationMs = 1200;

void setOverlay(const char *label, const char *value) {
    snprintf(g_overlayText, sizeof(g_overlayText), "%s: %s", label, value);
    g_overlayUntilMs = millis() + kOverlayDurationMs;
}

void drawOverlay() {
    if (millis() >= g_overlayUntilMs || g_display == nullptr) {
        return;
    }

    const int16_t boxX = 8;
    const int16_t boxY = 6;
    const int16_t boxW = 112;
    const int16_t boxH = 16;
    g_display->fillRoundRect(boxX, boxY, boxW, boxH, 4, kMainColor);
    g_display->setTextColor(kBackgroundColor);
    g_display->setTextSize(1);
    g_display->setCursor(boxX + 8, boxY + 5);
    g_display->print(g_overlayText);
    g_display->setTextColor(kMainColor);
}

void drawBootFrame(uint32_t elapsedMs) {
    if (g_display == nullptr) {
        return;
    }

    const uint8_t ring = 14 + static_cast<uint8_t>((elapsedMs / 60) % 20);
    int16_t x = 0;
    int16_t y = 0;
    uint16_t width = 0;
    uint16_t height = 0;

    g_display->clearDisplay();
    g_display->drawCircle(64, 50, ring, kMainColor);
    g_display->drawCircle(64, 50, ring + 12, kMainColor);

    g_display->setTextColor(kMainColor);
    g_display->setTextSize(3);
    g_display->getTextBounds("QBIT", 0, 0, &x, &y, &width, &height);
    g_display->setCursor((128 - static_cast<int16_t>(width)) / 2, 72);
    g_display->print("QBIT");

    g_display->setTextSize(1);
    g_display->getTextBounds("Offline companion", 0, 0, &x, &y, &width, &height);
    g_display->setCursor((128 - static_cast<int16_t>(width)) / 2, 96);
    g_display->print("Offline companion");
    g_display->display();
}
}  // namespace

bool displayTaskInit(Adafruit_SSD1327 &display) {
    g_display = &display;
    g_state = AppState::BootAnim;
    g_stateStartedAt = millis();
    g_overlayUntilMs = 0;
    return gifPlayerInit(display);
}

void displayTask(void *param) {
    (void)param;

    for (;;) {
        const uint32_t now = millis();
        if (g_state == AppState::BootAnim) {
            drawBootFrame(now - g_stateStartedAt);
            if ((now - g_stateStartedAt) >= kBootDurationMs) {
                g_state = AppState::GifPlayback;
                g_stateStartedAt = now;
                setOverlay("Face", gifPlayerCurrentFaceName());
                if (g_inputEventQueue != nullptr) {
                    xQueueReset(g_inputEventQueue);
                }
            }
        } else {
            InputEvent event;
            if (g_inputEventQueue != nullptr && xQueueReceive(g_inputEventQueue, &event, 0) == pdTRUE) {
                if (event.type == InputEventType::Tap) {
                    gifPlayerNextFace();
                    setOverlay("Face", gifPlayerCurrentFaceName());
                } else if (event.type == InputEventType::LongPress) {
                    setOverlay("Anim", gifPlayerTriggerNextAnimation());
                }
            }
            g_display->clearDisplay();
            gifPlayerRenderFrame();
            drawOverlay();
            g_display->display();
        }

        vTaskDelay(pdMS_TO_TICKS(kFrameDelayMs));
    }
}
