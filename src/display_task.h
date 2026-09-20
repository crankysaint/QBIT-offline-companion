#ifndef DISPLAY_TASK_H
#define DISPLAY_TASK_H

#include <Adafruit_SSD1327.h>

bool displayTaskInit(Adafruit_SSD1327 &display);
void displayTask(void *param);

#endif
