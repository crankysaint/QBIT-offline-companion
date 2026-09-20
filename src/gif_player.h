#ifndef GIF_PLAYER_H
#define GIF_PLAYER_H

#include <Arduino.h>
#include <Adafruit_SSD1327.h>

bool gifPlayerInit(Adafruit_SSD1327 &display);
void gifPlayerNextFace();
void gifPlayerTriggerNextAnimation();
const char *gifPlayerCurrentFaceName();
const char *gifPlayerCurrentAnimationName();
void gifPlayerRenderFrame(uint32_t nowMs);

#endif
