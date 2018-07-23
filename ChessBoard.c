

#include "ChessBoard.h"
#include "PossibleMovesList.h"
#include "Rules.h"

#include <stdlib.h>

struct chessBoard{
    ChessPiece *pieces[8][8];

    Player *player1;
    Player *player2;
};

ChessBoard *CreateChessBoard(Player *player1, Player *player2){
    ChessBoard *board = malloc(sizeof(ChessBoard));
    if (board == NULL) return NULL;

    ResetBoard(board);

    board->player1 = player1;
    board->player2 = player2;

    if (board->player1 == NULL || board->player2 == NULL){
        DeleteChessBoard(&board);
        return NULL;
    }

    return board;
}

void DeleteChessBoard(ChessBoard **board){
    if (*board != NULL){
        DeletePlayer(&((*board)->player1));
        DeletePlayer(&((*board)->player2));
        free(*board);
        *board = NULL;
    }
}

void InitializeBoard(ChessBoard *board){
    if (board != NULL){
        //Run through both players's lists
        int i;
        for (i = 0; i<PLAYER_MAX_PIECES; i++){
            ChessPiece *p1 = List_GetPieceAt(Player_GetList(GetPlayer(board, White)), i);
            if (p1 != NULL) board->pieces[ChessPiece_GetX(p1)][ChessPiece_GetY(p1)] = p1;

            ChessPiece *p2 = List_GetPieceAt(Player_GetList(GetPlayer(board, Black)), i);
            if (p2 != NULL) board->pieces[ChessPiece_GetX(p2)][ChessPiece_GetY(p2)] = p2;
        }
    }
}

void ResetBoard(ChessBoard *board){
    if (board != NULL){
        int i, j;
        for (i = 0; i<8; i++){
            for (j = 0; j<8; j++){
                board->pieces[j][i] = NULL;
            }
        }
    }
}

void UpdatePossibleMovesLists(ChessBoard *board){
    if (board == NULL) return;

    int i;
    for (i = 0; i<PLAYER_MAX_PIECES; i++){
        //Player1
        ChessPiece *piece = List_GetPieceAt(Player_GetList(GetPlayer(board, White)), i);
        if (piece != NULL){
            UpdatePossibleMovesList(board, piece);
        }
        //Player2
        piece = List_GetPieceAt(Player_GetList(GetPlayer(board, Black)), i);
        if (piece != NULL){
            UpdatePossibleMovesList(board, piece);
        }
    }
}

void UpdatePossibleMovesList(ChessBoard *board, ChessPiece *piece){
    if (piece == NULL || board == NULL) return;

    PossibleMovesList *movesList = ChessPiece_GetPossibleMovesList(piece);
    PossibleMovesList_SetLast(movesList, NULL);

    int i, j;
    for (i = 0; i<8; i++){
        for (j = 0; j<8; j++){
            int t = IsValidMoveAndKingNotInCheck(piece, j, i, board);
            if (t != 0)
                AddPossibleMoveOrReplace(movesList, j, i, t);
        }
    }

    //If there was no move to be added
    if (PossibleMovesList_GetLast(movesList) == NULL)
        PossibleMovesList_SetIsEmpty(movesList, 1);
    else PossibleMovesList_SetIsEmpty(movesList, 0);
}



void UpdateBoard(ChessBoard *board){
    ResetBoard(board);
    InitializeBoard(board);
}

//Gets And Sets
ChessPiece *ChessBoard_GetChessPieceAtPosition(ChessBoard *board, int x, int y){
    if (board == NULL) return NULL;

    return board->pieces[x][y];
}
void ChessBoard_SetChessPieceAtPosition(ChessBoard *board, int x, int y, ChessPiece *piece){
    if (board == NULL) return;

    board->pieces[x][y] = piece;
}

Player *GetPlayer(ChessBoard *board, ChessColor color){
    if (Player_GetColor(board->player1) == color) return board->player1;
    else return board->player2;
}
void ChessBoard_SetPlayer(ChessBoard *board, Player *player, ChessColor color){
    if (board == NULL || player == NULL) return;

    if (color == White) board->player1 = player;
    else board->player2 = player;
}
