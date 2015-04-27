
#ifndef _LED_H_
#define _LED_H_

typedef enum {
    LIT,
    UNLIT
} LEDState;

// typedef struct LED {
//     uint16_t number,
//     uint16_t size,
//     uint16_t positions[16],
//     uint16_t states,
// }

typedef enum {
    TIME,
    SCORE,
    PERIPHERAL
} LEDBlock;

extern uint16_t LEDStates[3];

void LED_delay();
void LED_longDelay();
// void writeLEDState(uint16_t LEDNumber, LEDState state);
void writeLEDState(LEDBlock ledBlock, uint16_t LEDNumber, LEDState state);
void init_LED();
void LED_pulseClock();
void pulseLatch();
void updateLEDs();

#endif