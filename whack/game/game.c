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
#include <stdlib.h>

volatile uint16_t gyro;
int16_t score;

double gamePeriod = 0.01;  // seconds
double upOpenTime = 1.0;   // seconds
double downOpenTime = 0.2; //seconds

typedef enum {
NICE,
EVIL,
COIN_NEEDED
} GameState;

volatile GameState gameState = COIN_NEEDED; //CHANGE ME
uint16_t coinVoltage;
uint16_t lastCoinVoltage;

void waiting() {
    printf("%d\n", coinVoltage);
    uint16_t coinInserted = (coinVoltage - lastCoinVoltage) > 1;
    if (coinInserted) {
        gameState = NICE;
        int i;
        for (i=0; i<3; i++) {
           push_up(&moles[i]);
        }
        gameTime = 0.0;
    }
}

uint16_t button_hit(Button* button) {
    _PIN* pin = button->pin;
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
    return 0;
}

double randDouble() {
    return ((double) (rand() % 256)) / 256.0;
}

void scheduleUp(Mole* mole) {
    mole->downWait = 2.0;
}

void scheduleDown(Mole* mole) {
    mole->upWait = 3.0;// + 1.0 * randDouble();
}

void reset_game() {
    gameState = COIN_NEEDED;
    score = 0;
    int i;
    for (i=0; i<3; i++) {
        Mole* mole = &moles[i];
        push_down(mole);
        // scheduleUp(mole);
    }
    mole_longDelay();
    for (i=0; i<3; i++) {
        turn_off(mole);
        mole->downWait = (double) mole->number;
    }
}

void updateScore() {
    printf("%d\n", score);
    if (score >= 5) {
        printf("YOU WIN!\n");
        reset_game();
    } else if (score <= -2) {
        printf("YOU LOSE\n");
        reset_game();
    }
    if (gameState == NICE && score >= 3) {
        gameState = EVIL;
        printf("Going to hardmode.\n");
    }
}

void moleHit(Mole* mole) {
    score += 1;
    updateScore();
    push_down(mole);
    scheduleUp(mole);
}

void moleMissed(Mole* mole) {
    score -= 1;
    updateScore();
    push_down(mole);
    scheduleUp(mole);
}

void switch_state() {
    int i;
    for (i=0; i<3; i++) {
        Mole* mole = &moles[i];
        if (mole->direction == UP) {
            if (mole->upTimePassed) {
                moleMissed(mole);
            } else if (mole->valveStatus == THROUGH
                && (gameTime - mole->upTime) > upOpenTime) {
                // printf("up: %f - %f > %f\n", gameTime, mole->upTime, upOpenTime);
                close_valves(mole);
            }
        } else if (mole->direction == DOWN) {
            if (mole->downTimePassed) {
                push_up(mole);
                scheduleDown(mole);
            } else if (mole->valveStatus == THROUGH
                && (gameTime - mole->downTime) > downOpenTime) {
                // printf("down: %f - %f > %f\n", gameTime, mole->downTime, downOpenTime);
                close_valves(mole);
            }
        }
    }
}

uint16_t hammerSwung() {
    return pin_read(&D[13]);
}

void senseButtons() {
    int i;
    for (i=0; i<3; i++) {
        Mole* mole = &moles[i];
        if (mole->direction == UP) {
            if (button_hit(mole->button)) {
                moleHit(mole);
            } else if (gameState == EVIL && hammerSwung()) {
                moleMissed(mole);
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
        mole = &moles[i];
        gameTime = gameTime + gamePeriod;
        // printf("game time: %f\n", gameTime);
        if (mole->direction == DOWN) {
            if ((gameTime - mole->downTime) > mole->downWait) {
                mole->downTimePassed = 1;
            }
        } else if (mole->direction == UP) {
            if ((gameTime - mole->upTime) > mole->upWait) {
                mole->upTimePassed = 1;
            }
        }
    }
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

// Mole Scheduling Loop
    timer_setPeriod(&timer1, gamePeriod);
    timer_start(&timer1);

// Mole Switching Loop
    timer_setPeriod(&timer2, 0.01);
    timer_start(&timer2);

// Button Reading Loop
    timer_setPeriod(&timer3, 0.001);
    timer_start(&timer3);

    reset_game();

    while (1) {
        if (gameState == COIN_NEEDED) {
            if (timer_flag(&timer1)) {
                timer_lower(&timer1);
                waiting();
            }
        } else {
            if (timer_flag(&timer3)) {
                timer_lower(&timer3);
                senseButtons();
            }
            if (timer_flag(&timer1)) {
                timer_lower(&timer1);
                updateTimes();
                switch_state();
            }
            if (timer_flag(&timer2)) {
                timer_lower(&timer2);
            }
        }
    }
}
