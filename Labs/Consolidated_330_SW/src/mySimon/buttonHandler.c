/*
 * buttonHandler.c
 *
 *  Created on: Oct 30, 2016
 *      Author: cdmoo
 */
#include "buttonHandler.h"
#include "simonDisplay.h"
#include "supportFiles/display.h"
#include "supportFiles/utils.h"
#include "globals.h"
#include <stdio.h>

#define ADC_COUNTER_MAX (50 / TICK_PERIOD) // max time in ms for the adc counter
#define NO_ERASE false // constant for drawing squares without erase flag
#define ERASE true // constant for drawing squares with erase flag


static uint8_t regionNumber; // global to contain the last touched region number
static bool enableFlag; // global to determine if the SM should be ticking
static bool isReleaseDetected; // global that detects a release and stores true for one tick
static int16_t lastTouchX, lastTouchY; // two globals that contained the last touched coordinates
static uint8_t lastTouchZ; // global containing the last touched pressure
static void clearFlags(); // protoype of helper function that resets SM flags
static void debugStatePrint();

static uint16_t adcCounter; // global acting as a counter for the state that waits on the ADC to settle

// button handler states
typedef enum buttonHandler_st_t {
    init_st, // initial state
    waiting_for_touch_st, // waiting for user input
    adc_counter_running_st, // wiating for the ADC to settle so that location can be determined
    is_touching_st, // once user has touched, waiting for user to release
    end_st // waits here until SM is reset through disable() / enable()
} buttonHandler_st_t;
static buttonHandler_st_t currentState = init_st; // global holding the current state of the SM

// getter for last touched Region Number
uint8_t buttonHandler_getRegionNumber() {
    return regionNumber;
}

// allows other SMs to raise the enable flag, causing this SM to tick
void buttonHandler_enable() {
    enableFlag = true; // set the global enable flag to true
}

// allows other SMs to lower the enable flag, causing this SM to tick
void buttonHandler_disable() {
    enableFlag = false; // set the global enable flag to false
}

// allows other SMs to wait on a user release being detected
bool buttonHandler_releaseDetected() {
    return isReleaseDetected; // return that stores true when a release is detected
}
// SM tick function
void buttonHandler_tick() {
    // state action
    switch(currentState) {
        case init_st: // no action for the init state
            break;
        case waiting_for_touch_st:
            adcCounter = 0; // reset ADC counter to 0
            // reset this releaseDetected bool to false so that it stays true for only 1 tick
            isReleaseDetected = false;
            break;
        case adc_counter_running_st: // state action for the adc state
            adcCounter++;  // in the adc counter running state, inrement the adc counter
            break;
        case is_touching_st: // state action for the is touching state
            adcCounter = 0; // reset adc counter to 0
            break;
        case end_st: // no action for the end state
            break;
    }

    // transitions
    switch(currentState) {
        case init_st: // transition for init state
            if(enableFlag) { // begin ticking if enable flag is raised
                currentState = waiting_for_touch_st; // transition to waiting for touch state
            }
            break;
        case waiting_for_touch_st: // transitions for waiting for touch state
            // if the enable flag is taken down, reset teh state machine by transitioning to init state
            // and clearing the board
            if(!enableFlag) { // if the enable flag is lowered while the SM is ticking
                currentState = init_st; // return to init state
            }
            else if(display_isTouched()) {  // if a touch is detected
                display_clearOldTouchData(); // clear the old touch data
                // and transition to the adc counter state so that the location can be fetched
                currentState = adc_counter_running_st;
            }
            break;
        case adc_counter_running_st: // transitions for adc counter running state
            // if the enable flag is taken down, reset teh state machine by transitioning to init state
            // and clearing the board
            if(!enableFlag) {
                currentState = init_st;
            }
            // if the user stops pressing, return to waiting for touch state
            else if(!display_isTouched()) {
                currentState = waiting_for_touch_st;
            }
            // if the user is still pressing and the counter expires
            else if(adcCounter >= ADC_COUNTER_MAX) {
                display_getTouchedPoint(&lastTouchX, &lastTouchY, &lastTouchZ); // fetch the most recent touch data
                regionNumber = simonDisplay_computeRegionNumber(lastTouchX, lastTouchY); // get the region of the touch
                simonDisplay_drawSquare(regionNumber, NO_ERASE); // draw a sqaure in the appropriate region
                currentState = is_touching_st; // transition to is_touching_state
            }
            break;
        case is_touching_st: // transitions for is touching state
            // if the enable flag is taken down, reset teh state machine by transitioning to init state
            // and clearing the board
            if(!enableFlag) {
                simonDisplay_drawSquare(regionNumber, ERASE); // erase square
                currentState = init_st; // return to init state
            }
            // stay in the is_touching_st until the user is no longer touching
            else if(!display_isTouched()) {
                // one the user has stopped touching, erase the square
                simonDisplay_drawSquare(regionNumber, ERASE);
                simonDisplay_drawButton(regionNumber); // and draw the button in its place
                isReleaseDetected = true; // set the release detected to be true
                currentState = waiting_for_touch_st; // and transition to waiting for touch
            }
            break;
        case end_st: // end state transitions
            if(!enableFlag) { // stay in the end state until the enable flag is lowered
                clearFlags(); // once the SM is disabled, clear all flags and counters
                currentState = init_st; // and transition to the init state
            }
            break;
    }
}

// clear all flags to reset the state machine
static void clearFlags() {
    isReleaseDetected = false; // reset isReleaseDetected flag
    adcCounter = 0; // reset adc counter
}


