/*
** Copyright (c) 2013, Bradley A. Minch
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met: 
** 
**     1. Redistributions of source code must retain the above copyright 
**        notice, this list of conditions and the following disclaimer. 
**     2. Redistributions in binary form must reproduce the above copyright 
**        notice, this list of conditions and the following disclaimer in the 
**        documentation and/or other materials provided with the distribution. 
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
** POSSIBILITY OF SUCH DAMAGE.
*/
#include <p24FJ64GB002.h>
#include "common.h"
#include "pin.h"

_PIN ra0, ra1, ra2, ra3, ra4;
_PIN rb0, rb1, rb2, rb3, rb4, rb5, rb7, rb8, rb9, rb10, rb11;
_PIN rb13, rb14, rb15;

void __digitalWrite(_PIN *self, uint16_t val) {
    if (val)
        bitset(self->port, self->bit);
    else
        bitclear(self->port, self->bit);
}

uint16_t __digitalRead(_PIN *self) {
    return bitread(self->port, self->bit);
}

uint16_t __analogRead(_PIN *self) {
    AD1CHS = self->annum;
    AD1CON1bits.SAMP = 1;
    while (AD1CON1bits.DONE==0) {}
    return ADC1BUF0;
}

void init_pin(void) {
    uint16_t i;

    AD1PCFG = 0xFFFF;

    pin_init(&rb0, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)&AD1PCFG, 0, 2, 0, 0, (uint16_t *)&RPOR0);
    pin_init(&rb1, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)&AD1PCFG, 1, 3, 8, 1, (uint16_t *)&RPOR0);
    pin_init(&rb2, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)&AD1PCFG, 2, 4, 0, 2, (uint16_t *)&RPOR1);
    pin_init(&rb3, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)&AD1PCFG, 3, 5, 8, 3, (uint16_t *)&RPOR1);
    pin_init(&rb4, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)NULL, 4, -1, 0, 4, (uint16_t *)&RPOR2);
    pin_init(&rb5, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)NULL, 5, -1, 0, -1, (uint16_t *)NULL);
    pin_init(&rb7, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)NULL, 7, -1, 8, 7, (uint16_t *)&RPOR3);
    pin_init(&rb8, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)NULL, 8, -1, 0, 8, (uint16_t *)&RPOR4);
    pin_init(&rb9, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)NULL, 9, -1, 8, 9, (uint16_t *)&RPOR4);
    pin_init(&rb10, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)NULL, 10, -1, 0, 10, (uint16_t *)&RPOR5);
    pin_init(&rb11, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)NULL, 11, -1, 8, 11, (uint16_t *)&RPOR5);
    pin_init(&rb13, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)&AD1PCFG, 13, 11, 8, 13, (uint16_t *)&RPOR6);
    pin_init(&rb14, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)&AD1PCFG, 14, 10, 0, 14, (uint16_t *)&RPOR7);
    pin_init(&rb15, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)&AD1PCFG, 15, 9, 8, 15, (uint16_t *)&RPOR7);
    pin_init(&ra0, (uint16_t *)&PORTA, (uint16_t *)&TRISA, 
             (uint16_t *)&AD1PCFG, 0, 0, 8, 5, (uint16_t *)&RPOR2);
    pin_init(&ra1, (uint16_t *)&PORTA, (uint16_t *)&TRISA, 
             (uint16_t *)&AD1PCFG, 1, 1, 0, 6, (uint16_t *)&RPOR3);
    pin_init(&ra2, (uint16_t *)&PORTA, (uint16_t *)&TRISA, 
             (uint16_t *)NULL, 2, -1, 0, -1, (uint16_t *)NULL);
    pin_init(&ra3, (uint16_t *)&PORTA, (uint16_t *)&TRISA, 
             (uint16_t *)NULL, 3, -1, 0, -1, (uint16_t *)NULL);
    pin_init(&ra4, (uint16_t *)&PORTA, (uint16_t *)&TRISA, 
             (uint16_t *)NULL, 4, -1, 0, -1, (uint16_t *)NULL);

    pin_digitalIn(&rb0);
    pin_digitalIn(&rb1);
    pin_digitalIn(&rb2);
    pin_digitalIn(&rb3);
    pin_digitalIn(&rb4);
    pin_digitalIn(&rb5);
    pin_digitalIn(&rb7);
    pin_digitalIn(&rb8);
    pin_digitalIn(&rb9);
    pin_digitalIn(&rb10);
    pin_digitalIn(&rb11);
    pin_digitalIn(&rb13);
    pin_digitalIn(&rb14);
    pin_digitalIn(&rb15);
    pin_digitalIn(&ra0);
    pin_digitalIn(&ra1);
    pin_digitalIn(&ra2);
    pin_digitalIn(&ra3);
    pin_digitalIn(&ra4);

    // Configure and enable the ADC module: 
    //   set output data format to fractional (FORM<1:0> = 0b10)
    //   set conversion trigger source select bits to internal counter 
    //     (SSRC<2:0> = 0b111)
    //   set auto-sample time bits to 12*TAD (SAMC<4:0> = 12)
    //   set conversion clock select bits to 64*TCY (ADCS<7:0> = 64)
    //   set initial channel selection to AN0
    AD1CON1 = 0x02E0; 
    AD1CON2 = 0;
    AD1CON3 = 0x0C40;
    AD1CHS = 0;
    AD1CON1bits.ADON = 1;
}

void pin_init(_PIN *self, uint16_t *port, uint16_t *tris, 
              uint16_t *ans, uint8_t bit, int annum, 
              uint8_t rpshift, uint16_t rpnum, uint16_t *rpor) {
    self->port = port;
    self->tris = tris;
    self->ans = ans;
    self->bit = bit;
    self->annum = annum;
    self->rpshift = rpshift;
    self->rpnum = rpnum;
    self->rpor = rpor;
    self->owner = NULL;
    self->write = NULL;
    self->read = NULL;
}

void pin_digitalIn(_PIN *self) {
    if (self->ans)
        bitset(self->ans, self->bit);
    bitset(self->tris, self->bit);
    self->write = NULL;
    self->read = __digitalRead;    
}

void pin_digitalOut(_PIN *self) {
    if (self->ans)
        bitset(self->ans, self->bit);
    bitclear(self->port, self->bit);
    bitclear(self->tris, self->bit);
    self->write = __digitalWrite;
    self->read = __digitalRead;
}

void pin_analogIn(_PIN *self) {
    if (!(self->ans))
        return;
    bitset(self->tris, self->bit);
    bitclear(self->ans, self->bit);
    self->write = NULL;
    self->read = __analogRead;
}

void pin_set(_PIN *self) {
    if (self->write==__digitalWrite)
        bitset(self->port, self->bit);
}

void pin_clear(_PIN *self) {
    if (self->write==__digitalWrite)
        bitclear(self->port, self->bit);
}

void pin_toggle(_PIN *self) {
    if (self->write==__digitalWrite)
        bitflip(self->port, self->bit);
}

void pin_write(_PIN *self, uint16_t val) {
    if (self->write)
        self->write(self, val);
}

uint16_t pin_read(_PIN *self) {
    if (self->read)
        return self->read(self);
    else
        return 0xFFFF;
}

