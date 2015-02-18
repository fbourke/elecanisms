// Made by Eric Schneider and Forrest Bourke for Elecanisms 2015

#include <p24FJ128GB206.h>
#include <math.h>
#include "common.h"
#include "control_tools.h"
#include "pin.h"

_FLIP_TRACKER FlipTracker;
int low_angle = 300;
int rawAngles[TRACKED_VELOCITY_POINTS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int get_flips() {
    return FlipTracker.flipNumber;
}

int get_angle() {
    int rawPos = (int) (pin_read(&A[3]) >> 6);
    if (rawPos < low_angle) { low_angle = rawPos; }
    return rawPos - low_angle;
}

int get_velocity() {
    int i, sum = 0;
    for (i = 0; i < (TRACKED_VELOCITY_POINTS - 1); i++) {
        sum += rawVelocity[i];
    }
    return sum / TRACKED_VELOCITY_POINTS;
}

void init_flip_tracking() {
    FlipTracker.rawPos = 0;
    FlipTracker.lastRawPos = 0;
    FlipTracker.lastLastRawPos = 0;
    FlipTracker.flipNumber = 0;
    FlipTracker.rawDiff = 0;
    FlipTracker.lastRawDiff = 0;
    FlipTracker.rawOffset = 0;
    FlipTracker.lastRawOffset = 0;
    FlipTracker.flipped = 0;
    pin_analogIn(&A[3]);
    FlipTracker.lastLastRawPos = pin_read(&A[3]) >> 6;
    FlipTracker.lastRawPos = pin_read(&A[3]) >> 6;
}

void track_flips(_TIMER *self) {
    // Get voltage output by MR sensor
    FlipTracker.rawPos = (int) (pin_read(&A[3]) >> 6);

    // Calculate differences between subsequent MR sensor readings
    FlipTracker.rawDiff = FlipTracker.rawPos - FlipTracker.lastRawPos;
    FlipTracker.lastRawDiff = FlipTracker.rawPos - FlipTracker.lastLastRawPos;
    FlipTracker.rawOffset = abs(FlipTracker.rawDiff);
    FlipTracker.lastRawOffset = abs(FlipTracker.lastRawDiff);

    // Update position record-keeping vairables
    FlipTracker.lastLastRawPos = FlipTracker.lastRawPos;
    FlipTracker.lastRawPos = FlipTracker.rawPos;

    // Keep track of flips over 180 degrees
    if((FlipTracker.lastRawOffset > FLIP_THRESHOLD) && (FlipTracker.flipped == 0)) { // enter this anytime the last offset is greater than the flip threshold AND it has not just flipped
        if (FlipTracker.lastRawDiff > 0) { FlipTracker.flipNumber--; }
        else { FlipTracker.flipNumber++; }
        FlipTracker.flipped = 1;
    }
    else { FlipTracker.flipped = 0; }
}

void init_angle_tracking() {
    pin_analogIn(&A[3]);
}

void track_velocity(_TIMER *self) {
    for (i = (TRACKED_VELOCITY_POINTS - 1); i > 0; i--) {
        rawVelocity[i] = rawVelocity[i - 1];
    }
    int flips = get_flips();
    int angle = get_angle();
    if (flips >= 0) {
        rawVelocity[0] = flips * TICKS_PER_REV + angle;
    }
    else {
        rawVelocity[0] = flips * TICKS_PER_REV + (angle - TICKS_PER_REV);
    }
}
