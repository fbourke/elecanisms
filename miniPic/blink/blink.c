#include <p24FJ64GB002.h>
#include "config.h"
#include "common.h"
#include "pin.h"
#include "timer.h"
#include "spi.h"
#include "xl.h"

_PIN *led1, *led2, *sw1;

int16_t main(void) {
    init_clock();
    init_pin();
    init_timer();
    init_spi();
    init_xl();

    sw1 = &ra3;
    led1 = &rb15;
    led2 = &rb14;
    pin_digitalOut(led1);
    pin_digitalOut(led2);

    pin_set(led1);
    timer_setPeriod(&timer1, 0.5);
    timer_start(&timer1);

    while (1) {
        if (timer_flag(&timer1)) {
            timer_lower(&timer1);
            pin_toggle(led1);
            pin_toggle(led2);
        }
//        pin_write(led2, !pin_read(sw1));
    }
}

