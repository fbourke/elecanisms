
#ifndef _MOLE_H_
#define _MOLE_H_

extern volatile uint16_t valveStates;

typedef struct Button
{
    volatile uint16_t state;
    volatile uint16_t prevState;
    // Button pins: D0, D1, D2, D8, D9
    _PIN* pin;
} Button;

typedef enum {
OPEN,
CLOSED
} ValveState;

typedef enum {
UP,
DOWN,
OFF
} MoleDirection;

typedef struct Mole {
    int number;
    uint16_t solenoidIn;
    uint16_t solenoidOut;
    Button* button;
    double upTime;
    double downTime;
    uint16_t upTimePassed;
    uint16_t downTimePassed;
    MoleDirection direction;
} Mole;

extern Button* buttons[3];
extern Mole* moles[3];

void init_moles(void);
void turn_Off(Mole* mole);
void push_down(Mole* mole);
void push_up(Mole* mole);

void mole_delay();
void mole_longDelay();
void init_mole_SR();
void mole_pulseClock();
void writeValveState(int valveNumber, ValveState state);

#endif