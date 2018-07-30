#include "global.h"
#include "unicodeboard.h" // 
#include "chesslogic.h"
#include "networkAPI.h"
#include <stdbool.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>

#define BUFFERSIZE 100

static squareInfo boardArray[BOARDGRIDSIZE][BOARDGRIDSIZE];

void UnicodeBoard_printBoard(){
	wchar_t temp;
	int num;

	wprintf(L"  ");
	for (int x = 0; x < BOARDGRIDSIZE; x++){
		temp = lowerCaseLetters[x];
		wprintf(L"%lc ", temp);
	}
	wprintf(L"\n");
	for (int y = BOARDGRIDSIZE-1; y >= 0; y--){
		num = y+1;
		wprintf(L"%d ", num);
		for (int x = 0; x < BOARDGRIDSIZE; x++){
			if (ChessLogic_getChessSquarePieceType(x,y) == nopiece){
				temp = blackChessPieces[nopiece];
				wprintf(L"%lc ", temp);
				continue;
			}
			if (ChessLogic_getChessSquarePieceColor(x,y) == black){
				temp = blackChessPieces[ChessLogic_getChessSquarePieceType(x,y)];
				wprintf(L"%lc ", temp);
			} else if (ChessLogic_getChessSquarePieceColor(x,y) == white){
				temp = whiteChessPieces[ChessLogic_getChessSquarePieceType(x,y)];
				wprintf(L"%lc ", temp);
			}
		}
		wprintf(L"\n");
	}
	wprintf(L"\n");
}

void Unicode_getBoardAndPrint(){
	ChessLogic_getBoardStateGrid(boardArray);
	wchar_t temp;
	int num;

	wprintf(L"This the new function\n");
	wprintf(L"  ");
	for (int x = 0; x < BOARDGRIDSIZE; x++){
		temp = lowerCaseLetters[x];
		wprintf(L"%lc ", temp);
	}
	wprintf(L"\n");
	for (int y = BOARDGRIDSIZE-1; y >= 0; y--){
		num = y+1;
		wprintf(L"%d ", num);
		for (int x = 0; x < BOARDGRIDSIZE; x++){
			if (boardArray[x][y].pieceType == nopiece){
				temp = blackChessPieces[nopiece];
				wprintf(L"%lc ", temp);
				continue;
			}
			if (boardArray[x][y].pieceColor == black){
				temp = blackChessPieces[(int)boardArray[x][y].pieceType];
				wprintf(L"%lc ", temp);
			} else if (boardArray[x][y].pieceColor == white){
				temp = whiteChessPieces[(int)boardArray[x][y].pieceType];
				wprintf(L"%lc ", temp);
			}
		}
		wprintf(L"\n");
	}
	wprintf(L"\n");
}

void UnicodeBoard_printAvailableMoves(char srcletter, char srcnumber){
	wchar_t temp;
	int num;

	wprintf(L"  ");
	for (int x = 0; x < BOARDGRIDSIZE; x++){
		temp = lowerCaseLetters[x];
		wprintf(L"%lc ", temp);
	}
	wprintf(L"\n");
	for (int y = BOARDGRIDSIZE-1; y >= 0; y--){
		num = y+1;
		wprintf(L"%d ", num);
		for (int x = 0; x < BOARDGRIDSIZE; x++){
			if (ChessLogic_getPieceAvailableMovesChar(srcletter, srcnumber, x, y) == true){
				wprintf(L"X ");
			} else if (ChessLogic_getChessSquarePieceColor(x,y) == black){
				temp = blackChessPieces[ChessLogic_getChessSquarePieceType(x,y)];
				wprintf(L"%lc ", temp);
			} else if (ChessLogic_getChessSquarePieceColor(x,y) == white){
				temp = whiteChessPieces[ChessLogic_getChessSquarePieceType(x,y)];
				wprintf(L"%lc ", temp);
			} else {
				wprintf(L"_ ");
			}
		}
		wprintf(L"\n");
	}
}

int main(){
	char srclocation[8];
	char dstlocation[8];
	char buffer[BUFFERSIZE];
	char whitestr[20] = "White\0";
	char blackstr[20] = "Black\0";
	int output = 0;
	setlocale(LC_CTYPE, "");
	ChessLogic_startNewGame();
	wprintf(L"Initial Board\n");
    //UnicodeBoard_printBoard();
    Unicode_getBoardAndPrint();
    NetworkAPI_getBoardString(buffer, BUFFERSIZE);
	wprintf(L"FEN String: %s w KQkq - 0 1\n", buffer);
    piecePosUpdate *temp = malloc(sizeof(piecePosUpdate));

    while (!ChessLogic_getCheckMateStatus() && !ChessLogic_getDrawStatus()){
    	if (ChessLogic_getCurrentColorTurn() == white){
    		wprintf(L"Current Turn: %s\n", whitestr);
    	} else {
    		wprintf(L"Current Turn: %s\n", blackstr);
    	}
    	if (ChessLogic_getCheckStatus() == white){
    		wprintf(L"White King Check\n");
    	} else if (ChessLogic_getCheckStatus() == black){
    		wprintf(L"Black King Check\n");
    	}
    	if (ChessLogic_castlingTriggered(temp)){
    		wprintf(L"Castling!\n");
    		wprintf(L"Rook = %d, Color = %d, (%d, %d)->(%d, %d)\n", 
    			temp->type, temp->color, temp->srcx, temp->srcy, temp->dstx, temp->dsty);
    	}
    	if (ChessLogic_enPassantTriggered(temp)){
    		wprintf(L"En Passant!\n");
    		wprintf(L"Pawn = %d, Removed at (%d, %d)\n", 
    			temp->type, temp->srcx, temp->srcy);
    	}

	    wprintf(L"Enter a piece (i.e. a2): ");
	    fgets(srclocation, 8, stdin);
	    wprintf(L"\n");
	    wprintf(L"Possible Moves: \n");
	    UnicodeBoard_printAvailableMoves(srclocation[0], srclocation[1]);
	    wprintf(L"\n");

	    wprintf(L"Enter a destination (i.e. a3): ");
	    fgets(dstlocation, 8, stdin);
	    wprintf(L"\n");

	    output = ChessLogic_movePieceChar(srclocation[0], srclocation[1], dstlocation[0], dstlocation[1]);
	    wprintf(L"Output: %d\n", output);
	    NetworkAPI_getMostRecentMove(buffer, BUFFERSIZE);
	    wprintf(L"Most Recent Legal Move: %s ", buffer);
	    Unicode_getBoardAndPrint();
		NetworkAPI_getBoardString(buffer, BUFFERSIZE);
		wprintf(L"FEN String: %s", buffer);
		if (ChessLogic_getCurrentColorTurn() == white) wprintf(L"w KQkq xx");
	    else wprintf(L"w KQkq xx");
	    wprintf(L" %d ", ChessLogic_getHalfMoveTimer());
	    wprintf(L"%d\n", ChessLogic_getFullMoveTimer());

	}
	if (ChessLogic_getDrawStatus()){
		wprintf(L"Draw!\n");
	} else if (ChessLogic_getCheckMateStatus() == white){
		wprintf(L"White King Check Mate--Black Wins!\n");
	} else if (ChessLogic_getCheckMateStatus() == black){
		wprintf(L"Black King Check Mate--White Wins!\n");
	}
}