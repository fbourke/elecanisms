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


_PIN *rangeFinderPin = &A[5];

void setup_pins() {
    pin_analogIn(sensorPosPin);
}

int16_t main(void) {
    init_clock();
    init_pin();
    init_timer();
    init_uart();  // Default is 19200 baud
    init_ui();
    init_oc();
    setup_pins();

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
