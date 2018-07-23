#ifndef _UNICODEBOARD_H_
#define _UNICODEBOARD_H_

/******************************************************
 * Chess Symbol UniCode
 ******************************************************/
static unsigned short whiteChessPieces[] = {
	0x005F,
	0x265F, //pawn
	0x265C, //rook
	0x265E, //knight
	0x265D, //bishop
	0x265B, //queen
	0x265A  //king
};

static unsigned short blackChessPieces[] = {
	0x005F,
	0x2659, //pawn
	0x2656, //rook
	0x2658, //knight
	0x2657, //bishop
	0x2655, //queen
	0x2654  //king
};

static unsigned short lowerCaseLetters[] = {
	0x0061, //a
	0x0062, //b
	0x0063, //c
	0x0064, //d
	0x0065, //e
	0x0066, //f
	0x0067, //g
	0x0068  //h
};

void UnicodeBoard_printBoard(void);

#endif