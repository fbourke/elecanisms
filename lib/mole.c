#include <p24FJ128GB206.h>
#include "common.h"
#include "pin.h"
#include "mole.h"

uint16_t valveStates = 0;

void mole_delay(){
   int c = 1;

   for ( c = 1 ; c <= 500 ; c++ ){ } /*5 kHz*/

}

void mole_longDelay(){
   int c = 1, d = 1;

   for ( c = 1 ; c <= 32000 ; c++ ){ 
        for ( d = 1 ; d <= 150 ; d++ ){}
    }
}

void init_mole() {
    pin_digitalOut(&D[3]);
    pin_digitalOut(&D[4]);

}

void mole_pulseClock(){
    pin_clear(&D[4]);
    mole_delay();
    pin_set(&D[4]);
    mole_delay();
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
