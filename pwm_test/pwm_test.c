#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "oc.h"
#include "pin.h"
#include "timer.h"
#include "uart.h"
#include <stdio.h>

uint8_t string[40];

int16_t main(void) {
    init_clock();
    init_oc();
    init_pin();
    init_timer();
    init_uart();

// void oc_pwm(_OC *self, _PIN *out, _TIMER *timer, float freq (Hz), uint16_t duty)
    oc_pwm(&oc[0], &D[0], NULL, 10000, 512 << 6);

    printf("Hello World!\n");

    printf("What is your name? ");
    uart_gets(&uart1, string, 40);
    printf("Hello %s!\n", string);

    printf("Testing PWM.\n");

    while (1) {
        // Change the PWM signal here
    }
}
