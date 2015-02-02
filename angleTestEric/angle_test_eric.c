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

#define LOOP_FREQ 5000
#define FLIP_THRESHOLD 700

_PIN *sensorPosPin = &A[3];
uint8_t string[40];

// Position tracking variables
volatile int updatedPos = 0;     // keeps track of the latest updated value of the MR sensor reading
volatile int rawPos = 0;         // current raw reading from MR sensor
volatile int lastRawPos = 0;     // last raw reading from MR sensor
volatile int lastLastRawPos = 0; // last last raw reading from MR sensor
volatile int flipNumber = 0;     // keeps track of the number of flips over the 180deg mark
volatile int tempOffset = 0;
volatile int rawDiff = 0;
volatile int lastRawDiff = 0;
volatile int rawOffset = 0;
volatile int lastRawOffset = 0;
volatile int flipped = 0;
volatile int velocity = 0;
volatile int acceleration = 0;
volatile int systemTime = 0;
volatile int lastFlips = 0;


void setup() {
    pin_analogIn(sensorPosPin);
    lastLastRawPos = pin_read(sensorPosPin);
    lastRawPos = pin_read(sensorPosPin);
    setup_motor_shield();
    pin_set(&D[6]);
}

void track_flips(_TIMER *self) {
    // Get voltage output by MR sensor
    rawPos = pin_read(sensorPosPin) >> 6;

    // Calculate differences between subsequent MR sensor readings
    rawDiff = rawPos - lastRawPos;
    lastRawDiff = rawPos - lastLastRawPos;
    rawOffset = abs(rawDiff);
    lastRawOffset = abs(lastRawDiff);

    // Update position record-keeping vairables
    lastLastRawPos = lastRawPos;
    lastRawPos = rawPos;

    // Keep track of flips over 180 degrees
    if((lastRawOffset > FLIP_THRESHOLD) && (flipped == 0)) { // enter this anytime the last offset is greater than the flip threshold AND it has not just flipped
        if(lastRawDiff > 0) { flipNumber--; }
        else { flipNumber++; }
        flipped = 1;
    }
    else {
        flipped = 0;
    }
}

void track_speed(_TIMER *self) {
    systemTime++;
    velocity = (flipNumber-lastFlips);
    lastFlips = flipNumber;
}

int16_t main(void) {
    init_clock();
    init_pin();
    init_timer();
    init_uart();  // Default is 19200 baud
    init_ui();
    init_oc();
    setup();

    timer_setPeriod(&timer1, 0.1);
    timer_start(&timer1);

    oc_pwm(&oc1, &D[3], NULL, 20000, 1023 << 6);  // D[3] is tri-stating


    timer_every(&timer2, (1.0/LOOP_FREQ), track_flips);

    timer_every(&timer3, 1, track_speed);

    printf("Control Loop Period: %f\n",1.0/LOOP_FREQ);

    while (1) {
        if (timer_flag(&timer1)) {
            timer_lower(&timer1);
            printf("rawDiff:\t%d\tflipNumber:\t%d\tvelocity:\t%d\taccel:\t%d\n",
                   rawDiff, flipNumber, velocity, acceleration);
        }
        if (!sw_read(&sw1)) {
            pin_write(&D[3],386 << 6);
        }
        else {
            pin_write(&D[3],1023 << 6);
        }
    }
}
