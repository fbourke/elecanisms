#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "uart.h"

typedef enum {
OPEN,
CLOSED
} ValveState;


uint16_t valveStates = 0;
int count = 0;

void delay(){
   int c = 1;

   for ( c = 1 ; c <= 500 ; c++ ){ } /*5 kHz*/

}

void longDelay(){
   int c = 1, d = 1;

   for ( c = 1 ; c <= 32000 ; c++ ){ 
        for ( d = 1 ; d <= 150 ; d++ ){}
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

void writeValveState(int valveNumber, ValveState state) {
    uint16_t bitMask;
    if (state == OPEN) {
        bitMask = 1<<valveNumber;
        valveStates = valveStates | bitMask;
    }
    if (state == CLOSED) {
        bitMask = ~(1<<valveNumber); 
        valveStates = valveStates & bitMask;
    }


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
  
    // 0 Lefty in
    // 1 Lefty out
    // 2 Righty out
    // 3 notWired
    // 4 Orange out
    // 5 Orange in
    // 6 Righty in
    // 7 unwired 



    while (1) {
        count = 0;
        writeValveState(0, OPEN);
        for(count = 0; count < 8; count++){
            pin_write(&D[3],valveStates & (1<<count));
            pulseClock();
        }
        longDelay();
        writeValveState(0, CLOSED);
          for(count = 0; count < 8; count++){
            pin_write(&D[3],valveStates & (1<<count));
            pulseClock();
        }
        longDelay();
                writeValveState(1, OPEN);
        for(count = 0; count < 8; count++){
            pin_write(&D[3],valveStates & (1<<count));
            pulseClock();
        }
        longDelay();
        writeValveState(1, CLOSED);
          for(count = 0; count < 8; count++){
            pin_write(&D[3],valveStates & (1<<count));
            pulseClock();
        }
        longDelay();
    }
}