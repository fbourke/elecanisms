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
#define MPU_WRITE_REG       4
#define MPU_READ_REG        5
#define MPU_WRITE_REGS      6
#define MPU_READ_REGS       7
#define XL_WRITE_REG        10
#define XL_READ_REG         11
#define XL_WRITE_REGS       12
#define XL_READ_REGS        13
#define XL_READ_FIFO        14
#define TOGGLE_SHOW_XLINT1  15
#define TOGGLE_SHOW_XLINT2  16
#define G_WRITE_REG         17
#define G_READ_REG          18
#define G_WRITE_REGS        19
#define G_READ_REGS         20
#define XM_WRITE_REG        21
#define XM_READ_REG         22
#define XM_WRITE_REGS       23
#define XM_READ_REGS        24

_PIN *mpucs, *g_cs, *xm_cs;
_PIN *led1, *led2, *sw1;
_PIN *xlint1, *xlint2;
uint8_t show_xlint1, show_xlint2;

void g_writeReg(uint8_t address, uint8_t value) {
    if (address<=0x38) {
        pin_clear(g_cs);
        spi_transfer(&spi1, address);
        spi_transfer(&spi1, value);
        pin_set(g_cs);
    }
}

uint8_t g_readReg(uint8_t address) {
    uint8_t value;

    if (address<=0x38) {
        pin_clear(g_cs);
        spi_transfer(&spi1, 0x80|address);
        value = spi_transfer(&spi1, 0);
        pin_set(g_cs);
        return value;
    } else
        return 0xFF;
}

void g_writeRegs(uint8_t address, uint8_t *buffer, uint8_t n) {
    uint8_t i;

    if (address+n<=0x38) {
        pin_clear(g_cs);
        spi_transfer(&spi1, 0x40|address);
        for (i = 0; i<n; i++)
            spi_transfer(&spi1, buffer[i]);
        pin_set(g_cs);
    }
}

void g_readRegs(uint8_t address, uint8_t *buffer, uint8_t n) {
    uint8_t i;

    if (address+n<=0x38) {
        pin_clear(g_cs);
        spi_transfer(&spi1, 0xC0|address);
        for (i = 0; i<n; i++)
            buffer[i] = spi_transfer(&spi1, 0);
        pin_set(g_cs);
    } else {
        for (i = 0; i<n; i++)
            buffer[i] = 0xFF;
    }
}

void xm_writeReg(uint8_t address, uint8_t value) {
    if (address<=0x3F) {
        pin_clear(xm_cs);
        spi_transfer(&spi1, address);
        spi_transfer(&spi1, value);
        pin_set(xm_cs);
    }
}

uint8_t xm_readReg(uint8_t address) {
    uint8_t value;

    if (address<=0x3F) {
        pin_clear(xm_cs);
        spi_transfer(&spi1, 0x80|address);
        value = spi_transfer(&spi1, 0);
        pin_set(xm_cs);
        return value;
    } else
        return 0xFF;
}

void xm_writeRegs(uint8_t address, uint8_t *buffer, uint8_t n) {
    uint8_t i;

    if (address+n<=0x3F) {
        pin_clear(xm_cs);
        spi_transfer(&spi1, 0x40|address);
        for (i = 0; i<n; i++)
            spi_transfer(&spi1, buffer[i]);
        pin_set(xm_cs);
    }
}

void xm_readRegs(uint8_t address, uint8_t *buffer, uint8_t n) {
    uint8_t i;

    if (address+n<=0x3F) {
        pin_clear(xm_cs);
        spi_transfer(&spi1, 0xC0|address);
        for (i = 0; i<n; i++)
            buffer[i] = spi_transfer(&spi1, 0);
        pin_set(xm_cs);
    } else {
        for (i = 0; i<n; i++)
            buffer[i] = 0xFF;
    }
}

void mpu_writeReg(uint8_t address, uint8_t value) {
    if (address<=0x7E) {
        pin_clear(mpucs);
        spi_transfer(&spi1, address);
        spi_transfer(&spi1, value);
        pin_set(mpucs);
    }
}

uint8_t mpu_readReg(uint8_t address) {
    uint8_t value;

    if (address<=0x7E) {
        pin_clear(mpucs);
        spi_transfer(&spi1, 0x80|address);
        value = spi_transfer(&spi1, 0);
        pin_set(mpucs);
        return value;
    } else
        return 0xFF;
}

void mpu_writeRegs(uint8_t address, uint8_t *buffer, uint8_t n) {
    uint8_t i;

    if (address+n<=0x7E) {
        pin_clear(mpucs);
        spi_transfer(&spi1, address);
        for (i = 0; i<n; i++)
            spi_transfer(&spi1, buffer[i]);
        pin_set(mpucs);
    }
}

void mpu_readRegs(uint8_t address, uint8_t *buffer, uint8_t n) {
    uint8_t i;

    if (address+n<=0x7E) {
        pin_clear(mpucs);
        spi_transfer(&spi1, 0x80|address);
        for (i = 0; i<n; i++)
            buffer[i] = spi_transfer(&spi1, 0);
        pin_set(mpucs);
    } else {
        for (i = 0; i<n; i++)
            buffer[i] = 0xFF;
    }
}

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
        case MPU_WRITE_REG:
            mpu_writeReg(USB_setup.wValue.b[0], USB_setup.wIndex.b[0]);
            BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case MPU_READ_REG:
            BD[EP0IN].address[0] = mpu_readReg(USB_setup.wValue.b[0]);
            BD[EP0IN].bytecount = 1;         // set EP0 IN byte count to 1
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case MPU_WRITE_REGS:
            USB_request.setup.bmRequestType = USB_setup.bmRequestType;
            USB_request.setup.bRequest = USB_setup.bRequest;
            USB_request.setup.wValue.w = USB_setup.wValue.w;
            USB_request.setup.wIndex.w = USB_setup.wIndex.w;
            USB_request.setup.wLength.w = USB_setup.wLength.w;
            break;
        case MPU_READ_REGS:
            mpu_readRegs(USB_setup.wValue.b[0], BD[EP0IN].address, USB_setup.wLength.b[0]);
            BD[EP0IN].bytecount = USB_setup.wLength.b[0];
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
        case G_WRITE_REG:
            g_writeReg(USB_setup.wValue.b[0], USB_setup.wIndex.b[0]);
            BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case G_READ_REG:
            BD[EP0IN].address[0] = g_readReg(USB_setup.wValue.b[0]);
            BD[EP0IN].bytecount = 1;         // set EP0 IN byte count to 1
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case G_WRITE_REGS:
            USB_request.setup.bmRequestType = USB_setup.bmRequestType;
            USB_request.setup.bRequest = USB_setup.bRequest;
            USB_request.setup.wValue.w = USB_setup.wValue.w;
            USB_request.setup.wIndex.w = USB_setup.wIndex.w;
            USB_request.setup.wLength.w = USB_setup.wLength.w;
            break;
        case G_READ_REGS:
            g_readRegs(USB_setup.wValue.b[0], BD[EP0IN].address, USB_setup.wLength.b[0]);
            BD[EP0IN].bytecount = USB_setup.wLength.b[0];
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case XM_WRITE_REG:
            xm_writeReg(USB_setup.wValue.b[0], USB_setup.wIndex.b[0]);
            BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case XM_READ_REG:
            BD[EP0IN].address[0] = xm_readReg(USB_setup.wValue.b[0]);
            BD[EP0IN].bytecount = 1;         // set EP0 IN byte count to 1
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case XM_WRITE_REGS:
            USB_request.setup.bmRequestType = USB_setup.bmRequestType;
            USB_request.setup.bRequest = USB_setup.bRequest;
            USB_request.setup.wValue.w = USB_setup.wValue.w;
            USB_request.setup.wIndex.w = USB_setup.wIndex.w;
            USB_request.setup.wLength.w = USB_setup.wLength.w;
            break;
        case XM_READ_REGS:
            xm_readRegs(USB_setup.wValue.b[0], BD[EP0IN].address, USB_setup.wLength.b[0]);
            BD[EP0IN].bytecount = USB_setup.wLength.b[0];
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
        case MPU_WRITE_REGS:
            mpu_writeRegs(USB_request.setup.wValue.b[0], BD[EP0OUT].address, USB_request.setup.wLength.b[0]);
            break;
        case XL_WRITE_REGS:
            xl_writeRegs(USB_request.setup.wValue.b[0], BD[EP0OUT].address, USB_request.setup.wLength.b[0]);
            break;
        case G_WRITE_REGS:
            g_writeRegs(USB_request.setup.wValue.b[0], BD[EP0OUT].address, USB_request.setup.wLength.b[0]);
            break;
        case XM_WRITE_REGS:
            xm_writeRegs(USB_request.setup.wValue.b[0], BD[EP0OUT].address, USB_request.setup.wLength.b[0]);
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

    mpucs = &ra2;
    pin_digitalOut(mpucs);
    pin_set(mpucs);
    xm_cs = mpucs;

    g_cs = &ra4;
    pin_digitalOut(g_cs);
    pin_set(g_cs);

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
