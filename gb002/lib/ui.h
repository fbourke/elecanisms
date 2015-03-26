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
#ifndef _UI_H_
#define _UI_H_

#include <stdint.h>

void init_ui(void);

typedef struct {
    uint16_t *port;
    uint16_t *tris;
    uint16_t *ans;
    uint8_t bit;
} _LED;

typedef struct {
    uint16_t *port;
    uint16_t *tris;
    uint16_t *ans;
    uint8_t bit;
} _SW;

extern _LED led1, led2;

void led_init(_LED *self, uint16_t *port, uint16_t *tris, 
              uint16_t *ans, uint8_t bit);
void led_on(_LED *self);
void led_off(_LED *self);
void led_toggle(_LED *self);
void led_write(_LED *self, uint16_t val);
uint16_t led_read(_LED *self);

extern _SW sw1;

void sw_init(_SW *self, uint16_t *port, uint16_t *tris, 
             uint16_t *ans, uint8_t bit);
uint16_t sw_read(_SW *self);

#endif
