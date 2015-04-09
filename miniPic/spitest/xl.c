#include <p24FJ64GB002.h>
#include "xl.h"
#include "common.h"
#include "pin.h"
#include "spi.h"

_PIN *xlcs;

void init_xl(void) {
    xlcs = &rb5;
    pin_digitalOut(xlcs);
    pin_set(xlcs);
    spi_open(&spi1, &rb3, &rb2, &rb13, 2e6);
}

void xl_sleep(void) {
    xl_writeReg(0x2D, 0x00);    // Put the accelerometer in standby mode
}

void xl_wake(void) {
    xl_writeReg(0x2D, 0x02);    // Put the accelerometer in measurement mode
}

void xl_writeReg(uint8_t address, uint8_t value) {
    if (address<=0x3F) {
        pin_clear(xlcs);
        spi_transfer(&spi1, 0x0A);
        spi_transfer(&spi1, address);
        spi_transfer(&spi1, value);
        pin_set(xlcs);
    }
}

uint8_t xl_readReg(uint8_t address) {
    uint8_t value;

    if (address<=0x3F) {
        pin_clear(xlcs);
        spi_transfer(&spi1, 0x0B);
        spi_transfer(&spi1, address);
        value = spi_transfer(&spi1, 0);
        pin_set(xlcs);
        return value;
    } else
        return 0xFF;
}

void xl_writeRegs(uint8_t address, uint8_t *buffer, uint8_t n) {
    uint8_t i;

    if (address+n<=0x3F) {
        pin_clear(xlcs);
        spi_transfer(&spi1, 0x0A);
        spi_transfer(&spi1, address);
        for (i = 0; i<n; i++)
            spi_transfer(&spi1, buffer[i]);
        pin_set(xlcs);
    }
}

void xl_readRegs(uint8_t address, uint8_t *buffer, uint8_t n) {
    uint8_t i;

    if (address+n<=0x3F) {
        pin_clear(xlcs);
        spi_transfer(&spi1, 0x0B);
        spi_transfer(&spi1, address);
        for (i = 0; i<n; i++)
            buffer[i] = spi_transfer(&spi1, 0);
        pin_set(xlcs);
    } else {
        for (i = 0; i<n; i++)
            buffer[i] = 0xFF;
    }
}

void xl_readFIFO(uint8_t *buffer, uint16_t n) {
    uint16_t i;

    if (n<512) {
        pin_clear(xlcs);
        spi_transfer(&spi1, 0x0D);
        for (i = 0; i<n; i++)
            buffer[i] = spi_transfer(&spi1, 0);
        pin_set(xlcs);
    } else {
        for (i = 0; i<n; i++)
            buffer[i] = 0xFF;
    }
}

