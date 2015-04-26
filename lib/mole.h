
#ifndef _MOLE_H_
#define _MOLE_H_


typedef enum {
    OPEN,
    CLOSED
} ValveState;


extern uint16_t valveStates;

void mole_delay();
void mole_longDelay();
void init_mole();
void mole_pulseClock();
void writeValveState(int valveNumber, ValveState state);

#endif