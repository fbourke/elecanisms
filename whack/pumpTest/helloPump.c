#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "oc.h"
#include "uart.h"


volatile uint16_t downTimePassed = 0;
volatile uint16_t upTimePassed = 0;
volatile uint16_t gyro;
volatile uint16_t moleHit;
uint16_t score;

double downTime;
double upTime;

double gamePeriod = 0.1;
double gameTime;

int compressed = 0;

uint16_t c;
uint16_t d;

typedef enum {
UP,
DOWN,
OFF
} Direction;


volatile Direction direction = OFF;

void turn_Off() {
	pin_clear(&D[5]);
	pin_clear(&D[6]);
}

void push_down(){
	direction = DOWN;
    pin_set(&D[5]);
    pin_clear(&D[6]);
    downTime = gameTime;
    downTimePassed = 0;

}

void push_up(){
	direction = UP;
    pin_clear(&D[5]);
    pin_set(&D[6]);
    upTime = gameTime;
    upTimePassed = 0;

}

void switch_state() {
	moleHit = !pin_read(&D[12]);
	gyro = pin_read(&D[13]);
	
	if (gyro){
		led_on(&led1);
	}
	else {
		led_off(&led1);
	}

	if (direction == UP) {
		if (upTimePassed || gyro || moleHit) {
			push_down();

		}
	}
	if (direction == DOWN || direction == OFF){
		if (downTimePassed){
			push_up();
		}
	}
}


void setup_pins() {
	int i;
	for (i = 0; i<6; i++) {
	pin_analogIn(&A[i]);
	}
}

void setup_motor() {
	pin_digitalOut(&D[2]);
	pin_digitalOut(&D[3]);
	pin_digitalOut(&D[4]);
	pin_set(&D[2]);
	pin_clear(&D[3]);
	pin_set(&D[4]);

	pin_digitalOut(&D[5]);
	pin_digitalOut(&D[6]);
 	pin_digitalIn(&D[13]);
 	pin_digitalIn(&D[12]);
	turn_Off();
}

void check_Hit() {
	moleHit = !pin_read(&D[12]);
	// printf("%d\n", moleHit);
	if (!compressed && moleHit) {
		score = score + 1;
		compressed = 1;
		led_on(&led2);
		printf("%d\n", score);
	}
	else if (compressed && !moleHit) {
		compressed = 0;
		led_off(&led2);

	}

}

void gameLogic(){
	if ((gameTime - downTime) > 3.0) {
		downTimePassed = 1;
	}
	if ((gameTime - upTime) > 3.0) {
		upTimePassed = 1;
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

// Main Loop
	timer_setPeriod(&timer1, gamePeriod);
	timer_start(&timer1);

// Reading Loop
	timer_setPeriod(&timer2, 0.001);
	timer_start(&timer2);


	while (1) {
		if (timer_flag(&timer1)) {
			timer_lower(&timer1);
			gameTime = gameTime + gamePeriod;
			gameLogic();
		}
		if (timer_flag(&timer2)) {
			timer_lower(&timer2);
			switch_state();
			check_Hit();
		}
	}
}