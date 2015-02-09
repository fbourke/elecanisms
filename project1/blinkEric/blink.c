#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"

#define RED 0
#define GREEN 1
#define BLUE 2

void setCorrectLED(int, int);
void allLEDsOff();

int16_t main(void) {
    init_clock();
    init_ui();
    init_timer();

    uint8_t color = 0;
    uint16_t counter = 0;
    uint16_t chunkcounter = 0;
    uint8_t upcount = 1;  // Outside of 1-5 bounds is uninteresting
    uint8_t chunkcount = 2;

    int lastSw2 = 1;
    int lastSw3 = 1;

    timer_setFreq(&timer1, 100);
    timer_start(&timer1);

    while (1) {
        setCorrectLED(chunkcounter < upcount, color);
        
        if (timer_flag(&timer1)) {
            timer_lower(&timer1);
            if (counter % chunkcount == 0) {
                chunkcounter = incrementAndWrap(chunkcounter, 6);
                if (!chunkcounter) color = incrementAndWrap(color, 3);
            }
            counter++;
        }

        int sw2Val = sw_read(&sw2);
        if (sw2Val && !lastSw2) {
            upcount = incrementAndWrap(upcount, 6);
            allLEDsOff();
            color = 0;
            counter = 0;
            chunkcounter = 0;
        }
        lastSw2 = sw2Val;

        int sw3Val = sw_read(&sw3);
        if (sw3Val && !lastSw3) {
            chunkcount *= 2;
            if (chunkcount > 30) chunkcount = 1;
        }
        lastSw3 = sw3Val;
    }
}

void setCorrectLED(int turnOn, int color) {
    if (turnOn) {
        if (color == RED)   led_on(&led1);
        if (color == GREEN) led_on(&led2);
        if (color == BLUE)  led_on(&led3);
    }
    else {
        if (color == RED)   led_off(&led1);
        if (color == GREEN) led_off(&led2);
        if (color == BLUE)  led_off(&led3);
        color++;
    }
    if (color == 2) color = 0;
}

int incrementAndWrap(int value, int limit) {
    if (value + 1 >= limit) return 0;
    else return value + 1;
}

void allLEDsOff() {
    led_off(&led1);
    led_off(&led2);
    led_off(&led3);
}