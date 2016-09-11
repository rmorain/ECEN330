/*
 * switches.c
 *
 *  Created on: Sep 10, 2016
 *      Author: cdmoo
 */

#include "switches.h"
#include "supportFiles/leds.h"
#include <stdio.h>

//Helper function to read GPIO registers
int32_t switches_readGpioRegister(int32_t offset) {
    return Xil_In32(XPAR_SLIDE_SWITCHES_BASEADDR + offset);
}

// initializes all parts of the board required by the switches
int32_t switches_init() {
    // initializes leds for use by switches
    leds_init(1);
    return SWITCHES_INIT_STATUS_OK;
}

// reads the value of the switches register
int32_t switches_read() {
    // calls the readGpio function with the offset corresponsing to the switches register
    return switches_readGpioRegister(SWITCHES_DATA_OFFSET);
}

void switches_runTest() {
    switches_init();

    // continually read the state of the switches and update the leds until all switches are on
    while(1) {
        int32_t switches_state = switches_read();

        // write the leds according to the current state (in hex) of the switches
        leds_write(switches_state);

        // if the all of the switches are up, exit the function
        if(switches_state == SWITCHES_EXIT_CODE) {
            // clear the leds before exiting
            leds_write(0);
            return;
        }
    }
}





