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
#include <math.h>
#include <stdio.h>

volatile uint16_t gyro;
int16_t score;

double timeInterval = 3;
double gamePeriod = 0.03;  // seconds
double upOpenTime = 1.0;   // seconds
double downOpenTime = 0.3; // seconds
double betweenUpTime = 0.1;
double lastUpTime = 0.0;
const double fullGameTime = 48.0;
int i; 
Mole* mole;

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

uint16_t playingGame() {
    if ((gameState == COIN_NEEDED) || (gameState == MODE_NEEDED)) {
        return 0;
    } else {
        return 1;
    }
}

uint16_t coinInserted;
void waitingForCoin() {
    coinVoltage = pin_read(&A[3]) >> 6;
    coinInserted = (coinVoltage > 100);
    if (coinInserted) {
        // printf("Coin inserted\n");
        flashAllLEDs();
        gameTime = 0;
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

void beginGame() {
    writeLEDState(PERIPHERAL, 0, UNLIT);
    writeLEDState(PERIPHERAL, 1, UNLIT);
    writeLEDBlock(TIME, LEDsLitUpTo(16));
    updateLEDs();
    gameState = NICE;
    score = 0;
    printf("starting game\n");
    for (i=0; i<3; i++) {
        mole = &moles[i];
        mole->downWait = (double) (mole->number);
    }
    lastUpTime = 0.0;
    gameTime = 0.0;
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
    if (gameMode == EASY) {
        mole->downWait = 1.0 + 2.0 * randDouble();
    } else if (gameMode == HARD) {
        mole->downWait = 0.5 + 1.0 * randDouble();
    }
}

void scheduleDown(Mole* mole) {
    if (gameMode == EASY) {
        mole->upWait = WAIT_MAX; // + 1.0 * randDouble();
    } else if (gameMode == HARD) {
        mole->upWait = 0.1 + 0.2 * randDouble();
    }
}

void reset_game() {
    gameTime = 0.0;
    gameState = COIN_NEEDED;
    allMolesDown();
    mole_longDelay();
    for (i=0; i<3; i++) {
        mole = &moles[i];
        close_valves(mole);
    }
}

uint16_t scoreNumber;
void incrementScore(int increment) {
    score += increment;
    scoreNumber = score / 3;
    writeLEDBlock(SCORE, LEDsLitUpTo(scoreNumber));
    printf("%d\n", score);
    if (scoreNumber >= 16) {
        printf("YOU WIN!\n");
        reset_game();
    } else if (score <= 0) {
        score = 0;
    }
    if (gameState == NICE && scoreNumber >= 10) {
        gameState = EVIL;
        printf("Going to hardmode.\n");
    }
}

void moleHit(Mole* mole) {
    incrementScore(1);
    push_down(mole);
    scheduleUp(mole);
}

void moleMissed(Mole* mole) {
    incrementScore(-1);
    push_down(mole);
    scheduleUp(mole);
}

void molePop(Mole* mole) {
    push_up(mole);
    scheduleDown(mole);
    lastUpTime = gameTime;
}

void switch_state() {
    for (i=0; i<3; i++) {
        mole = &moles[i];
        if (mole->direction == UP) {
            if (mole->upTimePassed) {
                moleMissed(mole);
            } else if (mole->valveStatus == THROUGH
                && (gameTime - mole->upTime) > upOpenTime) {
                close_valves(mole);
            }
        } else if (mole->direction == DOWN) {
            printf("%f - %f > %f\n", gameTime, lastUpTime, betweenUpTime);
            if (mole->downTimePassed
                && (gameTime - lastUpTime) > betweenUpTime) {
                    molePop(mole);
            } else if (mole->valveStatus == THROUGH
                && (gameTime - mole->downTime) > downOpenTime) {
                close_valves(mole);
            }
        }
    }
}

uint16_t hammerSwung() {
    return ((pin_read(&A[5]) >> 6) > 512);
}

void senseButtons() {
    for (i=0; i<3; i++) {
        mole = &moles[i];
        if (mole->direction == UP) {
            if (button_hit(mole->button)) {
                moleHit(mole);
            }
        }
    }

    if ((gameState == EVIL) && (hammerSwung())) {
        allMolesDown();
        for (i = 0; i < 3; ++i) {
            mole = &moles[i];
            scheduleUp(mole);
            if (gameMode == HARD) {
                incrementScore(-1);
            }
        }
    }
}

void setup_pins() {
    for (i = 0; i<6; i++) {
        pin_analogIn(&A[i]);
    }
}

int16_t peripheralCycleNumber;
void cyclePeripherals() {
    peripheralCycleNumber = ((int16_t) floor(gameTime / 0.15)) % 18 - 8;
    writeLEDBlock(PERIPHERAL, LEDsLitBetween(peripheralCycleNumber, 
                                             peripheralCycleNumber + 9));
}

double timeRemaining;
uint16_t lightNumber;
void updateTimes() {
    gameTime = gameTime + gamePeriod;

    if (playingGame()) {
        timeRemaining = fullGameTime - gameTime;
        lightNumber = (uint16_t) ceil((timeRemaining / fullGameTime) * 16.0);
        writeLEDBlock(TIME, LEDsLitUpTo(lightNumber));
        if (gameTime >= (timeInterval*16)) {
            printf("YOU LOSE!\n");
            reset_game();
        }
 
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
                updateTimes();
                cyclePeripherals();
                updateLEDs();
            }
        } else if (gameState == MODE_NEEDED) {
            if (timer_flag(&timer1)) {
                timer_lower(&timer1);
                updateTimes();
                waitingForMode();
                updateLEDs();
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
                updateLEDs();
            }
            if (timer_flag(&timer2)) {
                timer_lower(&timer2);
            }
        }
    }
}
