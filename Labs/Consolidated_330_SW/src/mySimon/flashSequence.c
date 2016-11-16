/*
 * flashSequence.c
 *
 *  Created on: Nov 4, 2016
 *      Author: cdmoo
 */
#include "flashSequence.h"
#include "globals.h"
#include "simonDisplay.h"
#include "supportFiles/display.h"
#include "supportFiles/utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define DISPLAY_COUNTER_MAX (500 / TICK_PERIOD) // limits the display of each square to a designated maximum
#define ERASE_SQUARE true // flag for drawing true color of a square
#define DRAW_SQUARE false // flag for drawing square as black to erase it

// states of the flash sequence SM
typedef enum {
    init_st, // initial state (will wait here until enabled)
    display_new_square_st, // state in charge of drawing a new square
    display_counter_running_st, // state responsable for timing the duration of each square
    end_st // end state (will wait here until disabled)
} flashSequence_state_t;

static flashSequence_state_t currentState = init_st; // initial state of the SM is init_st
static bool enableFlag = false; // this flag controls whether or not the SM stays in init / end state
static uint16_t displayCounter = 0; // counter for the display_counter_running_st
static uint16_t sequenceIterationLength = 0; // the number of elements in the current flash sequence
static uint16_t currentSequenceIndex = 0; // global that contains the current index of the flash sequence

// Turns on the state machine. Part of the interlock.
void flashSequence_enable() {
    enableFlag = true; // raise the enable flag
}

// Turns off the state machine. Part of the interlock.
void flashSequence_disable() {
    enableFlag = false; // lower the disable flag
}

// Other state machines can call this to determine if this state machine is finished.
bool flashSequence_isComplete() {
    return currentState == end_st; // return true if the SM is in the end_st
}

// Standard tick function.
void flashSequence_tick() {
    // state actions first
    switch(currentState) {
        case init_st: // no state actions for init state
            break;
        case display_new_square_st: // state actions for the display_new_square_st
            // draw the square corresponding to the sequence value at the current index
            simonDisplay_drawSquare(globals_getSequenceValue(currentSequenceIndex), DRAW_SQUARE);
            break;
        case display_counter_running_st: // state actions for the dislpay_counter_running_st
            displayCounter++; // increment the counter associtated with this state
            break;
        case end_st: // no state actions for the end state
            break;
    }

    // state transitions
    switch(currentState) {
        case init_st:
            // stay in the init state until the enable flag is raised
            if(enableFlag) { // if the enable flag is raised
                // get the new squence iteration length
                sequenceIterationLength = globals_getSequenceIterationLength();
                // transition to display new square
                currentState = display_new_square_st;
            }
            break;
        case display_new_square_st:
            // stay in this state for only one tick, transitioning to display_counter_running_state
            currentState = display_counter_running_st;
            break;
        case display_counter_running_st:
            // stay in the displaying state until the display counter expires
            if(displayCounter > DISPLAY_COUNTER_MAX) { // once the display counter expires
                displayCounter = 0; // reset the counter to 0
                // erase the square that was previously drawn
                simonDisplay_drawSquare(globals_getSequenceValue(currentSequenceIndex), ERASE_SQUARE);
                // increase the current sequence index in order to draw the next square
                currentSequenceIndex++;
                // check to see if the current index is greater than the length of the sequence
                if(currentSequenceIndex >= sequenceIterationLength) {
                    // if it is, that means that that the sequence has finished flashing, transition to end state
                    currentState = end_st;
                } else {
                    // otherwise, go back and display the enxt square
                    currentState = display_new_square_st;
                }
            }
            break;
        case end_st:
            // if enable flag is taken down, reset the counters and wait for the state machine to be re-enabled
            if(!enableFlag) {
                sequenceIterationLength = 0; // reset sequence iteration length
                currentSequenceIndex = 0; // reset the current index in the sequence
                currentState = init_st; // transition to the init state
            }
            break;
    }
}





