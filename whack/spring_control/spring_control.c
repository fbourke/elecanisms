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


// The code acts like a spring to linearly resist deflection from zero. Run
// Adjustment code from python to adjust the spring constant


#define CONTROL_FREQ    100  // Hz

int KP = 250;


void setMotorDirection(int error) {
    if (error < 0) {
        pin_clear(&D[5]);
        pin_set(&D[6]);
    }
    else if (error > 0) {
        pin_clear(&D[6]);
        pin_set(&D[5]);
    }
    else {
        pin_clear(&D[5]);
        pin_clear(&D[6]);
    }
}

void setMotorPWM(int error) {
    int output = KP * abs(error);
    if (output > 1023) { output = 1023; }
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
            KP = USB_setup.wValue.w;
            KP = USB_setup.wIndex.w;
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case GET_VALS:
            temp.w = KP;
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            temp.w = KP;
            BD[EP0IN].address[2] = temp.b[0];
            BD[EP0IN].address[3] = temp.b[1];
            BD[EP0IN].bytecount = 4;    // set EP0 IN byte count to 4
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;            
        case PRINT_VALS:
            printf("KP = %u\n", KP);
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

    while (1) {
        ServiceUSB();
        if (timer_flag(&timer1)) {
            timer_lower(&timer1);
            flips = get_flips();
            setMotorDirection(flips);
            setMotorPWM(flips);
        }
    }
}
