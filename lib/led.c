#include <p24FJ128GB206.h>
#include "common.h"
#include "pin.h"
#include "led.h"

LEDState allLEDS = LIT;

_PIN* pin_SData = &D[5];
_PIN* pin_LE = &A[4];
_PIN* pin_CLK = &D[6];
_PIN* pin_OE = &D[7];

uint16_t LEDStates = 0;


void LED_delay(){
    int c = 1;
    for ( c = 1 ; c <= 300 ; c++ ){
    
    } 

}

void LED_longDelay(){
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

void init_LED() {
    pin_digitalOut(pin_SData);
    pin_digitalOut(pin_LE);
    pin_digitalOut(pin_CLK);
    pin_digitalOut(pin_OE);
    pin_clear(pin_OE);
    pin_clear(pin_CLK);
    pin_clear(pin_LE);
    pin_clear(pin_SData);
}

void LED_pulseClock(){
    LED_delay();
    pin_set(pin_CLK);
    LED_delay();
    pin_clear(pin_CLK);
}

void pulseLatch(){
    LED_delay();
    pin_set(pin_LE);
    LED_delay();
    pin_clear(pin_LE);
}

void updateLEDs() {
    int count = 0;
    pin_set(pin_OE);    
    for(count = 47; count >= 0; count--) {
        if (count < 16) {
            pin_write(pin_SData, LEDStates & (1 << count));
            // pin_set(pin_SData);
        } else {
            pin_clear(pin_SData);
        }
        LED_pulseClock();
    } 
    pulseLatch();
    LED_delay();
    pin_clear(pin_OE);
}
