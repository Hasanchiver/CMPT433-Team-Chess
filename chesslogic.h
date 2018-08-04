#ifndef _CHESSLOGIC_H_
#define _CHESSLOGIC_H_
#include "global.h"
#include <stdbool.h>
#include <stdint.h>

/******************************************************
 * Accessor Chess Logic Functions
 ******************************************************/
void ChessLogic_startNewGame(void);

// x range: 0-7
// y range: 0-7
// returns -1 if not in range
int ChessLogic_getChessSquarePieceType(int srcx, int srcy);
int ChessLogic_getChessSquarePieceColor(int srcx, int srcy);

// letter range: a, b, c, d, e, f, g, h (must use BoardLetter enum)
// number range: 1, 2, 3, 4, 5, 6, 7, 8
// returns -1 if not in range
int ChessLogic_getChessSquarePieceTypeChar(char letter, char number);
int ChessLogic_getChessSquarePieceColorChar(char letter, char number);

// returns true if a piece at (srcx, srcy) can move to (dstx, dsty)
bool ChessLogic_getPieceAvailableMoves(int srcx, int srcy, int dstx, int dsty);

// similar to ChessLogic_getPieceAvailableMoves but can take i.e."e3 e4" as input
bool ChessLogic_getPieceAvailableMovesChar(char srcletter, char srcnumber, int dstx, int dsty);

// returns 1 for white player turn
// returns 2 for black player turn
int ChessLogic_getCurrentColorTurn(void);

// returns the number of turns played
int ChessLogic_getTurnCount(void);

// returns 0 for neither
// returns 1 for white in check
// returns 2 for black in check
int ChessLogic_getCheckStatus(void);

// returns 0 for neither
// returns 1 for white checkmate (Black Wins)
// returns 2 for black checkmate (White Wins)
int ChessLogic_getCheckMateStatus(void);	//non-zero game over, zero, continue





bool ChessLogic_getDrawStatus(void);

/******************************************************
 * Functions for LCD
 ******************************************************/
// updates an 8 by 8 2d array with all the possible moves of a piece
void ChessLogic_getPossibleMoves(uint8_t possibleMoves[BOARDGRIDSIZE][BOARDGRIDSIZE], int srcx, int srcy);

// updates an 8 by 8 2d array with the current state of the board
void ChessLogic_getBoardStateGrid(squareInfo boardStateGrid[BOARDGRIDSIZE][BOARDGRIDSIZE]);

// updates the struct input with information of the piece at (srcx, srcy)
void ChessLogic_getPieceInfo(squareInfo *piece, int srcx, int srcy);

bool ChessLogic_castlingTriggered(piecePosUpdate *pieceInfo);
bool ChessLogic_enPassantTriggered(piecePosUpdate *pieceInfo);

/******************************************************
 * Functions for StockFish UCI
 ******************************************************/

// Useful functions for Stockfish UCI
int ChessLogic_getHalfMoveTimer(void);
int ChessLogic_getFullMoveTimer(void);

bool ChessLogic_canWhiteKingSide(void);
bool ChessLogic_canWhiteQueenSide(void);
bool ChessLogic_canBlackKingSide(void);
bool ChessLogic_canBlackQueenSide(void);

/******************************************************
 * Mutator Chess Logic Functions
 ******************************************************/
// moves piece from one location to another
// returns 0 if successful
// returns -1 if move is not applicable
int ChessLogic_movePiece(int srcx, int srcy, int dstx, int dsty);
int ChessLogic_movePieceChar(char srcletter, char srcnumber, char dstletter, char dstnumber);

#endif