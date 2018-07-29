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
int ChessLogic_getCoordinatePieceType(int x, int y);
int ChessLogic_getCoordinatePieceColor(int x, int y);

// letter range: a, b, c, d, e, f, g, h (must use BoardLetter enum)
// number range: 1, 2, 3, 4, 5, 6, 7, 8
// returns -1 if not in range
int ChessLogic_getChessSquarePieceType(int srcx, int srcy);
int ChessLogic_getChessSquarePieceColor(int srcx, int srcy);
void ChessLogic_getPossibleMoves(uint8_t possibleMoves[BOARDGRIDSIZE][BOARDGRIDSIZE], int srcx, int srcy);
bool ChessLogic_getPieceAvailableMoves(int srcx, int srcy, int dstx, int dsty);
void ChessLogic_getPieceInfo(squareInfo *piece, int srcx, int srcy);

// returns 1 for white player turn
// returns 2 for black player turn
int ChessLogic_getCurrentColorTurn(void);

// returns the number of turns played
int ChessLogic_getTurnCount(void);

// returns 0 for neither
// returns 1 for white in check or checkmate
// returns 2 for black in check or checkmate
int ChessLogic_getCheckStatus(void);
int ChessLogic_getCheckMateStatus(void);	//non-zero game over, zero, continue

bool ChessLogic_getDrawStatus(void);

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

#endif