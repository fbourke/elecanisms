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
        writeLEDState(15, LIT);
        updateLEDs();
        mole_longDelay();
        writeLEDState(15, UNLIT);
        // allLEDS = UNLIT;
        updateLEDs();
        mole_longDelay();
    }
}
