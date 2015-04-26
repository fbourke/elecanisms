#include <p24FJ128GB206.h>
#include "common.h"
#include "pin.h"
#include "mole.h"
#include "schedule.h"

Button* buttons[3];
Mole* moles[3];

volatile uint16_t valveStates = 0;

_PIN* moleSRData = &D[3];
_PIN* moleSRClock = &D[4];

void init_moles(void) {

    button_init(&(buttons[0]), 0);
    button_init(&(buttons[1]), 1);
    button_init(&(buttons[2]), 2);

    mole_init(&(moles[0]), buttons[0], 0, 0, 1);
    mole_init(&(moles[1]), buttons[1], 1, 5, 4);
    mole_init(&(moles[2]), buttons[2], 2, 6, 2);

    printf("button %d on array with pin %d\n", buttons[0], buttons[0]->pin);

    init_mole_SR();
}

void mole_init(Mole** mole, Button* button, uint16_t number, uint16_t solenoidIn, int solenoidOut) {
    pin_digitalOut(&D[solenoidIn]);
    pin_digitalOut(&D[solenoidOut]);

    Mole newMole = {
        number,
        solenoidIn,
        solenoidOut,
        button,
        NULL,
        NULL,
        0,
        0,
        OFF
    };
    *mole = &newMole;
}

void button_init(Button** button, int pin)
{
    pin_digitalIn(&D[pin]);
    Button newButton = {
        0,
        0,
        &D[pin]
    };
    *button = &newButton;
    printf("button at location %d get value %d in init with pin %d\n", button, &newButton, newButton.pin);
}

void turn_Off(Mole* mole) {
    mole->direction = OFF;
    writeValveState(mole->solenoidIn, CLOSED);
    writeValveState(mole->solenoidOut, CLOSED);
    printf("off \n");
}

void push_down(Mole* mole) {
    mole->direction = DOWN;
    writeValveState(mole->solenoidIn, OPEN);
    writeValveState(mole->solenoidOut, CLOSED);
    mole->downTime = gameTime;
    mole->downTimePassed = 0;
    printf("down \n");
}

void push_up(Mole* mole) {
    mole->direction = UP;
    writeValveState(mole->solenoidIn, CLOSED);
    writeValveState(mole->solenoidOut, OPEN);
    mole->upTime = gameTime;
    mole->upTimePassed = 0;
    printf("up \n");

}

void mole_delay(){
    int c = 1;
    for ( c = 1 ; c <= 500 ; c++ ){}
}

void mole_longDelay(){
    int c = 1, d = 1;

    for ( c = 1 ; c <= 32000 ; c++ ){ 
        for ( d = 1 ; d <= 150 ; d++ ){}
    }
}

void init_mole_SR() {
    pin_digitalOut(moleSRClock);
    pin_digitalOut(moleSRData);
}

void updateValves() {
    int count;
    for(count = 0; count < 8; count++) {
        pin_write(moleSRData, valveStates & (1 << count));
        mole_pulseClock();
    }
}

void mole_pulseClock(){
    pin_clear(moleSRClock);
    mole_delay();
    pin_set(moleSRClock);
    mole_delay();
    pin_clear(moleSRClock);
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
