
#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "stdint.h"
#define BOARDGRIDSIZE 8 


typedef enum {false, true} bool;


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
	bool castling;
	int idleInSquare; // number of turns pawn has stayed in square
	bool availableMoves[BOARDGRIDSIZE][BOARDGRIDSIZE];


} squareInfo;



void dd_main(int argc, char *argv[]);


#endif
