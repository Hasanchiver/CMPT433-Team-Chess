#ifndef _GLOBAL_H
#define _GLOBAL_H
#include <stdbool.h>
#include "stdint.h"
#define BOARDGRIDSIZE 8 


typedef struct {
     unsigned char red,green,blue;
} PPMPixel;

struct PPMImage{

	int x;
	int y;
	PPMPixel *data;
};

typedef enum{
	nopiece,
	pawn,
	rook, 
	knight,
	bishop,
	queen,
	king
} ChessPiece;

typedef enum{
	nocolor,
	white,
	black
} Color;

typedef struct{
	char pieceType;
	char pieceColor;
	bool firstMove;
	bool doubleStep;
	bool castling;
	int idleInSquare; // number of turns pawn has stayed in square
	bool availableMoves[BOARDGRIDSIZE][BOARDGRIDSIZE];
} squareInfo;

// struct for LCD
typedef struct{
	int srcx;
	int srcy;
	int dstx;
	int dsty;
	int type;
	int color;
} piecePosUpdate;

void dd_main(int argc, char *argv[]);

void table_converter( uint8_t o_table1[8][8], uint8_t o_table2[8][8], uint8_t n_table1[8][8], uint8_t n_table2[8][8] );
void table_format_converter( squareInfo st[8][8] , uint8_t dt[8][8]);


void NetworkAPI_getBoardString(char *buffer, int buffersize);

#endif
