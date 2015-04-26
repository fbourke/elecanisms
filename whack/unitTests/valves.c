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
    init_mole();
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
        count = 0;
        writeValveState(0, OPEN);
        for(count = 0; count < 8; count++){
            pin_write(&D[3],valveStates & (1<<count));
            mole_pulseClock();
        }
        mole_longDelay();
        writeValveState(0, CLOSED);
          for(count = 0; count < 8; count++){
            pin_write(&D[3],valveStates & (1<<count));
            mole_pulseClock();
        }
        mole_longDelay();
                writeValveState(1, OPEN);
        for(count = 0; count < 8; count++){
            pin_write(&D[3],valveStates & (1<<count));
            mole_pulseClock();
        }
        mole_longDelay();
        writeValveState(1, CLOSED);
          for(count = 0; count < 8; count++){
            pin_write(&D[3],valveStates & (1<<count));
            mole_pulseClock();
        }
        mole_longDelay();
    }
}