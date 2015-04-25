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

typedef enum {
UP,
DOWN,
OFF
} Direction;

typedef struct Mole {
    _PIN* solenoid_pin1;
    _PIN* solenoid_pin2;
    double upTime;
    double downTime;
    uint16_t upTimePassed;
    uint16_t downTimePassed;
    Direction direction;
} Mole;

void mole_init(Mole* mole, int solenoid_pin1, int solenoid_pin2) {
    pin_digitalOut(&D[solenoid_pin1]);
    pin_digitalOut(&D[solenoid_pin2]);

    mole->solenoid_pin1 = &D[solenoid_pin1];
    mole->solenoid_pin2 = &D[solenoid_pin2];
    mole->upTime = NULL;
    mole->downTime = NULL;
    mole->upTimePassed = 0;
    mole->downTimePassed = 0;
    mole->direction = OFF;
}

struct Mole* testMole;

typedef struct Button
{
    volatile uint16_t state;
    volatile uint16_t prevState;
    _PIN* pin;
} Button;

void button_init(Button* button, int pin)
{
    pin_digitalIn(&D[pin]);
    button->state = 0;
    button->prevState = 0;
    button->pin = &D[pin];
}

Button* testButton;

typedef enum {
EASY,
HARD,
COIN_NEEDED
} GameState;

volatile GameState gameState = COIN_NEEDED;

void turn_Off(Mole* mole) {
    mole->direction = OFF;
    pin_clear(mole->solenoid_pin1);
    pin_clear(mole->solenoid_pin2);
    printf("off \n");
}

void push_down(Mole* mole) {
    mole->direction = DOWN;
    pin_set(mole->solenoid_pin1);
    pin_clear(mole->solenoid_pin2);
    mole->downTime = gameTime;
    mole->downTimePassed = 0;
    printf("down \n");

}

void push_up(Mole* mole) {
    mole->direction = UP;
    pin_clear(mole->solenoid_pin1);
    pin_set(mole->solenoid_pin2);
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
        push_up(testMole);
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
    turn_Off(testMole);
}

void switch_state() {
    gyro = pin_read(&D[13]);
    
    if (gyro){
        led_on(&led1);
    }
    else {
        led_off(&led1);
    }

    Mole* mole = testMole;
    Button* button = testButton;
    if (mole->direction == UP) {
        if (button_hit(button)) {
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
            push_down(testMole);
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

void setup_pins() {
    int i;
    for (i = 0; i<6; i++) {
        pin_analogIn(&A[i]);
    }
}


void updateTimes() {
    Mole* = testMole;
    gameTime = gameTime + gamePeriod;
    if ((gameTime - mole->downTime) > 3.0) {
        mole->downTimePassed = 1;
    }
    if ((gameTime - mole->upTime) > 3.0) {
        mole->upTimePassed = 1;
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
    setup_motor();

    testMole = init_mole(5, 6)
    testButton = init_button(10)

// Main Loop
    timer_setPeriod(&timer1, gamePeriod);
    timer_start(&timer1);

// Reading Loop
    timer_setPeriod(&timer2, 0.01);
    timer_start(&timer2);

    while (1) {
        if (gameState == COIN_NEEDED) {
            if (timer_flag(&timer1)) {
                timer_lower(&timer1);
                waiting();
            }
        } else {
            if (timer_flag(&timer1)) {
                timer_lower(&timer1);
                updateTimes();
            }
            if (timer_flag(&timer2)) {
                timer_lower(&timer2);
                switch_state();
            }
        }
    }
}
