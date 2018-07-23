

#ifndef RULES_H
#define RULES_H

#include "Game.h"


int MovePiece(ChessPiece *piece, int newX, int newY, Game *game);

int IsValidMove(ChessPiece *piece, int newX, int newY, ChessBoard *board);
int IsValidPawnMove(ChessPiece *piece, int newX, int newY, ChessBoard *board);
int IsValidRookMove(ChessPiece *piece, int newX, int newY, ChessBoard *board);
int IsValidKnightMove(ChessPiece *piece, int newX, int newY, ChessBoard *board);
int IsValidBishopMove(ChessPiece *piece, int newX, int newY, ChessBoard *board);
int IsValidQueenMove(ChessPiece *piece, int newX, int newY, ChessBoard *board);
int IsValidKingMove(ChessPiece *piece, int newX, int newY, ChessBoard *board);

ChessPiece *Promotion(ChessPiece *piece, Game *game, Type t);

int IsValidMoveAndKingNotInCheck(ChessPiece *piece, int newX, int newY, ChessBoard *board);
int WillLetKingInCheck(ChessPiece *piece, int newX, int newY, ChessBoard *board);
int IsKingInCheck(ChessBoard *board, ChessColor color);
int Checkmate(ChessPiece *king, ChessBoard *board);

void KillPiece(ChessBoard *board, int x, int y, ChessColor color);

typedef enum movetype{
    None, Normal, Attack,
    EnPassant,
    Castling,
    PromotionMove
}MoveType;

#endif // RULES_H
