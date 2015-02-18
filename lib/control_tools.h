// Made by Eric Schneider and Forrest Bourke for Elecanisms 2015

#ifndef _CONTROL_TOOLS_H_
#define _CONTROL_TOOLS_H_

#include <stdint.h>
#include "timer.h"

#define FLIP_TRACKING_FREQ 		5000
#define FLIP_THRESHOLD 			700
#define VELOCITY_TRACKING_FREQ	20
#define BAD_VELOCITY_THRESHOLD	600
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

typedef struct _VELOCITY_TRACKER {
    int rawVemf;
    int lastRawVemf;
    int rawVelocity[TRACKED_VELOCITY_POINTS];
} _VELOCITY_TRACKER;

void init_flip_tracking();
int get_flips();
void track_flips(_TIMER *self);

int get_angle();

void init_velocity_tracking();
int get_velocity();
void track_velocity(_TIMER *self);

#endif
