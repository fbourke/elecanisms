#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "uart.h"
#include <stdio.h>

uint8_t string[40];

int16_t main(void) {
    init_clock();
    init_uart();

    printf("Hello World!\n");

    printf("What is your name? ");
    uart_gets(&uart1, string, 40);
    printf("Hello %s!\n", string);

    printf("Type something at the prompt.\n");

    while (1) {
        printf(">> ");
        uart_gets(&uart1, string, 40);
        printf("You typed  '%s'\n", string);
    }
}
