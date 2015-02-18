#include <p24FJ128GB206.h>
#include <math.h>
#include <stdio.h>
#include "config.h"
#include "control_tools.h"
#include "common.h"
#include "oc.h"
#include "pin.h"
#include "timer.h"
#include "uart.h"
#include "ui.h"


#define RESPONSE_HEIGHT 1023
#define RESPONSE_WIDTH  10
#define CONTROL_FREQ    200  // Hz


void setMotorDirection(int flips) {
    if (flips < 0) {
        pin_clear(&D[5]);
        pin_set(&D[6]);
    }
    else if (flips > 0) {
        pin_clear(&D[6]);
        pin_set(&D[5]);
    }
    else {
        pin_clear(&D[5]);
        pin_clear(&D[6]);
    }
}

void setMotorPWM(int goal, int angle) {
    if (abs(angle - goal) < RESPONSE_WIDTH || abs(angle - (goal+200)) < RESPONSE_WIDTH || abs(angle - (goal+400)) < RESPONSE_WIDTH) {
        pin_write(&D[2], RESPONSE_HEIGHT << 6);
    }
    else {
        pin_write(&D[2], 0 << 6);
    }
}

void setup() {
    init_flip_tracking();
    setup_motor_shield();
    // Set the control loop timer
    timer_setPeriod(&timer1, 1.0 / CONTROL_FREQ);
    timer_start(&timer1);
    oc_pwm(&oc1, &D[2], NULL, 400, 0);  // D[2] is tri-stating
    led_on(&led1); led_on(&led2); led_on(&led3);
    timer_every(&timer2, 1.0 / FLIP_TRACKING_FREQ, track_flips);
}

int16_t main(void) {
    init_clock();
    init_oc();
    init_pin();
    init_timer();
    init_uart();  // Default is 19200 baud
    init_ui();
    setup();

    int flips = 0;
    int angle = 0;
    int goal_angle = 200;

    while (1) {
        if (timer_flag(&timer1)) {
            timer_lower(&timer1);
            flips = get_flips();
            angle = get_angle();
            setMotorDirection(flips);
            setMotorPWM(goal_angle, angle);
        }
    }
}
