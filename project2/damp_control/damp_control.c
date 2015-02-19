#include <p24FJ128GB206.h>
#include <math.h>
#include <stdio.h>
#include "config.h"
#include "control_tools.h"
#include "common.h"
#include "oc.h"
#include "pin.h"
#include "timer.h"
#include "uart.h"
#include "ui.h"
#include "usb.h"


#define CONTROL_FREQ    200  // Hz


int damping = 6; // 6 is a good value for anti-resistance, 15 is good for resistance
int direction = 1; //0 is resistance, 1 is positive feedback


void setMotor(int velocity) {
    if (velocity > 0 && direction || velocity < 0 && !direction) {
        pin_clear(&D[5]);
        pin_set(&D[6]);
    }
    else if (velocity < 0 && direction || velocity > 0 && !direction) {
        pin_clear(&D[6]);
        pin_set(&D[5]);
    }
    else {
        pin_clear(&D[5]);
        pin_clear(&D[6]);
    }
    int output = ((abs(velocity*damping)) > 1023) ? 1023 : abs(velocity*damping);
    pin_write(&D[2], output << 6);
}


void setup() {
    init_flip_tracking();
    setup_motor_shield();
    // Set the control loop timer
    timer_setPeriod(&timer1, 1.0 / CONTROL_FREQ);
    timer_start(&timer1);
    oc_pwm(&oc1, &D[2], NULL, 400, 0);  // D[2] is tri-stating
    led_on(&led1); led_on(&led2); led_on(&led3);
    timer_every(&timer2, 1.0 / FLIP_TRACKING_FREQ, track_flips);
    timer_every(&timer3, 1.0 / VELOCITY_TRACKING_FREQ, track_velocity);
    InitUSB();                             // initialize the USB registers and serial interface engine
    while (USB_USWSTAT != CONFIG_STATE) {  // while the peripheral is not configured...
        ServiceUSB();                      // ...service USB requests
    }
}


void VendorRequests(void) {
    WORD temp;

    switch (USB_setup.bRequest) {
        case HELLO:
            printf("Hello World!\n");
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case SET_VALS:
            damping = USB_setup.wValue.w;
            direction = USB_setup.wIndex.w;
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case GET_VALS:
            temp.w = damping;
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            temp.w = direction;
            BD[EP0IN].address[2] = temp.b[0];
            BD[EP0IN].address[3] = temp.b[1];
            BD[EP0IN].bytecount = 4;    // set EP0 IN byte count to 4
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;            
        case PRINT_VALS:
            printf("damping = %u, direction: %u\n", damping, direction);
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        default:
            USB_error_flags |= 0x01;  // set Request Error Flag
    }
}


int16_t main(void) {
    init_clock();
    init_oc();
    init_pin();
    init_timer();
    init_uart();  // Default is 19200 baud
    init_ui();
    setup();

    int flips = 0;
    int angle = 0;
    int velocity = 0;
    int goal_angle = 200;

    while (1) {
        ServiceUSB();
        if (timer_flag(&timer1)) {
            timer_lower(&timer1);
            flips = get_flips();
            angle = get_angle();
            velocity = get_velocity();
            setMotor(velocity);
        }
    }
}
