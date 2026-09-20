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

U8G2 *g_display = nullptr;
AppState g_state = AppState::BootAnim;
uint32_t g_stateStartedAt = 0;
uint32_t g_overlayUntilMs = 0;
char g_overlayText[24] = {0};

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

    const uint8_t boxY = 4;
    g_display->setDrawColor(1);
    g_display->drawRBox(8, boxY, 112, 16, 4);
    g_display->setDrawColor(0);
    g_display->setFont(u8g2_font_5x8_tf);
    g_display->drawUTF8(14, boxY + 11, g_overlayText);
    g_display->setDrawColor(1);
}

void drawBootFrame(uint32_t elapsedMs) {
    if (g_display == nullptr) {
        return;
    }

    const uint8_t ring = 14 + static_cast<uint8_t>((elapsedMs / 60) % 20);
    g_display->clearBuffer();
    g_display->drawCircle(64, 54, ring);
    g_display->drawCircle(64, 54, ring + 12);
    g_display->setFont(u8g2_font_logisoso20_tf);
    g_display->drawUTF8(24, 70, "QBIT");
    g_display->setFont(u8g2_font_6x12_tf);
    g_display->drawUTF8(12, 102, "Offline companion");
    g_display->sendBuffer();
}
}  // namespace

bool displayTaskInit(U8G2 &display) {
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
            while (g_inputEventQueue != nullptr && xQueueReceive(g_inputEventQueue, &event, 0) == pdTRUE) {
                if (event.type == InputEventType::Tap) {
                    gifPlayerNextFace();
                    setOverlay("Face", gifPlayerCurrentFaceName());
                } else if (event.type == InputEventType::LongPress) {
                    gifPlayerNextSpeed();
                    setOverlay("Speed", gifPlayerCurrentSpeedName());
                }
            }
            gifPlayerRenderFrame(now);
            drawOverlay();
            g_display->sendBuffer();
        }

        vTaskDelay(pdMS_TO_TICKS(kFrameDelayMs));
    }
}
