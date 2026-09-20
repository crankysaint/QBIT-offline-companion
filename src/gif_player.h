#ifndef GIF_PLAYER_H
#define GIF_PLAYER_H

#include <Arduino.h>
#include <U8g2lib.h>

bool gifPlayerInit(U8G2 &display);
void gifPlayerNextFace();
void gifPlayerNextSpeed();
const char *gifPlayerCurrentFaceName();
const char *gifPlayerCurrentSpeedName();
void gifPlayerRenderFrame(uint32_t nowMs);

#endif

