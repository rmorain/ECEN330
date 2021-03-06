/*
 * switches.h
 *
 *  Created on: Sep 10, 2016
 *      Author: cdmoo
 */

#ifndef SWITCHES_H_
#define SWITCHES_H_

#include "xil_io.h"
#include "supportFiles/leds.h"

#define SWITCHES_DATA_OFFSET 0x00 // offset from the base address to the switches register
#define SWITCHES_TRI_OFFSET 0x04 // offset from the base address to the triState register
#define SWITCHES_INIT_STATUS_OK 1 // exit code indicating successfull initialization
#define SWITCHES_INIT_STATUS_FAIL 0 // exit code indicating unsuccessfull initialization
#define SWITCHES_SW0_MASK 0x1 // button mask to return value of switch0
#define SWITCHES_SW1_MASK 0x2 // button mask to return value of switch1
#define SWITCHES_SW2_MASK 0x4 // button mask to return value of switch2
#define SWITCHES_SW3_MASK 0x8 // button mask to return value of switch3
#define SWITCHES_EXIT_CODE 0xF // button mask to return value of switch4

// Initializes the SWITCHES driver software and hardware. Returns one of the STATUS values defined above.
int32_t switches_init();

// Returns the current value of all 4 SWITCHESs as the lower 4 bits of the returned value.
// bit3 = SW3, bit2 = SW2, bit1 = SW1, bit0 = SW0.
int32_t switches_read();

// Runs a test of the switches. As you slide the switches, LEDs directly above the switches will illuminate.
// The test will run until all switches are slid upwards. When all 4 slide switches are slid upward,
// this function will return.
void switches_runTest();

#endif /* SWITCHES_H_ */
