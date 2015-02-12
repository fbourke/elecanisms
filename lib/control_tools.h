// Made by Eric Schneider and Forrest Bourke for Elecanisms 2015

#ifndef _CONTROL_TOOLS_H_
#define _CONTROL_TOOLS_H_

#include <stdint.h>
#include "timer.h"

#define FLIP_TRACKING_FREQ 5000
#define FLIP_THRESHOLD 700

typedef struct _FLIP_TRACKER {
    int rawPos;
    int lastRawPos;
    int lastLastRawPos;
    int flipNumber;
    int rawDiff;
    int lastRawDiff;
    int rawOffset;
    int lastRawOffset;
    int flipped;
} _FLIP_TRACKER;

void init_flip_tracking();
int get_flips();
void track_flips(_TIMER *self);

int get_angle();

#endif
