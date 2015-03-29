#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "oc.h"
#include "uart.h"


void setup_pins() {
	pin_analogIn(&A[0]);
}


uint16_t value;


int16_t main(void) {
	init_pin();
	init_clock();
	init_ui();
	init_uart();
	init_timer();
	init_oc();
	setup_pins();

	timer_setPeriod(&timer2, 0.01);
	timer_start(&timer2);

	while(1) {

		if (timer_flag(&timer2)) {
			timer_lower(&timer2);
			value = pin_read(&A[0]) >> 6;
			if (value > 158) {
				printf("%u\n",value);
			}
		}
	}
}