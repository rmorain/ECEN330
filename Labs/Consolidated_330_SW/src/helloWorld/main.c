
// Print out "hello world" on both the console and the LCD screen.

#define CIRCLE_OFFSET_X 55
#define CIRCLE_RADIUS 25

#define TRIANGLE_NEAR_OFFSET_Y 20
#define TRIANGLE_FAR_OFFSET_Y 70
#define TRIANGLE_OFFSET_X 25

#define MID_X (DISPLAY_WIDTH / 2)
#define MID_Y (DISPLAY_HEIGHT / 2)


#include <stdio.h>
#include "supportFiles/display.h"
int main() {
	display_init();  // Must init all of the software and underlying hardware for LCD.
	display_fillScreen(DISPLAY_BLACK);  // Blank the screen.
	printf("hello world!\n\r");  // This prints on the console.

	display_drawLine(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_GREEN);
	display_drawLine(DISPLAY_WIDTH, 0, 0, DISPLAY_HEIGHT, DISPLAY_GREEN);

	display_fillCircle(DISPLAY_WIDTH / 2 + CIRCLE_OFFSET_X, DISPLAY_HEIGHT / 2, CIRCLE_RADIUS, DISPLAY_RED);
	display_drawCircle(DISPLAY_WIDTH / 2 - CIRCLE_OFFSET_X, DISPLAY_HEIGHT / 2, CIRCLE_RADIUS, DISPLAY_RED);

	display_drawTriangle(MID_X, MID_Y + TRIANGLE_NEAR_OFFSET_Y, MID_X + TRIANGLE_OFFSET_X, MID_Y + TRIANGLE_FAR_OFFSET_Y,
	        MID_X - TRIANGLE_OFFSET_X, MID_Y + TRIANGLE_FAR_OFFSET_Y, DISPLAY_YELLOW);

	display_fillTriangle(MID_X, MID_Y - TRIANGLE_NEAR_OFFSET_Y, MID_X + TRIANGLE_OFFSET_X, MID_Y - TRIANGLE_FAR_OFFSET_Y,
	        MID_X - TRIANGLE_OFFSET_X, MID_Y - TRIANGLE_FAR_OFFSET_Y, DISPLAY_YELLOW);
}

// This function must be defined but can be left empty for now.
// You will use this function in a later lab.
// It is called in the timer interrupt service routine (see interrupts.c in supportFiles).
void isr_function() {
    // Empty for now.
}
