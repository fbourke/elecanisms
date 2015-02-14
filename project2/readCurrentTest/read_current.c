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
    pin_analogIn(&A[0]);
    pin_analogIn(&A[1]);
    pin_analogIn(&A[2]);
    pin_analogIn(&A[4]);
    pin_analogIn(&A[5]);
    lastLastRawPos = pin_read(sensorPosPin);
    lastRawPos = pin_read(sensorPosPin);
    setup_motor_shield();
    pin_set(&D[6]);
}

uint8_t string[40];
volatile int pin0, pin1, pin2, pin3, pin4, pin5;

// 
void read_pins(_TIMER *self) {
    pin0 = pin_read(&A[0]) >> 6;
    pin1 = pin_read(&A[1]) >> 6;
    pin2 = pin_read(&A[2]) >> 6;
    pin3 = pin_read(&A[3]) >> 6;
    pin4 = pin_read(&A[4]) >> 6;
    pin5 = pin_read(&A[5]) >> 6;
}

int16_t main(void) {
    init_clock();
    init_pin();
    init_timer();
    init_uart();
    init_oc();
    init_ui();
    setup();

    oc_pwm(&oc2, &D[2], NULL, 100, 1023 << 6);


    timer_setPeriod(&timer1, 0.05);
    timer_start(&timer1);

    timer_every(&timer2, 0.001, read_pins);

    int i;
    for (i = 0; i<6; i++) {
        pin_analogIn(&A[i]);
    }

    while (1) {
        if (timer_flag(&timer1)) {
            timer_lower(&timer1);
            int temppin0 = pin0;
            int temppin1 = pin1;
            int temppin9 = __digitalRead(&D[9]);
            if(!temppin9) {
                printf("%d %d\n", temppin0, temppin1);
            } 
        }

        if (!sw_read(&sw1)) {
            pin_write(&D[2], 512 << 6);
        }
        else {
            pin_write(&D[2], 0 << 6);
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