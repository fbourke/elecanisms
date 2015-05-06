
#ifndef _MOLE_H_
#define _MOLE_H_

extern volatile uint16_t valveStates;
extern const double WAIT_MAX;

typedef struct Button {
    volatile uint16_t state;
    volatile uint16_t prevState;
    // Button pins: D0, D1, D2, D8, D9
    _PIN* pin;
    uint16_t isSensitive;
    uint16_t LEDNumber;
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

typedef enum {
CUTOFF,
THROUGH
} MoleValveStatus;

typedef struct Mole {
    int number;
    uint16_t solenoidIn;
    uint16_t solenoidOut;
    Button* button;
    double upTime;
    double downTime;
    double upWait;
    double downWait;
    uint16_t upTimePassed;
    uint16_t downTimePassed;
    MoleDirection direction;
    MoleValveStatus valveStatus;
} Mole;

extern Button moleButtons[3];
extern Button modeButtons[2];
extern Mole moles[3];

void init_moles(void);
void mole_init(Mole* mole, Button* button, int number, uint16_t solenoidIn, uint16_t solenoidOut);
void button_init(Button* button, int pin, uint16_t LEDNumber);
void turn_off(Mole* mole);
void close_valves(Mole* mole);
void push_down(Mole* mole);
void push_up(Mole* mole);

void allMolesDown();
void mole_delay();
void mole_longDelay();
void init_mole_SR();
void mole_pulseClock();
void updateValves();
void writeValveState(int valveNumber, ValveState state);

void activate(Button* button);
void deactivate(Button* button);

#endif