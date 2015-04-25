#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"

typedef enum {
    LIGHT,
    NOLIGHT
} LEDState;


uint16_t LEDStates = 0;

int count = 0;


void delay(){
   int c = 1;

   for ( c = 1 ; c <= 300 ; c++ ){ } 

}

void longDelay(){
   int c = 1, d = 1;

   for ( c = 1 ; c <= 32000 ; c++ ){ 
        for ( d = 1 ; d <= 30 ; d++ ){}
    }
}

void writeLEDState(int LEDNumber, LEDState state) {
    uint16_t bitMask;
    if (state == LIGHT) {
        bitMask = 1<<LEDNumber;
        LEDStates = LEDStates | bitMask;
    }
    if (state == NOLIGHT) {
        bitMask = ~(1<<LEDNumber); 
        LEDStates = LEDStates & bitMask;
    }
}

void setup_pins() {
    pin_digitalOut(&D[5]);
    pin_digitalOut(&D[6]);
    pin_digitalOut(&D[7]);
    pin_digitalOut(&A[0]);
}

void pulseClock(){
    pin_clear(&D[6]);
    delay();
    pin_set(&D[6]);
    delay();
    pin_clear(&D[6]);
}

int16_t main(void) {
    init_clock();
    init_ui();
    init_timer();
    init_pin();
    setup_pins();

    // Sdat = D5
    // LE = A0 sut
    // CLK = D6 clock input
    //  OE = D7 when low, output drivers enabled

    while (1) {        
        writeLEDState(8, LIGHT);
        for(count = 0; count < 48; count++){
            pin_write(&D[5],LEDStates & (1<<count));
            if (count == 47){
                pin_clear(&D[7]);
                pin_set(&A[0]);
            } 
            pulseClock();
            pin_set(&D[7]);
        }
        delay();
        pin_clear(&A[0]);
        longDelay();
        pin_clear(&D[7]);
        longDelay();
    }
}

