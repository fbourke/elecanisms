#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "uart.h"


uint16_t data = 7;
int count = 0;

void delay(){
   int c = 1;

   for ( c = 1 ; c <= 500 ; c++ ){ } /*5 kHz*/

}

void longDelay(){
   int c = 1, d = 1;

   for ( c = 1 ; c <= 32000 ; c++ ){ 
        for ( d = 1 ; d <= 32 ; d++ ){}
    }
}

void setup_pins() {
    pin_digitalOut(&D[3]);
    pin_digitalOut(&D[4]);

}

void pulseClock(){
    pin_clear(&D[4]);
    delay();
    pin_set(&D[4]);
    delay();
    pin_clear(&D[4]);
}

int16_t main(void) {
    init_clock();
    init_ui();
    init_timer();
    init_uart();
    init_pin();
    setup_pins();
    // Data = D3
    // Clock = D4


    while (1) {
        count = 0;
/*        if (!sw_read(&sw1)) {
            data = 255;
        }
        else {
            data = 0;
        }*/
        data = 7;
        for(count = 0; count < 6; count++){
            pin_write(&D[3],data & 01);
            data>>=1;
            pulseClock();
        }
        longDelay();
    }
}