/*
 * ticTacToeControl.c
 *
 *  Created on: Oct 22, 2016
 *      Author: cdmoo
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "ticTacToeDisplay.h"
#include "supportFiles/display.h"
#include "minimax.h"
#include "minimaxTest.h"
#include "../switchesAndButtons/buttons.h"
#include "../intervalTimer/intervalTimer.h"
#include "supportFiles/utils.h"

#define TIMER_PERIOD 50 // the current period of each interrupt
// the maximum time in ms of the adc counter divided by the interrupt period
// to determine the number of ticks to stay in that state
#define ADC_COUNTER_MAX (50 / TIMER_PERIOD)
// the maximum time in ms of the splash screen counter divided by the interrupt period
// to determine the number of ticks to stay in that state
#define SPLASH_SCREEN_COUNTER_MAX (3000 / TIMER_PERIOD)
// the maximum time in ms of the first move counter divided by the interrupt period
// to determine the number of ticks to stay in that state
#define FIRST_MOVE_COUNTER_MAX (3000 / TIMER_PERIOD)
#define TEST_TICK_PERIOD_MS 50 // period of a tick while running the test


// States for the controller state machine.
typedef enum gameControl_st_t {
    init_st,                 // Start here, stay in this state for just one tick.
    splash_screen_st,       // state for displaying the instructions message before the game begins
    waiting_first_move_st,   // state for waiting on the player to make the first move (else computer makes first move)
    adc_counter_running_st,     // waiting for the touch-controller ADC to settle.
    player_turn_waiting_st,    // waiting for the player to make a move
    evaluate_player_move_st,   // state that determines validity of move, returning control to player if move was invalid
    computer_turn_st,  // state waiting for computer to make move
    end_game_st  // state at the end of the game, waiting for reset button to be pressed
} gameControl_st_t;
// initialize the state to initState
static gameControl_st_t currentState = init_st;

static uint32_t adcCounter; // global keeping tracks of ticks in the adc state
static uint32_t splashScreenCounter; // global keeping tracks of ticks in the splash screen state
static uint32_t firstMoveCounter; // global keeping tack of the waiting for the first move state

static bool isPlayerTurn; // global keeping track of whose turn it is
static bool isPlayerX; // global keeping track of whose playing which character
static minimax_board_t gameBoard; // global game board used for keeping track of game state
static minimax_move_t computerNextMove; // global holding the computers calculated next move
static minimax_move_t playerNextMove; // global keeping track of the players next move
static minimax_score_t currentScore; // global keeping track of score

// function declarations (definitions found below)
static void debugStatePrint();
static bool isPlayerMoveValid();
static void playNextMove();
static bool resetGame();
static void eraseGameBoard();

void ticTacToeControl_tick() {
    //perform state action first
    switch(currentState) {
        case init_st: // no state action in the init state
            break;
        case splash_screen_st:
            splashScreenCounter++;  // increase the splash screen counter during each tick in the state
            break;
        case waiting_first_move_st:
            firstMoveCounter++; // increase the first move counter during each tick in the state
            break;
        case adc_counter_running_st:
            adcCounter++; // increase the adc counter during each tick in the state
            break;
        case player_turn_waiting_st:
            isPlayerTurn = true; // during the play turn state, set the global indicating that it is the players turn
            break;
        case evaluate_player_move_st:
            // to evaluate the player move, fetch and store the players move on the touch screen
            ticTacToeDisplay_touchScreenComputeBoardRowColumn(&playerNextMove.row, &playerNextMove.column);
            // reset the score to include the most recent computed score
            currentScore = minimax_computeBoardScore(&gameBoard, isPlayerX);
            break;
        case computer_turn_st:
            isPlayerTurn = false; // during the computer's turn, set the global indicating that it is not the players turn
            // use minimax to compute the next move based off of the current board
            minimax_computeNextMove(&gameBoard, !isPlayerX, &computerNextMove.row, &computerNextMove.column);
            playNextMove(); // mark the move on the game board and draw it to the screen
            currentScore = minimax_computeBoardScore(&gameBoard, !isPlayerX); // update the current score after the move
            break;
        case end_game_st: // no state action for the end game state
            break;
    }

    // perform state update next
    switch(currentState) {
        case init_st:
            // immediately transition from the init state to the splash screen state
            currentState = splash_screen_st;
            break;
        case splash_screen_st:
            // during the splash screen state, test to see if the counter has reached its maximum
            if(splashScreenCounter >= SPLASH_SCREEN_COUNTER_MAX) {
                // if the timer has expired, clear the splash screen
                ticTacToeDisplay_clearSplashScreen();
                ticTacToeDisplay_drawBoardLines(); // draw the game board
                minimax_initBoard(&gameBoard); // initialize the game board
                currentState = waiting_first_move_st; // transition to teh waiting for first move state
            }
            break;
        case waiting_first_move_st:
            // only leave this state for 2 conditions
            //      1) if the display is touched (indicating the player will take the first move)
            if(display_isTouched()) {
                isPlayerX = true; // flag the player as playing x
                isPlayerTurn = true; // flag the players turn
                display_clearOldTouchData(); // clear out the old touch data
                currentState = adc_counter_running_st; // transition to the adc state to determine location of touch
            //     2) leave this state if the timer has expired (indicating that the computer will take the first turn)
            } else if(firstMoveCounter >= FIRST_MOVE_COUNTER_MAX) {
                // if it is the computers turn, set the flag indicating
                isPlayerX = false;
                currentState = computer_turn_st; // transition to the computer turn state
            }
            break;
        case adc_counter_running_st:
            // this state waits the prescribed amount after detecting a touch before determining its location
            // if the counter expires and the player has stopped touching, return control to the player
            if(!display_isTouched() && adcCounter >= ADC_COUNTER_MAX) {
                currentState = player_turn_waiting_st; // leave back to the player turn state
                // if the counter expires and the player is still touching
            } else if(display_isTouched() && adcCounter >= ADC_COUNTER_MAX) {
                // go to the state that will evaluate the players move for validity
                currentState = evaluate_player_move_st;
            }
            break;
        case player_turn_waiting_st: // waits for the player to make a move on the touch screen
            // if the player touches
            if(display_isTouched()) {
                display_clearOldTouchData();  // clear old touch data
                currentState = adc_counter_running_st; // go to the adc wait state to determine location
            }
            break;
        case computer_turn_st: // state allowing computer to play
            // if the game is over
            if(minimax_isGameOver(currentScore)) {
                // transition to the end game state
                currentState = end_game_st;
            } else {
                // otherwise return control to the player
                currentState = player_turn_waiting_st;
            }
            break;
        case evaluate_player_move_st: // this states transition contains logic to determine validity of players move
            // if the game is over
            if(minimax_isGameOver(currentScore)) {
                // transition to the end game state
                currentState = end_game_st;
                // if the players move was valid
            } else if(isPlayerMoveValid()) {
                // update the board with the players move
                playNextMove();
                currentState = computer_turn_st; // return control to the computer
            } else {
                // if the players move was invalid, return control to the player without
                // playing his move
                currentState = player_turn_waiting_st;
            }
            break;
        case end_game_st:  // wait in the end game state for the game to be reset
            if(resetGame()) { // if the game is reset, using button -
                eraseGameBoard(); // erase all the moves on the game board
                minimax_initBoard(&gameBoard); // re-initialize the game board
                currentState = waiting_first_move_st; // transition the wiaiting for first move state
                adcCounter = 0; // reset adc counter
                splashScreenCounter = 0; // reset splash screen counter
                firstMoveCounter = 0; // reset first move counter
                currentScore = 0; // reset current score counter
            }
            break;
    }
}

// helper function that contains logic for putting the next move onto the board
static void playNextMove() {
    // if it is the players turn
    if(isPlayerTurn) {
        // if the player is playing x
        if(isPlayerX) {
            // mark the appropriate square with an x
            gameBoard.squares[playerNextMove.row][playerNextMove.column] = MINIMAX_PLAYER_SQUARE;
            // draw the move
            ticTacToeDisplay_drawX(playerNextMove.row, playerNextMove.column, false);
        } else {
            // mark the appropriate square with an o
            gameBoard.squares[playerNextMove.row][playerNextMove.column] = MINIMAX_OPPONENT_SQUARE;
            // draw the move
            ticTacToeDisplay_drawO(playerNextMove.row, playerNextMove.column, false);
        }
        // draw the computers move
    } else {
        // if the computer is playing x
        if(!isPlayerX) {
            // mark the appropriate square with an x
            gameBoard.squares[computerNextMove.row][computerNextMove.column] = MINIMAX_PLAYER_SQUARE;
            // draw the move
            ticTacToeDisplay_drawX(computerNextMove.row, computerNextMove.column, false);
        } else {
            // mark the appropriate sqaure with an o
            gameBoard.squares[computerNextMove.row][computerNextMove.column] = MINIMAX_OPPONENT_SQUARE;
            // draw the move
            ticTacToeDisplay_drawO(computerNextMove.row, computerNextMove.column, false);
        }
    }
}

// helper function to determine the validity of a move
static bool isPlayerMoveValid() {
    // if the square is empty, return true
    return !gameBoard.squares[playerNextMove.row][playerNextMove.column];
}

// helper functino to determine if the game should be reset
static bool resetGame() {
    // read the buttons and mask to extract the value of button 0
    return buttons_read() & BUTTONS_BTN0_MASK;
}

// helper function that redraws the dispay as blank squares
static void eraseGameBoard() {
    // iterate over each row
    for(uint32_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
        // iterate over each column
        for(uint32_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
            // if the square is non empty
            if(gameBoard.squares[i][j]) {
                // draw x and o with the erase flag set to true
                ticTacToeDisplay_drawX(i, j, true);
                ticTacToeDisplay_drawO(i, j, true);
            }
        }
    }
}

// runs a test and shows how the control module is used
void ticTacToeControl_runTest() {
    // indefinitely loops the the control tick
    while(1) {
        ticTacToeControl_tick(); // calls the control tick function
        utils_msDelay(TEST_TICK_PERIOD_MS); // waits the prescribed period (50 ms)
    }
}


