

#ifndef PLAYER_H
#define PLAYER_H

#include "ChessPiece.h"
#include "List.h"

#define PLAYER_MAX_PIECES 16

typedef struct player Player;

Player *CreatePlayer();

void InitializeList(Player *player);

void DeletePlayer(Player **player);

char* Player_GetName(Player *player);
void Player_SetName(Player *player, const char* name);

List *Player_GetList(Player *player);

ChessPiece *Player_GetLastMovedPiece(Player *player);
void Player_SetLastMovedPiece(Player *player, ChessPiece *p);

ChessColor Player_GetColor(Player *player);

#endif // PLAYER_H
