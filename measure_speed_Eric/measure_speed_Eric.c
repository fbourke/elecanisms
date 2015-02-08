#include <p24FJ128GB206.h>
#include <math.h>
#include <stdio.h>
#include "config.h"
#include "common.h"
#include "pin.h"
#include "timer.h"
#include "uart.h"
#include "ui.h"
#include "oc.h"


// NOTE: Worked on this code on 2-7. The idea was to read the number of ticks
// for each flip and estimate a speed. It never seemed to work
// Ideas:
//      -- Is there a way to estimate the interval that the flip counter is
//         actually running at?
//      -- Is there a way to speed up flip counter?
//      -- What the hell was the countsAtFlip variable doing? It never seemed
//         right
//      -- I thought I got the math right for the timer flips but they never
//         added up to the same RPM as the flip counter, which is correct

#define PRINT_FREQ 8
#define FLIP_TRACKING_FREQ 5000
#define SPEED_CHECK_FREQ 20
#define FLIP_THRESHOLD 700


_PIN *sensorPosPin = &A[3];

// Position tracking variables
volatile int rawPos = 0;      // current raw reading from MR sensor
volatile int lastRawPos = 0;
volatile int lastLastRawPos = 0;
volatile int flipNumber = 0;  // keeps track of the number of flips over the 180deg mark
volatile int rawDiff = 0;
volatile int lastRawDiff = 0;
volatile int rawOffset = 0;
volatile int lastRawOffset = 0;
volatile int counterBetweenFlips = 0;
volatile int countsAtFlip = 0;

// Velocity tracking variables
volatile int flipCountingVelocity = 0;
volatile int lastFlips = 0;
volatile int flipTimerVelocity = 0;

void setup() {
    pin_analogIn(sensorPosPin);
    lastLastRawPos = pin_read(sensorPosPin);
    lastRawPos = pin_read(sensorPosPin);
    setup_motor_shield();
    pin_set(&D[6]);
}

void track_flips(_TIMER *self) {
    rawPos = pin_read(sensorPosPin) >> 6;
    rawDiff = rawPos - lastRawPos;
    lastRawDiff = rawPos - lastLastRawPos;
    lastLastRawPos = lastRawPos;
    lastRawPos = rawPos;

    // Keep track of flips over 180 degrees
    if((abs(lastRawDiff) > FLIP_THRESHOLD) && (counterBetweenFlips > 0)) {
        if (lastRawDiff > 0) { flipNumber++; }
        else { flipNumber--; }
        countsAtFlip = counterBetweenFlips;
        counterBetweenFlips = 0;
    }
    else {
        counterBetweenFlips++;
    }
}

void track_speed(_TIMER *self) {
    // The 30 goes from flips/second to RPM
    flipCountingVelocity = (flipNumber - lastFlips) * SPEED_CHECK_FREQ * 30;
    lastFlips = flipNumber;
    // The 30 goes from flips/second to RPM
    // printf("flipNumber: %d, countsAtFlip: %d\n", flipNumber, countsAtFlip);
    flipTimerVelocity = 30 * FLIP_TRACKING_FREQ / countsAtFlip;
    printf("Basic V: %d, New V: %d\n", flipCountingVelocity, flipTimerVelocity);
    // printf("flipNumber: %d \n\n", flipNumber);
}

int16_t main(void) {
    init_clock();
    init_pin();
    init_timer();
    init_uart();  // Default is 19200 baud
    init_ui();
    init_oc();
    setup();

    // Set the print timer
    // timer_setPeriod(&timer1, 1.0 / PRINT_FREQ);
    // timer_start(&timer1);
    timer_every(&timer2, 1.0 / FLIP_TRACKING_FREQ, track_flips);
    timer_every(&timer3, 1.0 / SPEED_CHECK_FREQ, track_speed);
    // printf("PRINT Period: %f\n", 1.0 / PRINT_FREQ);
    printf("FLIP_TRACKING Period: %f\n", 1.0 / FLIP_TRACKING_FREQ);
    printf("SPEED_CHECK Period: %f\n", 1.0 / SPEED_CHECK_FREQ);

    // Start the motor PWM signal
    oc_pwm(&oc1, &D[3], NULL, 5000, 1023 << 6);

    while (1) {
        // if (timer_flag(&timer1)) {
        //     timer_lower(&timer1);
        //     printf("%d %d %d\n", flipCountingVelocity, flipTimerVelocity, flipNumber);
        // }
        if (!sw_read(&sw1)) {
            pin_write(&D[3], 128 << 6);
        }
        else {
            pin_write(&D[3], 1023 << 6);
        }
    }
}
