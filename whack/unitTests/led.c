#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"

typedef enum {
    LIT,
    UNLIT
} LEDState;

LEDState allLEDS = LIT;

_PIN* pin_SData = &D[5];
_PIN* pin_LE = &A[4];
_PIN* pin_CLK = &D[6];
_PIN* pin_OE = &D[7];

uint16_t LEDStates = 0;

int count = 0;

void delay(){
    int c = 1;
    for ( c = 1 ; c <= 300 ; c++ ){
    
    } 

}

void longDelay(){
   int c = 1, d = 1;
   for ( c = 1 ; c <= 32000 ; c++ ){ 
        for ( d = 1 ; d <= 30 ; d++ ){}
    }
}

void writeLEDState(int LEDNumber, LEDState state) {
    uint16_t bitMask;
    if (state == LIT) {
        bitMask = 1<<LEDNumber;
        LEDStates = LEDStates | bitMask;
    }
    if (state == UNLIT) {
        bitMask = ~(1<<LEDNumber); 
        LEDStates = LEDStates & bitMask;
    }
}

void setup_pins() {
    pin_digitalOut(pin_SData);
    pin_digitalOut(pin_LE);
    pin_digitalOut(pin_CLK);
    pin_digitalOut(pin_OE);
    pin_clear(pin_OE);
    pin_clear(pin_CLK);
    pin_clear(pin_LE);
    pin_clear(pin_SData);
}

void pulseClock(){
    delay();
    pin_set(pin_CLK);
    delay();
    pin_clear(pin_CLK);
}

void pulseLatch(){
    delay();
    pin_set(pin_LE);
    delay();
    pin_clear(pin_LE);
}

void updateLEDs() {
    pin_set(pin_OE);    
    for(count = 0; count < 48; count++) {
        if (allLEDS == LIT && count < 16) {
            // pin_write(pin_SData, LEDStates & (1 << count));
            pin_set(pin_SData);
        } else {
            pin_clear(pin_SData);
        }
        pulseClock();
    }
    pulseLatch();
    delay();
    pin_clear(pin_OE);
}

int16_t main(void) {
    init_clock();
    init_ui();
    init_timer();
    init_pin();
    setup_pins();

    while (1) {
        allLEDS = LIT;
        // writeLEDState(0, LIT);
        updateLEDs();
        longDelay();
        // writeLEDState(0, UNLIT);
        allLEDS = UNLIT;
        updateLEDs();
        longDelay();
    }
}

