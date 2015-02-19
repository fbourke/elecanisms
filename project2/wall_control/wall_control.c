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


// This code hits the walls and linearly ramps the resistance up until the
// resistance is full at one turn past the wall. You can run the python
// adjustment file to change the wall position.
// Open question: What if we change the linear ramping to something non-linear?


#define HELLO       0   // Vendor request that prints "Hello World!"
#define SET_VALS    1   // Vendor request that receives 2 unsigned integer values
#define GET_VALS    2   // Vendor request that returns 2 unsigned integer values
#define PRINT_VALS  3   // Vendor request that prints 2 unsigned integer values 
#define RESPONSE_HEIGHT 1023
#define CONTROL_FREQ    20  // Hz

int negativeWall = -2;
int positiveWall = 2;


void setMotorDirection(int flips) {
    if (flips < 0) {
        pin_clear(&D[5]);
        pin_set(&D[6]);
    }
    else if (flips > 0) {
        pin_clear(&D[6]);
        pin_set(&D[5]);
    }
    else {
        pin_clear(&D[5]);
        pin_clear(&D[6]);
    }
}

void setMotorPWM(int flips, int angle) {
    if (flips == negativeWall) {
        // Maps 0-800 to 0-1023, Takes care of the 6 bit shift
        pin_write(&D[2], abs(angle - TICKS_PER_REV) * 83);
    }
    else if (flips == positiveWall) {
        // Maps 0-800 to 0-1023, Takes care of the 6 bit shift
        pin_write(&D[2], angle * 83);
    }
    else if (flips < negativeWall || flips > positiveWall) {
        pin_write(&D[2], RESPONSE_HEIGHT << 6);
    }
    else {
        pin_write(&D[2], 0 << 6);
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
            negativeWall = -1 * (int) USB_setup.wValue.w;
            positiveWall = USB_setup.wIndex.w;
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case GET_VALS:
            temp.w = abs(negativeWall);
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            temp.w = positiveWall;
            BD[EP0IN].address[2] = temp.b[0];
            BD[EP0IN].address[3] = temp.b[1];
            BD[EP0IN].bytecount = 4;    // set EP0 IN byte count to 4
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;            
        case PRINT_VALS:
            printf("negativeWall = %u, positiveWall = %u\n", negativeWall, positiveWall);
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        default:
            USB_error_flags |= 0x01;  // set Request Error Flag
    }
}


void setup() {
    init_flip_tracking();
    setup_motor_shield();
    timer_setPeriod(&timer1, 1.0 / CONTROL_FREQ);  // Set the control loop timer
    timer_start(&timer1);
    oc_pwm(&oc1, &D[2], NULL, 400, 0);  // D[2] is tri-stating
    led_on(&led1); led_on(&led2); led_on(&led3);
    timer_every(&timer2, 1.0 / FLIP_TRACKING_FREQ, track_flips);
    InitUSB();                             // initialize the USB registers and serial interface engine
    while (USB_USWSTAT != CONFIG_STATE) {  // while the peripheral is not configured...
        ServiceUSB();                      // ...service USB requests
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

    while (1) {
        ServiceUSB();
        if (timer_flag(&timer1)) {
            timer_lower(&timer1);
            flips = get_flips();
            angle = get_angle();
            setMotorDirection(flips);
            setMotorPWM(flips, angle);
        }
    }
}
