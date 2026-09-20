#ifndef DISPLAY_TASK_H
#define DISPLAY_TASK_H

#include <U8g2lib.h>

bool displayTaskInit(U8G2 &display);
void displayTask(void *param);

#endif

