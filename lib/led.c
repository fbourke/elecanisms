#include <p24FJ128GB206.h>
#include "common.h"
#include "pin.h"
#include "led.h"
#include <math.h>
#include <stdio.h>

LEDState allLEDS = LIT;
int c, d, e;

_PIN* pin_SData = &D[5];
_PIN* pin_LE = &A[4];
_PIN* pin_CLK = &D[6];
_PIN* pin_OE = &D[7];
volatile uint16_t dummy;

uint16_t LEDStates[3];
uint16_t LEDMap[3][16] = {
    {3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12},
    {3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12},
    {4, 5, 8, 9, 10, 11, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0}
};

void LED_delay() {
    for ( c = 1 ; c <= 300 ; c++ ){
    
    }
}

void peripheralFlash() {
    for (c=2; c<=5; c++) {
        writeLEDState(PERIPHERAL, c, LIT);
    }
    updateLEDs();
}

void peripheralUnflash() {
    for (c=2; c<=5; c++) {
        writeLEDState(PERIPHERAL, c, UNLIT);
    }
    updateLEDs();
}

void LED_5Hz_Delay(){
    for ( c = 1 ; c <= 10000 ; c++ ) { 
        for ( d = 1 ; d <= 4 ; d++ ) {
            dummy++;
        }
    }
}

uint16_t LEDBlockIndex(LEDBlock block) {
    if (block == TIME) {
        return 0;
    } else if (block == SCORE) {
        return 1;
    } else if (block == PERIPHERAL) {
        return 2;
    }
}

uint16_t blockIndex;
uint16_t* stateBuffer;
uint16_t LEDPostion;
uint16_t LEDbitMask;
void writeLEDState(LEDBlock ledBlock, uint16_t LEDNumber, LEDState state) {
    blockIndex = LEDBlockIndex(ledBlock);
    stateBuffer = &LEDStates[blockIndex];
    LEDPostion = LEDMap[blockIndex][LEDNumber];
    LEDbitMask;
    if (state == LIT) {
        LEDbitMask = 1<<LEDPostion;
        *stateBuffer = *stateBuffer | LEDbitMask;
    } if (state == UNLIT) {
        LEDbitMask = ~(1<<LEDPostion); 
        *stateBuffer = *stateBuffer & LEDbitMask;
    }
}


void writeLEDBlock(LEDBlock ledBlock, uint16_t states) {
    for (c = 0; c < 16; ++c) {
        LEDbitMask = 1 << c;
        if (states & LEDbitMask) {
            writeLEDState(ledBlock, c, LIT);
        } else {
            writeLEDState(ledBlock, c, UNLIT);
        }
    }
}

uint16_t LEDsLitUpTo(uint16_t lightNumber) {
    return (1 << lightNumber) - 1;
}

uint16_t LEDsLitBetween(int16_t minimumInclusive, int16_t maximumExclusive) {
    if (minimumInclusive < 0) {minimumInclusive = 0;}
    if (minimumInclusive > 15) {minimumInclusive = 15;}
    if (maximumExclusive < 1) {maximumExclusive = 1;}
    if (maximumExclusive > 16) {maximumExclusive = 16;}
    return LEDsLitUpTo(minimumInclusive) ^ LEDsLitUpTo(maximumExclusive);
}

uint16_t AllLEDsOn() {
    return -1;
}

uint16_t AllLEDsOff() {
    return 0;
}

void resetLEDs() {
    for (d = 0; d < 3; ++d)
    {
        LEDStates[d] = AllLEDsOff();
    }
    updateLEDs();
}

void lightAllLEDs(uint16_t time, uint16_t score) {
    writeLEDBlock(TIME, LEDsLitUpTo(time));
    writeLEDBlock(SCORE, LEDsLitUpTo(score));
    writeLEDBlock(PERIPHERAL, AllLEDsOn());
    updateLEDs();    
}

void flashAllLEDs(uint16_t time, uint16_t score, uint16_t flashNumber, uint16_t keepOn) {
    for (e = 0; e < flashNumber; e++) {
        lightAllLEDs(time, score);
        LED_5Hz_Delay();
        resetLEDs();
        LED_5Hz_Delay();
    }
    if (keepOn) {
        lightAllLEDs(time, score);
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

void printLEDs() {
    for (d=0; d<3; d++) {
        for (c=15; c>=0; c--) {
            printf("%d", !!(LEDStates[0] & (1 << c)));
        }
        printf("|");
    }
    printf("\n");
}

int LEDSwitchCount;
void updateLEDs() {
    // printLEDs();
    LEDSwitchCount = 0;
    for(LEDSwitchCount = 47; LEDSwitchCount >= 0; LEDSwitchCount--) {
        if (LEDSwitchCount < 16) {
            pin_write(pin_SData, LEDStates[0] & (1 << (LEDSwitchCount % 16)));
        } else if (LEDSwitchCount < 32) {
            pin_write(pin_SData, LEDStates[1] & (1 << (LEDSwitchCount % 16)));
        } else {
            pin_write(pin_SData, LEDStates[2] & (1 << (LEDSwitchCount % 16)));            
        }
        LED_pulseClock();
    } 
    pulseLatch();
    LED_delay();
    pin_clear(pin_OE);
}

void fullTime() {
    for (c = 0; c < 16; c++)
    {
        writeLEDState(TIME, c, LIT);
    }
    updateLEDs();
}
