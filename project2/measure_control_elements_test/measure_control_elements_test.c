#include <p24FJ128GB206.h>
#include <stdio.h>
#include "config.h"
#include "control_tools.h"
#include "common.h"
#include "pin.h"
#include "timer.h"
#include "uart.h"
#include "ui.h"
#include "oc.h"


int16_t main(void) {
    init_clock();
    init_pin();
    init_timer();
    init_uart();  // Default is 19200 baud
    init_ui();
    init_flip_tracking();
    init_angle_tracking();

    // Set the print timer
    timer_setPeriod(&timer1, 0.1);
    timer_start(&timer1);
    // Set the interrupt timer to track flips
    timer_every(&timer2, 1.0 / FLIP_TRACKING_FREQ, track_flips);
    timer_every(&timer3, 1.0 / VELOCITY_TRACKING_FREQ, track_velocity);

    while (1) {
        if (timer_flag(&timer1)) {
            timer_lower(&timer1);
            printf("Flips: %d\n", get_flips());
            printf("Angle: %d\n", get_angle());
            printf("Velocity: %d\n", get_velocity());
        }
    }
}
