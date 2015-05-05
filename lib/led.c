#include <p24FJ128GB206.h>
#include "common.h"
#include "pin.h"
#include "led.h"
#include <math.h>

LEDState allLEDS = LIT;

_PIN* pin_SData = &D[5];
_PIN* pin_LE = &A[4];
_PIN* pin_CLK = &D[6];
_PIN* pin_OE = &D[7];

uint16_t LEDStates[3];

uint16_t LEDMap[3][16] = {
    {3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12},
    {3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12},
/*    {4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},*/
    {3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12},};

void LED_delay() {
    int c = 1;
    for ( c = 1 ; c <= 300 ; c++ ){
    
    }
}

void LED_longDelay(){
   int c = 1, d = 1;
   for ( c = 1 ; c <= 32000 ; c++ ) { 
        for ( d = 1 ; d <= 30 ; d++ ) {}
    }
}

// void writeLEDState(uint16_t LEDNumber, LEDState state) {
//     uint16_t bitMask;
//     uint16_t bufferNumber = floor(LEDNumber / 16);
//     uint16_t bufferPosition = LEDNumber % 16;

//     writeLEDState(bufferNumber, bufferPosition, state);
// }

void writeLEDState(LEDBlock ledBlock, uint16_t LEDNumber, LEDState state) {
    uint16_t* buffer = &LEDStates[ledBlock];
    uint16_t LEDPostion = LEDMap[ledBlock][LEDNumber];
    uint16_t bitMask;
    if (state == LIT) {
        bitMask = 1<<LEDPostion;
        *buffer = *buffer | bitMask;
    } if (state == UNLIT) {
        bitMask = ~(1<<LEDPostion); 
        *buffer = *buffer & bitMask;
    }
}

void writeLEDBlock(LEDBlock ledBlock, uint16_t states) {
    int i;
    for (i = 0; i < 16; ++i) {
        uint16_t bitMask = 1 << i;
        if (states & bitMask) {
            writeLEDState(ledBlock, i, LIT);
        } else {
            writeLEDState(ledBlock, i, UNLIT);
        }
    }
}

uint16_t LEDsLitUpTo(uint16_t lightNumber) {
    return (1 << lightNumber) - 1;
}

uint16_t LEDsLitBetween(uint16_t minimumInclusive, uint16_t maximumExclusive) {
    return LEDsLitUpTo(minimumInclusive) ^ LEDsLitUpTo(maximumExclusive);
}

void resetLEDs() {
    int i;
    for (i = 0; i < 3; ++i)
    {
        LEDStates[i] = 0;
    }
    updateLEDs();
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

void printLEDs() {
    int i, j;
    for (j=0; j<3; j++) {
        for (i=15; i>=0; i--) {
            printf("%d", !!(LEDStates[0] & (1 << i)));
        }
        printf("|");
    }
    printf("\n");
}

void updateLEDs() {
    // printLEDs();
    int count = 0;
    for(count = 47; count >= 0; count--) {
        if (count < 16) {
            pin_write(pin_SData, LEDStates[0] & (1 << (count % 16)));
        } else if (count < 32) {
            pin_write(pin_SData, LEDStates[1] & (1 << (count % 16)));
        } else {
            pin_write(pin_SData, LEDStates[2] & (1 << (count % 16)));            
        }
        LED_pulseClock();
    } 
    pulseLatch();
    LED_delay();
    pin_clear(pin_OE);
}

void fullTime() {
    int i;
    for (i = 0; i < 16; i++)
    {
        writeLEDState(TIME, i, LIT);
    }
        updateLEDs();
}