

#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "Player.h"

typedef struct chessBoard ChessBoard;

ChessBoard *CreateChessBoard(Player *player1, Player *player2);
void InitializeBoard(ChessBoard *board);
void ResetBoard(ChessBoard *board);
void UpdateBoard(ChessBoard *board);

void UpdatePossibleMovesLists(ChessBoard *board);
void UpdatePossibleMovesList(ChessBoard *board, ChessPiece *piece);

void DeleteChessBoard(ChessBoard **board);

ChessPiece *ChessBoard_GetChessPieceAtPosition(ChessBoard *board, int x, int y);
void ChessBoard_SetChessPieceAtPosition(ChessBoard *board, int x, int y, ChessPiece *piece);

Player *GetPlayer(ChessBoard *board, ChessColor color);
void ChessBoard_SetPlayer(ChessBoard *board, Player *player, ChessColor color);

#endif // CHESSBOARD_H
