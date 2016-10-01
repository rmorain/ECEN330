/*
 * clockControl.c
 *
 *  Created on: Sep 28, 2016
 *      Author: cdmoo
 */
#include <stdio.h>
#include "clockDisplay.h"
#include "supportFiles/display.h"

#define TIMER_PERIOD 50 // the current period of each interrupt
// the maximum time in ms of the adc counter divided by the interrupt period
#define ADC_COUNTER_MAX (50 / TIMER_PERIOD)
// the maximum time in ms of the auto-increment counter divided by the interrupt period
#define AUTO_COUNTER_MAX (500 / TIMER_PERIOD)
// the maximum time in ms of the rate counter divided by the interrupt period
#define RATE_COUNTER_MAX (100 / TIMER_PERIOD)
// the maximum time in ms of the main clock counter (which advances the clock by 1 secondd once a second)
// divided by the interrupt period
#define MAIN_CLOCK_COUNTER_MAX (1000 / TIMER_PERIOD)

// States for the controller state machine.
enum clockControl_st_t {
    init_st,                 // Start here, stay in this state for just one tick.
    never_touched_st,        // Wait here until the first touch - clock is disabled until set.
    waiting_for_touch_st,    // waiting for touch, clock is enabled and running.
    adc_counter_running_st,     // waiting for the touch-controller ADC to settle.
    auto_counter_running_st,   // waiting for the auto-update delay to expire
                                 // (user is holding down button for auto-inc/dec)
    rate_counter_running_st,   // waiting for the rate-timer to expire to know when to perform the auto inc/dec.
    rate_counter_expired_st,   // when the rate-timer expires, perform the inc/dec function.
    add_second_to_clock_st   // add a second to the clock time and reset the ms counter.
} currentState = init_st;

// global variable storing the adc counter, which assures that the state machine pauses for 50 ms
// after a registered touch before checking for the location of the touch
static uint32_t adcCounter;
// global variable storing the auto counter, which assures that the state maching pauses for 500 ms
// after registering a touch before beginning the 10X advancement of the auto-increment sequence
static uint32_t autoCounter;
// the rate counter sets the rate of the autoincrement sequence, which occurs 10X a second
static uint32_t rateCounter;
// the mainClockCounter sets the rate of the main clock functionality, which advances the clock
// by 1 second every second
static uint32_t mainClockCounter;
// on each state transfer print off the new state for debugging
void debugStatePrint();

void clockControl_tick() {
  // Perform state action first.
  switch(currentState) {
    case init_st: // no state actions for the init state
      break;
    case never_touched_st:
        // state actions for the never touched state include setting all of the counters to 0
        // so the SM is in a clean state before it starts advancing time
        adcCounter = 0;
        autoCounter = 0;
        rateCounter = 0;
        mainClockCounter = 0;
        break;
    case waiting_for_touch_st:
        // similiar to the never touched state, while the SM is awaiting user input, the SM will
        // reset all of the counters to 0, besides the main clock counter
        adcCounter = 0;
        autoCounter = 0;
        rateCounter = 0;
        // always advance the mainClockCounter to keep advancing time
        mainClockCounter++;
        break;
    case adc_counter_running_st:
        // advance the adc counter will in the state so that it will timeout correctly
        adcCounter++;
        break;
    case auto_counter_running_st:
        // advance the auto-increment counter will in the state so that it will timeout correctly
        autoCounter++;
        break;
    case rate_counter_running_st:
        // advance the auto-increment rate counter will in the state so that it will timeout correctly
        rateCounter++;
        break;
    case rate_counter_expired_st:
        // once the auto-increment rate counter has timed out, reset it it to 0 so the cycle may begin again
        rateCounter = 0;
        break;
    case add_second_to_clock_st:
        // once the main clock counter has timed out, reset it to 0 so that the cycle of advancing time
        // every second may begin again
        mainClockCounter = 0;
        break;
    default:
        // catch any erroneous state transfers and print the results
        printf("clockControl_tick state action: hit default\n\r");
        break;
  }

  // Perform state update next.
  switch(currentState) {
    case init_st:
        // in the init state move directly to the never touched state after the first tick
        currentState = never_touched_st;
        break;
    case never_touched_st:
        // the never touched state waits for user input and makes no state transfers as long as
        // there is no user input
        if(display_isTouched()) {
            // if there is user input, clear the old touch data
            display_clearOldTouchData();
            // and transfer to the adc counter running state
            currentState = adc_counter_running_st;
        }
        break;
    case waiting_for_touch_st:
        // the waiting for touch state waits for user input and makes no state transfers as long as
        // there is no user input outside of advancing the time once per second
        if(display_isTouched()) {
            // if user input is detected
            display_clearOldTouchData();
            // transfer to the adc counter running state
            currentState = adc_counter_running_st;
        }
        else if(mainClockCounter >= MAIN_CLOCK_COUNTER_MAX) {
            // if there is no user input recieved, and the main clock timer maxes out,
            // advance the time by one second by transfering to the add second state
            currentState = add_second_to_clock_st;
        }
        break;
    case adc_counter_running_st:
        // if there is no user input in the adc counter state, wait for the adc counter to max out
        if(!display_isTouched() && adcCounter >= ADC_COUNTER_MAX) {
            // oncre the adc counter maxes out with no user input, that means that the user input was
            // short touch, which means that the timer should be incremented once and then returned to
            // the waiting for touch state
            clockDisplay_performIncDec();
            currentState = waiting_for_touch_st;
        }
        // if there is user input, that means that the user is holding down the touch
        else if(display_isTouched() && adcCounter >= ADC_COUNTER_MAX) {
            // if they hold down the touch long enough for the adc counter to max out,
            // enter the SM into the auto counter running state
            currentState = auto_counter_running_st;
        }
        break;
    case auto_counter_running_st:
        // if at any moment the user removes their touch from the screen during the auto counter
        // running state
        if(!display_isTouched()) {
            // perform one more incDec and transfer the SM to th waiting state
            clockDisplay_performIncDec();
            currentState = waiting_for_touch_st;
        }
        // if the user maintains their touch long enough for the auto counter to max out
        else if(display_isTouched() && autoCounter >= AUTO_COUNTER_MAX) {
            // enter the state into the fast autoincrement state, which a cycle beginning
            // in the rate counter running state
            clockDisplay_performIncDec();
            currentState = rate_counter_running_st;
        }
        break;
    case rate_counter_running_st:
        // if the user releases their touch
        if(!display_isTouched()) {
            // go back to waiting for input state
            currentState = waiting_for_touch_st;
        }
        // if the user maintains their touch and maxes out the rate counter
        else if(display_isTouched() && rateCounter >= RATE_COUNTER_MAX) {
            // transfer the SM into the rate counter expired state,
            // where the rate timer will be reset and an incDec will be performed
            currentState = rate_counter_expired_st;
        }
        break;
    case rate_counter_expired_st:
        // if the user releases their touch
        if(!display_isTouched()) {
            // transfer the SM to the waiting for input state
            currentState = waiting_for_touch_st;
        }
        else if(display_isTouched()) {
            // if the user maintains their touch, perform an incDec
            clockDisplay_performIncDec();
            // and transfer the SM back to the rate counter state
            currentState = rate_counter_running_st;
        }
        break;
    case add_second_to_clock_st:
        // maintains the incrementation of time once a second by advancing time
        // and then returning the SM back to the waiting for input state, where the
        // mainClockCounter will continue keeping time
        clockDisplay_advanceTimeOneSecond();
        currentState = waiting_for_touch_st;
        break;
    default:
        // catch any erroneous state transfers and print the results
        printf("clockControl_tick state update: hit default\n\r");
        break;
  }

  // on each state transfer print off the new state for debugging
  debugStatePrint();
}


// This is a debug state print routine. It will print the names of the states each
// time tick() is called. It only prints states if they are different than the
// previous state.
void debugStatePrint() {
  static clockControl_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false;                // previousState will be defined, firstPass is false.
    previousState = currentState;     // keep track of the last state that you were in.
    switch(currentState) {            // This prints messages based upon the state that you were in.
      case init_st:
        printf("init_st\n\r");
        break;
      case never_touched_st:
        printf("never_touched_st\n\r");
        break;
      case waiting_for_touch_st:
        printf("waiting_for_touch_st\n\r");
        break;
      case adc_counter_running_st:
        printf("adc_timer_running_st\n\r");
        break;
      case auto_counter_running_st:
        printf("auto_timer_running_st\n\r");
        break;
      case rate_counter_running_st:
        printf("rate_timer_running_st\n\r");
        break;
      case rate_counter_expired_st:
        printf("rate_timer_expired_st\n\r");
        break;
      case add_second_to_clock_st:
        printf("add_second_to_clock_state\n\r");
        break;
     }
  }
}


