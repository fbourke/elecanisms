
#ifndef _LED_H_
#define _LED_H_

typedef enum {
    LIT,
    UNLIT
} LEDState;

extern LEDState allLEDS;

extern _PIN* pin_SData;
extern _PIN* pin_LE;
extern _PIN* pin_CLK;
extern _PIN* pin_OE;

extern uint16_t LEDStates;


void LED_delay();
void LED_longDelay();
void writeLEDState(int LEDNumber, LEDState state);
void init_LED();
void LED_pulseClock();
void pulseLatch();
void updateLEDs();

#endif