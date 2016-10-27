#include "minimax.h"
#include "minimaxTest.h"
#include <stdio.h>

#define BOARD_DIMENSIONS 3
#define MINIMAX_PLAYER_SQUARE 2
#define MINIMAX_OPPONENT_SQUARE 1
#define MINIMAX_EMPTY_SQUARE 0

int8_t expectedPlayerScores[] = {-1, -1, -1, -1, -1, -1, 10, -1, -1, -1, -1, -1, -1, -1, -1};
int8_t expectedOppScores[] = {-1, -1, -10, -1, -1, -1, -10, -10, -1, -1, -1, -1, -1, -1, -1};

void minimaxTest_runComputeScoreTestSuite() {

    int i;
    for(i = 0; i < NUMBER_MINIMAX_TEST_BOARDS; i++) {
        minimaxTest_printBoard(minimaxTest_boards + i);
        int16_t scorePlayer = minimax_computeBoardScore(minimaxTest_boards + i, true);
        int16_t scoreOpp = minimax_computeBoardScore(minimaxTest_boards + i, false);
        printf("board1 player: expected %d,  calculated %d\n", expectedPlayerScores[i], scorePlayer);
        printf("board1 opp: expected %d,  calculated %d\n", expectedOppScores[i], scoreOpp);
        scorePlayer != expectedPlayerScores[i] || scoreOpp != expectedOppScores[i] ? printf("BOARD %d FAILED COMPUTE SCORE\n", i+1) : printf("BOARD %d PASSED COMPUTE SCORE\n", i+1);
        printf("\n");
    }
}

void minimaxTest_runComputeNextMoveTestSuite() {

}

void minimaxTest_printAllBoards() {
    int i;
    for(i = 0; i < NUMBER_MINIMAX_TEST_BOARDS; i++) {
        printf("**BOARD %d**\n", i+1);
        minimaxTest_printBoard(minimaxTest_boards + i);
    }
}

void minimaxTest_printBoard(minimax_board_t* board) {
    int i, j;
    for(i = 0; i < BOARD_DIMENSIONS; i++) {
        for(j = 0; j < BOARD_DIMENSIONS; j++) {
            if(board->squares[i][j] == MINIMAX_PLAYER_SQUARE) printf("X ");
            else if(board->squares[i][j] == MINIMAX_OPPONENT_SQUARE) printf("O ");
            else printf("  ");
        }
        printf("\n");
    }
    printf("\n");
}

minimax_board_t minimaxTest_board1, minimaxTest_board2, minimaxTest_board3, minimaxTest_board4, minimaxTest_board5,
                minimaxTest_board6, minimaxTest_board7, minimaxTest_board8, minimaxTest_board9, minimaxTest_board10,
                minimaxTest_board11, minimaxTest_board12, minimaxTest_board13, minimaxTest_board14, minimaxTest_board15;

minimax_board_t minimaxTest_boards[NUMBER_MINIMAX_TEST_BOARDS];

void minimaxTest_initTestBoards() {
    minimaxTest_board1.squares[0][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board1.squares[0][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board1.squares[0][2] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board1.squares[1][0] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board1.squares[1][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board1.squares[1][2] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board1.squares[2][0] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board1.squares[2][1] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board1.squares[2][2] = MINIMAX_OPPONENT_SQUARE;

    minimaxTest_board2.squares[0][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board2.squares[0][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board2.squares[0][2] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board2.squares[1][0] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board2.squares[1][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board2.squares[1][2] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board2.squares[2][0] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board2.squares[2][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board2.squares[2][2] = MINIMAX_OPPONENT_SQUARE;

    minimaxTest_board3.squares[0][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board3.squares[0][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board3.squares[0][2] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board3.squares[1][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board3.squares[1][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board3.squares[1][2] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board3.squares[2][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board3.squares[2][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board3.squares[2][2] = MINIMAX_PLAYER_SQUARE;

    minimaxTest_board4.squares[0][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board4.squares[0][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board4.squares[0][2] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board4.squares[1][0] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board4.squares[1][1] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board4.squares[1][2] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board4.squares[2][0] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board4.squares[2][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board4.squares[2][2] = MINIMAX_PLAYER_SQUARE;

    minimaxTest_board5.squares[0][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board5.squares[0][1] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board5.squares[0][2] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board5.squares[1][0] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board5.squares[1][1] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board5.squares[1][2] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board5.squares[2][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board5.squares[2][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board5.squares[2][2] = MINIMAX_EMPTY_SQUARE;

    minimaxTest_board6.squares[0][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board6.squares[0][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board6.squares[0][2] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board6.squares[1][0] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board6.squares[1][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board6.squares[1][2] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board6.squares[2][0] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board6.squares[2][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board6.squares[2][2] = MINIMAX_EMPTY_SQUARE;

    // draw exp: 0
    minimaxTest_board7.squares[0][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board7.squares[0][1] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board7.squares[0][2] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board7.squares[1][0] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board7.squares[1][1] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board7.squares[1][2] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board7.squares[2][2] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board7.squares[2][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board7.squares[2][1] = MINIMAX_PLAYER_SQUARE;

    minimaxTest_board8.squares[0][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board8.squares[0][1] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board8.squares[0][2] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board8.squares[1][0] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board8.squares[1][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board8.squares[1][2] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board8.squares[2][2] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board8.squares[2][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board8.squares[2][1] = MINIMAX_PLAYER_SQUARE;

    minimaxTest_board9.squares[0][0] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board9.squares[0][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board9.squares[0][2] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board9.squares[1][0] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board9.squares[1][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board9.squares[1][2] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board9.squares[2][0] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board9.squares[2][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board9.squares[2][2] = MINIMAX_EMPTY_SQUARE;

    minimaxTest_board10.squares[0][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board10.squares[0][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board10.squares[0][2] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board10.squares[1][0] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board10.squares[1][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board10.squares[1][2] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board10.squares[2][0] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board10.squares[2][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board10.squares[2][2] = MINIMAX_PLAYER_SQUARE;

    minimaxTest_board11.squares[0][0] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board11.squares[0][1] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board11.squares[0][2] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board11.squares[1][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board11.squares[1][1] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board11.squares[1][2] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board11.squares[2][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board11.squares[2][1] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board11.squares[2][2] = MINIMAX_PLAYER_SQUARE;

    minimaxTest_board12.squares[0][0] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board12.squares[0][1] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board12.squares[0][2] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board12.squares[1][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board12.squares[1][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board12.squares[1][2] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board12.squares[2][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board12.squares[2][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board12.squares[2][2] = MINIMAX_PLAYER_SQUARE;

    minimaxTest_board13.squares[0][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board13.squares[0][1] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board13.squares[0][2] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board13.squares[1][0] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board13.squares[1][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board13.squares[1][2] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board13.squares[2][0] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board13.squares[2][1] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board13.squares[2][2] = MINIMAX_EMPTY_SQUARE;

    minimaxTest_board14.squares[0][0] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board14.squares[0][1] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board14.squares[0][2] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board14.squares[1][0] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board14.squares[1][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board14.squares[1][2] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board14.squares[2][0] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board14.squares[2][1] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board14.squares[2][2] = MINIMAX_OPPONENT_SQUARE;

    minimaxTest_board15.squares[0][0] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board15.squares[0][1] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board15.squares[0][2] = MINIMAX_OPPONENT_SQUARE;
    minimaxTest_board15.squares[1][0] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board15.squares[1][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board15.squares[1][2] = MINIMAX_PLAYER_SQUARE;
    minimaxTest_board15.squares[2][0] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board15.squares[2][1] = MINIMAX_EMPTY_SQUARE;
    minimaxTest_board15.squares[2][2] = MINIMAX_OPPONENT_SQUARE;

    minimaxTest_boards[0] = minimaxTest_board1;
    minimaxTest_boards[1] = minimaxTest_board2;
    minimaxTest_boards[2] = minimaxTest_board3;
    minimaxTest_boards[3] = minimaxTest_board4;
    minimaxTest_boards[4] = minimaxTest_board5;
    minimaxTest_boards[5] = minimaxTest_board6;
    minimaxTest_boards[6] = minimaxTest_board7;
    minimaxTest_boards[7] = minimaxTest_board8;
    minimaxTest_boards[8] = minimaxTest_board9;
    minimaxTest_boards[9] = minimaxTest_board10;
    minimaxTest_boards[10] = minimaxTest_board11;
    minimaxTest_boards[11] = minimaxTest_board12;
    minimaxTest_boards[12] = minimaxTest_board13;
    minimaxTest_boards[13] = minimaxTest_board14;
    minimaxTest_boards[14] = minimaxTest_board15;
}
