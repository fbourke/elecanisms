#include <p24FJ128GB206.h>
#include <stdint.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "pin.h"
#include "spi.h"
#include "uart.h"

uint8_t MPU_ACCEL_XOUT_H = 0x3B;
uint8_t MPU_ACCEL_XOUT_L = 0x3C;
uint8_t MPU_ACCEL_YOUT_H = 0x3D;
uint8_t MPU_ACCEL_YOUT_L = 0x3E;
uint8_t MPU_ACCEL_ZOUT_H = 0x3F;
uint8_t MPU_ACCEL_ZOUT_L = 0x40;

uint8_t MPU_GYRO_CONFIG = 0x1B;

uint8_t MPU_GYRO_XOUT_H = 0x43;
uint8_t MPU_GYRO_XOUT_L = 0x44;
uint8_t MPU_GYRO_YOUT_H = 0x45;
uint8_t MPU_GYRO_YOUT_L = 0x46;
uint8_t MPU_GYRO_ZOUT_H = 0x47;
uint8_t MPU_GYRO_ZOUT_L = 0x48;
uint8_t MPU_ACCEL_CONFIG = 0x1C;
uint8_t num_bytes = 6;

_PIN MPU9250_CSN, MPU9250_INT;
_PIN FOO_SCK, FOO_MISO, FOO_MOSI;

void mpu_readRegs(uint8_t address, uint8_t *buffer, uint8_t n) {
    uint8_t i;

    if (address+n<=0x7E) {
        pin_clear(&MPU9250_CSN);
        spi_transfer(&spi1, 0x80|address);
        for (i = 0; i<n; i++)
            buffer[i] = spi_transfer(&spi1, 0);
        pin_set(&MPU9250_CSN);
    } else {
        for (i = 0; i<n; i++)
            buffer[i] = 0xFF;
    }
}

void mpu_writeRegs(uint8_t address, uint8_t *buffer, uint8_t n) {
    uint8_t i;

    if (address+n<=0x7E) {
        pin_clear(&MPU9250_CSN);
        spi_transfer(&spi1, address);
        for (i = 0; i<n; i++)
            spi_transfer(&spi1, buffer[i]);
        pin_set(&MPU9250_CSN);
    }
}


int16_t main(void) {

	init_clock();
    init_ui();
    init_pin();    
    init_uart();  // Default is 19200 baud
    init_spi();

    pin_digitalOut(&D[3]);
    pin_digitalOut(&D[4]);
    pin_digitalOut(&D[13]);
	pin_init(&FOO_SCK, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)&ANSB, 9, 9, 8, 9, (uint16_t *)&RPOR4);
    pin_init(&FOO_MISO, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)&ANSB, 14, 14, 0, 14, (uint16_t *)&RPOR7);
    pin_init(&FOO_MOSI, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)&ANSB, 8, 8, 0, 8, (uint16_t *)&RPOR4);


   	pin_init(&MPU9250_CSN, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)NULL, 13, -1, 0, -1, (uint16_t *)NULL);
    pin_init(&MPU9250_INT, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
             (uint16_t *)NULL, 11, -1, 0, -1, (uint16_t *)NULL);



    pin_digitalOut(&MPU9250_CSN);
    pin_set(&MPU9250_CSN);


    spi_open(&spi1, &FOO_MISO, &FOO_MOSI, &FOO_SCK, 1e6);


    // # Set accel range to +/-2000 dps.
    uint8_t scale_2000 = 0x18;
    mpu_writeRegs(MPU_ACCEL_CONFIG, &scale_2000, 1);

    while(1) {
    	uint8_t values[6] = {0,0,0,0,0,0};
    	int32_t thresholdSq = 100000000;
		mpu_readRegs(MPU_ACCEL_XOUT_H, &values, 6);
		int16_t x = 256*values[0] + values[1];
        int16_t y = 256*values[2] + values[3]; 
        int16_t z = 256*values[4] + values[5];

        x = x>32767 ? x - 65536 : x;
        y = y>32767 ? y - 65536 : y;
        z = z>32767 ? z - 65536 : z;

        int32_t sumsq = pow((int32_t) x, 2) + pow((int32_t)y, 2) + pow((int32_t)z, 2);

        // if ((x > threshold && x < 255)
        //  || (y > threshold && y < 255)
        //  || (z > threshold && z < 255)) {
         if (sumsq > thresholdSq) {
            pin_set(&D[13]);
            led_on(&led1);
            led_on(&led2);
        }
        else {
            pin_clear(&D[13]);
            led_off(&led1);
            led_off(&led2);
        }

        printf("%li\n",sumsq);
	}
}