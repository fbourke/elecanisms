// Made by Eric Schneider and Forrest Bourke for Elecanisms 2015

#ifndef _CONTROL_TOOLS_H_
#define _CONTROL_TOOLS_H_

#include <stdint.h>
#include "timer.h"

#define FLIP_TRACKING_FREQ 		5000
#define FLIP_THRESHOLD 			700
#define TICKS_PER_REV           790  // On the angle sensor
#define VELOCITY_TRACKING_FREQ	500
#define TRACKED_VELOCITY_POINTS	10

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

void init_angle_tracking();
int get_angle();

void init_velocity_tracking();
int get_velocity();
void track_velocity(_TIMER *self);

#endif
