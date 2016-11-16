/*
 * verifySequence.c
 *
 *  Created on: Nov 5, 2016
 *      Author: cdmoo
 */

#include "verifySequence.h"
#include "buttonHandler.h"
#include "globals.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

// the maximum time the sequence will wiat for user input before registering a timeout error
#define TIMEOUT_COUNTER_MAX (2000 / TICK_PERIOD)

// global flag that causes the state machine to be stalled at the end / init state until another SM
// raises the flag, allowing the verify sequence SM to begin ticking
static bool enableFlag = false;
static bool isTimeOutError = false; // global storing whether or not the user has committed a timeout error
static bool isUserInputError = false; // global storing whether or not the user has committed an input error
// counter used in the verify state, keeping track of how long the SM has been waiting for user input
static uint16_t timeoutCounter = 0;
static uint8_t lastTouchedRegionNumber; // global keeping track of the last region the user touched

// stores the sequence iteration length so that the verify sequence SM can verify that the user has reached the end
// of the sequence successfully
static uint16_t sequenceIterationLength = 0;
// keeps track of where the user is at in the sequence so that the SM can verify that the user has touched the correct button
static uint16_t currentSequenceIndex = 0;

static void resetFlags(); // prototype for a helper function that resets all flags in the SM

typedef enum {
    init_st,
    waiting_user_input_st,
    verify_user_input_st,
    end_st
} verifySequence_state_t;

static verifySequence_state_t currentState = init_st;

// State machine will run when enabled.
void verifySequence_enable() {
    enableFlag = true;
}

// This is part of the interlock. You disable the state-machine and then enable it again.
void verifySequence_disable() {
    enableFlag = false;
}

// Used to detect if there has been a time-out error.
bool verifySequence_isTimeOutError() {
    return isTimeOutError;
}

// Used to detect if the user tapped the incorrect sequence.
bool verifySequence_isUserInputError() {
    return isUserInputError;
}

// Used to detect if the verifySequence state machine has finished verifying.
bool verifySequence_isComplete() {
    return currentState == end_st;
}

// Standard tick function.
void verifySequence_tick() {
    // state actions
    switch(currentState) {
        case init_st: // no state actions for the init state
            break;
        case waiting_user_input_st: // state actions for the waiting on user input state
            timeoutCounter++; // increment the timeout counter
            break;
        case verify_user_input_st: // state actions for the verify user input state
            timeoutCounter = 0; // reset the timeout coutner back to 0
            break;
        case end_st: // no state actions for the end state
            break;
    }

    // transitions
    switch(currentState) {
        case init_st: // transitions for the init state
            // stay in the init state until the enable flag is raised
            if(enableFlag) {
                // once the enable flag is raised, enable the button handler state machine to respond to user input
                buttonHandler_enable();
                // get the current sequence iteration length from the global module
                sequenceIterationLength = globals_getSequenceIterationLength();
                currentState = waiting_user_input_st; // transition to the waiting for user input state
            }
            break;
        case waiting_user_input_st: // transitions for the wiating on user input state
            // transition out of the state if the timeout counter is expired, indicating that the user took too long
            // to respond
            if(timeoutCounter > TIMEOUT_COUNTER_MAX) {
                isTimeOutError = true; // set the timoutError flag to true
                buttonHandler_disable(); // disbale the button handler
                currentState = end_st; // transition to end state
            // if the button handler detects a user release, the user has responded and the SM should transition
            // to a state that can handle the response
            } else if(buttonHandler_releaseDetected()) {
                // set the lastTouchedRegion global to the value detected by the buttonHandler SM
                lastTouchedRegionNumber = buttonHandler_getRegionNumber();
                buttonHandler_disable(); // disable the button handler SM
                currentState = verify_user_input_st; // transition to the verify input state
            }
            break;
        case verify_user_input_st: // verify user input state transitions
            // first check if user input is correct by comparing the touch region to the sequence region
            if(lastTouchedRegionNumber == globals_getSequenceValue(currentSequenceIndex++)) {
                // if the user input was correct
                // first check to see if the sequence is over and successfully completed
                if(currentSequenceIndex < sequenceIterationLength) {
                    // if the sequence is not over, go back to wiating on user input
                    buttonHandler_enable(); // re enable the buttonHandler SM
                    currentState = waiting_user_input_st; // transition to the user input state
                } else {
                    // if the sequence is over, disable the button handler SM
                    buttonHandler_disable();
                    currentState = end_st; // transition to end state
                }
            // if the user input is incorrect
            } else {
                // disable the buttonHandlerSM
                buttonHandler_disable();
                isUserInputError = true; // set the inputError global to true
                currentState = end_st; // transition to the end state
            }
            break;
        case end_st:
            // stay in the end state until the enable flag is lowered (by calling disable() )
            if(!enableFlag) {
                resetFlags(); // once the SM is disabled, reset the flags
                currentState = init_st; // transition to the init state
            }
            break;
    }
}

// helper function that resets the stateful components of the SM for when the SM needs to be restarted
static void resetFlags() {
    sequenceIterationLength = 0; // set the sequence iteration back to 0
    currentSequenceIndex = 0; // reset the current sequence index to 0
    isTimeOutError = false; // reset the error flags
    isUserInputError = false; // reset the error flags
    timeoutCounter = 0; // reset the timeout counter back to 0
}
