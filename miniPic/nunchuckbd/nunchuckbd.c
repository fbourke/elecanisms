#include <p24FJ64GB002.h>
#include "config.h"
#include "common.h"
#include "usb.h"
#include "pin.h"
#include "i2c.h"

_PIN *led1, *led2, *sw1;

void ClassRequests(void) {
    BYTE i;

    switch (USB_setup.bRequest) {
        case GET_REPORT:
            for (i = 0; i<8; i++)
                BD[EP0IN].address[i] = 0;           // send a null report
            BD[EP0IN].bytecount = 0x08;             // set byte j to 8
            BD[EP0IN].status = 0xC8;                // send packet as DATA1, set UOWN bit
            break;
        case SET_REPORT:
            USB_request.setup.bmRequestType = USB_setup.bmRequestType;  // processing a SET_REPORT request
            USB_request.setup.bRequest = USB_setup.bRequest;
            USB_request.setup.wValue.w = USB_setup.wValue.w;
            USB_request.setup.wIndex.w = USB_setup.wIndex.w;
            USB_request.setup.wLength.w = USB_setup.wLength.w;
            break;
        case GET_PROTOCOL:
            BD[EP0IN].address[0] = USB_protocol;    // copy current protocol into EP0 IN buffer
            BD[EP0IN].bytecount = 0x01;             // set byte j to 1
            BD[EP0IN].status = 0xC8;                // send packet as DATA1, set UOWN bit
            break;
        case SET_PROTOCOL:
            USB_protocol = USB_setup.wValue.b[0];   // update the new protocol value
            BD[EP0IN].bytecount = 0x00;             // set byte j to 0
            BD[EP0IN].status = 0xC8;                // sent packet as DATA1, set UOWN bit
            break;
        case GET_IDLE:
            BD[EP0IN].address[0] = USB_idle_rate;   // copy current idle rate into EP0 IN buffer
            BD[EP0IN].bytecount = 0x01;             // set byte j to 1
            BD[EP0IN].status = 0xC8;                // send packet as DATA1, set UOWN bit
            break;
        case SET_IDLE:
            USB_idle_rate = USB_setup.wValue.b[1];  // update the new idle rate
            BD[EP0IN].bytecount = 0x00;             // set byte j to 0
            BD[EP0IN].status = 0xC8;                // sent packet as DATA1, set UOWN bit
            break;
        default:
            USB_error_flags |= 0x01;    // set Request Error Flag
    }
}

void ClassRequestsIn(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

void ClassRequestsOut(void) {
    switch (USB_request.setup.bRequest) {
        case SET_REPORT:
            USB_LED_states = BD[EP0OUT].address[0];
            break;
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

void VendorRequests(void) {
    switch (USB_setup.bRequest) {
        default:
            USB_error_flags |= 0x01;    // set Request Error Flag
    }
}

void VendorRequestsIn(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

void VendorRequestsOut(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

int main(void) {
    unsigned char nunchuck[6];
    unsigned int i;
    int x, y, z;
    int absx, absy, absz;
    int maxxy, maxyz;
    int minr, maxr, r;
    long r2;
    int normx, normy;
    char dx, dy;
    unsigned int flip_y_axis;

    init_clock();
    init_pin();
    init_i2c();

    sw1 = &ra3;
    led1 = &rb15;
    led2 = &rb14;
    pin_digitalOut(led1);
    pin_digitalOut(led2);

    i2c_open(&i2c1, 100e3);
    i2c_idle(&i2c1);

    i2c_start(&i2c1);
    i2c_putc(&i2c1, 0xA4);
    i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0x40);
    i2c_idle(&i2c1);
    i2c_putc(&i2c1, 0);
    i2c_idle(&i2c1);
    i2c_stop(&i2c1);
    i2c_idle(&i2c1);

    flip_y_axis = 1;

    InitUSB();                              // initialize the USB registers and serial interface engine
    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        ServiceUSB();                       // ...service USB requests
    }

    while (1) {
        pin_write(led1, flip_y_axis);
        if ((!(BD[EP1IN].status&0x80))          // if UOWN bit of EP1 IN or EP2 IN status
            || (!(BD[EP2IN].status&0x80))) {    //   registers are set, read nunchuck data
            i2c_start(&i2c1);
            i2c_putc(&i2c1, 0xA4);
            i2c_idle(&i2c1);
            i2c_putc(&i2c1, 0);
            i2c_idle(&i2c1);
            i2c_stop(&i2c1);
            for (i = 700; i; i--) {}            // delay about 200µs
            i2c_idle(&i2c1);
            i2c_start(&i2c1);
            i2c_putc(&i2c1, 0xA5);
            i2c_idle(&i2c1);
            for (i = 0; i<6; i++) {
                nunchuck[i] = (i2c_getc(&i2c1)^0x17)+0x17;
                if (i<5)
                    i2c_ack(&i2c1);
                else
                    i2c_nak(&i2c1);
                i2c_idle(&i2c1);
            }
            i2c_stop(&i2c1);
            i2c_idle(&i2c1);
        }
        if (!(BD[EP1IN].status&0x80)) {     // see if UOWN bit of EP1 IN status register is 
                                            //   clear (i.e., PIC owns EP1 IN buffer)
            for (i = 0; i<8; i++)           // prepare EP1 IN buffer to send a null 
                BD[EP1IN].address[i] = 0;   //   keyboard report
            i = 2;
            if (nunchuck[0]<100)
                BD[EP1IN].address[i++] = 0x04;  // USB keycode for 'a'
            if (nunchuck[0]>156)
                BD[EP1IN].address[i++] = 0x07;  // USB keycode for 'd'
            if (nunchuck[1]>156)
                BD[EP1IN].address[i++] = 0x1A;  // USB keycode for 'w'
            if (nunchuck[1]<100)
                BD[EP1IN].address[i++] = 0x16;  // USB keycode for 's'
/*
            if (!(nunchuck[5]&0x02))
                BD[EP1IN].address[i++] = 0x06;  // USB keycode for 'c'
            if (!(nunchuck[5]&0x01))
                BD[EP1IN].address[i] = 0x1D;    // USB keycode for 'z'
*/
            BD[EP1IN].bytecount = 8;        // set EP1 IN byte count to 8
            // toggle the DATA01 bit of the EP1 IN status register and set UOWN and DTS bits
            BD[EP1IN].status = ((BD[EP1IN].status&0x40)^0x40)|0x88;
        }
        if (!(BD[EP2IN].status&0x80)) {     // see if UOWN bit of EP2 IN status register is 
                                            //   clear (i.e., PIC owns EP2 IN buffer)
            for (i = 0; i<3; i++)           // prepare EP2 IN buffer to send a null 
                BD[EP2IN].address[i] = 0;   //   mouse report

            if (pin_read(sw1)==0)
                flip_y_axis = !flip_y_axis;

            x = (int)(((unsigned int)(nunchuck[2])<<2)|(((unsigned int)(nunchuck[5])>>2)&0x03)) - 512;
            y = (int)(((unsigned int)(nunchuck[3])<<2)|(((unsigned int)(nunchuck[5])>>4)&0x03)) - 512;
            z = (int)(((unsigned int)(nunchuck[4])<<2)|(((unsigned int)(nunchuck[5])>>6)&0x03)) - 512;

            absx = (x<0) ? -x:x;
            absy = (y<0) ? -y:y;
            absz = (z<0) ? -z:z;

            maxxy = (absx>absy) ? absx:absy;
            maxyz = (absy>absz) ? absy:absz;

            minr = (maxxy>maxyz) ? maxxy:maxyz;
            maxr = absx + absy + absz;

            r2 = (long)absx*(long)absx + (long)absy*(long)absy + (long)absz*(long)absz;
            r = (minr + maxr)>>1;
            while (maxr>minr+1) {
                if (((long)r*(long)r)>r2)
                    maxr = r;
                else
                    minr = r;
                r = (minr + maxr)>>1;
            }

            normx = (127*x)/r;
            normy = (127*y)/r;

            if ((r<190) || (r>230)) {
                dx = 0;
                dy = 0;
            } else {
                if (normx>19) {
                    dx = (char)((normx - 20)>>1);
                } else if (normx<-19) {
                    dx = (char)((normx + 20)>>1);
                } else {
                    dx = 0;
                }
                if (normy+30>19) {
                    dy = (char)((normy + 30 - 20)>>1);
                } else if (normy+30<-19) {
                    dy = (char)((normy + 30 + 20)>>1);
                } else {
                    dy = 0;
                }
            }

            if (!(nunchuck[5]&0x02))
                BD[EP2IN].address[0] |= 0x01;   // C button -> mouse button 1
            if (!(nunchuck[5]&0x01))
                BD[EP2IN].address[0] |= 0x02;   // Z button -> mouse button 2
            BD[EP2IN].address[1] = dx;
            BD[EP2IN].address[2] = flip_y_axis ? -dy:dy;
            BD[EP2IN].bytecount = 3;        // set EP2 IN byte count to 3
            // toggle the DATA01 bit of the EP2 IN status register and set UOWN and DTS bits
            BD[EP2IN].status = ((BD[EP2IN].status&0x40)^0x40)|0x88;
        }
        ServiceUSB();                       // service any pending USB requests
    }
}

