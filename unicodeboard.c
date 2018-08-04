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
#include <stdint.h>

#include<arpa/inet.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>


#include "udpListener.h"



#define SERVER "192.168.7.1"
#define BUFLEN 150
#define PORT 16888




void parseInfo( char str[], int *o_row, int *o_col, int *n_row, int *n_col );
void matrix_update( uint8_t table[8][8] , uint8_t table2[8][8] )  ;


int is_select = 0;
uint8_t prev_row = 0, prev_col = 0;


int main(){

	UdpListener_startListening();


	ChessLogic_startNewGame();



    //Joe's Part
    dd_main(0, NULL);


    //loadRestartScene();


    
    //loadNewGameScene();

  



    squareInfo ot[8][8];
    squareInfo nt[8][8];


 	uint8_t ott[8][8];
    uint8_t ntt[8][8];
	uint8_t ott2[8][8];
    uint8_t ntt2[8][8];




    int ret;
    int row, col;

    uint8_t possibleMoves[BOARDGRIDSIZE][BOARDGRIDSIZE], transpose[8][8];
    squareInfo *cell_info = malloc(sizeof(squareInfo));
    memset(cell_info, 0, sizeof(squareInfo));

    piecePosUpdate *castle = malloc(sizeof(squareInfo));
	memset(castle, 0, sizeof(piecePosUpdate));




	//Init UDP 

	struct sockaddr_in si_other;
    int s;
    uint32_t slen = sizeof(si_other);


    char buf[BUFLEN];
    char message[BUFLEN];
 
    if ( (s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        printf("socket creation error\n");
        exit(-1);
    }
 
    memset((char *) &si_other, 0, sizeof(si_other));

    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
     
    if (inet_pton(AF_INET, SERVER , &si_other.sin_addr) == 0) 
    {
        printf("inet_aton() failed\n");
        exit(-1);
    }



    //Game start

    int win = 1;


    //0 -> white move next, 1 -> black move next
    int turn = 0;

    int ai_move_src_row = 0, ai_move_src_col = 0;
    int ai_move_dst_row = 0, ai_move_dst_col = 0;



    while(1){


		//BUG - Game Logic
		restart: ChessLogic_startNewGame();



	    loadNewGameScene();




	    while(1){


	    	if( turn == 1 ){



				if( (win = ChessLogic_getCheckMateStatus()) == 0 ){

	        		}else{

	        			loadWinScene(win);

	        			win_touchPanelListen();		//Any click will return
	        			printf("Next Round");
	        			turn = 0;

	        			goto restart;
	        			
	        			//--------------------------------------------------------------------

	        		}








	    		//sprintf();

	    		NetworkAPI_getBoardString( message, 150 );

	    		strcat(message, " b - - 0 0 moves\n");

	    		if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen) == -1 )
	        	{
	            	printf("udp send error, restart everything\n");
	            	exit(-1);
	        	}

	        	memset(buf, 0, BUFLEN);

	        	if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
	        	{
	            	printf("udp recv error, restart everything\n");
	            	exit(-1);
	        	}

	        	printf("%s\n", buf);

	        	parseInfo( buf, &ai_move_src_row, &ai_move_src_col, &ai_move_dst_row, &ai_move_dst_col );

	        	
	        	printf("result : %d %d %d %d\n", ai_move_src_row, ai_move_src_col, ai_move_dst_row, ai_move_dst_col);


	        	//while(1);


	        	//Move


	        	if (  ChessLogic_movePiece(ai_move_src_row, ai_move_src_col, ai_move_dst_row, ai_move_dst_col) == -1   ){


	        		//printf("%d\n", ChessLogic_getCheckMateStatus());


	        		if( (win = ChessLogic_getCheckMateStatus()) == 0 ){

	        			printf("program should not go here if game part is correct\n");
	        			exit(-1);

	        		}else{


	        			loadWinScene(win);

	        			win_touchPanelListen();		//Any click will return

	        			printf("Next Round");

	        			turn = 0;

	        			goto restart;
	        			
	        			//--------------------------------------------------------------------

	        		}



	        		
	        	}




	        	ChessLogic_getBoardStateGrid(nt);

				table_format_converter( ot, ott );
				table_format_converter( nt, ntt );

		    	table_converter(ott, ott2, ntt, ntt2);

				update_gameboard(  ott2, ntt2  );
	        	
	    		turn = 0;

	    		//while(1);

	    	}else{

	    		while(1){


	    			ret = gameboard_touchPanelListen(&col, &row);
		    	
			    	if( ret == 1){

		    		// Click on the board

		    			if(is_select == 1){


			    			if (  ChessLogic_movePiece(prev_row, prev_col, row, col) == -1   ){


			    				if( (win = ChessLogic_getCheckMateStatus()) != 0 ){

				        			
			    					loadWinScene(win);

				        			win_touchPanelListen();		//Any click will return

				        			printf("Next Round");
				        			turn = 0;

				        			goto restart;

			        				//--------------------------------------------------------------------

			        			}



			    				deselectChessPiece();
			    				is_select = 0;

			    			}else{

			    				//Normal move
			    				ChessLogic_getBoardStateGrid(nt);

								table_format_converter( ot, ott );
								table_format_converter( nt, ntt );

			    				table_converter(ott, ott2, ntt, ntt2);

								update_gameboard(  ott2, ntt2  );

								turn = 1;
								break;
			    				
			    			}

		    				is_select = 0;

		    			}else{


			    			
			    			ChessLogic_getPossibleMoves(possibleMoves, row, col);
				    		matrix_update( possibleMoves, transpose);
				    		selectChessPiece(  transpose );

				    		ChessLogic_getBoardStateGrid(ot);

				    		prev_row = row;
				    		prev_col = col;
				    		is_select = 1;

		    			}

			    	} else if(ret == 2){

			    		//user click a button

			    		turn = 0;
			    		is_select = 0;

			    		goto restart;

			    		break;


			    	}else{

			    		//User click nothing

			    		if(is_select == 1){
			    			deselectChessPiece();
			    		}

			    	}

			    	delay(500);

	    		}
				


	    	}





	    }

	}



    UdpListener_cleanup();

}






void matrix_update( uint8_t table[8][8], uint8_t table2[8][8]){
    
    for(int i=0; i < 8; ++i)
        for(int j=0; j < 8; ++j)
        {
            table2[j][i] = table[i][j];
        }
	
}

void table_converter( uint8_t o_table1[8][8], uint8_t o_table2[8][8], uint8_t n_table1[8][8], uint8_t n_table2[8][8] ){

	matrix_update(o_table1, o_table2);
	matrix_update(n_table1, n_table2);

}






void table_format_converter( squareInfo st[8][8] , uint8_t dt[8][8]){

	uint8_t type = 0;

	for(int i = 0; i < 8; ++i){

		for(int j = 0; j < 8; ++j){

			switch( st[i][j].pieceType ){

				case nopiece: 
					type = 100;
					break;
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
				default :
					printf("%d %d %d\n", i, j, st[i][j].pieceType);

					printf("table convert error, program should not come here\n");
					exit(-1);
			}

			if( st[i][j].pieceColor == black ){
				type += 10;
			}

			dt[i][j] = type;

		}

	}

}




void parseInfo( char str[], int *o_row, int *o_col, int *n_row, int *n_col ){

	char* im_info = malloc(5);
	
	strncpy(im_info, str + 9, 4);
	im_info[4] = 0;



	(*o_row) = (uint8_t)(im_info[0] - 'a');
	(*o_col) = atoi(&im_info[1]) - 1;

	(*n_row) = (uint8_t)(im_info[2] - 'a');
	(*n_col) = atoi(&im_info[3]) - 1;

}









