#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "oc.h"
#include "uart.h"
#include "mole.h"
#include "schedule.h"

volatile uint16_t gyro;
int16_t score;

double gamePeriod = 0.01;

typedef enum {
EASY,
HARD,
COIN_NEEDED
} GameState;

volatile GameState gameState = COIN_NEEDED;

void waiting() {
    uint16_t coinVoltage = pin_read(&A[5]) >> 6;
    // printf("%d\n", coinVoltage);
    uint16_t coinInserted = coinVoltage > 159;
    if (coinInserted) {
        gameState = EASY;
        push_up(moles[0]);
        gameTime = 0.0;
    }
}

uint16_t button_hit(Button* button) {
    printf("button: %d\n", &button);
    printf("getting button state\n");
    _PIN* pin = button->pin;
    printf("got pin %d\n", button->pin);
    int c;
    for (c=0; c<32000; c++) {}
    uint16_t read = !pin_read(pin);
    button->state = read;
    if (!button->prevState && button->state) {
        button->prevState = 1;
        return 1;
    }
    else if (button->prevState && !button->state) {
        button->prevState = 0;
        led_off(&led2);
    }
    printf("herpderp\n");
    for (c=0; c<32000; c++) {}
    return 0;
}

void reset_game() {
    gameState = COIN_NEEDED;
    score = 0;
    int i;
    for (i=0; i<3; i++) {
        turn_Off(moles[i]);
    }
}

void switch_state() {
    gyro = pin_read(&D[13]);
    
    if (gyro){
        led_on(&led1);
    }
    else {
        led_off(&led1);
    }

    int i;
    for (i=0; i<3; i++) {
        Mole* mole = moles[i];
        if (button_hit(buttons[i])) {
            printf("button %d is hit\n", i);
        } else {
            printf("button %d is not hit\n", i);
        }
        if (mole->direction == UP) {
            if (button_hit(mole->button)) {
                score += 1;
                printf("%d\n", score);
                if (score >= 5) {
                    printf("YOU WIN!\n");
                    reset_game();
                }
                if (gameState == EASY && score >= 3) {
                    gameState = HARD;
                    printf("Going to hardmode.\n");
                }
                push_down(mole);
            } else if (((gameState == HARD) && gyro) || ((gameState == EASY) && mole->upTimePassed)) {
                score -= 1;
                printf("%d\n", score);
                if (score <= -2) {
                    printf("YOU LOSE\n");
                    reset_game();
                }
                push_down(mole);
            }
        }
        if (mole->direction == OFF) {
            if (mole->downTimePassed){
                push_up(mole);
            }
        }
        if (mole->direction == DOWN) {
            if (mole->downTimePassed) {
                push_up(mole);
            }
            if ((gameTime - mole->downTime) > 0.5) {
                turn_Off(mole);
            }
        }
    }
}

void setup_pins() {
    int i;
    for (i = 0; i<6; i++) {
        pin_analogIn(&A[i]);
    }
}


void updateTimes() {
    int i;
    Mole* mole;
    for (i=0; i<3; i++) {
        printf("update %d\n", i);
        mole = moles[i];
        gameTime = gameTime + gamePeriod;
        if ((gameTime - mole->downTime) > 3.0) {
            mole->downTimePassed = 1;
        }
        if ((gameTime - mole->upTime) > 3.0) {
            mole->upTimePassed = 1;
        }
    }
    printf("done updating\n");
}

int16_t main(void) {
    init_pin();
    init_clock();
    init_ui();
    init_uart();
    init_timer();
    init_oc();
    setup_pins();
    init_moles();

    printf("begin\n");
    printf("D: %d\n", &D[0]);

// Main Loop
    timer_setPeriod(&timer1, gamePeriod);
    timer_start(&timer1);

// Reading Loop
    timer_setPeriod(&timer2, 0.01);
    timer_start(&timer2);

    while (1) {
        // if (gameState == COIN_NEEDED) {
        //     if (timer_flag(&timer1)) {
        //         timer_lower(&timer1);
        //         waiting();
        //     }
        // } else {
            if (timer_flag(&timer1)) {
                timer_lower(&timer1);
                updateTimes();
            }
            if (timer_flag(&timer2)) {
                timer_lower(&timer2);
                switch_state();
            }
        // }
    }
}
