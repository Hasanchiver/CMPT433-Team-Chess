#include "global.h"
#include "unicodeboard.h" // 
#include <stdbool.h>
#include "chesslogic.h"
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "lcd.h"
#include <string.h>
#include <stdlib.h>


void matrix_update( uint8_t table[8][8] , uint8_t table2[8][8] )  ;


int is_select = 0;
uint8_t prev_row = 0, prev_col = 0;



/*void UnicodeBoard_printBoard(){
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

void UnicodeBoard_printAvailableMoves(int srcx, int srcy){
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
			if (ChessLogic_getPieceAvailableMoves(srcx, srcy, x, y) == true){
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
}*/

int main(){


	//char srclocation[8];
	//char dstlocation[8];


	/*char whitestr[20] = "White\0";
	char blackstr[20] = "Black\0";*/
	int output = 0;
	//setlocale(LC_CTYPE, "");
	ChessLogic_startNewGame();

	//wprintf(L"Initial Board\n");
    //UnicodeBoard_printBoard();








    //Joe's Part
    dd_main(0, NULL);

    loadNewGameScene();

    int ret;
    int row, col;
    uint8_t possibleMoves[BOARDGRIDSIZE][BOARDGRIDSIZE], transpose[8][8];

    squareInfo *cell_info = malloc(sizeof(squareInfo));

    memset(cell_info, 0, sizeof(squareInfo));


    while(1){

    	ret = gameboard_touchPanelListen(&col, &row);
    	
    	
    	if( ret == 1){


    		if(is_select == 1){


    			if (  ChessLogic_movePiece(prev_row, prev_col, row, col) == -1   ){
    				//printf("%d %d\n", prev_row, prev_col);
    				//printf("%d %d\n", row, col);
    				
    				deselectChessPiece();
    				is_select = 0;

    			}else{
    				//legal move
    				if(0 /*is Special*/){
    					//handle 

    				}else{


    					printf("I am here\n");

    					//normal move
    					
    					
    					ChessLogic_getPieceInfo(cell_info, row, col);
    					
    					int type = 0;

    					printf("cell piece %d\n", cell_info->pieceType);

    					switch( cell_info->pieceType ){
    						case pawn:	
    							type = 0;
    							break;
    						case rook:	
    							type = 3;
    							break;

    						case knight:
    							type = 1;
    							break;

    						case bishop:
    							type = 2;
    							break;

    						case queen:
    							type = 5;
    							break;

    						case king:
    							type = 4;
    							break;
    					}

    					if( cell_info->pieceColor == 2){
    						type += 10;
    					}

    					//printf("%d %d %d %d\n", prev_row, prev_col , row , col);

    					printf("%d\n", type);
    					moveChessPiece( prev_col, prev_row, col, row,  type );
    					
    				}
    				
    			}

    			is_select = 0;

    		}else{

    			

    			ChessLogic_getPossibleMoves(possibleMoves, row, col);

    			//printf("%d %d\n", row, col);


	    		matrix_update( possibleMoves, transpose);

	    		selectChessPiece(  transpose );

	    		prev_row = row;
	    		prev_col = col;
	    		is_select = 1;


    		}



    		
    		




    	}else if(ret == 2){
    		//user click a button



    		
    	}else{
    		//User click nothing

    		if(is_select == 1){
    			deselectChessPiece();

    		}

    	}





    	delay(500);		//debouncing time




    }



































    
/*
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
	}*/

}






void matrix_update( uint8_t table[8][8], uint8_t table2[8][8]){
    
    for(int i=0; i < 8; ++i)
        for(int j=0; j < 8; ++j)
        {
            table2[j][i] = table[i][j];
        }
	
}


