

#ifndef CHESSPIECE_H
#define CHESSPIECE_H

#include "PossibleMovesList.h"

typedef enum chessColor{
    White, Black
} ChessColor;

typedef enum type{
    Pawn,
    Rook, Knight, Bishop,
    Queen, King
}Type;

typedef struct chessPiece ChessPiece;

ChessPiece *CreateChessPiece(int x, int y, ChessColor color, Type type);

Type GetTypeByChar(char t);
char GetCharByType(Type t);

void DeletePiece(ChessPiece **piece);

int ChessPiece_GetX(ChessPiece *piece);
int ChessPiece_GetY(ChessPiece *piece);
void ChessPiece_SetPosition(ChessPiece *piece, int x, int y);

int ChessPiece_GetType(ChessPiece *piece);
void ChessPiece_SetType(ChessPiece *piece, Type t);

ChessColor ChessPiece_GetColor(ChessPiece *piece);

int ChessPiece_GetNumberOfMoves(ChessPiece *piece);
void ChessPiece_AddNumberOfMoves(ChessPiece *piece, int inc);
void ChessPiece_SetNumberOfMoves(ChessPiece *piece, int n);

PossibleMovesList *ChessPiece_GetPossibleMovesList(ChessPiece *piece);

#endif // CHESSPIECE_H
