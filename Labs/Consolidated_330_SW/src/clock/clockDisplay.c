/*
 * clockDisplay.c
 *
 *  Created on: Sep 28, 2016
 *      Author: cdmoo
 */
#include "clockDisplay.h"
#include <stdio.h>
#include "supportFiles/display.h"
#include "string.h"
#include "supportFiles/utils.h"
#include "../intervalTimer/intervalTimer.h"

// size of the clock numbers - also scales the size of the arrows
#define CLOCK_TEXT_SIZE 6
#define NUM_DISPLAY_CHARS 8 // number of characters in the clock string

#define ARROW_WIDTH (12 * CLOCK_TEXT_SIZE) // width in pixels of an arrow
#define ARROW_HEIGHT (9 * CLOCK_TEXT_SIZE) // height in pixels of an arrow
#define ARROW_PADDING (5.5 * CLOCK_TEXT_SIZE) // width in pixels of space between each arrow
// width in pixels of the entire block containing all arrows and clock text
#define CLOCK_DISPLAY_WIDTH (3*ARROW_WIDTH + 2*ARROW_PADDING)
// offset of the first arrow from the left side
#define ARROW_OFFSET_X0 (DISPLAY_WIDTH / 2 - CLOCK_DISPLAY_WIDTH / 2)
// offset of the second arrow from the left side
#define ARROW_OFFSET_X1 (ARROW_OFFSET_X0 + ARROW_WIDTH + ARROW_PADDING)
// offset of the third arrow from the left side
#define ARROW_OFFSET_X2 (ARROW_OFFSET_X1 + ARROW_WIDTH + ARROW_PADDING)
// vertical offset of an arrow from the vertical middle of the screen
#define ARROW_MIDY_OFFSET (6 * CLOCK_TEXT_SIZE)
// vertical offset of the arrow from the top of the screen
#define ARROW_OFFSET_Y0 ((DISPLAY_HEIGHT / 2) - ARROW_MIDY_OFFSET)
// vertical offset of the arrow from the bottom of the screen
#define ARROW_OFFSET_Y1 ((DISPLAY_HEIGHT / 2) + ARROW_MIDY_OFFSET)

#define CURSOR_POS_X0 ARROW_OFFSET_X0 // offset of the text to be rendered frim the left
// 1st cursor position for writing clock text
#define CURSOR_POS_Y (DISPLAY_HEIGHT / 2 - 3 * CLOCK_TEXT_SIZE)
// 2nd cursor position for writing clock text
#define CURSOR_POS_X1 (CURSOR_POS_X0 + ARROW_WIDTH / 2)
// 3rd cursor position for writing clock text
#define CURSOR_POS_X2 (CURSOR_POS_X0 + ARROW_WIDTH)
// 4th cursor position for writing clock text
#define CURSOR_POS_X3 ARROW_OFFSET_X1
// 5th cursor position for writing clock text
#define CURSOR_POS_X4 (CURSOR_POS_X3 + ARROW_WIDTH / 2)
// 6th cursor position for writing clock text
#define CURSOR_POS_X5 (CURSOR_POS_X3 + ARROW_WIDTH)
// 7th cursor position for writing clock text
#define CURSOR_POS_X6 ARROW_OFFSET_X2
// 8th cursor position for writing clock text
#define CURSOR_POS_X7 (CURSOR_POS_X6 + ARROW_WIDTH / 2)
#define MAX_MINUTES 59 // max number of minutes before clock rolls over
#define MAX_HOURS 12 // max number of hours before clock rolls over
#define MAX_SECONDS 59 // max number of seconds in a minute
#define MIN_HOURS 1 // minimum number of hours set after rollover
#define NO_FORCE_UPDATE_ALL 0 // boolean to be passed into updateClock to assure that all
                              // digits are not updated, only the digits that have changed
#define FORCE_UPDATE_ALL 1 // boolean to be passed into updateClock to make sure that all digits are rewritten
#define TEST_DELAY 1000 // number of miliseconds to wait in between each update invocation during testing
#define TEST_DELAY_SMALL 100 // number of miliseconds to wait during timekeeping X10 during testing

// the following three constants are for dividing the screen into the six pre-defined touch
// zones corresponding to the increment and decrement of hours, minutes, and seconds
#define Y_MID_DIVIDER (DISPLAY_HEIGHT / 2)
#define X_DIVIDER_1 (DISPLAY_WIDTH / 3)
#define X_DIVIDER_2 (DISPLAY_WIDTH * 2 / 3)

// an array holding all of the cursor positions so that a for loop can be used to render clock text
static const uint32_t cursorPositions[NUM_DISPLAY_CHARS] = {CURSOR_POS_X0, CURSOR_POS_X1,
        CURSOR_POS_X2, CURSOR_POS_X3, CURSOR_POS_X4, CURSOR_POS_X5, CURSOR_POS_X6, CURSOR_POS_X7};

static uint32_t seconds = 0; // global variable storing clock seconds
static uint32_t minutes = 0; // global variable storing clock minutes
static uint32_t hours = 1; // global variable storing clock hours
static char currentTime[NUM_DISPLAY_CHARS]; // string storing the most recent calculated time
static char displayedTime[NUM_DISPLAY_CHARS]; // string storing the most recently displayed time
static int16_t lastTouchX; // global variable storing most recent touch data on the x axis
static int16_t lastTouchY; // global variable storing most recent touch data on the y axis
static uint8_t lastTouchZ; // global variable storing most recent touch data on the z axis (pressure)

// this function is responsible initializing all of the hardware it needs to interact with and set the display
// up for the initial clock screen
void clockDisplay_init() {
    display_init();  // Must init all of the software and underlying hardware for LCD.
    display_fillScreen(DISPLAY_BLACK);  // Blank the screen.
    display_setTextColor(DISPLAY_RED);  // Make the text red.
    display_setTextSize(CLOCK_TEXT_SIZE); // set the text to the predifined size

    // draw the top left arrow
    display_fillTriangle(ARROW_OFFSET_X0, ARROW_OFFSET_Y0, ARROW_OFFSET_X0 + ARROW_WIDTH, ARROW_OFFSET_Y0,
            ARROW_OFFSET_X0 + ARROW_WIDTH / 2, ARROW_OFFSET_Y0 - ARROW_HEIGHT, DISPLAY_GREEN);
    // draw the top middle arrow
    display_fillTriangle(ARROW_OFFSET_X1, ARROW_OFFSET_Y0, ARROW_OFFSET_X1 + ARROW_WIDTH, ARROW_OFFSET_Y0,
            ARROW_OFFSET_X1 + ARROW_WIDTH / 2, ARROW_OFFSET_Y0 - ARROW_HEIGHT, DISPLAY_GREEN);
    // draw the top right arrow
    display_fillTriangle(ARROW_OFFSET_X2, ARROW_OFFSET_Y0, ARROW_OFFSET_X2 + ARROW_WIDTH, ARROW_OFFSET_Y0,
            ARROW_OFFSET_X2 + ARROW_WIDTH / 2, ARROW_OFFSET_Y0 - ARROW_HEIGHT, DISPLAY_GREEN);

    // draw the bottom left triangle
    display_fillTriangle(ARROW_OFFSET_X0, ARROW_OFFSET_Y1, ARROW_OFFSET_X0 + ARROW_WIDTH, ARROW_OFFSET_Y1,
            ARROW_OFFSET_X0 + ARROW_WIDTH / 2, ARROW_OFFSET_Y1 + ARROW_HEIGHT, DISPLAY_GREEN);
    // draw the bottom middle triangle
    display_fillTriangle(ARROW_OFFSET_X1, ARROW_OFFSET_Y1, ARROW_OFFSET_X1 + ARROW_WIDTH, ARROW_OFFSET_Y1,
            ARROW_OFFSET_X1 + ARROW_WIDTH / 2, ARROW_OFFSET_Y1 + ARROW_HEIGHT, DISPLAY_GREEN);
    // draw the bottom right triangle
    display_fillTriangle(ARROW_OFFSET_X2, ARROW_OFFSET_Y1, ARROW_OFFSET_X2 + ARROW_WIDTH, ARROW_OFFSET_Y1,
            ARROW_OFFSET_X2 + ARROW_WIDTH / 2, ARROW_OFFSET_Y1 + ARROW_HEIGHT, DISPLAY_GREEN);

    //init the time current time string to calculated time (1:00:00 after initialization)
    sprintf(currentTime, "%02hd:%02hd:%02hd", hours, minutes, seconds);
    //init the displayed time string to the most recently displayed time (1:00:00)
    sprintf(displayedTime, "%02hd:%02hd:%02hd", hours, minutes, seconds);

    // update the time display to display the current time string
    clockDisplay_updateTimeDisplay(FORCE_UPDATE_ALL);
}

// in charge of erasing a previously displayed character and writing a new character
void updateDisplayChar(int index) {
    // set the cursor to the correct position as indicated by the index
    display_setCursor(cursorPositions[index], CURSOR_POS_Y);
    // set the text to black so that it will overwrite the previously displayed character
    display_setTextColor(DISPLAY_BLACK);
    // write the black overwrite to the screen, effectively erasing the past character
    display_print(displayedTime[index]);

    // reset the cursor to the position at the correct index
    display_setCursor(cursorPositions[index], CURSOR_POS_Y);
    // set text color to red
    display_setTextColor(DISPLAY_RED);
    // print the new time character
    display_print(currentTime[index]);
    // set the most recent displayed time to what was most recently displayed from the
    // current time string
    displayedTime[index] = currentTime[index];
}

// selectively updates changed characters in the clock text
void clockDisplay_updateTimeDisplay(bool forceUpdateAll) {
    // temporary string that holds the newly calculated time string
    char temp[NUM_DISPLAY_CHARS];
    // writing the most recent time to the temporary string
    sprintf(temp, "%02hd:%02hd:%02hd", hours, minutes, seconds);
    // copy the temp string into the current time string
    strncpy(currentTime, temp, NUM_DISPLAY_CHARS);

    // iterate through each character in the time string
    for(int i = 0; i < NUM_DISPLAY_CHARS; i++) {
        // if the character has changed from the most recent display
        // or if the caller has forced every character to be updated
        // update the char by erasing the old char and writing the new one
        if(currentTime[i] != displayedTime[i] || forceUpdateAll) {
            updateDisplayChar(i);
        }
    }
}

// called either when minutes max out at 60 or when a user is autoincrementing the hours
void incrementHours() {
    // if the hours have not reached 12
    if(hours < MAX_HOURS) {
        // increment the hours
        hours++;
    }
    else {
        // if the hours have reached the limit of 12, set them back to 1
        hours = MIN_HOURS;
    }
    // rerender the display after changes have been made
    clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);
}

// called either when seconds max out at 60 or when a user is autoincrementing the minutes
void incrementMinutes() {
    // if the minutes have not reached the max of 59
    if(minutes < MAX_MINUTES) {
        // increment them
        minutes++;
    }
    else {
        // otherwise reset the minutes to 0
        minutes = 0;
        // and increment the hours
        incrementHours();
    }
    // rerender the display after changes have been made
    clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);
}

// called by the control module to advance time forward or by the user to set time
void incrementSeconds() {
    // if the seconds have not reached the max of 59
    if(seconds < MAX_SECONDS) {
        // increment them
        seconds++;
    }
    else {
        // otherwise reset the seconds to 0
        seconds = 0;
        // and increment the minutes
        incrementMinutes();
    }
    // rerender the display after changes have been made
    clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);
}

// called when the user is manually setting hours
void decrementHours() {
    // if the hours have not reached the minimum of 1
    if(hours > MIN_HOURS ) {
        // decrement the hours
        hours--;
    }
    else {
        // otherwise reset the hours to 12
        hours = MAX_HOURS;
    }
    // rerender the display after changes have been made
    clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);
}

// called when the user is manually setting minutes
void decrementMinutes() {
    // if the minutes have not reached the minimum of 0
    if(minutes > 0) {
        // decrement them
        minutes--;
    }
    else {
        // otherwise rollover the minutes to 59
        minutes = MAX_MINUTES;
        // and decrement the hours
        decrementHours();
    }
    // rerender the display after changes have been made
    clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);
}

// called when the user is manually setting seconds
void decrementSeconds() {
    // if the seconds have not reached the minimum of 0
    if(seconds > 0) {
        // decrement the seconds
        seconds--;
    }
    else {
        // otherwise rollover the seconds to 59
        seconds = MAX_SECONDS;
        // and decrement them
        decrementMinutes();
    }
    // rerender the display after changes have been made
    clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);
}

// processes touch data and executes the appropriate action depending on the user touch
void clockDisplay_performIncDec() {
    // fetch the touch data from the display module and store them in global variables
    display_getTouchedPoint(&lastTouchX, &lastTouchY, &lastTouchZ);
    // first test if the touch was in any of the top 3 zones
    if(lastTouchY < Y_MID_DIVIDER) {
        // test if the touch was in the leftmost zone corresponding to increment hours
        if(lastTouchX < X_DIVIDER_1) {
            // increment hours accordingly
            incrementHours();
        }
        // test if the touch was in the middle zone corresponding to increment minutes
        else if(lastTouchX < X_DIVIDER_2) {
            // increment minutes accordingly
            incrementMinutes();
        }
        else {
            // if the touch was not in the other two top zones, increment seconds
            incrementSeconds();
        }
    }
    // if the touch was not in the top zones, it must have occured in the bottom 3 zones
    else {
        // test if the touch was in the leftmost zone corresponding to decrement hours
        if(lastTouchX < X_DIVIDER_1) {
            // decrement hours accordingly
            decrementHours();
        }
        // test if the touch was in the leftmost zone corresponding to decrement seconds
        else if(lastTouchX < X_DIVIDER_2) {
            // decrement minutes accordingly
            decrementMinutes();
        }
        // if the touch was not in the other two bottom zones, decrement seconds
        else {
            decrementSeconds();
        }
    }
}

// called during standard time keeping state of the control state machind
void clockDisplay_advanceTimeOneSecond() {
    // increment 1 seconds every second
    incrementSeconds();
}


// tests the increment / decrement abilities of the display and provides a demo on
// how to use this package
void clockDisplay_runTest() {
    // any use of this module must call init first
    clockDisplay_init();

    // test increment hours and delay 1 second
    incrementHours();
    utils_msDelay(TEST_DELAY);
    // test deccrement hours and delay 1 second
    decrementHours();
    utils_msDelay(TEST_DELAY);
    // test increment minutes and delay 1 second
    incrementMinutes();
    utils_msDelay(TEST_DELAY);
    // test deccrement minutes and delay 1 second
    decrementMinutes();
    utils_msDelay(TEST_DELAY);
    // test increment seconds and delay 1 second
    incrementSeconds();
    utils_msDelay(TEST_DELAY);
    // test deccrement seconds and delay 1 second
    decrementSeconds();
    utils_msDelay(TEST_DELAY);

    // at a speed of once per tenth of a second, increment seconds
    // and check the rollover into minutes
    for(int i = 0; i < 100; i++) {
        incrementSeconds();
        utils_msDelay(TEST_DELAY_SMALL);
    }
}




