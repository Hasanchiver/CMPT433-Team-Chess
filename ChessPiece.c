

#include "ChessPiece.h"
#include "PossibleMovesList.h"

#include <stdlib.h>

struct chessPiece{
    int x, y;

    ChessColor color;
    Type type;

    int numberOfMoves;

    struct possibleMovesList *possibleMovesList;
};

ChessPiece *CreateChessPiece(int x, int y, ChessColor color, Type type){
    ChessPiece *chessPiece = malloc(sizeof(ChessPiece));
    if (chessPiece == NULL) return NULL;

    chessPiece->x = x;
    chessPiece->y = y;

    chessPiece->color   = color;
    chessPiece->type    = type;

    chessPiece->numberOfMoves = 0;

    chessPiece->possibleMovesList = CreatePossibleMovesList();
    if (chessPiece->possibleMovesList == NULL){
        DeletePiece(&chessPiece);
        return NULL;
    }

    return chessPiece;
}

void DeletePiece(ChessPiece **piece){
    if (*piece != NULL){
        DeletePossibleMovesList(&((*piece)->possibleMovesList));
        free(*piece);
        *piece = NULL;
    }
}

Type GetTypeByChar(char t){
    switch(t){
        case 'P':   return Pawn;
        case 'R':   return Rook;
        case 'N':   return Knight;
        case 'B':   return Bishop;
        case 'K':   return King;
        case 'Q':   return Queen;
        default:    return Pawn;
    }
}

char GetCharByType(Type t){
    switch(t){
        case Pawn:      return 'P';
        case Rook:      return 'R';
        case Knight:    return 'N';
        case Bishop:    return 'B';
        case King:      return 'K';
        case Queen:     return 'Q';
        default:        return 'P';
    }
}

//Gets And Sets

//Position
int ChessPiece_GetX(ChessPiece *piece){
    if (piece == NULL) return -1;

    return piece->x;
}
int ChessPiece_GetY(ChessPiece *piece){
    if (piece == NULL) return -1;

    return piece->y;
}
void ChessPiece_SetPosition(ChessPiece *piece, int x, int y){
    if (piece == NULL) return;

    if (x != -1) piece->x = x;
    if (y != -1) piece->y = y;
}

//Type
int ChessPiece_GetType(ChessPiece *piece){
    if (piece == NULL) return -1;

    return piece->type;
}
void ChessPiece_SetType(ChessPiece *piece, Type t){
    if (piece == NULL) return;

    piece->type = t;
}

//Color
ChessColor ChessPiece_GetColor(ChessPiece *piece){
    if (piece == NULL) return -1;

    return piece->color;
}

//Number Of Moves
int ChessPiece_GetNumberOfMoves(ChessPiece *piece){
    if (piece == NULL) return -1;

    return piece->numberOfMoves;
}
void ChessPiece_AddNumberOfMoves(ChessPiece *piece, int inc){
	if (piece == NULL) return;

	piece->numberOfMoves += inc;
}
void ChessPiece_SetNumberOfMoves(ChessPiece *piece, int n){
    if (piece == NULL) return;

    piece->numberOfMoves = n;
}

//Possible Moves List
PossibleMovesList *ChessPiece_GetPossibleMovesList(ChessPiece *piece){
    if (piece == NULL) return NULL;

    return piece->possibleMovesList;
}
