#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "oc.h"
#include "uart.h"


volatile uint16_t switchPressed;
volatile uint16_t gyro;

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

}

void push_up(){
	direction = UP;
    pin_clear(&D[5]);
    pin_set(&D[6]);

}

void switch_state() {

	switchPressed = !sw_read(&sw1);
	gyro = pin_read(&D[13]);
	
	if (gyro){
		led_on(&led1);
	}
	else {
		led_off(&led1);
	}

	if (direction == UP) {
		if (!switchPressed && gyro) {
			push_down();

		}
	}
	if (direction == DOWN || direction == OFF){
		if (switchPressed){
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
	turn_Off();
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

 	pin_digitalIn(&D[13]);

	timer_setPeriod(&timer2, 0.01);
	timer_start(&timer2);

	while (1) {
		if (timer_flag(&timer2)) {
			timer_lower(&timer2);
			switch_state();
		}
	}
}