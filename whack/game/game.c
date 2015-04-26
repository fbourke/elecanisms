#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "oc.h"
#include "uart.h"

volatile uint16_t gyro;
int16_t score;

double gamePeriod = 0.01;
double gameTime;

volatile uint16_t valveStates = 0;

typedef enum {
UP,
DOWN,
OFF
} MoleDirection;

typedef struct Button
{
    volatile uint16_t state;
    volatile uint16_t prevState;
    // Button pins: D0, D1, D2, D8, D9
    _PIN* pin;
} Button;

void button_init(Button* button, int pin)
{
    pin_digitalIn(&D[pin]);
    button->state = 0;
    button->prevState = 0;
    button->pin = &D[pin];
}

typedef struct Mole {
    int number;
    _PIN* solenoidIn;
    _PIN* solenoidOut;
    Button* button;
    double upTime;
    double downTime;
    uint16_t upTimePassed;
    uint16_t downTimePassed;
    MoleDirection direction;
} Mole;

void mole_init(Mole* mole, Button* button, int number, int solenoidIn, int solenoidOut) {
    pin_digitalOut(&D[solenoidIn]);
    pin_digitalOut(&D[solenoidOut]);

    mole->number = number;
    mole->solenoidIn = &D[solenoidIn];
    mole->solenoidOut = &D[solenoidOut];
    mole->button = button;
    mole->upTime = NULL;
    mole->downTime = NULL;
    mole->upTimePassed = 0;
    mole->downTimePassed = 0;
    mole->direction = OFF;
}

Button* buttons[3];
Mole* moles[3];

void init_moles(void) {
    button_init(buttons[0], 0);
    button_init(buttons[1], 1);
    button_init(buttons[2], 2);

    mole_init(moles[0], buttons[0], 0, 0, 1);
    mole_init(moles[1], buttons[1], 1, 5, 4);
    mole_init(moles[2], buttons[2], 2, 6, 2);
}

typedef enum {
EASY,
HARD,
COIN_NEEDED
} GameState;

volatile GameState gameState = COIN_NEEDED;

void turn_Off(Mole* mole) {
    mole->direction = OFF;
    pin_clear(mole->solenoidIn);
    pin_clear(mole->solenoidOut);
    printf("off \n");
}

void push_down(Mole* mole) {
    mole->direction = DOWN;
    pin_set(mole->solenoidIn);
    pin_clear(mole->solenoidOut);
    mole->downTime = gameTime;
    mole->downTimePassed = 0;
    printf("down \n");

}

void push_up(Mole* mole) {
    mole->direction = UP;
    pin_clear(mole->solenoidIn);
    pin_set(mole->solenoidOut);
    mole->upTime = gameTime;
    mole->upTimePassed = 0;
    printf("up \n");

}

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
    button->state = !pin_read(button->pin);
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
        mole = moles[i];
        gameTime = gameTime + gamePeriod;
        if ((gameTime - mole->downTime) > 3.0) {
            mole->downTimePassed = 1;
        }
        if ((gameTime - mole->upTime) > 3.0) {
            mole->upTimePassed = 1;
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
