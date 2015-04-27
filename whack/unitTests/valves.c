#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "uart.h"
#include "mole.h"


int16_t main(void) {
    init_clock();
    init_ui();
    init_timer();
    init_uart();
    init_pin();
    init_moles();
    int count = 0;
    // Data = D3
    // Clock = D4
  
    // 0 Lefty in
    // 1 Lefty out
    // 2 Righty out
    // 3 notWired
    // 4 Orange out
    // 5 Orange in
    // 6 Righty in
    // 7 unwired 
    while (1) {
        int i;
        Mole* mole;
        for (i=0; i<3; i++) {
            mole = &moles[i];
            push_up(mole);
            // writeValveState(mole->solenoidIn, OPEN);
            // writeValveState(mole->solenoidOut, CLOSED);
            // updateValves();
            mole_longDelay();
            // writeValveState(mole->solenoidIn, CLOSED);
            // writeValveState(mole->solenoidOut, CLOSED);
            // updateValves();
            turn_Off(mole);
            mole_longDelay();
        }
        for (i=0; i<3; i++) {
            mole = &moles[i];
            // writeValveState(mole->solenoidIn, CLOSED);
            // writeValveState(mole->solenoidOut, OPEN);
            // updateValves();
            push_down(mole);
            mole_longDelay();
            // writeValveState(mole->solenoidIn, CLOSED);
            // writeValveState(mole->solenoidOut, CLOSED);
            // updateValves();
            turn_Off(mole);
            mole_longDelay();
        }
    }
}