#include <p24FJ128GB206.h>
#include <stdio.h>
#include "config.h"
#include "common.h"
#include "timer.h"
#include "uart.h"

// Reads the analog voltage on the pins specified in read_pins
// Reads the pins in an interrupt specified by timer2

uint8_t string[40];
volatile int pin3, pin4;

// 
void read_pins(_TIMER *self) {
    pin3 = pin_read(&A[3]) >> 6;
    pin4 = pin_read(&A[4]) >> 6;
}

int16_t main(void) {
    init_clock();
    init_pin();
    init_timer();
    init_uart();

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
            printf("%d %d\n", pin3, pin4);
        }
    }
}
