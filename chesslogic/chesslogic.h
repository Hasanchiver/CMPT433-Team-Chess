#ifndef _CHESSLOGIC_H_
#define _CHESSLOGIC_H_
#include <stdbool.h>

/******************************************************
 * Accessor Chess Logic Functions
 ******************************************************/
void ChessLogic_startNewGame(void);

// x range: 0-7
// y range: 0-7
// returns -1 if not in range
int ChessLogic_getCoordinatePieceType(int x, int y);
int ChessLogic_getCoordinatePieceColor(int x, int y);

// letter range: a, b, c, d, e, f, g, h (must use BoardLetter enum)
// number range: 1, 2, 3, 4, 5, 6, 7, 8
// returns -1 if not in range
int ChessLogic_getChessSquarePieceType(char letter, char number);
int ChessLogic_getChessSquarePieceColor(char letter, char number);

bool ChessLogic_getPieceAvailableMoves(char srcletter, char srcnumber, int dstx, int dsty);
int ChessLogic_getCurrentTurn(void);
int ChessLogic_getCheckStatus(void);
//int ChessLogic_getCheckMateStatus(void);

/******************************************************
 * Mutator Chess Logic Functions
 ******************************************************/
// moves piece from one location to another
// returns 0 if successful
// returns -1 if move is not applicable
int ChessLogic_movePiece(char srcletter, char srcnumber, char dstletter, char dstnumber);

#endif