#include "global.h"
#include "unicodeboard.h" // 
#include <stdbool.h>
#include "chesslogic.h"
#include <stdio.h>
#include <wchar.h>
#include <locale.h>


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
			if (ChessLogic_getCoordinatePieceType(x,y) == nopiece){
				temp = blackChessPieces[nopiece];
				wprintf(L"%lc ", temp);
				continue;
			}
			if (ChessLogic_getCoordinatePieceColor(x,y) == black){
				temp = blackChessPieces[ChessLogic_getCoordinatePieceType(x,y)];
				wprintf(L"%lc ", temp);
			} else if (ChessLogic_getCoordinatePieceColor(x,y) == white){
				temp = whiteChessPieces[ChessLogic_getCoordinatePieceType(x,y)];
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
			if (ChessLogic_getPieceAvailableMoves(srcletter, srcnumber, x, y) == true){
				wprintf(L"X ");
			} else if (ChessLogic_getCoordinatePieceColor(x,y) == black){
				temp = blackChessPieces[ChessLogic_getCoordinatePieceType(x,y)];
				wprintf(L"%lc ", temp);
			} else if (ChessLogic_getCoordinatePieceColor(x,y) == white){
				temp = whiteChessPieces[ChessLogic_getCoordinatePieceType(x,y)];
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
	char whitestr[20] = "White\0";
	char blackstr[20] = "Black\0";
	int output = 0;
	setlocale(LC_CTYPE, "");
	ChessLogic_startNewGame();
	wprintf(L"Initial Board\n");
    UnicodeBoard_printBoard();

    while (!ChessLogic_getCheckMateStatus() && !ChessLogic_getDrawStatus()){
    	if (ChessLogic_getCurrentTurn() == white){
    		wprintf(L"Current Turn: %s\n", whitestr);
    	} else {
    		wprintf(L"Current Turn: %s\n", blackstr);
    	}
    	if (ChessLogic_getCheckStatus() == white){
    		wprintf(L"White King Check\n");
    	} else if (ChessLogic_getCheckStatus() == black){
    		wprintf(L"Black King Check\n");
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

	    output = ChessLogic_movePiece(srclocation[0], srclocation[1], dstlocation[0], dstlocation[1]);
	    wprintf(L"%c%c to %c%c: %d\n", srclocation[0], srclocation[1], dstlocation[0], dstlocation[1], output);
	    UnicodeBoard_printBoard();
	}
	if (ChessLogic_getDrawStatus()){
		wprintf(L"Draw!\n");
	} else if (ChessLogic_getCheckMateStatus() == white){
		wprintf(L"White King Check Mate--Black Wins!\n");
	} else if (ChessLogic_getCheckMateStatus() == black){
		wprintf(L"Black King Check Mate--White Wins!\n");
	}
}