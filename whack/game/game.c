#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "oc.h"
#include "uart.h"
#include "mole.h"
#include "led.h"
#include "schedule.h"
#include <stdlib.h>

volatile uint16_t gyro;
int16_t score;

double gamePeriod = 0.01;  // seconds
double upOpenTime = 1.0;   // seconds
double downOpenTime = 0.3; // seconds

typedef enum {
NICE,
EVIL,
COIN_NEEDED,
MODE_NEEDED
} GameState;

typedef enum {
EASY,
HARD
} GameMode;

volatile GameState gameState = COIN_NEEDED;
GameMode gameMode = EASY;

uint16_t coinVoltage;

void waitingForCoin() {
    int i;
    coinVoltage = pin_read(&A[3]) >> 6;
    uint16_t coinInserted = (coinVoltage > 100);
    if (coinInserted) {
        printf("Coin inserted\n");
        gameState = MODE_NEEDED;
    }
}

uint16_t button_hit(Button* button) {
    button->state = !pin_read(button->pin);
    if (!button->prevState && button->state) {
        button->prevState = 1;
        return 1;
    } else if (button->prevState && !button->state) {
        button->prevState = 0;
    }
    return 0;
}

void waitingForMode() {
    if (button_hit(&modeButtons[0])) {
        gameMode = EASY;
        beginGame(); return;
    } else if (button_hit(&modeButtons[1])) {
        gameMode = HARD;
        beginGame(); return;
    }
    if ((gameTime - (int) gameTime) < 0.5) {
        writeLEDState(PERIPHERAL, 0, LIT);
        writeLEDState(PERIPHERAL, 1, UNLIT);
        updateLEDs();
    } else {
        writeLEDState(PERIPHERAL, 0, UNLIT);
        writeLEDState(PERIPHERAL, 1, LIT);
        updateLEDs();
    }
}

double randDouble() {
    return ((double) (rand() % 256)) / 256.0;
}

void scheduleUp(Mole* mole) {
    mole->downWait = 2.0;
}

void scheduleDown(Mole* mole) {
    if (gameMode == EASY) {
        mole->upWait = WAIT_MAX; // + 1.0 * randDouble();
    } else if (gameMode == HARD) {
        mole->upWait = 1.0 + 3.0 * randDouble();
    }
}

void reset_game() {
    resetLEDs();
    gameTime = 0.0;
    gameState = COIN_NEEDED;
    allMolesDown();
    mole_longDelay();
    int i;
    for (i=0; i<3; i++) {
        Mole* mole = &moles[i];
        close_valves(mole);
    }
}

void beginGame() {
    writeLEDState(PERIPHERAL, 0, UNLIT);
    writeLEDState(PERIPHERAL, 1, UNLIT);
    updateLEDs();
    gameState = NICE;
    score = 0;
    printf("starting game\n");
    int i; Mole* mole;
    for (i=0; i<3; i++) {
        mole = &moles[i];
        mole->downWait = (double) (mole->number);
    }
    gameTime = 0.0;
}

void updateScore(int increment) {
    score += increment;
    if (increment > 0 && score >= 0) {
        writeLEDState(TIME, score - 1, LIT);
        updateLEDs();
    }
    if (increment < 0 && score >= 0) {
        writeLEDState(TIME, score, UNLIT);
        updateLEDs();
    }
    printf("%d\n", score);
    if (score >= 16) {
        printf("YOU WIN!\n");
        reset_game();
    } else if (score <= 0) {
        score = 0;
    }
    if (gameState == NICE && score >= 3) {
        gameState = EVIL;
        printf("Going to hardmode.\n");
    }
}

void moleHit(Mole* mole) {
    updateScore(1);
    push_down(mole);
    scheduleUp(mole);
}

void moleMissed(Mole* mole) {
    updateScore(-1);
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
    return ((pin_read(&A[5]) >> 6) > 512);
}

void senseButtons() {
    int i;
    for (i=0; i<3; i++) {
        Mole* mole = &moles[i];
        if (mole->direction == UP) {
            if (button_hit(mole->button)) {
                moleHit(mole);
            }
        }
    }
    if ((gameState == EVIL) && (hammerSwung())) {
        allMolesDown();
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
    gameTime = gameTime + gamePeriod;
    for (i=0; i<3; i++) {
        mole = &moles[i];
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
    init_LED();

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
                waitingForCoin();
            }
        } else if (gameState == MODE_NEEDED) {
            if (timer_flag(&timer1)) {
                timer_lower(&timer1);
                updateTimes();
                waitingForMode();
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
                updateValves();
            }
            if (timer_flag(&timer2)) {
                timer_lower(&timer2);
            }
        }
    }
}
