/*
 * simonMain.c
 *
 *  Created on: Oct 30, 2016
 *      Author: cdmoo
 */
#include <stdio.h>
#include "simonDisplay.h"
#include "buttonHandler.h"
#include "supportFiles/display.h"
#include "flashSequence.h"
#include "verifySequence.h"
#include "simonControl.h"
#include "supportFiles/utils.h"
#include "../intervalTimer/intervalTimer.h"
#include <stdio.h>
#include "supportFiles/leds.h"
#include "supportFiles/globalTimer.h"
#include "supportFiles/interrupts.h"
#include <stdbool.h>
#include <stdint.h>
#include "supportFiles/display.h"
#include "xparameters.h"
#include "globals.h"

typedef void (functionPointer_t)();
#define MAX_DURATION_TIMER_INDEX 2
static double maxDuration_g;                    // The max duration across all ticks.
static const char* maxDurationFunctionName_g;   // Name of the function with longest duration is here.
#define SIMON_CONTROL_TICK "simonControl_tick()" // used for debugging the slowest tick function
#define VERIFY_SEQUENCE_TICK "verifySequence_tick()" // used for debugging the slowest tick function
#define FLASH_SEQUENCE_TICK "flashSequency_tick()" // used for debugging the slowest tick function
#define BUTTON_HANDLER_TICK "buttonHandler_tick()" // used for debugging the slowest tick function

// This assumes that the interval timer was initialized.
static void tickTimer(functionPointer_t* fp,
                      const char* functionName) {
    intervalTimer_reset(MAX_DURATION_TIMER_INDEX);  // Reset the timer.
    intervalTimer_start(MAX_DURATION_TIMER_INDEX);  // Start the timer.
    fp();                                           // Invoke the tick function passed in by argument.
    intervalTimer_stop(MAX_DURATION_TIMER_INDEX);   // Stop the timer.
    double duration;                                // Keep track of tick duration.
    duration = intervalTimer_getTotalDurationInSeconds(MAX_DURATION_TIMER_INDEX);  // Get the duration.
    if (maxDuration_g < duration) {                 // Keep the max duration.
        maxDuration_g = duration;
        maxDurationFunctionName_g = functionName;   // Keep track of which function had the max. duration.
    }
}


/************ Flag Method ***********
************************************/

#define TOTAL_SECONDS 90
// The formula for computing the load value is based upon the formula from 4.1.1 (calculating timer intervals)
// in the Cortex-A9 MPCore Technical Reference Manual 4-2.
// Assuming that the prescaler = 0, the formula for computing the load value based upon the desired period is:
// load-value = (period * timer-clock) - 1
#define TIMER_PERIOD (TICK_PERIOD * .001)
#define TIMER_CLOCK_FREQUENCY (XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2)
#define TIMER_LOAD_VALUE ((TIMER_PERIOD * TIMER_CLOCK_FREQUENCY) - 1.0)

int main()
{
    // Initialize the GPIO LED driver and print out an error message if it fails (argument = true).
       // You need to init the LEDs so that LD4 can function as a heartbeat.
    leds_init(true);
    // Init all interrupts (but does not enable the interrupts at the devices).
    // Prints an error message if an internal failure occurs because the argument = true.
    interrupts_initAll(true);
    interrupts_setPrivateTimerLoadValue(TIMER_LOAD_VALUE);
    u32 privateTimerTicksPerSecond = interrupts_getPrivateTimerTicksPerSecond();
    printf("private timer ticks per second: %ld\n\r", privateTimerTicksPerSecond);
    // Allow the timer to generate interrupts.
    interrupts_enableTimerGlobalInts();
    display_init(); // this task is not time dependent, so we will do it before we start ticking
    display_fillScreen(DISPLAY_BLACK);    // Clear the display.
    // Keep track of your personal interrupt count. Want to make sure that you don't miss any interrupts.
     int32_t personalInterruptCount = 0;
    // Start the private ARM timer running.
    interrupts_startArmPrivateTimer();
    // Enable interrupts at the ARM.
    interrupts_enableArmInts();
    // interrupts_isrInvocationCount() returns the number of times that the timer ISR was invoked.
    // This value is maintained by the timer ISR. Compare this number with your own local
    // interrupt count to determine if you have missed any interrupts.
     while (interrupts_isrInvocationCount() < (TOTAL_SECONDS * privateTimerTicksPerSecond)) {
      if (interrupts_isrFlagGlobal) {  // This is a global flag that is set by the timer interrupt handler.
          // Count ticks.
          personalInterruptCount++;
          tickTimer(flashSequence_tick, FLASH_SEQUENCE_TICK);    // Tick a state machine.
          tickTimer(verifySequence_tick, VERIFY_SEQUENCE_TICK);  // Tick a state machine.
          tickTimer(buttonHandler_tick, BUTTON_HANDLER_TICK);    // Tick a state machine.
          tickTimer(simonControl_tick, SIMON_CONTROL_TICK);      // Tick a state machine.
          interrupts_isrFlagGlobal = 0;
      }
   }
   interrupts_disableArmInts();
   printf("isr invocation count: %ld\n\r", interrupts_isrInvocationCount()); // print the total interrupts
   printf("internal interrupt count: %ld\n\r", personalInterruptCount); // print the number of interrups detected by my SM
   printf("Max duration: %s %f\n", maxDurationFunctionName_g, maxDuration_g); // print the slowest tick function and its tick time
   return 0;
}

void isr_function(){}


