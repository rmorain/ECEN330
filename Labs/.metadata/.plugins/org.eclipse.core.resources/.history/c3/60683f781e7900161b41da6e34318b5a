/*
 * buttons.h
 *
 *  Created on: Sep 10, 2016
 *      Author: cdmoo
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "xil_io.h"
#include "supportFiles/display.h"

#define BUTTONS_DATA_OFFSET 0x00 // offset from the base address to the gpioData register
#define BUTTONS_TRI_OFFSET 0x04 // offset from the base address to the gpioTriState register
#define BUTTONS_INIT_STATUS_OK 1 // successfull return code from init function
#define BUTTONS_INIT_STATUS_FAIL 0 // unSuccessfull return code from init function
#define BUTTONS_BTN0_MASK 0x1 // button mask to return value of button0
#define BUTTONS_BTN1_MASK 0x2 // button mask to return value of button1
#define BUTTONS_BTN2_MASK 0x4 // button mask to return value of button2
#define BUTTONS_BTN3_MASK 0x8 // button mask to return value of button3
// value indicating that all buttons are pressed and that the testButton function should return
#define BUTTONS_EXIT_CODE 0xF

// Initializes the button driver software and hardware. Returns one of the defined status values (above).
int32_t buttons_init();

// Returns the current value of all 4 buttons as the lower 4 bits of the returned value.
// bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
int32_t buttons_read();

// Runs a test of the buttons. As you push the buttons, graphics and messages will be written to the LCD
// panel. The test will until all 4 pushbuttons are simultaneously pressed.
void buttons_runTest();



#endif /* BUTTONS_H_ */
