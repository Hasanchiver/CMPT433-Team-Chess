

#ifndef GAME_H
#define GAME_H

#include "ChessBoard.h"
#include "ChessPiece.h"
#include <stdbool.h>

typedef enum state{
    Playing,
    PromotionMenu,
    PauseMenu,
    PauseOptionsMenu,
    EndGameMenu,
    MainMenu
}State;

typedef struct game Game;

void Game_init(void);
void Game_cleanup();


Game* CreateGame();
void RestartGame(Game *game);

void StartGame(Game *game);
void Play(Game *game);
void UpdateGame(Game *game);

void StopGame(Game *game);
void DeleteGame(Game **game);

void ClearSTDIN(char *s);

int Game_IsCheck(Game *game);
void Game_SetIsCheck(Game *game, int a);

int Game_IsCheckmate(Game *game);
void Game_SetIsCheckmate(Game *game, int a);

int Game_IsInGame(Game *game);
void Game_SetIsInGame(Game *game, int a);

ChessColor Game_GetTurn(Game *game);
void Game_SetTurn(Game *game, ChessColor t);
void ChangePlayer(Game *game);

State Game_GetState(Game *game);
void Game_SetState(Game *game, State s);

ChessPiece *Game_GetSelectedPiece(Game *game);
void Game_SetSelectedPiece(Game *game, ChessPiece *piece);

ChessBoard* Game_GetBoard(Game *game);
void Game_SetBoard(Game *game, ChessBoard *board);

#endif // GAME_H
