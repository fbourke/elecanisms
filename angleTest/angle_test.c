#include <p24FJ128GB206.h>
#include <math.h>
#include <stdio.h>
#include "config.h"
#include "common.h"
#include "pin.h"
#include "timer.h"
#include "uart.h"
#include "ui.h"

_PIN *sensorPosPin = &A[3];
uint8_t string[40];

// Position tracking variables
int updatedPos = 0;     // keeps track of the latest updated value of the MR sensor reading
int rawPos = 0;         // current raw reading from MR sensor
int lastRawPos = 0;     // last raw reading from MR sensor
int lastLastRawPos = 0; // last last raw reading from MR sensor
int flipNumber = 0;     // keeps track of the number of flips over the 180deg mark
int tempOffset = 0;
int rawDiff = 0;
int lastRawDiff = 0;
int rawOffset = 0;
int lastRawOffset = 0;
const int flipThresh = 700;  // threshold to determine whether or not a flip over the 180 degree mark occurred
int flipped = 0;

void setup() {
    pin_analogIn(sensorPosPin);
    lastLastRawPos = pin_read(sensorPosPin);
    lastRawPos = pin_read(sensorPosPin);
}

int16_t main(void) {
    init_clock();
    init_pin();
    init_timer();
    init_uart();  // Default is 19200 baud
    init_ui();
    setup();

    timer_setPeriod(&timer1, 0.05);
    timer_start(&timer1);

    while (1) {
        // Get voltage output by MR sensor
        rawPos = pin_read(sensorPosPin) >> 6;

        // Calculate differences between subsequent MR sensor readings
        rawDiff = rawPos - lastRawPos;
        lastRawDiff = rawPos - lastLastRawPos;
        rawOffset = abs(rawDiff);
        lastRawOffset = abs(lastRawDiff);

        // Update position record-keeping vairables
        lastLastRawPos = lastRawPos;
        lastRawPos = rawPos;

        // Keep track of flips over 180 degrees
        if((lastRawOffset > flipThresh) && (flipped == 0)) { // enter this anytime the last offset is greater than the flip threshold AND it has not just flipped
            printf("rawDiff: %d, lastRawDiff: %d\n\n", rawDiff, lastRawDiff);
            if(lastRawDiff > 0) { flipNumber--; }
            else { flipNumber++; }

            if(rawOffset > flipThresh) { // check to see if the data was good and the most current offset is above the threshold
              updatedPos = rawPos + flipNumber*rawOffset; // update the pos value to account for flips over 180deg using the most current offset 
              tempOffset = rawOffset;
            }
            else {                     // in this case there was a blip in the data and we want to use lastactualOffset instead
              updatedPos = rawPos + flipNumber*lastRawOffset;  // update the pos value to account for any flips over 180deg using the LAST offset
              tempOffset = lastRawOffset;
            }
            flipped = 1;
        }
        else {
            updatedPos = rawPos + flipNumber*tempOffset; // need to update pos based on what most recent offset is 
            flipped = 0;
        }

        if (timer_flag(&timer1)) {
            timer_lower(&timer1);
            printf("updatedPos: %d, flipNumber: %d\n", updatedPos, flipNumber);
        }
    }
}
