#include <p24FJ64GB002.h>
#include "config.h"
#include "common.h"
#include "uart.h"
#include "string.h"
#include <stdio.h>

uint8_t returnString[64];
uint16_t validResponse = 1;
_PIN *led1, *led2;

void init_bluart(void) {
    uart_open(&uart2, &ra0, &ra1, &rb1, &rb0, 115200., 'N', 1, 0, NULL, 0, NULL, 0);
}

void setup_leds(void) {
    led1 = &rb15;
    led2 = &rb14;
    pin_digitalOut(led1);
    pin_digitalOut(led2);
}

void bt_forward_reponse(void) {
    uart_gets(&uart2, returnString, 64);
    // printf(returnString);
}

void bt_send_command(uint8_t *send_string) {
    uart_puts(&uart2, send_string);
    // printf(send_string);
}

void setup_bluetooth_module() {
    pin_set(led2);
    uart_puts(&uart2, "$$$\n\r");

    uart_gets(&uart2, returnString, 64);
    pin_set(led1);
    if (strcmp(returnString, "CMD\n"))

    bt_send_command("SA,0\n\r");
    // bt_forward_reponse();
    // uart_gets(&uart1, returnString, 40);
    // if (strcmp(returnString, "AOK\n")) validResponse = 0;
    // printf("GA");
    // uart_gets(&uart1, returnString, 40);
    // if (strcmp(returnString, "0\n")) validResponse = 0;    

    bt_send_command("SM,1\r\n");
    // bt_forward_reponse();
    // uart_gets(&uart1, returnString, 40);
    // if (strcmp(returnString, "AOK\n")) validResponse = 0;
    // printf("GM");
    // uart_gets(&uart1, returnString, 40);
    // if (strcmp(returnString, "Mstr\n")) validResponse = 0;

    bt_send_command("SN,Whack-a-tooth\r\n");
    // bt_forward_reponse();
    // uart_gets(&uart1, returnString, 40);
    // if (strcmp(returnString, "AOK\n")) validResponse = 0;
    // printf("GN");
    // uart_gets(&uart1, returnString, 40);
    // if (strcmp(returnString, "Whack-a-tooth\n")) validResponse = 0;

    bt_send_command("R,1\r\n");
    // bt_forward_reponse();
    // uart_gets(&uart1, returnString, 40);
    // if (strcmp(returnString, "Reboot!\n")) validResponse = 0;

    // while(1) {
    //     bt_send_command("Hello, World!\n");
    // }
}

int16_t main(void) {
    init_clock();
    init_pin();
    init_uart();
    init_bluart(); // sets up uart2

    setup_leds();

    setup_bluetooth_module();

    // while (1) {
    //     uart_gets(&uart1, returnString, 40);
    // }
}
