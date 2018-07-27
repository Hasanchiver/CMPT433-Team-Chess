#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#define BOARDGRIDSIZE 8 

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

#endif