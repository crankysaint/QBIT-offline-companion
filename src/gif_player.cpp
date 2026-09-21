#include "gif_player.h"

#include <FluxGarage_RoboEyes.h>
#include <freertos/semphr.h>

namespace {
struct FaceProfile {
    const char *name;
    uint8_t mood;
    uint8_t position;
    bool curiosity;
    bool sweat;
};

struct OneShotAnimation {
    const char *name;
    void (*play)();
};

RoboEyes<Adafruit_SSD1327> *g_roboEyes = nullptr;
SemaphoreHandle_t g_roboEyesMutex = nullptr;
size_t g_faceIndex = 0;
size_t g_animationIndex = 0;

constexpr uint8_t kBackgroundColor = 0x00;
constexpr uint8_t kMainColor = 0x0F;
constexpr uint8_t kEyeWidth = 38;
constexpr uint8_t kEyeHeight = 38;
constexpr uint8_t kEyeBorderRadius = 10;
constexpr int kEyeSpacing = 14;

constexpr FaceProfile kFaces[] = {
    {"Default", DEFAULT, DEFAULT, false, false},
    {"Happy", HAPPY, DEFAULT, true, false},
    {"Tired", TIRED, DEFAULT, false, false},
    {"Angry", ANGRY, DEFAULT, true, false},
};

const FaceProfile &currentFace() {
    return kFaces[g_faceIndex];
}

void playBlink() {
    if (g_roboEyes != nullptr) {
        g_roboEyes->blink();
    }
}

void playConfused() {
    if (g_roboEyes != nullptr) {
        g_roboEyes->anim_confused();
    }
}

void playLaugh() {
    if (g_roboEyes != nullptr) {
        g_roboEyes->anim_laugh();
    }
}

constexpr OneShotAnimation kAnimations[] = {
    {"Blink", playBlink},
    {"Confused", playConfused},
    {"Laugh", playLaugh},
};

const OneShotAnimation &currentAnimation() {
    return kAnimations[g_animationIndex];
}

void applyCurrentFace() {
    if (g_roboEyes == nullptr) {
        return;
    }

    const FaceProfile &face = currentFace();
    g_roboEyes->setMood(face.mood);
    g_roboEyes->setPosition(face.position);
    g_roboEyes->setCuriosity(face.curiosity ? ON : OFF);
    g_roboEyes->setSweat(face.sweat ? ON : OFF);
}

bool lockRoboEyes() {
    return g_roboEyesMutex != nullptr && xSemaphoreTake(g_roboEyesMutex, portMAX_DELAY) == pdTRUE;
}
}  // namespace

bool gifPlayerInit(Adafruit_SSD1327 &display) {
    static RoboEyes<Adafruit_SSD1327> roboEyes(display);
    g_roboEyes = &roboEyes;
    if (g_roboEyesMutex == nullptr) {
        g_roboEyesMutex = xSemaphoreCreateMutex();
    }
    if (g_roboEyesMutex == nullptr || !lockRoboEyes()) {
        return false;
    }

    g_faceIndex = 0;
    g_animationIndex = 0;

    g_roboEyes->begin(128, 128, 60);
    g_roboEyes->setDisplayColors(kBackgroundColor, kMainColor);
    g_roboEyes->setWidth(kEyeWidth, kEyeWidth);
    g_roboEyes->setHeight(kEyeHeight, kEyeHeight);
    g_roboEyes->setBorderradius(kEyeBorderRadius, kEyeBorderRadius);
    g_roboEyes->setSpacebetween(kEyeSpacing);
    g_roboEyes->setAutoblinker(ON, 3, 2);
    g_roboEyes->setIdleMode(ON, 2, 2);
    applyCurrentFace();
    xSemaphoreGive(g_roboEyesMutex);
    return true;
}

void gifPlayerNextFace() {
    if (!lockRoboEyes()) {
        return;
    }
    g_faceIndex = (g_faceIndex + 1) % (sizeof(kFaces) / sizeof(kFaces[0]));
    applyCurrentFace();
    xSemaphoreGive(g_roboEyesMutex);
}

const char *gifPlayerTriggerNextAnimation() {
    if (!lockRoboEyes()) {
        return "";
    }
    const char *animationName = currentAnimation().name;
    currentAnimation().play();
    g_animationIndex = (g_animationIndex + 1) % (sizeof(kAnimations) / sizeof(kAnimations[0]));
    xSemaphoreGive(g_roboEyesMutex);
    return animationName;
}

const char *gifPlayerCurrentFaceName() {
    if (!lockRoboEyes()) {
        return "";
    }
    const char *faceName = currentFace().name;
    xSemaphoreGive(g_roboEyesMutex);
    return faceName;
}

const char *gifPlayerCurrentAnimationName() {
    if (!lockRoboEyes()) {
        return "";
    }
    const char *animationName = currentAnimation().name;
    xSemaphoreGive(g_roboEyesMutex);
    return animationName;
}

void gifPlayerRenderFrame() {
    if (g_roboEyes == nullptr || !lockRoboEyes()) {
        return;
    }
    g_roboEyes->drawEyes();
    xSemaphoreGive(g_roboEyesMutex);
}
