#include "global.h"
#include "chesslogic.h"
#include "networkAPI.h"
#include <stdio.h>

#define BUFFERSIZE 128

int main(){
	char buffer[BUFFERSIZE];
	ChessLogic_startNewGame();
	NetworkAPI_getBoardString(buffer, BUFFERSIZE);
	printf("%s\n", buffer);
}