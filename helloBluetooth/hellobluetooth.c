#include <p24FJ128GB206.h>
#include "ui.h"
#include "config.h"
#include "common.h"
#include "uart.h"
#include "string.h"
#include <stdio.h>

uint8_t returnString[64];
uint16_t validResponse = 1;
_UART* RN42 = &uart2;

void init_bluart(void) {
    uart_open(&uart2, &D[0], &D[1], &D[2], &D[3], 115200., 'N', 1, 0, NULL, 0, NULL, 0);
}

void bt_forward_reponse(void) {
    uart_gets(&uart2, returnString, 40);
    printf(returnString);
}

void bt_send_command(uint8_t *send_string) {
    uart_puts(&uart2, send_string);
    printf(send_string);
}

void setup_bluetooth_module() {
    bt_send_command("$$$\r\n");
    pin_clear(&D[0]);
    bt_forward_reponse();

    bt_send_command("SA,0\r\n");
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


}

int16_t main(void) {
    init_clock();
    init_ui();
    init_pin();
    init_uart();   // sets up uart1
    init_bluart(); // sets up uart2
    led_on(&led1);

    setup_bluetooth_module();

    led_on(&led3);

    while (1) {
        uart_gets(&uart1, returnString, 40);
    }
}
