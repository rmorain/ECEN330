#ifndef MINIMAXTESTDATA_H_
#define MINIMAXTESTDATA_H_

#define NUMBER_MINIMAX_TEST_BOARDS 15

extern minimax_board_t minimaxTest_board1, minimaxTest_board2, minimaxTest_board3, minimaxTest_board4, minimaxTest_board5,
                        minimaxTest_board6, minimaxTest_board7, minimaxTest_board8, minimaxTest_board9, minimaxTest_board10,
                        minimaxTest_board11, minimaxTest_board12, minimaxTest_board13, minimaxTest_board14, minimaxTest_board15;

extern minimax_board_t minimaxTest_boards[NUMBER_MINIMAX_TEST_BOARDS];

void minimaxTest_initTestBoards();
void minimaxTest_runComputeScoreTestSuite();
void minimaxTest_runComputeNextMoveTestSuite();
void minimaxTest_printBoard(minimax_board_t* board);
void minimaxTest_printAllBoards();

#endif
