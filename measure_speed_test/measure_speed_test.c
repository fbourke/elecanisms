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


// This code runs the motor at a variable speed when switched and prints
// the velocity to terminal. Good for testing accuracy of magnetic sensor


#define FLIP_TRACKING_FREQ 5000
#define SPEED_CHECK_FREQ 100
#define FLIP_THRESHOLD 700


_PIN *sensorPosPin = &A[3];

// Position tracking variables
volatile int rawPos = 0;         // current raw reading from MR sensor
volatile int lastRawPos = 0;
volatile int lastLastRawPos = 0;
volatile int flipNumber = 0;     // keeps track of the number of flips over the 180deg mark
volatile int rawDiff = 0;
volatile int lastRawDiff = 0;
volatile int rawOffset = 0;
volatile int lastRawOffset = 0;
volatile int flipped = 0;

// Velocity tracking variables
volatile int velocity = 0;
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
    else { flipped = 0; }
}

void track_speed(_TIMER *self) {
    velocity = (flipNumber - lastFlips) * SPEED_CHECK_FREQ * 30; // The 30 scalar turns flips/sec into RPM
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

    // Set the print timer
    timer_setPeriod(&timer1, 0.01);
    timer_start(&timer1);
    // Set the interrupt timer to track flips
    timer_every(&timer2, 1.0 / FLIP_TRACKING_FREQ, track_flips);
    printf("Control Loop Period: %f\n", 1.0 / FLIP_TRACKING_FREQ);
    // Set the interrupt timer to track speed
    timer_every(&timer3, 1.0/SPEED_CHECK_FREQ, track_speed);

    // Start the motor PWM signal
    oc_pwm(&oc1, &D[3], NULL, 20000, 1023 << 6);

    while (1) {
        if (timer_flag(&timer1)) {
            timer_lower(&timer1);
            //printf("rawDiff:\t%d\tflips:\t%d\tvel (RPM):\t%d\n",
            //        rawDiff, flipNumber, velocity);
            printf("%d\n",velocity);
        }
        if (!sw_read(&sw1)) {
            pin_write(&D[3], 128 << 6);
        }
        else {
            pin_write(&D[3], 1023 << 6);
        }
    }
}


/*
PWM 386

1800
1800
1800
1800
2100
1800
1800
1500
900
300
0
0
0
0
0
0
0



PWM 256, 100hz

3000
3000
3000
3000
3000
3000
6000
3000
3000
3000
3000
3000
3000
0
3000
3000
3000
3000
0
3000
3000
3000
0
3000
0
3000
3000
0
3000
0
3000
0
0
3000
0
0
3000
0
0
0
0
3000
0
0
0
0
0
0
0
0

PWM 128, freq 20
3600
3600
4200
4200
4200
4200
4200
3600
3600
3600
3600
3600
4800
4800
4800
4800
4800
3600
3600
3600
3600
3600
4800
4800
4800
4800
4800
3600
3600
3600
3600
3600
4800
4800
4800
4800
4800
3600
3600
3600
3600
3600
4200
4200
4200
4200
4200
3600
3600
3600
3600
3600
3600
3600
3600
3600
3600
4200
4200
4200
4200
4200
3000
3000
3000
3000
3000
3000
3000
3000
3000
3000
3000
3000
3000
3000
3000
2400
2400
2400
2400
2400
2400
2400
2400
2400
2400
1800
1800
1800
1800
1800
1200
1200
1200
1200
1200
600
600
600
600
600
600
600
600
600
600
600
600
600
600
600
0
0
0
0
0
0
0
0
0
0
0
0
0

PWM 126, freq 100
3000
3000
6000
3000
6000
3000
6000
3000
3000
3000
3000
6000
3000
6000
3000
6000
3000
3000
3000
3000
3000
3000
6000
3000
6000
3000
6000
3000
6000
3000
0
6000
3000
6000
3000
6000
3000
6000
6000
3000
6000
0
6000
3000
6000
3000
6000
3000
3000
6000
3000
3000
6000
3000
3000
3000
3000
3000
3000
3000
3000
3000
3000
3000
3000
0
3000
3000
3000
3000
0
3000
3000
0
3000
3000
0
3000
0
3000
0
3000
0
3000
0
0
3000
0
0
3000
0
0
0
0
0
3000
0
0
0
0
0
0
0
0
0
0
0
0
0
0



*/