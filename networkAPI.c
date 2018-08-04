#include "global.h"
#include "chesslogic.h"
#include <stdio.h> //for sprintf

const char upperCase[] = "XPRNBQK";
const char lowerCase[] = "xprnbqk";

// example: rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR
void NetworkAPI_getBoardString(char *buffer, int buffersize){ // must give at least size 100
	char temp;
	int index = 0;
	int emptyInc = 0;
	for (int y = BOARDGRIDSIZE-1; y >= 0; y--){
		for (int x = 0; x < BOARDGRIDSIZE; x++){
			if (index > (buffersize-2)){
				// error, buffer is not big enough
				break;
			}
			if (ChessLogic_getChessSquarePieceType(x,y) != nopiece &&
				emptyInc > 0){
				index += sprintf(&buffer[index], "%d", emptyInc);
				emptyInc = 0;
			}
			if (ChessLogic_getChessSquarePieceColor(x,y) == black){
				temp = lowerCase[ChessLogic_getChessSquarePieceType(x,y)];
				index += sprintf(&buffer[index], "%c", temp);

			} else if (ChessLogic_getChessSquarePieceColor(x,y) == white){
				temp = upperCase[ChessLogic_getChessSquarePieceType(x,y)];
				index += sprintf(&buffer[index], "%c", temp);
			} else if (ChessLogic_getChessSquarePieceType(x,y) == nopiece){
				emptyInc++;
			}
		}
		if (emptyInc > 0){
			index += sprintf(&buffer[index], "%d", emptyInc);
			emptyInc = 0;
		}
		if (y != 0){
			index += sprintf(&buffer[index], "/");
		}
	}
}
