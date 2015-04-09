#ifndef _XL_H_
#define _XL_H_

#include <stdint.h>

void init_xl(void);

void xl_sleep(void);
void xl_wake(void);
void xl_writeReg(uint8_t address, uint8_t value);
uint8_t xl_readReg(uint8_t address);
void xl_writeRegs(uint8_t address, uint8_t *buffer, uint8_t n);
void xl_readRegs(uint8_t address, uint8_t *buffer, uint8_t n);
void xl_readFIFO(uint8_t *buffer, uint16_t n);

#endif
