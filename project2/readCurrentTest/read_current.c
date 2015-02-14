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


// Runs the motor at a PWM speed when the switch is pressed, and prints out
// the read Vemf and current


volatile int pin0, pin1, pin2, pin3, pin4, pin5;


void setup() {
    pin_analogIn(&A[0]);
    pin_analogIn(&A[1]);
    pin_analogIn(&A[2]);
    pin_analogIn(&A[3]);
    pin_analogIn(&A[4]);
    pin_analogIn(&A[5]);
    setup_motor_shield();
    pin_set(&D[6]);
}

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
    init_oc();
    init_pin();
    init_timer();
    init_uart();
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
