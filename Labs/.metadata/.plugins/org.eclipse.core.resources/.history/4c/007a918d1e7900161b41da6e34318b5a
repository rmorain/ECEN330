/*
 * buttons.c
 *
 *  Created on: Sep 10, 2016
 *      Author: cdmoo
 */

#include "buttons.h"
#include <stdio.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define BUTTONS_POSITION_ZERO 0  // button farthest to the right
#define BUTTONS_POSITION_ONE 1  // button 2nd to the right
#define BUTTONS_POSITION_TWO 2 // button 2nd to the left
#define BUTTONS_POSITION_THREE 3 // button farthest to the left
#define BUTTON_HEIGHT (DISPLAY_HEIGHT / 2) // height in pixels of a button
#define BUTTON_WIDTH (DISPLAY_WIDTH / 4) // width in pixels of a button
#define BUTTON_TEXT_OFFSETX (BUTTON_WIDTH / 4) // offset of the text from the left inside a button
#define BUTTON_TEXT_OFFSETY (BUTTON_HEIGHT / 2) // offset of the text from the top of a button
#define BUTTON_COUNT 3 // highest index possible for a button

// act as a global state indicating the display status of each button
unsigned char displayingBtn0 = 0, displayingBtn1 = 0, displayingBtn2 = 0, displayingBtn3 = 0;

// helper function to read the GPIO button register
int32_t buttons_readGpioRegister(int32_t offset) {
    // return the register value at the calculated address
    return Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR + offset);
}

// helper function to draw a 'button rect' to the screen with text
void buttons_drawButton(unsigned char buttonPosition, uint16_t fillColor, uint16_t textColor, const char buttonText[]) {
    // buttons begin in chronological order on the right side of the screen
    // so the the starting x position is calculated as (max count - position) * width
    int16_t buttonX = BUTTON_WIDTH * (BUTTON_COUNT - buttonPosition);

    // draw a filled rectangle with the calcuted starting x and the parameters
    display_fillRect(buttonX, 0, BUTTON_WIDTH, BUTTON_HEIGHT, fillColor);
    // set the cursor in the appropriate place depending on button position
    display_setCursor(buttonX + BUTTON_TEXT_OFFSETX, 0 + BUTTON_TEXT_OFFSETY);
    // set color and print the button specific string on top of the button
    display_setTextColor(textColor);
    display_print(buttonText);
}

void buttons_clearButton(unsigned char buttonPosition) {
    // buttons begin in chronological order on the right side of the screen
    // so the the starting x position is calculated as (max count - position) * width
    int16_t buttonX = BUTTON_WIDTH * (BUTTON_COUNT - buttonPosition);

    // draw a filled rectangle with the calcuted starting x and the parameters
    display_fillRect(buttonX, 0, BUTTON_WIDTH, BUTTON_HEIGHT, DISPLAY_BLACK);
}

// helper function to display current button state on the screen
void buttons_displayState(int32_t buttons_state) {
    // mask the button_state to fetch status of button0
    // if the button is pressed and is not currently being displayed
    // display the button at the correct position with the correct text and colors
    if((buttons_state & BUTTONS_BTN0_MASK) && !displayingBtn0) {
        buttons_drawButton(BUTTONS_POSITION_ZERO, DISPLAY_YELLOW, DISPLAY_BLACK, "BTN0");
        // set global state of button0 to 1 (pressed)
        displayingBtn0 = 1;
    }
    // if the button is not pressed and is currently being displayed, clear the button
    else if(!(buttons_state & BUTTONS_BTN0_MASK) && displayingBtn0) {
        buttons_clearButton(BUTTONS_POSITION_ZERO);
        // set global state of button0 to 0 (not pressed)
        displayingBtn0 = 0;
    }
    //if the button is being pressed and not currently being displayed, display the button
    if((buttons_state & BUTTONS_BTN1_MASK) && !displayingBtn1) {
        buttons_drawButton(BUTTONS_POSITION_ONE, DISPLAY_GREEN, DISPLAY_BLACK, "BTN1");
        // set global state of button1 to 1 (pressed)
        displayingBtn1 = 1;
    }
    // if the button is not pressed and is currently being displayed, clear the button
    else if(!(buttons_state & BUTTONS_BTN1_MASK) && displayingBtn1) {
        buttons_clearButton(BUTTONS_POSITION_ONE);
        // set global state of button1 to 0 (not pressed)
        displayingBtn1 = 0;
    }
    //if the button is being pressed and not currently being displayed, display the button
    if((buttons_state & BUTTONS_BTN2_MASK) && !displayingBtn2) {
        buttons_drawButton(BUTTONS_POSITION_TWO, DISPLAY_RED, DISPLAY_WHITE, "BTN2");
        // set global state of button2 to 1 (pressed)
        displayingBtn2 = 1;
    }
    // if the button is not pressed and is currently being displayed, clear the button
    else if(!(buttons_state & BUTTONS_BTN2_MASK) && displayingBtn2) {
        buttons_clearButton(BUTTONS_POSITION_TWO);
        // set global state of button2 to 0 (not pressed)
        displayingBtn2 = 0;
    }
    //if the button is being pressed and not currently being displayed, display the button
    if((buttons_state & BUTTONS_BTN3_MASK) && !displayingBtn3) {
        buttons_drawButton(BUTTONS_POSITION_THREE, DISPLAY_BLUE, DISPLAY_WHITE, "BTN3");
        // set global state of button3 to 1 (pressed)
        displayingBtn3 = 1;
    }
    // if the button is not pressed and is currently being displayed, clear the button
    else if(!(buttons_state & BUTTONS_BTN3_MASK) && displayingBtn3) {
        buttons_clearButton(BUTTONS_POSITION_THREE);
        // set global state of button3 to 0 (not pressed)
        displayingBtn3 = 0;
    }
}

// reads the register value passing in the button registers offset
int32_t buttons_read() {
    return buttons_readGpioRegister(BUTTONS_DATA_OFFSET);
}

// init elemenets of the dispay and clear the screen for future button drawing
int32_t buttons_init() {
    display_init();  // Must init all of the software and underlying hardware for LCD.
    display_fillScreen(DISPLAY_BLACK);  // Blank the screen.
    display_setCursor(0, 0);            // The upper left of the LCD screen.
    display_setTextSize(2);             // Make the text a little larger.
    return BUTTONS_INIT_STATUS_OK;
}

void buttons_runTest() {
    buttons_init();

    // continually read the state of the buttons and update the screen until all buttons are on
    while(1) {
        int32_t buttons_state = buttons_read();

        // write the leds according to the current state (in hex) of the switches
        buttons_displayState(buttons_state);

        // if the all of the buttons are pressed, exit the function
        if(buttons_state == BUTTONS_EXIT_CODE) {
            // clear the screen before exiting
            display_fillScreen(DISPLAY_BLACK);  // Blank the screen.
            return;
        }
    }
}



