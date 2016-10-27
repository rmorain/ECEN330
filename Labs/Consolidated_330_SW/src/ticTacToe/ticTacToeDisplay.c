/*
 * ticTacToeDisplay.c
 *
 *  Created on: Oct 19, 2016
 *      Author: cdmoo
 */
#include "ticTacToeDisplay.h"
#include "supportFiles/display.h"
#include "../switchesAndButtons/switches.h"
#include "../switchesAndButtons/buttons.h"
#include "supportFiles/utils.h"
#include <stdio.h>

#define BOARD_LINE_X1 DISPLAY_WIDTH / 3  // x value for the left vertical board line
#define BOARD_LINE_X2 2 * DISPLAY_WIDTH / 3 // x value for the right vertical board line
#define BOARD_LINE_Y1 DISPLAY_HEIGHT / 3 // y value for the top horizontal board line
#define BOARD_LINE_Y2 2 * DISPLAY_HEIGHT / 3 // y value for the bottom horizontal line

#define SQUARE_WIDTH DISPLAY_WIDTH / 3 // width of a game board square
#define SQUARE_HEIGHT DISPLAY_HEIGHT / 3 // height of a game board square
#define MID_SQUARE_OFFSET_X DISPLAY_WIDTH / 6 // horizontal padding between a square and a drawn 'o'
#define MID_SQUARE_OFFSET_Y DISPLAY_HEIGHT / 6 // verical padding for a drawn 'o'
#define NEAR_SQUARE_OFFSET_X 20 // horizontal padding to draw 1 diagonal of an 'x'
#define NEAR_SQUARE_OFFSET_Y 10 // vertical padding to draw 1 diagonal of an 'x'
#define FAR_SQUARE_OFFSET_X 80 // horizontal padding to draw the other diagonal of an 'x'
#define FAR_SQUARE_OFFSET_Y 70 // vertical padding to draw the other diagonal of an 'x'
#define CIRCLE_RADIUS 30 // radius of a drawn 'o'

#define ROW_ZERO 0 // constant representing the first row on the game board
#define ROW_ONE 1 // constant representing the second row on the game board
#define ROW_TWO 2 // constant representing the third row on the game board
#define COLUMN_ZERO 0 // constant representing the first column on the game board
#define COLUMN_ONE 1 // constant representing the second column on the game board
#define COLUMN_TWO 2 // constant representing the third column on the game board

#define ADC_DELAY_MS 50 // delay used in the test function for touch detection

#define SPLASH_CURSOR_POS_X 30 // initial x cursor position used when drawing the splash screen
#define SPLASH_CURSOR_POS_Y 80 // initial y cursor position used when drawing the splash screen
#define SPLASH_LINE_OFFSET 20 // horizontal padding for the splash screen
#define SPLASH_TEXT_SIZE 2 // text size for the splash screen text

// function in charge of preparing the display for drawing
void ticTacToeDisplay_init() {
    display_init();  // Must init all of the software and underlying hardware for LCD.
    display_fillScreen(DISPLAY_BLACK);  // Blank the screen.
    ticTacToeDisplay_drawBoardLines(); // draw the 4 game board lines
}

// draws an x at the specified position on the table
void ticTacToeDisplay_drawX(uint8_t row, uint8_t column, bool erase) {
    // calculate the top left corner coordinates of the sqaure where the x will be drawn
    uint16_t baseX = column * SQUARE_WIDTH;
    uint16_t baseY = row * SQUARE_HEIGHT;

    // set the drawing corner based on the erase flag, if it is true, draw black (to erase)
    // otherwise draw yellow so that it is visible
    uint16_t color = erase ? DISPLAY_BLACK : DISPLAY_YELLOW;

    // to draw an x, draw to diagonal lines with the appropiate beginning and ending positions
    // draw the first diagonal
    display_drawLine(baseX + NEAR_SQUARE_OFFSET_X, baseY + NEAR_SQUARE_OFFSET_Y,
            baseX + FAR_SQUARE_OFFSET_X, baseY + FAR_SQUARE_OFFSET_Y, color);
    // draw the second diagonal
    display_drawLine(baseX + FAR_SQUARE_OFFSET_X, baseY + NEAR_SQUARE_OFFSET_Y,
            baseX + NEAR_SQUARE_OFFSET_X, baseY + FAR_SQUARE_OFFSET_Y, color);
}

// draws an o to the screen at a given position on the board
void ticTacToeDisplay_drawO(uint8_t row, uint8_t column, bool erase) {
    // calculate the center of the appropriate square using the row and column parameters
    uint16_t circleX = column * SQUARE_WIDTH + MID_SQUARE_OFFSET_X;
    uint16_t circleY = row * SQUARE_HEIGHT + MID_SQUARE_OFFSET_Y;

    // set the drawing corner based on the erase flag, if it is true, draw black (to erase)
    // otherwise draw yellow so that it is visible
    uint16_t color = erase ? DISPLAY_BLACK : DISPLAY_YELLOW;

    // draw a circle at the calcuated position representing an 'o' on the game board
    display_drawCircle(circleX, circleY, CIRCLE_RADIUS, color);
}

// determines the position of a touch on the board in terms of row and column coordinates
void ticTacToeDisplay_touchScreenComputeBoardRowColumn(uint8_t* row, uint8_t* column) {
    // keep track of the touched position in terms of pixels
    int16_t x, y;
    uint8_t z;
    // use the display function to store the touch data in pixels
    display_getTouchedPoint(&x, &y, &z);

    // determine game board position of the the touch
    // test first what column it is in
    if(x < BOARD_LINE_X1) {
        *column = COLUMN_ZERO; // it is in the first column if its x is less than the first line
    } else if(x < BOARD_LINE_X2) {
        *column = COLUMN_ONE; // it is in the second column if its x is less than the second line
    } else {
        *column = COLUMN_TWO; // otherwise it is in the third column
    }

    // test what row it is in
    if(y < BOARD_LINE_Y1) {
        *row = ROW_ZERO; // it is in the first row if its x is less than the first line
    } else if(y < BOARD_LINE_Y2) {
        *row = ROW_ONE; // it is in the second row if its x is less than the first line
    } else {
        *row = ROW_TWO; // otherwise it is in the third row
    }
}

// draws the splash screen to the board
void ticTacToeDisplay_drawSplashScreen() {
    display_init();  // Must init all of the software and underlying hardware for LCD.
    display_fillScreen(DISPLAY_BLACK);  // Blank the screen.
    display_setTextSize(SPLASH_TEXT_SIZE); // set the text to the prescribed size
    display_setCursor(SPLASH_CURSOR_POS_X, SPLASH_CURSOR_POS_Y); // set the cursor at the appropriate position
    display_println("Touch board to play X"); // print the first line of the splash screen
    display_setCursor(SPLASH_CURSOR_POS_X, SPLASH_CURSOR_POS_Y + SPLASH_LINE_OFFSET); // reset the cursors to be one line down
    display_println("        --or--"); // print the second line of the splash screen
    display_setCursor(SPLASH_CURSOR_POS_X, SPLASH_CURSOR_POS_Y + 2 * SPLASH_LINE_OFFSET); // reset the cursors to be one line down
    display_println("wait for the computer"); // print the third line of the splash screen
    display_setCursor(SPLASH_CURSOR_POS_X, SPLASH_CURSOR_POS_Y + 3 * SPLASH_LINE_OFFSET); // reset the cursors to be one line down
    display_println("      and play O"); // print the last line of the splash screen
}

// erases the splash screen by rewriting the text in black
void ticTacToeDisplay_clearSplashScreen() {
    display_setTextColor(DISPLAY_BLACK);  // set the text color to be black so that it will effectively erase what was written before
    display_setTextSize(SPLASH_TEXT_SIZE); // set the text to the prescribed size
    display_setCursor(SPLASH_CURSOR_POS_X, SPLASH_CURSOR_POS_Y); // set the cursor to the appropriate position
    display_println("Touch board to play X"); // black out the first line
    display_setCursor(SPLASH_CURSOR_POS_X, SPLASH_CURSOR_POS_Y + SPLASH_LINE_OFFSET); // reset the cursors to be one line down
    display_println("        --or--"); // black out the second line
    display_setCursor(SPLASH_CURSOR_POS_X, SPLASH_CURSOR_POS_Y + 2 * SPLASH_LINE_OFFSET); // reset the cursors to be one line down
    display_println("wait for the computer"); // black out the third line
    display_setCursor(SPLASH_CURSOR_POS_X, SPLASH_CURSOR_POS_Y + 3 * SPLASH_LINE_OFFSET); // reset the cursors to be one line down
    display_println("      and play O"); // black out the fourth line
}

// shows how the display module is usedf
void ticTacToeDisplay_runTest() {
    bool exitTest = false; // flag for exiting the outer loop (entire program)
    bool exitSession; // flag for exiting the inner loop (current drawing session, resets the board)
   // test loop, continue while exit flag is false
    while(!exitTest) {
        ticTacToeDisplay_init(); // init the display before use

        exitSession = false; // set the exit session flag to false
        uint8_t row, column; // keep track of the row and column to draw

        // session loop, continue while exit flags are not set
        while(!exitSession && !exitTest) {
            // monitor button 0 which indicates that the session needs to be restarted
            if(buttons_read() & BUTTONS_BTN0_MASK) {
                exitSession = true; // set the exit session flag if the button is pressed
            }

            // monitor button 1 which indicates that the test needs to end
            if(buttons_read() & BUTTONS_BTN1_MASK) {
                exitTest = true; // set the exit test flag to false if button is pressed
            }

            // keep testing for a detected touch, which will indicate that a move is to be drawn
            if(display_isTouched()) {
                // if a touch is detected, clear the old data
                display_clearOldTouchData();
                // wiat the prescribed 50 ms for the adc to settle so that position can be detected
                utils_msDelay(ADC_DELAY_MS);
                // use the position to determine the row and column of the move
                ticTacToeDisplay_touchScreenComputeBoardRowColumn(&row, &column);
                // read the switch 0 value to determine if an x or o is to be drawn
                switches_read() & SWITCHES_SW0_MASK
                        ? ticTacToeDisplay_drawO(row, column, false) // switch 0 up indicates o
                        : ticTacToeDisplay_drawX(row, column, false); // switch 0 up indicates o
            }

        }
    }
    display_fillScreen(DISPLAY_BLACK);  // Blank the screen.
}

// draws the 4 game board lines to the screen
void ticTacToeDisplay_drawBoardLines() {
    // draw vertical left line
    display_drawLine(BOARD_LINE_X1, 0, BOARD_LINE_X1, DISPLAY_HEIGHT, DISPLAY_YELLOW);
    // draw vertical right line
    display_drawLine(BOARD_LINE_X2, 0, BOARD_LINE_X2, DISPLAY_HEIGHT, DISPLAY_YELLOW);
    // draw horizontal top line
    display_drawLine(0, BOARD_LINE_Y1, DISPLAY_WIDTH, BOARD_LINE_Y1, DISPLAY_YELLOW);
    // draw horizontal bottom line
    display_drawLine(0, BOARD_LINE_Y2, DISPLAY_WIDTH, BOARD_LINE_Y2, DISPLAY_YELLOW);
}

