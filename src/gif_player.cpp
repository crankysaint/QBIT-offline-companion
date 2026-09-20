#include "gif_player.h"

#include <FluxGarage_RoboEyes.h>

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

Adafruit_SSD1327 *g_display = nullptr;
RoboEyes<Adafruit_SSD1327> *g_roboEyes = nullptr;
size_t g_faceIndex = 0;
size_t g_animationIndex = 0;

constexpr uint8_t kBackgroundColor = 0x00;
constexpr uint8_t kMainColor = 0x0F;

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
}  // namespace

bool gifPlayerInit(Adafruit_SSD1327 &display) {
    g_display = &display;
    static RoboEyes<Adafruit_SSD1327> roboEyes(display);
    g_roboEyes = &roboEyes;

    g_faceIndex = 0;
    g_animationIndex = 0;

    g_roboEyes->begin(128, 128, 60);
    g_roboEyes->setDisplayColors(kBackgroundColor, kMainColor);
    g_roboEyes->setWidth(38, 38);
    g_roboEyes->setHeight(38, 38);
    g_roboEyes->setBorderradius(10, 10);
    g_roboEyes->setSpacebetween(14);
    g_roboEyes->setAutoblinker(ON, 3, 2);
    g_roboEyes->setIdleMode(ON, 2, 2);
    applyCurrentFace();
    return true;
}

void gifPlayerNextFace() {
    g_faceIndex = (g_faceIndex + 1) % (sizeof(kFaces) / sizeof(kFaces[0]));
    applyCurrentFace();
}

const char *gifPlayerTriggerNextAnimation() {
    const char *animationName = currentAnimation().name;
    currentAnimation().play();
    g_animationIndex = (g_animationIndex + 1) % (sizeof(kAnimations) / sizeof(kAnimations[0]));
    return animationName;
}

const char *gifPlayerCurrentFaceName() {
    return currentFace().name;
}

const char *gifPlayerCurrentAnimationName() {
    return currentAnimation().name;
}

void gifPlayerRenderFrame(uint32_t nowMs) {
    (void)nowMs;

    if (g_display == nullptr || g_roboEyes == nullptr) {
        return;
    }
    g_roboEyes->update();
}
