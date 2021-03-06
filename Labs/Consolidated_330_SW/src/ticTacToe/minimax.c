/*
 * minimax.c
 *
 *  Created on: Oct 6, 2016
 *      Author: cdmoo
 */


#include "minimax.h"
#include "minimaxTest.h"
#include <limits.h>
#include <stdio.h>

#define MAX_BOARD_INDEX 2 // highets row / column index on the game boarde
#define MOVE_SCORE_TABLE_SIZE 10 // the size for each move / score array create during recursion
#define INIT_SCORE_TABLE_VAL -5 // value that score arrays are initialized to
#define INIT_MOVE_TABLE_VAL 255 // value that the move arrays are initialized to
#define STARTING_RECURSION_DEPTH 0 // initial depth parameter for recursion
#define CHOICE_INIT_VAL  255 // value that the choice global is initialized to
#define COMPUTER_FIRST_MOVE_X 1 // if the computer is given the first move, it is hard coded to move to this x
#define COMPUTER_FIRST_MOVE_Y 1 // if the computer is given the first move, it is hard coded to move to this y
//for debugging: number of boards created in a run through of the recursion, reset after each computeNextStep is finished
uint32_t boardCount = 0;

// global variable that is overwritten during each call of minimax but contains the most recent selection
minimax_move_t choice = { .row = CHOICE_INIT_VAL, .column = CHOICE_INIT_VAL };

// function delcarations -- definitions found below
uint16_t findChoiceIndex(minimax_score_t* scoreTable, bool player);
minimax_score_t minimax_rec(minimax_board_t* board, bool player, uint16_t depth);
void initArrays(minimax_move_t* moveTable, minimax_score_t* scoreTable);
void addMoveToTable(minimax_move_t* moveTable, minimax_move_t move);
void addScoreToTable(minimax_score_t* scoreTable, minimax_score_t score);
bool isBoardEmpty(minimax_board_t* board);

// Determine that the game is over by looking at the score.
bool minimax_isGameOver(minimax_score_t score) {
    return score != MINIMAX_NOT_ENDGAME;
}

// This routine is not recursive but will invoke the recursive minimax function.
// It computes the row and column of the next move based upon:
// the current board,
// the player. true means the computer is X. false means the computer is O.
void minimax_computeNextMove(minimax_board_t* board, bool player, uint8_t* row, uint8_t* column) {
    if(isBoardEmpty(board)) {
        choice.row = COMPUTER_FIRST_MOVE_Y;
        choice.column = COMPUTER_FIRST_MOVE_X;
    } else {
        // recurses and stores the new choice in the global choice varaible
        minimax_rec(board, player, STARTING_RECURSION_DEPTH);
    }
    // extract the row value to a global variable
    *row = choice.row;
    // extract the column value to a global variable
    *column = choice.column;
    // reset the global that keeps track of boards created during recursion to 0
    boardCount = 0;
}

// the recursive function that produces all possible board combinations and caclutes the most
// advantageous move for the computer to take
minimax_score_t minimax_rec(minimax_board_t* board, bool player, uint16_t depth) {
    // transfer the blobal value of the board count to give each created board a unique number
    uint16_t boardNum = boardCount;

    // base case of the recursion
        // first, compute the board score
    minimax_score_t score = minimax_computeBoardScore(board, !player);
    // if the board score indicates the the game is over
    if(minimax_isGameOver(score)) {
        // return the computed score -- passing in !player indicates that the last person to take a move
        // left the board in an end game state -- they won
        return minimax_computeBoardScore(board, !player);
    }

    // if the base case is passed, all possible moves from this board on will be calculated
    // the following arrays represent a move / score table that will keep track of the best
    // possible move
    minimax_score_t scoreTable[MOVE_SCORE_TABLE_SIZE];
    minimax_move_t moveTable[MOVE_SCORE_TABLE_SIZE];
    // init the table with a call to the utility function
    initArrays(moveTable, scoreTable);

    // after creating the tables, recurse on each of the empty squares left in the game
    uint32_t i, j;
    // iterate through each row
    for(i = 0; i < MINIMAX_BOARD_ROWS; i++) {
        // iterate through each column
        for(j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
            // check if the swuare is empty
            if(board->squares[i][j] == MINIMAX_EMPTY_SQUARE) {
                // depending on whose turn it is, assign the square to have an x or o value
                board->squares[i][j] = player ? MINIMAX_PLAYER_SQUARE : MINIMAX_OPPONENT_SQUARE;

                // compute the best possible score of this board by recursively calling miminmax
                // on this board, but switching the player (it is the next players turn)
                minimax_score_t score = minimax_rec(board, !player, depth);

                // create a move struct and store the current i and j values (corresponding to
                // row and column values) in the move
                minimax_move_t curMove;
                curMove.row = i;
                curMove.column = j;
                // add the move to the move table
                addMoveToTable(moveTable, curMove);

                // add the score to the score table
                addScoreToTable(scoreTable, score);

                //undo the change to the board (return the square to empty)
                board->squares[i][j] = MINIMAX_EMPTY_SQUARE;
            }
        }
    }

    // in order to return the most advantageous move, first find the index of the highest score
    uint16_t index = findChoiceIndex(scoreTable, player);
    // then return the move that corresponds to that score
    choice = moveTable[index];
    return scoreTable[index];
}

// Returns the score of the board, based upon the player and the board.
// This returns one of 4 values: MINIMAX_PLAYER_WINNING_SCORE,
// MINIMAX_OPPONENT_WINNING_SCORE, MINIMAX_DRAW_SCORE, MINIMAX_NOT_ENDGAME
// Note: the player argument makes it possible to speed up this function.
// Assumptions:
// (1) if player == true, the last thing played was an 'X'.
// (2) if player == false, the last thing played was an 'O'.
// Hint: If you know the game was not over when an 'X'was played,
// you don't need to look for 'O's, and vice-versa.
minimax_score_t minimax_computeBoardScore(minimax_board_t* board, bool player) {
    // since only either X or O needs to be searched based off the player argument, determine which one will be searched for
    uint16_t valToSearch;
    valToSearch = player ? MINIMAX_PLAYER_SQUARE : MINIMAX_OPPONENT_SQUARE;

    // search for wins in rows, columns, and diagonals
    uint32_t i, j, rowCount, columnCount, diagonalCount1, diagonalCount2, nonEmptyCount;
    // keep a count of each row, column, and diagonal so that a win can be detected
    rowCount = columnCount = diagonalCount1 = diagonalCount2 = nonEmptyCount = 0;
    // iterate through each row
    for(i = 0; i < MINIMAX_BOARD_ROWS; i++) {\
        // iterate through each column
        for(j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {

            // search each row
            if(board->squares[i][j] == valToSearch) {
                // increment row count if the appropriate value is detected
                rowCount++;
            }

            // search each column by using the reversed coordinates of the row check
            if(board->squares[j][i] == valToSearch) {
                // increment  count if the appropriate value is detected
                columnCount++;
            }

            // search diagonal 1 (top right to bottom left) by checking only when i = j
            if(i == j && board->squares[i][j] == valToSearch) {
                // increment diagonal1 count if the appropriate value is detected
                diagonalCount1++;
            }

            // search diagonal 1 (top left to bottom right)
            if(i + j == MAX_BOARD_INDEX && board->squares[i][j] == valToSearch) {
                // increment diagonal2 count if the appropriate value is detected
                diagonalCount2++;
            }

            // if a square is non zero, it is non empty and should be counted so that recursion
            // can end when the game is in a draw
            if(board->squares[i][j]) {
                nonEmptyCount++;
            }
        }

        // check to see if the row count or column count reached a win
        if(rowCount == MINIMAX_BOARD_ROWS || columnCount == MINIMAX_BOARD_COLUMNS) {
            // return the appropriate score if a win is detected
            return player ? MINIMAX_PLAYER_WINNING_SCORE : MINIMAX_OPPONENT_WINNING_SCORE;
        }
        // reset the row and column count after each row / column
        rowCount = columnCount = 0;
    }

    // after iterating through the whole board, check to see if either diagonal reached a win
    if(diagonalCount1 == MINIMAX_BOARD_ROWS || diagonalCount2 == MINIMAX_BOARD_ROWS) {
        // return the apropriate end game score if a win is reached
        return player ? MINIMAX_PLAYER_WINNING_SCORE : MINIMAX_OPPONENT_WINNING_SCORE;
    }

    // if the non empty count indicates that the board is full
    if(nonEmptyCount == (MINIMAX_BOARD_COLUMNS * MINIMAX_BOARD_ROWS)) {
        // return the score indicating that the game is a draw
        return MINIMAX_DRAW_SCORE;
    }

    // if no endgame is reached return the approprate score
    return MINIMAX_NOT_ENDGAME;
}

// helper function to init the board to all empty squares.
void minimax_initBoard(minimax_board_t* board) {
    uint32_t i, j;
    // iterate through every row
    for(i = 0; i < MINIMAX_BOARD_ROWS; i++) {
        // iterate through every column
        for(j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
            // intialize each square to an empty value
            board->squares[i][j] = MINIMAX_EMPTY_SQUARE;
        }
    }
}

// helper function to init the move / score table to empty scores and moves
void initArrays(minimax_move_t* moveTable, minimax_score_t* scoreTable) {
    uint32_t i;
    // iterate through the whole table
    for(i = 0; i < MOVE_SCORE_TABLE_SIZE; i++) {
        // create a move
        minimax_move_t tempMove;
        tempMove.row = INIT_MOVE_TABLE_VAL; // populate move with init row value
        tempMove.column = INIT_MOVE_TABLE_VAL; // populate move with init row value
        moveTable[i] = tempMove; // insert the move into the table

        minimax_score_t tempScore = INIT_SCORE_TABLE_VAL; // create and init a score
        scoreTable[i] = tempScore; // insert it into the table
    }
}

// helper function  to add a move into the move table
void addMoveToTable(minimax_move_t* moveTable, minimax_move_t move) {
    uint32_t i = 0;
    // search for the earliest open index in the table
    while(moveTable[i].row != INIT_MOVE_TABLE_VAL) {
        i++;
    }
    // insert the move into the earliest open index
    moveTable[i] = move;
}


// helper function to add a score into the score table
void addScoreToTable(minimax_score_t* scoreTable, minimax_score_t score) {
    uint32_t i = 0;
    // search for the earliest open index
    while(scoreTable[i] != INIT_SCORE_TABLE_VAL) {
        i++;
    }
    // insert the score into the earliest open index
    scoreTable[i] = score;
}

// helper function that accepts a move / score table and returns the most advantageous
// move / score pair from the table
uint16_t findChoiceIndex(minimax_score_t* scoreTable, bool player) {
    uint16_t lowestIndex, highestIndex; // keep track of lowest and highest score / move pairs
    int16_t lowestVal = SHRT_MAX; // init the lowest val to be the maximum value for a short
    int16_t highestVal = SHRT_MIN; // init the highest val to be the minimum value for a short
    uint32_t i = 0;
    // iterate through the entire move / score table
    while(scoreTable[i] != INIT_SCORE_TABLE_VAL) {
        // if the current score is lower than the lowest val
        if(scoreTable[i] < lowestVal) {
            // replace the lowest score and the lowest index
            lowestVal = scoreTable[i];
            lowestIndex = i;
        }
        // if the current score is higher than the highest val
        if(scoreTable[i] > highestVal) {
            // replace the lowest score and the lowest index
            highestVal = scoreTable[i];
            highestIndex = i;
        }
        i++;
    }

    // if the search was done on behalf of a player, return the higest score index
    // if it was done on behalf of the oppenent, return the lowest score index
    return player ? highestIndex : lowestIndex;
}

// helper function to determine if the board is empty
bool isBoardEmpty(minimax_board_t* board) {
    uint32_t i, j;
    // iterate through each row
    for(i = 0; i < MINIMAX_BOARD_ROWS; i++) {
        // iterate through each column
        for(j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
            // check if the swuare is empty
            if(board->squares[i][j] != MINIMAX_EMPTY_SQUARE) {
                // if there is an empty square, return false
                return false;
            }
        }
    }
    // return true if no empty squares were detected
    return true;
}

// NOTE I have a massive test module in minimaxTest.h.  My tests were gettings so big
// that it didn't make sense to contain them and the test boards in the minimax module
// the TA that passed me off said not to include the test module, however the minimax_runTest()
// method runs those test suites
void minimax_runTest() {
    minimaxTest_runComputeScoreTestSuite();
    minimaxTest_runComputeNextMoveTestSuite();
}
