
#include "Player.h"
#include "List.h"

#include <stdlib.h>
#include <string.h>

#define PAWNS_N 8

struct player{
    char name[15];

    List *pieceList;
    ChessPiece *lastMovedPiece;

    ChessColor color;
};

Player *CreatePlayer(ChessColor color, char name[]){
    Player *player = malloc(sizeof(Player));
    if (player == NULL) return NULL;

    player->pieceList = CreateList();

    if (player->pieceList == NULL){
        DeletePlayer(&player);
        return NULL;
    }

    player->color = color;
    strcpy(player->name, name);

    player->lastMovedPiece = NULL;

    return player;
}

void InitializeList(Player *player){
    //Pawns
    int i;
    for (i = 0; i<PAWNS_N; i++){
        if (player->color == White){
            List_AddPiece(player->pieceList, CreateChessPiece(i, 6, player->color, Pawn));
        } else List_AddPiece(player->pieceList, CreateChessPiece(i, 1, player->color, Pawn));
    }

    //Rooks
    if (player->color == White){
        List_AddPiece(player->pieceList, CreateChessPiece(0, 7, player->color, Rook));
        List_AddPiece(player->pieceList, CreateChessPiece(7, 7, player->color, Rook));
    } else{
        List_AddPiece(player->pieceList, CreateChessPiece(0, 0, player->color, Rook));
        List_AddPiece(player->pieceList, CreateChessPiece(7, 0, player->color, Rook));
    }

    //Knights
    if (player->color == White){
        List_AddPiece(player->pieceList, CreateChessPiece(1, 7, player->color, Knight));
        List_AddPiece(player->pieceList, CreateChessPiece(6, 7, player->color, Knight));
    } else{
        List_AddPiece(player->pieceList, CreateChessPiece(1, 0, player->color, Knight));
        List_AddPiece(player->pieceList, CreateChessPiece(6, 0, player->color, Knight));
    }

    //Bishops
    if (player->color == White){
        List_AddPiece(player->pieceList, CreateChessPiece(2, 7, player->color, Bishop));
        List_AddPiece(player->pieceList, CreateChessPiece(5, 7, player->color, Bishop));
    } else{
        List_AddPiece(player->pieceList, CreateChessPiece(2, 0, player->color, Bishop));
        List_AddPiece(player->pieceList, CreateChessPiece(5, 0, player->color, Bishop));
    }

    //Queen
    if (player->color == White){
        List_AddPiece(player->pieceList, CreateChessPiece(3, 7, player->color, Queen));
    } else{
        List_AddPiece(player->pieceList, CreateChessPiece(3, 0, player->color, Queen));
    }

    //King
    if (player->color == White){
        List_AddPiece(player->pieceList, CreateChessPiece(4, 7, player->color, King));
    } else{
        List_AddPiece(player->pieceList, CreateChessPiece(4, 0, player->color, King));
    }
}

void DeletePlayer(Player **player){
    if (*player != NULL){
        DeleteList(&((*player)->pieceList));
        free(*player);
        *player = NULL;
    }
}

//Gets And Sets

char* Player_GetName(Player *player){
    if (player == NULL) return NULL;

    return player->name;
}
void Player_SetName(Player *player, const char* name){
    if (player == NULL) return;

    strcpy(player->name, name);
}

List *Player_GetList(Player *player){
    if (player == NULL) return NULL;

    return player->pieceList;
}

ChessPiece *Player_GetLastMovedPiece(Player *player){
    if (player == NULL) return NULL;

    return player->lastMovedPiece;
}
void Player_SetLastMovedPiece(Player *player, ChessPiece *p){
    if (player == NULL) return;

    player->lastMovedPiece = p;
}

ChessColor Player_GetColor(Player *player){
    if (player == NULL) return -1;

    return player->color;
}
