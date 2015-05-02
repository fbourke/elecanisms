#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "led.h"

/*
LEDS (left side)
TOP
- 12
- 13
- 14
- 15
- 8
- 9
- 10
- 11
- 4
- 5
- 6
- 7
- 0
- 1
- 2
- 3
BOTTOM
*/

int16_t main(void) {
    init_clock();
    init_ui();
    init_timer();
    init_pin();
    init_LED();

    while (1) {
        // allLEDS = LIT;
        int i;
        for (i = 0; i < 16; i++)
        {
            writeLEDState(SCORE, i, LIT);
            updateLEDs();
            LED_longDelay();
        }
        for (i = 0; i < 16; i++)
        {
            writeLEDState(SCORE, i, UNLIT);
            updateLEDs();
            LED_longDelay();        
        }
        for (i = 0; i < 16; i++)
        {
            writeLEDState(TIME, i, LIT);
            updateLEDs();
            LED_longDelay();
        }
        for (i = 0; i < 16; i++)
        {
            writeLEDState(TIME, i, UNLIT);
            updateLEDs();
            LED_longDelay();        
        }
    }
}
