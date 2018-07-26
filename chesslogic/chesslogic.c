#include "global.h"
#include "chesslogic.h"
#include <stdlib.h> // for abs()
#include <pthread.h>

//#include <wchar.h> // for error checking

#define WHITESIDE 0
#define BLACKSIDE 7
#define FIRSTLETTER "a"
#define FIRSTNUMBER "1"
#define DRAWTIMER 50

typedef struct{
	char pieceType;
	char pieceColor;
	bool firstMove;
	bool castling;
	int idleInSquare; // number of turns pawn has stayed in square
	bool availableMoves[BOARDGRIDSIZE][BOARDGRIDSIZE];
} squareInfo;

static pthread_mutex_t chessMutex = PTHREAD_MUTEX_INITIALIZER;

static squareInfo logicBoard[BOARDGRIDSIZE][BOARDGRIDSIZE];
static Color currentTurn = white;

// flags for Stockfish UCI
static int fullMoveTimer = 0;
static bool whiteCanQueenSide = false;
static bool whiteCanKingSide = false;
static bool blackCanQueenSide = false;
static bool blackCanKingSide = false;

static bool blackCheckFlag = false;
static bool whiteCheckFlag = false;
static bool blackCheckMateFlag = false;
static bool whiteCheckMateFlag = false;
static bool drawFlag = false;
static int halfMoveTimer = 0;

static int numberOfChecks = 0;
static int checkingPieceLocation = -1;
static int kingInCheckLocation = -1;

static bool enPasseFlag = false;
static bool queensideFlag = false;
static bool kingsideFlag = false;
static int turnNumber = 1;

static void ChessLogic_initLogicBoard(void){
	blackCheckFlag = false;
	whiteCheckFlag = false;
	blackCheckMateFlag = false;
	whiteCheckMateFlag = false;
	drawFlag = false;
	halfMoveTimer = 0;
	fullMoveTimer = 0;
	for (int y = 0; y < BOARDGRIDSIZE; y++){
		for (int x = 0; x < BOARDGRIDSIZE; x++){
			logicBoard[x][y].pieceType = nopiece;
			logicBoard[x][y].pieceColor = nocolor;
			logicBoard[x][y].firstMove = true;
			logicBoard[x][y].idleInSquare = 0;
		}
	}

	// setup pawn pieces
	for (int x = 0; x < BOARDGRIDSIZE; x++){
		logicBoard[x][1].pieceType = pawn;
		logicBoard[x][1].pieceColor = white;

		logicBoard[x][BOARDGRIDSIZE-2].pieceType = pawn;
		logicBoard[x][BOARDGRIDSIZE-2].pieceColor = black;
	}

	// setup rook, knight and bishop pieces
	for (int x = 0; x < (BOARDGRIDSIZE/2)-1; x++){
		logicBoard[x][0].pieceType = rook + x;
		logicBoard[x][0].pieceColor = white;
		logicBoard[BOARDGRIDSIZE-1-x][0].pieceType = rook + x;
		logicBoard[BOARDGRIDSIZE-1-x][0].pieceColor = white;

		logicBoard[x][BOARDGRIDSIZE-1].pieceType = rook + x;
		logicBoard[x][BOARDGRIDSIZE-1].pieceColor = black;
		logicBoard[BOARDGRIDSIZE-1-x][BOARDGRIDSIZE-1].pieceType = rook + x;
		logicBoard[BOARDGRIDSIZE-1-x][BOARDGRIDSIZE-1].pieceColor = black;
	}

	// setup queen pieces
	logicBoard[3][0].pieceType = queen;
	logicBoard[3][0].pieceColor = white;

	logicBoard[3][BOARDGRIDSIZE-1].pieceType = queen;
	logicBoard[3][BOARDGRIDSIZE-1].pieceColor = black;

	// setup king pieces
	logicBoard[4][0].pieceType = king;
	logicBoard[4][0].pieceColor = white;

	logicBoard[4][BOARDGRIDSIZE-1].pieceType = king;
	logicBoard[4][BOARDGRIDSIZE-1].pieceColor = black;
}

static int getBoardLetterIncrement(char letter){
	int letterdiff = letter - *FIRSTLETTER;
	if (letterdiff < 0 || letterdiff >= BOARDGRIDSIZE) return -1;
	return letterdiff;
}

static int getBoardNumberIncrement(char number){
	int numberdiff = number - *FIRSTNUMBER;
	if (numberdiff < 0 || numberdiff >= BOARDGRIDSIZE) return -1;
	return numberdiff;
}

static int xyCoordinateToInteger(int x, int y){
	// Converts xy coordinate to integer
	return (y * BOARDGRIDSIZE + x);
}

static void integerToXYCoordinate(int val, int *x, int *y){
	*x = val % BOARDGRIDSIZE;
	*y = (val - *x) / BOARDGRIDSIZE;
}

/******************************************************
 * Special Rules - Chess Logic
 ******************************************************/
static void ChessLogic_incrementIdlePawns(){
	for (int y = 0; y < BOARDGRIDSIZE; y++){
		for (int x = 0; x < BOARDGRIDSIZE; x++){
			if (logicBoard[x][y].pieceType == pawn){
				logicBoard[x][y].idleInSquare++;
			}
		}
	}
}

static void ChessLogic_checkPromotePawn(int srcx, int srcy, int dstx, int dsty){
	if (dsty == BLACKSIDE && logicBoard[srcx][srcy].pieceColor == white){
		logicBoard[srcx][srcy].pieceType = queen;
	} else if (dsty == WHITESIDE && logicBoard[srcx][srcy].pieceColor == black){
		logicBoard[srcx][srcy].pieceType = queen;
	}
	return;
}

/******************************************************
 * Basic Rules - Chess Logic
 ******************************************************/
static bool ChessLogic_isSameColor(int srcx, int srcy, int dstx, int dsty){
	if (logicBoard[srcx][srcy].pieceColor == 
		logicBoard[dstx][dsty].pieceColor) return true;
	return false;
}

static bool CheckLogic_isJumpingOverPiece(int srcx, int srcy, int dstx, int dsty){
	int diffx = abs(srcx - dstx);
	int diffy = abs(srcy - dsty);
	// Checks if move is jumping over pieces (don't count for knight)
	// Moving in vertical direction
	if (srcx == dstx){
		if (srcy < dsty){ // Piece moves +y direction, so check after srcy and before dsty
			for (int y = srcy + 1; y < dsty; y++){
				if (logicBoard[srcx][y].pieceType != nopiece) return -1;
			}
		}
		if (srcy > dsty){ // Piece moves +y direction, so check after dsty and before srcy
			for (int y = dsty + 1; y < srcy; y++){
				if (logicBoard[srcx][y].pieceType != nopiece) return -1;
			}
		}
	}
	// Moving in a horizontal direction
	if (srcy == dsty){
		if (srcx < dstx){ // Piece moves +x direction, so check after srcy and before dsty
			for (int x = srcx + 1; x < dstx; x++){
				if (logicBoard[x][srcy].pieceType != nopiece) return -1;
			}
		}
		if (srcx > dstx){ // Piece moves -x direction, so check after dsty and before srcy
			for (int x = dstx + 1; x < srcx; x++){
				if (logicBoard[x][srcy].pieceType != nopiece) return -1;
			}
		}
	}
	// Moving in a diagonal direction
	if (diffx == diffy){
		if ((srcx < dstx) && (srcy < dsty)){ // Piece moves +x +y direction
			for (int i = 1; i < diffx; i++){
				if (logicBoard[srcx+i][srcy+i].pieceType != nopiece) return -1;
			}
		}
		if ((srcx < dstx) && (srcy > dsty)){ // Piece moves +x -y direction
			for (int i = 1; i < diffx; i++){
				if (logicBoard[srcx+i][srcy-i].pieceType != nopiece) return -1;
			}
		}
		if ((srcx > dstx) && (srcy < dsty)){ // Piece moves -x +y direction
			for (int i = 1; i < diffx; i++){
				if (logicBoard[srcx-i][srcy+i].pieceType != nopiece) return -1;
			}
		}
		if ((srcx > dstx) && (srcy > dsty)){ // Piece moves -x -y direction
			for (int i = 1; i < diffx; i++){
				if (logicBoard[srcx-i][srcy-i].pieceType != nopiece) return -1;
			}
		}
	}
	return 0;
}

static bool ChessLogic_kingMoves(int srcx, int srcy, int dstx, int dsty){
	int diffx = srcx - dstx;
	int diffy = srcy - dsty;

	// Can only move one square in any direction (
	// Move up or down
	if (diffx == 0 && abs(diffy) == 1) return true;
	// Move left or right
	if (diffy == 0 && abs(diffx) == 1) return true;
	// Move diagonally
	if (abs(diffx) == 1 && abs(diffy) == 1) return true;

	// Castling
	// White queenside
	if (diffy == 0 && diffx == 2 && whiteCheckFlag == false &&
		logicBoard[srcx][srcy].firstMove == true && 
		logicBoard[srcx][srcy].pieceColor == white && 
		logicBoard[0][WHITESIDE].pieceType == rook &&
		logicBoard[0][WHITESIDE].firstMove == true){
		queensideFlag = true;
		whiteCanQueenSide = true;
		return true;
	}
	// White kingside
	if (diffy == 0 && diffx == -2 && whiteCheckFlag == false &&
		logicBoard[srcx][srcy].firstMove == true && 
		logicBoard[srcx][srcy].pieceColor == white && 
		logicBoard[BOARDGRIDSIZE-1][WHITESIDE].pieceType == rook &&
		logicBoard[BOARDGRIDSIZE-1][WHITESIDE].firstMove == true){
		kingsideFlag = true;
		whiteCanKingSide = true;
		return true;
	}
	// Black queenside
	if (diffy == 0 && diffx == 2 && blackCheckFlag == false &&
		logicBoard[srcx][srcy].firstMove == true && 
		logicBoard[srcx][srcy].pieceColor == black && 
		logicBoard[0][BLACKSIDE].pieceType == rook &&
		logicBoard[0][BLACKSIDE].firstMove == true){
		queensideFlag = true;
		blackCanQueenSide = true;
		return true;
	}
	// Black kingside
	if (diffy == 0 && diffx == -2 && blackCheckFlag == false &&
		logicBoard[srcx][srcy].firstMove == true && 
		logicBoard[srcx][srcy].pieceColor == black && 
		logicBoard[BOARDGRIDSIZE-1][BLACKSIDE].pieceType == rook &&
		logicBoard[BOARDGRIDSIZE-1][BLACKSIDE].firstMove == true){
		kingsideFlag = true;
		blackCanKingSide = true;
		return true;
	}
	return false;
}

static bool ChessLogic_queenMoves(int srcx, int srcy, int dstx, int dsty){
	int diffx = abs(srcx - dstx);
	int diffy = abs(srcy - dsty);
	// Can move any number of squares in any directions
	// Move up or down
	if (diffx == 0 && diffy > 0) return true;
	// Move left or right
	if (diffx > 0 && diffy == 0) return true;
	// Move diagonally
	if (diffx == diffy) return true;
	return false;
}

static bool ChessLogic_rookMoves(int srcx, int srcy, int dstx, int dsty){
	int diffx = abs(srcx - dstx);
	int diffy = abs(srcy - dsty);
	// Can move any number of squares forwards, backwards, left, or right
	// Move up or down
	if (diffx == 0 && diffy > 0) return true;
	// Move left or right
	if (diffx > 0 && diffy == 0) return true;
	return false;
}

static bool ChessLogic_bishopMoves(int srcx, int srcy, int dstx, int dsty){
	int diffx = abs(srcx - dstx);
	int diffy = abs(srcy - dsty);
	// Can move any number of squares diagonally
	// Move diagonally
	return (diffx == diffy);
}

static bool ChessLogic_knightMoves(int srcx, int srcy, int dstx, int dsty){
	int diffx = abs(srcx - dstx);
	int diffy = abs(srcy - dsty);
	// Moves in an L-shaped movement
	return ((diffx == 2 && diffy == 1) || (diffx == 1 && diffy == 2));
}

static bool ChessLogic_pawnMoves(int srcx, int srcy, int dstx, int dsty){
	int diffx = abs(dstx - srcx);
	int diffy = dsty - srcy;
	// Can move 2 squares forward if first move and no piece on that square
	if (diffx == 0 && diffy == 2 && 
		logicBoard[srcx][srcy].pieceColor == white &&
		logicBoard[srcx][srcy].firstMove == true &&
		logicBoard[dstx][dsty].pieceType == nopiece) return true;
	if (diffx == 0 && diffy == -2 && 
		logicBoard[srcx][srcy].pieceColor == black &&
		logicBoard[srcx][srcy].firstMove == true &&
		logicBoard[dstx][dsty].pieceType == nopiece) return true;
	// Can move 1 square forward and no piece on that square
	if (diffx == 0 && diffy == 1 &&
		logicBoard[srcx][srcy].pieceColor == white &&
		logicBoard[dstx][dsty].pieceType == nopiece) return true;
	if (diffx == 0 && diffy == -1 &&
		logicBoard[srcx][srcy].pieceColor == black &&
		logicBoard[dstx][dsty].pieceType == nopiece) return true;
	// Can move 1 square diagonally forward to capture
	if (diffx == 1 && diffy == 1 && 
		logicBoard[srcx][srcy].pieceColor == white &&
		logicBoard[dstx][dsty].pieceType != nopiece) return true;
	if (diffx == 1 && diffy == -1 && 
		logicBoard[srcx][srcy].pieceColor == black &&
		logicBoard[dstx][dsty].pieceType != nopiece) return true;
	// en passe capture
	if (diffx == 1 && diffy == 1 && 
		logicBoard[srcx][srcy].pieceColor == white &&
		logicBoard[dstx][dsty-1].pieceType == pawn &&
		logicBoard[dstx][dsty-1].pieceColor == black &&
		logicBoard[dstx][dsty-1].idleInSquare == 1){
		enPasseFlag = true;
		return true;
	}
	if (diffx == 1 && diffy == -1 && 
		logicBoard[srcx][srcy].pieceColor == black &&
		logicBoard[dstx][dsty+1].pieceType == pawn &&
		logicBoard[dstx][dsty+1].pieceColor == white &&
		logicBoard[dstx][dsty+1].idleInSquare == 1) {
		enPasseFlag = true;
		return true;
	}
	return false;
}

static bool ChessLogic_isCheck(int srcx, int srcy, int dstx, int dsty){
	if (logicBoard[dstx][dsty].pieceType == king &&
		logicBoard[dstx][dsty].pieceColor == white){
		whiteCheckFlag = true;
		checkingPieceLocation = xyCoordinateToInteger(srcx, srcy);
		return true;
	} else if (logicBoard[dstx][dsty].pieceType == king &&
		logicBoard[dstx][dsty].pieceColor == black){
		blackCheckFlag = true;
		checkingPieceLocation = xyCoordinateToInteger(srcx, srcy);
		return true;
	}
	return false;
}

static void ChessLogic_processMove(int srcx, int srcy, int dstx, int dsty){
	if (logicBoard[srcx][srcy].pieceType == pawn ||
		logicBoard[dstx][dsty].pieceType != nopiece){
		halfMoveTimer = 0;
	}

	if (logicBoard[srcx][srcy].pieceType == pawn && enPasseFlag == true){
		if (logicBoard[dstx][dsty-1].pieceColor == black){
			logicBoard[dstx][dsty-1].pieceType = nopiece;
			logicBoard[dstx][dsty-1].pieceColor = nocolor;
			logicBoard[dstx][dsty-1].idleInSquare = 0;
		}
		if (logicBoard[dstx][dsty+1].pieceColor == white){
			logicBoard[dstx][dsty+1].pieceType = nopiece;
			logicBoard[dstx][dsty+1].pieceColor = nocolor;
			logicBoard[dstx][dsty+1].idleInSquare = 0;
		}
	}

	if (logicBoard[srcx][srcy].pieceType == king && 
		logicBoard[srcx][srcy].pieceColor == white && 
		queensideFlag == true){
		logicBoard[0][WHITESIDE].pieceType = nopiece;
		logicBoard[0][WHITESIDE].pieceColor = nocolor;
		logicBoard[0][WHITESIDE].firstMove = false;
		logicBoard[dstx+1][dsty].pieceType = rook;
		logicBoard[dstx+1][dsty].pieceColor = white;
		logicBoard[dstx+1][dsty].firstMove = false;
	}
	if (logicBoard[srcx][srcy].pieceType == king && 
		logicBoard[srcx][srcy].pieceColor == white && 
		kingsideFlag == true){
		logicBoard[BOARDGRIDSIZE-1][WHITESIDE].pieceType = nopiece;
		logicBoard[BOARDGRIDSIZE-1][WHITESIDE].pieceColor = nocolor;
		logicBoard[BOARDGRIDSIZE-1][WHITESIDE].firstMove = false;
		logicBoard[dstx-1][dsty].pieceType = rook;
		logicBoard[dstx-1][dsty].pieceColor = white;
		logicBoard[dstx-1][dsty].firstMove = false;
	}

	if (logicBoard[srcx][srcy].pieceType == king && 
		logicBoard[srcx][srcy].pieceColor == black && 
		queensideFlag == true){
		logicBoard[0][BLACKSIDE].pieceType = nopiece;
		logicBoard[0][BLACKSIDE].pieceColor = nocolor;
		logicBoard[0][BLACKSIDE].firstMove = false;
		logicBoard[dstx+1][dsty].pieceType = rook;
		logicBoard[dstx+1][dsty].pieceColor = black;
		logicBoard[dstx+1][dsty].firstMove = false;
	}
	if (logicBoard[srcx][srcy].pieceType == king && 
		logicBoard[srcx][srcy].pieceColor == black && 
		kingsideFlag == true){
		logicBoard[BOARDGRIDSIZE-1][BLACKSIDE].pieceType = nopiece;
		logicBoard[BOARDGRIDSIZE-1][BLACKSIDE].pieceColor = nocolor;
		logicBoard[BOARDGRIDSIZE-1][BLACKSIDE].firstMove = false;
		logicBoard[dstx-1][dsty].pieceType = rook;
		logicBoard[dstx-1][dsty].pieceColor = black;
		logicBoard[dstx-1][dsty].firstMove = false;
	}

	logicBoard[dstx][dsty].pieceType = logicBoard[srcx][srcy].pieceType;
	logicBoard[dstx][dsty].pieceColor = logicBoard[srcx][srcy].pieceColor;
	logicBoard[dstx][dsty].firstMove = false;
	logicBoard[dstx][dsty].idleInSquare = 0;

	logicBoard[srcx][srcy].pieceType = nopiece;
	logicBoard[srcx][srcy].pieceColor = nocolor;
	logicBoard[srcx][srcy].firstMove = false;
	logicBoard[srcx][srcy].idleInSquare = 0;

	halfMoveTimer += 1;
}

static void ChessLogic_nextTurn(){
	if (currentTurn == white) { 
		currentTurn = black;
		turnNumber++;

	} 
	else if (currentTurn == black){ 
		currentTurn = white;
		turnNumber++;
		fullMoveTimer++;
	}
}

/******************************************************
 * Calculate Possible Moves For Each Piece
 ******************************************************/
// calculates possible moves to get out of check by removing 
// possible moves that don't get out of check
// if result has no possible moves, it is checkmate
// 3 ways to get out of check:
// 	- move king out of check
// 	- move another piece to block the check
// 	- move another piece to capture the checking piece
// In the case of double check or more, only possible response
// is to move the king, else checkmate
static int ChessLogic_recalculateMovesInCheck(int srcx, int srcy){
	int numOfPossibleMoves = 0;
	int kingx, kingy, cpx, cpy;
	int king2cpDiffx, king2cpDiffy, dst2cpDiffx, dst2cpDiffy;
	integerToXYCoordinate(checkingPieceLocation, &cpx, &cpy);
	integerToXYCoordinate(kingInCheckLocation, &kingx, &kingy);
	king2cpDiffx = kingx - cpx;
	king2cpDiffy = kingy - cpy;

	for (int dsty = 0; dsty < BOARDGRIDSIZE; dsty++){
		for (int dstx = 0; dstx < BOARDGRIDSIZE; dstx++){
			if (!logicBoard[srcx][srcy].availableMoves[dstx][dsty]) continue;
			if (numberOfChecks > 1){
				// set available moves of other piece to false if
				// multiple pieces are checking king
				logicBoard[srcx][srcy].availableMoves[dstx][dsty] = false;
				continue;
			}
			if (dstx == cpx && dsty == cpy){
				// Available move can capture the checking piece
				numOfPossibleMoves++;
				continue;
			}
			dst2cpDiffx = dstx - cpx;
			dst2cpDiffy = dsty - cpy;
			if (kingx == srcx && kingy == srcy){
				if (king2cpDiffx == 0 && dst2cpDiffx == 0 &&
					(king2cpDiffy/abs(king2cpDiffy)) == (dst2cpDiffy/abs(dst2cpDiffy)) ){
					logicBoard[srcx][srcy].availableMoves[dstx][dsty] = false;
				} else if (king2cpDiffy == 0 && dst2cpDiffy == 0 &&
					(king2cpDiffx/abs(king2cpDiffx)) == (dst2cpDiffx/abs(dst2cpDiffx)) ){
					logicBoard[srcx][srcy].availableMoves[dstx][dsty] = false;
				} else if (king2cpDiffx != 0 && dst2cpDiffx != 0 && 
					king2cpDiffy != 0 && dst2cpDiffy != 0 && 
					abs(king2cpDiffx) == abs(king2cpDiffy) &&
					abs(dst2cpDiffx) == abs(dst2cpDiffy) &&
					(king2cpDiffx/abs(king2cpDiffx)) == (dst2cpDiffx/abs(dst2cpDiffx)) &&
					(king2cpDiffy/abs(king2cpDiffy)) == (dst2cpDiffy/abs(dst2cpDiffy)) ){
					logicBoard[srcx][srcy].availableMoves[dstx][dsty] = false;
				} else {
					numOfPossibleMoves++;
				}
				continue;
			}
			if (logicBoard[cpx][cpy].availableMoves[dstx][dsty]){
				// Available move can block the check
				if (king2cpDiffx == 0 && dst2cpDiffx == 0 &&
					(king2cpDiffy/abs(king2cpDiffy)) == (dst2cpDiffy/abs(dst2cpDiffy)) ){
					// blocking a vertical attack
					numOfPossibleMoves++;
					continue;
				} else if (king2cpDiffy == 0 && dst2cpDiffy == 0 &&
					(king2cpDiffx/abs(king2cpDiffx)) == (dst2cpDiffx/abs(dst2cpDiffx)) ){
					// blocking horizontal attack
					numOfPossibleMoves++;
					continue;
				} else if (king2cpDiffx != 0 && dst2cpDiffx != 0 && 
					king2cpDiffy != 0 && dst2cpDiffy != 0 &&
					(king2cpDiffx/abs(king2cpDiffx)) == (dst2cpDiffx/abs(dst2cpDiffx)) &&
					(king2cpDiffy/abs(king2cpDiffy)) == (dst2cpDiffy/abs(dst2cpDiffy)) ){
					// blocking diagonal attack
					numOfPossibleMoves++;
					continue;
				}
			}
			logicBoard[srcx][srcy].availableMoves[dstx][dsty] = false;
		}
	}
	return numOfPossibleMoves;
}

static void ChessLogic_calculateMoves(int srcx, int srcy){
	char currentPiece = logicBoard[srcx][srcy].pieceType;
	for (int dsty = 0; dsty < BOARDGRIDSIZE; dsty++){
		for (int dstx = 0; dstx < BOARDGRIDSIZE; dstx++){
			logicBoard[srcx][srcy].availableMoves[dstx][dsty] = false;
			if (ChessLogic_isSameColor(srcx, srcy, dstx, dsty)) continue;
			if (CheckLogic_isJumpingOverPiece(srcx, srcy, dstx, dsty)) continue;

			if (currentPiece == king){
				if (!ChessLogic_kingMoves(srcx, srcy, dstx, dsty)) continue;
				logicBoard[srcx][srcy].availableMoves[dstx][dsty] = true;
				queensideFlag = false;
				kingsideFlag = false;
			} else if (currentPiece == queen){
				if (!ChessLogic_queenMoves(srcx, srcy, dstx, dsty)) continue;
				if (ChessLogic_isCheck(srcx, srcy, dstx, dsty)) continue;
				logicBoard[srcx][srcy].availableMoves[dstx][dsty] = true;
			} else if (currentPiece == rook){
				if (!ChessLogic_rookMoves(srcx, srcy, dstx, dsty)) continue;
				if (ChessLogic_isCheck(srcx, srcy, dstx, dsty)) continue;
				logicBoard[srcx][srcy].availableMoves[dstx][dsty] = true;
			} else if (currentPiece == bishop){
				if (!ChessLogic_bishopMoves(srcx, srcy, dstx, dsty)) continue;
				if (ChessLogic_isCheck(srcx, srcy, dstx, dsty)) continue;
				logicBoard[srcx][srcy].availableMoves[dstx][dsty] = true;
			} else if (currentPiece == knight){
				if (!ChessLogic_knightMoves(srcx, srcy, dstx, dsty)) continue;
				if (ChessLogic_isCheck(srcx, srcy, dstx, dsty)) continue;
				logicBoard[srcx][srcy].availableMoves[dstx][dsty] = true;
			} else if (currentPiece == pawn){
				if (!ChessLogic_pawnMoves(srcx, srcy, dstx, dsty)) continue;
				if (ChessLogic_isCheck(srcx, srcy, dstx, dsty)) continue;
				logicBoard[srcx][srcy].availableMoves[dstx][dsty] = true;
				enPasseFlag = false;
			}
		}
	}
}

static void ChessLogic_recalculateKingMoves(int kingx, int kingy){
	for (int y = 0; y < BOARDGRIDSIZE; y++){
		for (int x = 0; x < BOARDGRIDSIZE; x++){
			if (logicBoard[x][y].pieceType == nopiece) continue;
			if (ChessLogic_isSameColor(kingx, kingy, x, y)) continue;

			if (logicBoard[kingx][kingy].firstMove){
				if (logicBoard[x][y].availableMoves[kingx+2][kingy]){
					logicBoard[kingx][kingy].availableMoves[kingx+2][kingy] = false;
				}
				if (logicBoard[x][y].availableMoves[kingx-2][kingy]){
					logicBoard[kingx][kingy].availableMoves[kingx-2][kingy] = false;
				}
			}

			if (logicBoard[x][y].pieceType == pawn &&
				logicBoard[x][y].pieceColor == white){
				if (x < (BOARDGRIDSIZE-1) && x >= 0 &&
					logicBoard[kingx][kingy].availableMoves[x+1][y+1]){
					logicBoard[kingx][kingy].availableMoves[x+1][y+1] = false;
				}
				if (x > 0 && x < BOARDGRIDSIZE &&
					logicBoard[kingx][kingy].availableMoves[x-1][y+1]){
					logicBoard[kingx][kingy].availableMoves[x-1][y+1] = false;
				}
			}

			if (logicBoard[x][y].pieceType == pawn &&
				logicBoard[x][y].pieceColor == black){
				if (x < (BOARDGRIDSIZE-1) && x >= 0 &&
					logicBoard[kingx][kingy].availableMoves[x+1][y-1]){
					logicBoard[kingx][kingy].availableMoves[x+1][y-1] = false;
				}
				if (x > 0 && x < BOARDGRIDSIZE &&
					logicBoard[kingx][kingy].availableMoves[x-1][y-1]){
					logicBoard[kingx][kingy].availableMoves[x-1][y-1] = false;
				}
			}

			if ((kingx+1) < BOARDGRIDSIZE){
				if (logicBoard[x][y].availableMoves[kingx+1][kingy]){
					logicBoard[kingx][kingy].availableMoves[kingx+1][kingy] = false;
					if (logicBoard[kingx][kingy].firstMove){
						logicBoard[kingx][kingy].availableMoves[kingx+2][kingy] = false;
					}
				}
				if ((kingy+1) < BOARDGRIDSIZE){
					if (logicBoard[x][y].availableMoves[kingx+1][kingy+1]){
						logicBoard[kingx][kingy].availableMoves[kingx+1][kingy+1] = false;
					}
				}
				if ((kingy-1) < BOARDGRIDSIZE){
					if (logicBoard[x][y].availableMoves[kingx+1][kingy-1]){
						logicBoard[kingx][kingy].availableMoves[kingx+1][kingy-1] = false;
					}
				}
			}
			if ((kingx-1) >= 0){
				if (logicBoard[x][y].availableMoves[kingx-1][kingy]){
					logicBoard[kingx][kingy].availableMoves[kingx-1][kingy] = false;
					if (logicBoard[kingx][kingy].firstMove){
						logicBoard[kingx][kingy].availableMoves[kingx-2][kingy] = false;
					}
				}
				if ((kingy+1) < BOARDGRIDSIZE){
					if (logicBoard[x][y].availableMoves[kingx-1][kingy+1]){
						logicBoard[kingx][kingy].availableMoves[kingx-1][kingy+1] = false;
					}
				}
				if ((kingy-1) < BOARDGRIDSIZE){
					if (logicBoard[x][y].availableMoves[kingx-1][kingy-1]){
						logicBoard[kingx][kingy].availableMoves[kingx-1][kingy-1] = false;
					}
				}
			}
			if ((kingy+1) < BOARDGRIDSIZE){
				if (logicBoard[x][y].availableMoves[kingx][kingy+1])
					logicBoard[kingx][kingy].availableMoves[kingx][kingy+1] = false;
			}
			if ((kingy-1) < BOARDGRIDSIZE){
				if (logicBoard[x][y].availableMoves[kingx][kingy-1]){
					logicBoard[kingx][kingy].availableMoves[kingx][kingy-1] = false;
				}
			}
		}
	}
}

static void ChessLogic_updateAllPieceMoves(void){
	int whiteKingx, whiteKingy, blackKingx, blackKingy;
	int kingInCheckx, kingInChecky;
	int numMovesToEscapeCheck = 0;
	checkingPieceLocation = -1;
	whiteCheckFlag = false;
	blackCheckFlag = false;
	whiteCanQueenSide = false;
	whiteCanKingSide = false;
	blackCanQueenSide = false;
	blackCanKingSide = false;
	numberOfChecks = 0;
	
	for (int srcy = 0; srcy < BOARDGRIDSIZE; srcy++){
		for (int srcx = 0; srcx < BOARDGRIDSIZE; srcx++){
			if (logicBoard[srcx][srcy].pieceType == nopiece) continue;
			ChessLogic_calculateMoves(srcx, srcy);
			if (logicBoard[srcx][srcy].pieceType == king && 
				logicBoard[srcx][srcy].pieceColor == white){
				whiteKingx = srcx;
				whiteKingy = srcy;
			} else if (logicBoard[srcx][srcy].pieceType == king && 
				logicBoard[srcx][srcy].pieceColor == black){
				blackKingx = srcx;
				blackKingy = srcy;
			}
		}
	}
	ChessLogic_recalculateKingMoves(whiteKingx, whiteKingy);
	ChessLogic_recalculateKingMoves(blackKingx, blackKingy);

	// Skip next section if there is no check
	if (!blackCheckFlag && !whiteCheckFlag){ 
		return;
	} else if (blackCheckFlag){
		kingInCheckLocation = xyCoordinateToInteger(blackKingx, blackKingy);
		kingInCheckx = blackKingx;
		kingInChecky = blackKingy;
	} else if (whiteCheckFlag){
		kingInCheckLocation = xyCoordinateToInteger(whiteKingx, whiteKingy);
		kingInCheckx = whiteKingx;
		kingInChecky = whiteKingy;
	}

	for (int srcy = 0; srcy < BOARDGRIDSIZE; srcy++){
		for (int srcx = 0; srcx < BOARDGRIDSIZE; srcx++){
			if (logicBoard[srcx][srcy].pieceColor == 
				logicBoard[kingInCheckx][kingInChecky].pieceColor){
				numMovesToEscapeCheck += ChessLogic_recalculateMovesInCheck(srcx, srcy);
			}
		}
	}
	// wprintf(L"Number of Moves to Escape Check = %d\n", numMovesToEscapeCheck);
	if (numMovesToEscapeCheck == 0 && blackCheckFlag){
		blackCheckMateFlag = true;
	} else if (numMovesToEscapeCheck == 0 && whiteCheckFlag){
		whiteCheckMateFlag = true;
	}
}

/******************************************************
 * Accessor Chess Logic Functions
 ******************************************************/
void ChessLogic_startNewGame(void){
	ChessLogic_initLogicBoard();
	ChessLogic_updateAllPieceMoves();
}

int ChessLogic_getCoordinatePieceType(int x, int y){
	if (x < 0 || x >= BOARDGRIDSIZE ||
		y < 0 || y >= BOARDGRIDSIZE) return -1;
	return logicBoard[x][y].pieceType;
}

int ChessLogic_getCoordinatePieceColor(int x, int y){
	if (x < 0 || x >= BOARDGRIDSIZE ||
		y < 0 || y >= BOARDGRIDSIZE) return -1;
	return logicBoard[x][y].pieceColor;
}

int ChessLogic_getChessSquarePieceType(char letter, char number){
	int x = getBoardLetterIncrement(letter);
	int y = getBoardNumberIncrement(number);
	if (x == -1 || y == -1) return -1;
	return logicBoard[x][y].pieceType;
}

int ChessLogic_getChessSquarePieceColor(char letter, char number){
	int x = getBoardLetterIncrement(letter);
	int y = getBoardNumberIncrement(number);
	if (x == -1 || y == -1) return -1;
	return logicBoard[x][y].pieceColor;
}

bool ChessLogic_getPieceAvailableMoves(char srcletter, char srcnumber, int dstx, int dsty){
	int srcx = getBoardLetterIncrement(srcletter);
	int srcy = getBoardNumberIncrement(srcnumber);
	if (srcx == -1 || srcy == -1) return false;
	if (logicBoard[srcx][srcy].pieceType == nopiece) return false;
	return logicBoard[srcx][srcy].availableMoves[dstx][dsty];
}

int ChessLogic_getCurrentColorTurn(void){
	return currentTurn;
}

int ChessLogic_getTurnCount(void){
	return turnNumber;
}

int ChessLogic_getCheckStatus(void){
	if (whiteCheckFlag) return white;
	if (blackCheckFlag) return black;
	return 0;
}

int ChessLogic_getCheckMateStatus(void){
	if (whiteCheckMateFlag) return white;
	if (blackCheckMateFlag) return black;
	return 0;
}

bool ChessLogic_getDrawStatus(void){
	return drawFlag;
}

/******************************************************
 * Functions for StockFish UCI
 ******************************************************/
int ChessLogic_getHalfMoveTimer(void){
	return halfMoveTimer;
}

int ChessLogic_getFullMoveTimer(void){
	return fullMoveTimer;
}

bool ChessLogic_canWhiteKingSide(void){
	return whiteCanKingSide;
}

bool ChessLogic_canWhiteQueenSide(void){
	return whiteCanQueenSide;
}

bool ChessLogic_canBlackKingSide(void){
	return blackCanKingSide;
}

bool ChessLogic_canBlackQueenSide(void){
	return blackCanQueenSide;
}

/******************************************************
 * Mutator Chess Logic Functions
 ******************************************************/
int ChessLogic_movePiece(char srcletter, char srcnumber, char dstletter, char dstnumber){
	int srcx, srcy, dstx, dsty;
	if (halfMoveTimer == DRAWTIMER) drawFlag = true;
	if (whiteCheckMateFlag || blackCheckMateFlag || drawFlag) return -1;
	
	srcx = getBoardLetterIncrement(srcletter);
	srcy = getBoardNumberIncrement(srcnumber);

	// Return -1 if input is incorrect
	if (srcx == -1 || srcy == -1) return -1;

	// Return -1 if user tries to move a square with no piece
	if (logicBoard[srcx][srcy].pieceType == nopiece) return -1;

	// Return -1 if user tries to move piece of wrong color
	if (logicBoard[srcx][srcy].pieceColor != currentTurn) return -1;

	dstx = getBoardLetterIncrement(dstletter);
	dsty = getBoardNumberIncrement(dstnumber);
	
	// Return -1 if input is incorrect
	if (dstx == -1 || dsty == -1) return -1;

	// Return -1 if user tries to move piece to same position
	if (srcx == dstx && srcy == dsty) return -1;

	// Return -1 if user tries to capture a king
	if (logicBoard[dstx][dsty].pieceType == king) return -1;

	if (logicBoard[srcx][srcy].pieceType == pawn) {
		if (!ChessLogic_pawnMoves(srcx, srcy, dstx, dsty)) return -1;
		ChessLogic_checkPromotePawn(srcx, srcy, dstx, dsty);
	} else if (!logicBoard[srcx][srcy].availableMoves[dstx][dsty]) return -1;
	
	if (logicBoard[srcx][srcy].pieceType == king) {
		if (!ChessLogic_kingMoves(srcx, srcy, dstx, dsty)) return -1;
	}

	pthread_mutex_lock(&chessMutex);
	ChessLogic_processMove(srcx, srcy, dstx, dsty);
	ChessLogic_incrementIdlePawns();
	ChessLogic_updateAllPieceMoves();
	ChessLogic_nextTurn();
	pthread_mutex_unlock(&chessMutex);
	return 0;
}