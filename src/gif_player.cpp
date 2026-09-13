#include "gif_player.h"

namespace {
struct Frame {
    uint16_t durationMs;
    uint8_t leftEyeHeight;
    uint8_t rightEyeHeight;
    int8_t mouthSmile;
    int8_t mouthYOffset;
};

struct FaceAnimation {
    const char *name;
    const Frame *frames;
    size_t frameCount;
};

struct SpeedProfile {
    const char *name;
    uint8_t multiplierPct;
};

U8G2 *g_display = nullptr;
size_t g_faceIndex = 0;
size_t g_frameIndex = 0;
size_t g_speedIndex = 1;
uint32_t g_lastFrameMs = 0;

constexpr SpeedProfile kSpeedProfiles[] = {
    {"Slow", 140},
    {"Normal", 100},
    {"Fast", 70},
};

constexpr Frame kCalmFrames[] = {
    {1400, 16, 16, 0, 0},
    {120, 8, 8, 0, 0},
    {110, 2, 2, 0, 0},
    {120, 8, 8, 0, 0},
};

constexpr Frame kSmileFrames[] = {
    {1200, 16, 16, 12, -2},
    {120, 8, 8, 12, -2},
    {100, 2, 2, 12, -2},
    {120, 8, 8, 12, -2},
};

constexpr Frame kWinkFrames[] = {
    {900, 16, 16, 6, 2},
    {200, 16, 2, 6, 2},
    {350, 16, 16, 6, 2},
    {200, 2, 16, 6, 2},
};

constexpr FaceAnimation kFaces[] = {
    {"Calm", kCalmFrames, sizeof(kCalmFrames) / sizeof(kCalmFrames[0])},
    {"Smile", kSmileFrames, sizeof(kSmileFrames) / sizeof(kSmileFrames[0])},
    {"Wink", kWinkFrames, sizeof(kWinkFrames) / sizeof(kWinkFrames[0])},
};

const FaceAnimation &currentFace() {
    return kFaces[g_faceIndex];
}

const SpeedProfile &currentSpeed() {
    return kSpeedProfiles[g_speedIndex];
}

uint16_t scaledDuration(const Frame &frame) {
    return static_cast<uint16_t>((frame.durationMs * currentSpeed().multiplierPct) / 100);
}

void drawEye(U8G2 &display, int16_t centerX, int16_t centerY, uint8_t height) {
    const uint8_t width = 18;
    if (height <= 3) {
        display.drawHLine(centerX - (width / 2), centerY, width);
        return;
    }
    display.drawRBox(centerX - (width / 2), centerY - (height / 2), width, height, 4);
}

void drawFaceFrame(U8G2 &display, const Frame &frame) {
    display.clearBuffer();
    display.setDrawColor(1);
    display.drawCircle(64, 58, 42);
    display.drawCircle(64, 58, 43);
    drawEye(display, 48, 48, frame.leftEyeHeight);
    drawEye(display, 80, 48, frame.rightEyeHeight);

    if (frame.mouthSmile > 8) {
        display.drawLine(48, 76 + frame.mouthYOffset, 56, 84 + frame.mouthYOffset);
        display.drawLine(56, 84 + frame.mouthYOffset, 72, 86 + frame.mouthYOffset);
        display.drawLine(72, 86 + frame.mouthYOffset, 80, 76 + frame.mouthYOffset);
    } else if (frame.mouthSmile < 3) {
        display.drawHLine(50, 78 + frame.mouthYOffset, 28);
    } else {
        display.drawLine(48, 80 + frame.mouthYOffset, 56, 84 + frame.mouthYOffset);
        display.drawLine(56, 84 + frame.mouthYOffset, 72, 84 + frame.mouthYOffset);
        display.drawLine(72, 84 + frame.mouthYOffset, 80, 80 + frame.mouthYOffset);
    }

    display.setFont(u8g2_font_6x12_tf);
    const uint8_t labelX = static_cast<uint8_t>((128 - display.getUTF8Width(currentFace().name)) / 2);
    display.drawUTF8(labelX, 116, currentFace().name);
}
}  // namespace

bool gifPlayerInit(U8G2 &display) {
    g_display = &display;
    g_faceIndex = 0;
    g_frameIndex = 0;
    g_speedIndex = 1;
    g_lastFrameMs = millis();
    return true;
}

void gifPlayerNextFace() {
    g_faceIndex = (g_faceIndex + 1) % (sizeof(kFaces) / sizeof(kFaces[0]));
    g_frameIndex = 0;
    g_lastFrameMs = millis();
}

void gifPlayerNextSpeed() {
    g_speedIndex = (g_speedIndex + 1) % (sizeof(kSpeedProfiles) / sizeof(kSpeedProfiles[0]));
    g_lastFrameMs = millis();
}

const char *gifPlayerCurrentFaceName() {
    return currentFace().name;
}

const char *gifPlayerCurrentSpeedName() {
    return currentSpeed().name;
}

void gifPlayerRenderFrame(uint32_t nowMs) {
    if (g_display == nullptr) {
        return;
    }

    const FaceAnimation &face = currentFace();
    if ((nowMs - g_lastFrameMs) >= scaledDuration(face.frames[g_frameIndex])) {
        g_frameIndex = (g_frameIndex + 1) % face.frameCount;
        g_lastFrameMs = nowMs;
    }

    drawFaceFrame(*g_display, face.frames[g_frameIndex]);
}
