#include <p24FJ64GB002.h>
#include <stdint.h>
#include "config.h"
#include "common.h"
#include "usb.h"
#include "pin.h"
#include "spi.h"
#include "xl.h"

#define TOGGLE_LED1         1
#define TOGGLE_LED2         2
#define READ_SW1            3
#define XL_WRITE_REG        10
#define XL_READ_REG         11
#define XL_WRITE_REGS       12
#define XL_READ_REGS        13
#define XL_READ_FIFO        14
#define TOGGLE_SHOW_XLINT1  15
#define TOGGLE_SHOW_XLINT2  16

_PIN *led1, *led2, *sw1;
_PIN *xlint1, *xlint2;
uint8_t show_xlint1, show_xlint2;

//void ClassRequests(void) {
//    switch (USB_setup.bRequest) {
//        default:
//            USB_error_flags |= 0x01;                    // set Request Error Flag
//    }
//}

void VendorRequests(void) {
    WORD32 address;

    switch (USB_setup.bRequest) {
        case TOGGLE_LED1:
            pin_toggle(led1);
            BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case TOGGLE_LED2:
            pin_toggle(led2);
            BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case READ_SW1:
            BD[EP0IN].address[0] = (uint8_t)pin_read(sw1);
            BD[EP0IN].bytecount = 1;         // set EP0 IN byte count to 1
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case XL_WRITE_REG:
            xl_writeReg(USB_setup.wValue.b[0], USB_setup.wIndex.b[0]);
            BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case XL_READ_REG:
            BD[EP0IN].address[0] = xl_readReg(USB_setup.wValue.b[0]);
            BD[EP0IN].bytecount = 1;         // set EP0 IN byte count to 1
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case XL_WRITE_REGS:
            USB_request.setup.bmRequestType = USB_setup.bmRequestType;
            USB_request.setup.bRequest = USB_setup.bRequest;
            USB_request.setup.wValue.w = USB_setup.wValue.w;
            USB_request.setup.wIndex.w = USB_setup.wIndex.w;
            USB_request.setup.wLength.w = USB_setup.wLength.w;
            break;
        case XL_READ_REGS:
            xl_readRegs(USB_setup.wValue.b[0], BD[EP0IN].address, USB_setup.wLength.b[0]);
            BD[EP0IN].bytecount = USB_setup.wLength.b[0];
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case XL_READ_FIFO:
            xl_readFIFO(BD[EP0IN].address, USB_setup.wLength.w);
            BD[EP0IN].bytecount = USB_setup.wLength.b[0];
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case TOGGLE_SHOW_XLINT1:
            show_xlint1 = show_xlint1 ? 0:1;
            BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit            
            break;
        case TOGGLE_SHOW_XLINT2:
            show_xlint2 = show_xlint2 ? 0:1;
            BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
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
    WORD32 address;

    switch (USB_request.setup.bRequest) {
        case XL_WRITE_REGS:
            xl_writeRegs(USB_request.setup.wValue.b[0], BD[EP0OUT].address, USB_request.setup.wLength.b[0]);
            break;
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

int16_t main(void) {
    init_clock();
    init_pin();
    init_spi();
    init_xl();

    sw1 = &ra3;
    led1 = &rb15;
    led2 = &rb14;
    pin_digitalOut(led1);
    pin_digitalOut(led2);

    xlint1 = &rb7;
    xlint2 = &rb4;
    show_xlint1 = 0;
    show_xlint2 = 0;

    InitUSB();                              // initialize the USB registers and serial interface engine
    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        ServiceUSB();                       // ...service USB requests
    }
    while (1) {
        ServiceUSB();                       // service any pending USB requests
        if (show_xlint1)
            pin_write(led1, pin_read(xlint1));
        if (show_xlint2)
            pin_write(led2, pin_read(xlint2));
    }
}
