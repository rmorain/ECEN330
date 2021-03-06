/*
 * simonDisplay.c
 *
 *  Created on: Oct 30, 2016
 *      Author: cdmoo
 */
#include "simonDisplay.h"
#include "supportFiles/display.h"
#include "supportFiles/utils.h"
#include <stdio.h>

#define MID_SCREEN_X DISPLAY_WIDTH / 2 // line running down the middle of the screen verticaly
#define MID_SCREEN_Y DISPLAY_HEIGHT / 2 // line running down the middle of the screen horizontaly
#define BUTTON_ORIGIN_X1 (DISPLAY_WIDTH / 4 - SIMON_DISPLAY_BUTTON_WIDTH / 2) // upper left hand x coordinate of the top left button
#define BUTTON_ORIGIN_X2 (DISPLAY_WIDTH * 3 / 4 - SIMON_DISPLAY_BUTTON_WIDTH / 2) // upper left hand x coordinate of the top right button
#define BUTTON_ORIGIN_Y1 (DISPLAY_HEIGHT / 4 - SIMON_DISPLAY_BUTTON_HEIGHT / 2) // upper left hand y coordinate of the top left button
#define BUTTON_ORIGIN_Y2 (DISPLAY_HEIGHT * 3 / 4 - SIMON_DISPLAY_BUTTON_HEIGHT / 2) // upper left hand y coordinate of the top right button
#define SQUARE_ORIGIN_X1 (DISPLAY_WIDTH / 4 - SIMON_DISPLAY_SQUARE_WIDTH / 2) // upper left hand x coordinate of the top left square
#define SQUARE_ORIGIN_X2 (DISPLAY_WIDTH * 3 / 4 - SIMON_DISPLAY_SQUARE_WIDTH / 2) // upper left hand x coordinate of the top right square
#define SQUARE_ORIGIN_Y1 (DISPLAY_HEIGHT / 4 - SIMON_DISPLAY_SQUARE_HEIGHT / 2) // upper left hand y coordinate of the top left square
#define SQUARE_ORIGIN_Y2 (DISPLAY_HEIGHT * 3 / 4 - SIMON_DISPLAY_SQUARE_HEIGHT / 2) // upper left hand y coordinate of the top right square



// recieves touch coordinates and returns what region number they occured in
int8_t simonDisplay_computeRegionNumber(int16_t x, int16_t y) {
    // if the x coordinate was to the left of the vertical mid line, restrict the region number to squares 0, 2
    if(x < MID_SCREEN_X) {
        // if the y coordinate was above the mid screen line
        if(y < MID_SCREEN_Y) {
            // the touched coordinate was zero
            return SIMON_DISPLAY_REGION_0;
        } else {
            // othersise, the touched coordinate was 2
            return SIMON_DISPLAY_REGION_2;
        }
    // if the y coordinate was to the right of the vertical mid line, restrict the region number to squares 1, 3
    } else {
        // if the y coordinate was below the mid screen liine
        if(y < MID_SCREEN_Y) {
            // the touched coordinate was 1
            return SIMON_DISPLAY_REGION_1;
        } else {
            // otherwise, the touhced coordinate was 3
            return SIMON_DISPLAY_REGION_3;
        }
    }
}

// function that accepts a region number and draws the button from that region
void simonDisplay_drawButton(uint8_t regionNumber) {
    //switch based on the region number
    switch(regionNumber) {
        case SIMON_DISPLAY_REGION_0:
            // draw the button for region 0 by drawing a square with the corresponding, precalculated verticies
            display_fillRect(BUTTON_ORIGIN_X1, BUTTON_ORIGIN_Y1, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_RED);
            break;
        case SIMON_DISPLAY_REGION_1:
            // draw the button for region 1 by drawing a square with the corresponding, precalculated verticies
            display_fillRect(BUTTON_ORIGIN_X2, BUTTON_ORIGIN_Y1, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_YELLOW);
            break;
        case SIMON_DISPLAY_REGION_2:
            // draw the button for region 2 by drawing a square with the corresponding, precalculated verticies
            display_fillRect(BUTTON_ORIGIN_X1, BUTTON_ORIGIN_Y2, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_BLUE);
            break;
        case SIMON_DISPLAY_REGION_3:
            // draw the button for region 3 by drawing a square with the corresponding, precalculated verticies
            display_fillRect(BUTTON_ORIGIN_X2, BUTTON_ORIGIN_Y2, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_GREEN);
            break;
    }
}

// function that draws all four buttons at once
void simonDisplay_drawAllButtons() {
    // draw the button for region 0 with its corresponding color
    display_fillRect(BUTTON_ORIGIN_X1, BUTTON_ORIGIN_Y1, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_RED);
    // draw the button for region 1 with its corresponding color
    display_fillRect(BUTTON_ORIGIN_X2, BUTTON_ORIGIN_Y1, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_YELLOW);
    // draw the button for region 2 with its corresponding color
    display_fillRect(BUTTON_ORIGIN_X1, BUTTON_ORIGIN_Y2, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_BLUE);
    // draw the button for region 3 with its corresponding color
    display_fillRect(BUTTON_ORIGIN_X2, BUTTON_ORIGIN_Y2, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_GREEN);
}

// erase all the buttons at once by drawing them black
void simonDisplay_eraseAllButtons() {
    // erase button 0
    display_fillRect(BUTTON_ORIGIN_X1, BUTTON_ORIGIN_Y1, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_BLACK);
    // erase button 1
    display_fillRect(BUTTON_ORIGIN_X2, BUTTON_ORIGIN_Y1, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_BLACK);
    // erase button 2
    display_fillRect(BUTTON_ORIGIN_X1, BUTTON_ORIGIN_Y2, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_BLACK);
    // erase button 3
    display_fillRect(BUTTON_ORIGIN_X2, BUTTON_ORIGIN_Y2, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_BLACK);
}

// draws a single square at a time with an erase bool parameter
void simonDisplay_drawSquare(uint8_t regionNo, bool erase) {
    uint16_t color; // save the color to be drawn in this local variable
    // switch based off of the region number
    switch(regionNo) {
        case SIMON_DISPLAY_REGION_0:
            // if the erase boolean is true, draw the square in black, effectively erasing it.  Otherwise, draw
            // the appropriate color for this region
            color = erase ? DISPLAY_BLACK : DISPLAY_RED;
            // draw the square with the coordinates for region 0
            display_fillRect(SQUARE_ORIGIN_X1, SQUARE_ORIGIN_Y1, SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT, color);
            break;
        case SIMON_DISPLAY_REGION_1:
            // if the erase boolean is true, draw the square in black, effectively erasing it.  Otherwise, draw
            // the appropriate color for this region
            color = erase ? DISPLAY_BLACK : DISPLAY_YELLOW;
            // draw the square with the coordinates for region 1
            display_fillRect(SQUARE_ORIGIN_X2, SQUARE_ORIGIN_Y1, SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT, color);
            break;
        case SIMON_DISPLAY_REGION_2:
            // if the erase boolean is true, draw the square in black, effectively erasing it.  Otherwise, draw
            // the appropriate color for this region
            color = erase ? DISPLAY_BLACK : DISPLAY_BLUE;
            // draw the square with the coordinates for region 2
            display_fillRect(SQUARE_ORIGIN_X1, SQUARE_ORIGIN_Y2, SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT, color);
            break;
        case SIMON_DISPLAY_REGION_3:
            // if the erase boolean is true, draw the square in black, effectively erasing it.  Otherwise, draw
            // the appropriate color for this region
            color = erase ? DISPLAY_BLACK : DISPLAY_GREEN;
            // draw the square with the coordinates for region 3
            display_fillRect(SQUARE_ORIGIN_X2, SQUARE_ORIGIN_Y2, SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT, color);
            break;
    }
}





