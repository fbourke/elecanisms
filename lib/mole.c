#include <p24FJ128GB206.h>
#include "common.h"
#include "pin.h"
#include "mole.h"
#include "schedule.h"
#include <math.h>

const double WAIT_MAX = 64000.0; 

Button moleButtons[3];
Button modeButtons[2];
Mole moles[3];

volatile uint16_t valveStates = 0;

_PIN* moleSRData = &D[3];
_PIN* moleSRClock = &D[4];

void init_moles(void) {

    button_init(&moleButtons[0], 0);
    button_init(&moleButtons[1], 2);
    button_init(&moleButtons[2], 1);
    button_init(&modeButtons[0], 8);
    button_init(&modeButtons[1], 9);

    mole_init(&moles[0], &moleButtons[0], 0, 0, 1);
    mole_init(&moles[1], &moleButtons[1], 1, 5, 4);
    mole_init(&moles[2], &moleButtons[2], 2, 6, 2);

    init_mole_SR();
}

void mole_init(Mole* mole, Button* button, int number, uint16_t solenoidIn, uint16_t solenoidOut) {
    mole->number = number;
    mole->solenoidIn = solenoidIn;
    mole->solenoidOut = solenoidOut;
    mole->button = button;
    mole->upTime = 0.0;
    mole->downTime = 0.0;
    mole->upWait = (double) WAIT_MAX;
    mole->downWait = (double) WAIT_MAX;
    mole->upTimePassed = 0;
    mole->downTimePassed = 0;
    mole->direction = OFF;
    mole->valveStatus = CUTOFF;
}

void button_init(Button* button, int pin) {
    pin_digitalIn(&D[pin]);

    button->state = 0;
    button->prevState = 0;
    button->pin = &D[pin];
}

void printMole(Mole* mole) {
    printf("in: %d, out: %d, valveStates: ", mole->solenoidIn, mole->solenoidOut);
    int i;
    for (i=7; i>=0; i--) {
        printf("%d", !!(valveStates & (1 << i)));
    }
    printf("\n");
}

void turn_off(Mole* mole) {
    mole->direction = OFF;
    close_valves(mole);
}

void close_valves(Mole* mole) {
    mole->valveStatus = CUTOFF;
    writeValveState(mole->solenoidIn, CLOSED);
    writeValveState(mole->solenoidOut, CLOSED);
    updateValves();
    printf("closing mole %d\n", mole->number);
    // printMole(mole);
}


void setMoleDown(Mole* mole) {
    mole->direction = DOWN;
    mole->valveStatus = THROUGH;
    writeValveState(mole->solenoidIn, CLOSED);
    writeValveState(mole->solenoidOut, OPEN);
    mole->downTime = gameTime;
    mole->downTimePassed = 0;
    mole->downWait = WAIT_MAX;
    printf("lowering mole %d\n", mole->number);
    // printMole(mole);
}

void push_down(Mole* mole) {
    setMoleDown(mole);
    updateValves();
}

void allMolesDown(void) {
    int i;
    Mole* mole;
    for (i=0; i<3; i++) {
        mole = &moles[i];
        setMoleDown(mole);
    }
    updateValves();
}

void push_up(Mole* mole) {
    mole->direction = UP;
    mole->valveStatus = THROUGH;
    writeValveState(mole->solenoidIn, OPEN);
    writeValveState(mole->solenoidOut, CLOSED);
    updateValves();

    mole->upTime = gameTime;
    mole->upTimePassed = 0;
    mole->downWait = (double) WAIT_MAX;
    printf("rasing mole %d\n", mole->number);
    // printMole(mole);
}

void mole_delay(){
    int c = 1;
    for ( c = 1 ; c <= 500 ; c++ ){}
}

void mole_mediumDelay(){
    int c = 1;

    for ( c = 1 ; c <= 32000 ; c++ ){ 
    }
}

void mole_longDelay(){
    int c = 1, d = 1;

    for ( c = 1 ; c <= 32000 ; c++ ){ 
        for ( d = 1 ; d <= 50 ; d++ ){}
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
    mole_delay();
    pin_write(moleSRData, 0);
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
