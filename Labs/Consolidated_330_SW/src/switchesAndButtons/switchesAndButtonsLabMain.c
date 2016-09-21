/*
 * switchesAndButtonsLabMain.c
 *
 *  Created on: Sep 10, 2016
 *      Author: cdmoo
 */
#include "switches.h"
#include "buttons.h"

int main() {
    // enters the program into the switch loop
    switches_runTest();

    // when the program has exited from the switch loop,
    // it will enter into the buttons loop
    buttons_runTest();
}

// This function must be defined but can be left empty for now.
// You will use this function in a later lab.
// It is called in the timer interrupt service routine (see interrupts.c in supportFiles).
void isr_function() {
    // Empty for now.
}
