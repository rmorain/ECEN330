/*
 * simonControl.c
 *
 *  Created on: Nov 10, 2016
 *      Author: cdmoo
 */
#include "simonControl.h"
#include "globals.h"
#include "supportFiles/display.h"
#include "flashSequence.h"
#include "verifySequence.h"
#include "simonDisplay.h"
#include "buttonHandler.h"
#include "supportFiles/utils.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define SEQUENCE_MAX 100  // maximum length of the simon sequence
#define NUMBER_OF_REGIONS 4 // number of touch regions on the game board
#define STARTING_SESSION_SEQUENCE_LENGTH 4 // when the game restarts, this is the length of the sequence
#define STARTING_ITERATION_SEQUENCE_LENGTH 1 // starting length of the the current sequence iteration
#define YAY_WIN_COUNTER_MAX (1500 / TICK_PERIOD) // win state max duration
#define NEW_LEVEL_COUNTER_MAX (3500 / TICK_PERIOD) // new level screen max duration
#define LOSE_MESSAGE_COUNTER_MAX (1500 / TICK_PERIOD) // lose screen max duration
#define LONGEST_SEQUENCE_MESSAGE_COUNTER_MAX (2500 / TICK_PERIOD) // longest sequence message screen max duration
#define TEXT_SIZE_LARGE 4 // largest size of text, used for the game title in the splash screen
#define TEXT_SIZE_SMALL 2 // smallest size of text
#define SPLASH_SCREEN_CURSOR_X 50 // cursor marking the splash screen title
#define SPLASH_SCREEN_CURSOR_Y 70 // cursor marking the splash screen title
#define MESSAGE_SCREEN_CURSOR_X 10 // cursor marking a generic message screen text
#define MESSAGE_SCREEN_CURSOR_Y 100 // cursor marking a generic message screen text
#define ERASE true // flag to draw text in black (erasing it)
#define NO_ERASE false // flag to draw text in white (erasing it)
#define LONGEST_SEQ_MESSAGE_LENGTH 22 // length of the string holding the longest sequence message
#define SEQUENCE_LENGTH_OFFSET 1 // used to calculate the current longest sequence when a user breaks their record
#define TEST_TICK_PERIOD_MS 50 // the tick period in miliseconds used for testing


// control SM states
typedef enum {
    init_st, // beginning state, stays here until enabled
    splash_screen_st, // displays the splash screen and waits for user touch
    flashing_sequence_st, // flashes the current sequence iteration to the user
    verifying_sequence_st, // waits for user to repeat the sequence and checks its validity
    yay_win_st, // if the user completes the current session sequence, they are presented with this win message screen
    new_level_feedback_st, // after the win screen, this screen waits for the user to decide if they want a new session
    lose_st, // if there is an error in validation, the user is presented with this lose message screen
    show_longest_sequence_st // before starting a new session, the user is shown this screen with their longest sequence
} simonControl_st_t;

// initialize globals
static simonControl_st_t currentState = init_st; // set the beginning game state to init_st
static uint32_t splashScreenCounter = 0; // set the splash screen counter to 0
// set the iteration sequence length to its starting value
static uint8_t iterationSequenceLength = STARTING_ITERATION_SEQUENCE_LENGTH;
// set the starting session sequence length to its starting value
static uint8_t sessionSequenceLength = STARTING_SESSION_SEQUENCE_LENGTH;
static uint8_t longestSequenceLength = 0; // clear the longest sequence length to -

static void drawSplashScreen(bool); // prototype for a draw splash screen helper function
static void drawWinScreen(bool); // prototype for the draw win screen helper function
static void drawNewLevelFeedbackScreen(bool); // prototype for the draw new level screen helper function
static void drawLongestSequenceScreen(bool erase); // prototype for the draw longest sequence screen helper function
static void drawLoseScreen(bool erase); // prototype for the draw lose screen helper function
static void updateLongestSequenceLength(); // prototype for a helper function that updates the longest sequence record


// intialize state counters to 0
static uint16_t yayWinCounter = 0; // set the win state counter to 0
static uint16_t newLevelCounter = 0; // set the new level counter to 0
static uint16_t loseMessageCounter = 0; // set the lose state counter to 0
static uint16_t longestSequenceMessageCounter = 0; // set the longest message state counter to 0

// creates a new sequence and sets the global sequence to its result
static void generateNewSequence() {
    uint8_t tempSequence[SEQUENCE_MAX]; // creates a temporary array holding the generated sequence
    // iterate through the array, adding a random int to each location
    for(int i = 0; i < SEQUENCE_MAX; i++) {
        tempSequence[i] = rand() % NUMBER_OF_REGIONS; // add a random number % 4 to each location
    }
    globals_setSequence(tempSequence, SEQUENCE_MAX); // pass this sequence to the global module
}

// SM tick function
void simonControl_tick() {
    // state actions
    switch(currentState) {
        case init_st: // no state actions for init state
            break;
        case splash_screen_st: // state actions for splash screen state
            splashScreenCounter++; // increment the splash screen counter
            break;
        case flashing_sequence_st: // no state actions for flash sequence state
            break;
        case verifying_sequence_st: // no state actions for verify sequence state
            break;
        case yay_win_st: // win screen state actions
            yayWinCounter++; // increment the win screen counter
            break;
        case new_level_feedback_st: // new level state actions
            newLevelCounter++; // increment the new level counter
            break;
        case lose_st: // lose state actions
            loseMessageCounter++; // increment the lose state actions
            break;
        case show_longest_sequence_st: // longest sequence state actions
            longestSequenceMessageCounter++; // increment the longest sequence counter
            break;
    }

    // state transitions
    switch(currentState) {
        case init_st: // only stay in the init state for one tick
            drawSplashScreen(NO_ERASE); // draw the splash screen
            currentState = splash_screen_st; // transition to splash screen state
            break;
        case splash_screen_st:
            // stay in the splash screen as long as their is no user input
            if(display_isTouched()) { // if the user touhces the display
                // seed the random generator with the splash screen counter, because this depends on
                // user interation, it is effectively seeded with a random number, guarenteeing that the
                // sequence will be random each time the user plays
                srand(splashScreenCounter);
                drawSplashScreen(ERASE); // erase the splash screen
                splashScreenCounter = 0; // reset the splash screen counter to 0
                generateNewSequence(); // generate a new sequence for this session
                // reset the iteration sequence length to its starting value
                iterationSequenceLength = STARTING_ITERATION_SEQUENCE_LENGTH;
                // reset teh session sequence length to its starting value
                sessionSequenceLength = STARTING_SESSION_SEQUENCE_LENGTH;
                // pass the current iteration length to the global module
                globals_setSequenceIterationLength(iterationSequenceLength);
                flashSequence_enable(); // enable the flash sequence SM, allowing it to start ticking
                currentState = flashing_sequence_st; // transition to the flash sequence state
            }
            break;
        case flashing_sequence_st:
            // stay in the flash sequence state until the flash sequence SM is done
            if(flashSequence_isComplete()) {
                // once the flash sequence SM is complete, disable it
                flashSequence_disable();
                // enable the verify sequence SM, allowing it to start ticking
                verifySequence_enable();
                // draw all the buttons
                simonDisplay_drawAllButtons();
                currentState = verifying_sequence_st; // transition to the verify sequence state
            }
            break;
        case verifying_sequence_st:
            // stay in the verify sequence state until the verify sequence SM is complete
            if(verifySequence_isComplete()) {
                // once the verify sequence SM is complete, disbale the verify sequence SM
                verifySequence_disable();
                simonDisplay_eraseAllButtons(); // erase all the buttons
                // check the verify SM for errors
                if(verifySequence_isTimeOutError() || verifySequence_isUserInputError()) {
                    // if there are errors, draw the lose screen
                    drawLoseScreen(NO_ERASE);
                    currentState = lose_st; // and transition to the lose state
                // if there are no errors but the user has passed the level
                } else if(iterationSequenceLength >= sessionSequenceLength) {
                    drawWinScreen(NO_ERASE); // draw the win screen message
                    updateLongestSequenceLength(); // update the longest sequence record for the user
                    currentState = yay_win_st; // transition to win state
                } else {
                    // otherwise, assume that the user is still in the current level
                    // and increment and set the new iteration length
                    globals_setSequenceIterationLength(++iterationSequenceLength);
                    flashSequence_enable(); // re enable the flash sequence SM
                    updateLongestSequenceLength(); // update the lognest sequence record for the user
                    currentState = flashing_sequence_st; // transition to flashing sequence state
                }
            }
            break;
        case yay_win_st:
            // stay in the win message state until its counter expires
            if(yayWinCounter > YAY_WIN_COUNTER_MAX) {
                // win the counter expires, reset the counter to 0
                yayWinCounter = 0;
                drawWinScreen(ERASE); // erase the win message
                drawNewLevelFeedbackScreen(NO_ERASE); // draw the new level feedback screen
                currentState = new_level_feedback_st; // transition to the new level feedback state
            }
            break;
        case new_level_feedback_st:
            // stay in level state until one of two conditions is satisfied
            // first, if the screen counter maxes out, indicating the user is done with their session
            if(newLevelCounter > NEW_LEVEL_COUNTER_MAX) {
                newLevelCounter = 0; // reset the new level screen counter
                drawNewLevelFeedbackScreen(ERASE); // erase the new level screen text
                drawLongestSequenceScreen(NO_ERASE); // draw the next screen (longest sequence)
                currentState = show_longest_sequence_st; // transition to longest sequence screen
            // second, transition if the user touches the screen, indicating they want to start a new level
            } else if(display_isTouched()) {
                newLevelCounter = 0; // reset the the counter to 0
                drawNewLevelFeedbackScreen(ERASE); // erase the current screen text
                iterationSequenceLength = STARTING_ITERATION_SEQUENCE_LENGTH; // reset the iterationlength for the next session
                globals_setSequenceIterationLength(iterationSequenceLength); //reset the global iteration length for the net session
                sessionSequenceLength++; // increment the counter controlling the session sequence length
                generateNewSequence(); // generate a fresh sequence for the next session
                flashSequence_enable(); // enable the flash sequence SM, allowing it to start ticking
                currentState = flashing_sequence_st; // transition to the flash sequence state
            }
            break;
        case lose_st:
            // stay in the lose message state until the counter expires
            if(loseMessageCounter > LOSE_MESSAGE_COUNTER_MAX) {
                // once the counter expires, reset the counter to 0
                loseMessageCounter = 0;
                drawLoseScreen(ERASE); // erase the screen text
                drawLongestSequenceScreen(NO_ERASE); // draw the longest sequence screen (next state)
                currentState = show_longest_sequence_st; // transition to longest sequence state
            }
            break;
        case show_longest_sequence_st:
            // stay in the longest sequence message state until the counter expires
            if(longestSequenceMessageCounter > LONGEST_SEQUENCE_MESSAGE_COUNTER_MAX) {
                // once the counter expires reset the longest sequence counter to 0
                longestSequenceMessageCounter = 0;
                longestSequenceLength = 0; // reset the longest sequence length to 0
                drawLongestSequenceScreen(ERASE); // erase the longest sequence screen text
                drawSplashScreen(NO_ERASE); // draw the splash screen
                currentState = splash_screen_st; // transition to the splash screen state
            }
            break;
    }
}

// helper function to draw the splash screen, bool erase passed in to determine if
// screen should be drawn or erased
static void drawSplashScreen(bool erase) {
    // if the erase bool was true, set the text color to black so that it will effectively erase the text,
    // otherwhise, set the text color to white
    erase ? display_setTextColor(DISPLAY_BLACK) : display_setTextColor(DISPLAY_WHITE);
    display_setTextSize(TEXT_SIZE_LARGE); // set text size to large for the simon title
    display_setCursor(SPLASH_SCREEN_CURSOR_X, SPLASH_SCREEN_CURSOR_Y); // set up the correct cursor for the title
    display_println("  SIMON\n"); // print the large simon title
    display_setTextSize(TEXT_SIZE_SMALL); // set the text size to small for the lower line of the splash screen
    display_println("      touch to start"); // print the lower line
}

// helper function to draw the win screen
static void drawWinScreen(bool erase) {
    // if the erase bool was true, set the text color to black so that it will effectively erase the text,
    // otherwhise, set the text color to white
    erase ? display_setTextColor(DISPLAY_BLACK) : display_setTextColor(DISPLAY_WHITE);
    display_setTextSize(TEXT_SIZE_SMALL); // set the appropriate text size
    display_setCursor(MESSAGE_SCREEN_CURSOR_X, MESSAGE_SCREEN_CURSOR_Y); // set the cursor to the appropriate place for this message
    display_println("Yay! you win"); // print the win screen message
}

// helper function to draw the new level screen
static void drawNewLevelFeedbackScreen(bool erase) {
    // if the erase bool was true, set the text color to black so that it will effectively erase the text,
    // otherwhise, set the text color to white
    erase ? display_setTextColor(DISPLAY_BLACK) : display_setTextColor(DISPLAY_WHITE);
    display_setTextSize(TEXT_SIZE_SMALL); // set the appropriate text size
    display_setCursor(MESSAGE_SCREEN_CURSOR_X, MESSAGE_SCREEN_CURSOR_Y); // set the cursor to the appropriate place for this message
    display_println("Touch to start new level"); // print the new level screen message
}

// helper function to draw the longest sequence screen
static void drawLongestSequenceScreen(bool erase) {
    // since this function must be responsable for drawing the current score but erasing the previous score
    // stoer the last printed as a static variable so that it can be used for erasing
    static uint16_t lastPrinted;
    char message[LONGEST_SEQ_MESSAGE_LENGTH]; // string containing the message
    if(!erase) {
        // if the message is not to be erased, concatenate the longest sequence into its message
        sprintf(message, "Longest Sequence: %d\n", longestSequenceLength);
        lastPrinted = longestSequenceLength; // and update the last printed (so that it may be erased in the future)
    } else {
        // if the message is to be erased, concatenate the last printed score into the message
        sprintf(message, "Longest Sequence: %d\n", lastPrinted);
    }
    // if the erase bool was true, set the text color to black so that it will effectively erase the text,
    // otherwhise, set the text color to white
    erase ? display_setTextColor(DISPLAY_BLACK) : display_setTextColor(DISPLAY_WHITE);
    display_setTextSize(TEXT_SIZE_SMALL); // set the appropriate text size
    display_setCursor(MESSAGE_SCREEN_CURSOR_X, MESSAGE_SCREEN_CURSOR_Y); // set the cursor to the appropriate place for this message
    display_println(message); // print the longest sequence message to the screen
}

// helper function to draw the lose screen
static void drawLoseScreen(bool erase) {
    // since this function must be responsable for drawing the current score but erasing the previous score
    // stoer the last printed as a static variable so that it can be used for erasing
    erase ? display_setTextColor(DISPLAY_BLACK) : display_setTextColor(DISPLAY_WHITE);
    display_setTextSize(TEXT_SIZE_SMALL); // set the appropriate text size for this message
    display_setCursor(MESSAGE_SCREEN_CURSOR_X, MESSAGE_SCREEN_CURSOR_Y); // move the cursor to the appropriate place
    display_println("Sorry, you lose"); // print the lose message to the screen
}

// helper function to update the longest sequence reached (high score)
static void updateLongestSequenceLength() {
    // if the current iteration length is longer than the previous record
    if(iterationSequenceLength > longestSequenceLength) {
        // update the previous record to match the current iteration length
        longestSequenceLength = iterationSequenceLength - SEQUENCE_LENGTH_OFFSET;
    }
}

// runs a test and shows how the control module is used
void simonControl_runTest() {
    // indefinitely loops all of the SM ticks
    while(1) {
        simonControl_tick(); // calls the control tick function
        buttonHandler_tick(); // calls the buttonHandler tick function
        verifySequence_tick(); // calls verify sequence tick function
        flashSequence_tick(); // calls the flash sequence tick function
        utils_msDelay(TEST_TICK_PERIOD_MS); // waits the prescribed period (50 ms)
    }
}
