

/*
List Of Possible Moves
A linked list, with a pointer to the first and the last elements. The end
pointer points to the last element of current iteraction, not the end of the list.
In the Update function, last is set as NULL before the additions. If it's still
NULL after the additions, it means there's no move available, so isEmpty = 1.

The last pointer works with the linked list to make it be able to replace
nodes's informations when adding new moves, instead of deleting and creating
new nodes. Each addition is actually a replacement of the next node after latest
interaction.Last is set to the next so it moves accordingly. If no such node  exists,
then a new one is created.
*/

#include "PossibleMovesList.h"
#include "ChessBoard.h"
#include "Rules.h"

#include <stdlib.h>

struct possibleMovesNode{
    int x, y;
    int moveType;
    struct possibleMovesNode *next;
};

struct possibleMovesList{
    int isEmpty;
    PossibleMovesNode *first;
    PossibleMovesNode *last;
};

PossibleMovesList *CreatePossibleMovesList(){
    PossibleMovesList *l = malloc(sizeof(PossibleMovesList));
    if (l == NULL) return NULL;

    l->first = NULL;
    l->last = NULL;
    l->isEmpty = 1;

    return l;
}

PossibleMovesNode *CreatePossibleMovesNode(int x, int y, int t){
    PossibleMovesNode *node = malloc(sizeof(PossibleMovesNode));
    if (node == NULL) return NULL;

    node->next = NULL;
    node->x = x;
    node->y = y;
    node->moveType = t;

    return node;
}

/*
The fact that this funtion doesn't allocate a new node if there's a node available (just
change an existing node's values), means that there's no need to clear the list everytime
as well (free). By doing that, the number of operations in the heap is greatly reduced,
increasing the speed.
*/
void AddPossibleMoveOrReplace(PossibleMovesList *l, int x, int y, int t){
    if (l == NULL) return;

    //If first hasn't been created, l is empty, and l->cur is still NULL
    if (l->first == NULL){
        l->first = CreatePossibleMovesNode(x, y, t);
        l->last = l->first;
        l->isEmpty = 0;
        return;
    }

    //If it's first iteration (needs to check l->first)
    if (l->last == NULL){
        l->last = l->first;
    } else if (l->last->next != NULL){
        //If next exists, it'll be replaced
        l->last = l->last->next;
    } else{
        //If next is NULL, current list size can't contain all needed nodes
        l->last->next = CreatePossibleMovesNode(x, y, t);
        l->last = l->last->next;
        return;
    }
    //Replace
    l->last->x = x;
    l->last->y = y;
    l->last->moveType = t;
}

int GetPossibleMoveType(PossibleMovesList *l, int x, int y){
    if (l == NULL) return 0;
    if (l->isEmpty == 1) return 0;

    PossibleMovesNode *paux = l->first;
    while(paux != NULL){
        if (paux->x == x && paux->y == y) return paux->moveType;
        if (paux == l->last) return 0;
        paux = paux->next;
    }

    return 0;
}
/*
void UpdatePossibleMovesLists(ChessBoard *board){
    if (board == NULL) return;

    int i;
    for (i = 0; i<board->player1->pieceList->lSize; i++){
        //Player1
        ChessPiece *piece = GetPieceAt(board->player1->pieceList, i);
        if (piece != NULL){
            UpdatePossibleMovesList(board, piece);
        }
        //Player2
        piece = GetPieceAt(board->player2->pieceList, i);
        if (piece != NULL){
            UpdatePossibleMovesList(board, piece);
        }
    }
}*/

/*
void UpdatePossibleMovesList(ChessBoard *board, ChessPiece *piece){
    if (piece == NULL || board == NULL) return;

    piece->possibleMovesList->last = NULL;

    int i, j;
    for (i = 0; i<8; i++){
        for (j = 0; j<8; j++){
            int t = IsValidMoveAndKingNotInCheck(piece, j, i, board);
            if (t != 0)
                AddPossibleMoveOrReplace(piece->possibleMovesList, j, i, t);
        }
    }

    //If there was no move to be added
    if (piece->possibleMovesList->last == NULL)
        piece->possibleMovesList->isEmpty = 1;
    else piece->possibleMovesList->isEmpty = 0;
}*/

void DeletePossibleMovesList(PossibleMovesList **l){
    if (*l != NULL){
        PossibleMovesNode *paux = NULL, *prem = NULL;
        paux = (*l)->first;
        while(paux != NULL){
            prem = paux;
            paux = paux->next;
            DeletePossibleMovesNode(&prem);
        }
        free(*l);
        *l = NULL;
    }
}

void DeletePossibleMovesNode(PossibleMovesNode **node){
    if (*node != NULL){
        free(*node);
        *node = NULL;
    }
}


//GETS AND SETS

//Pointers
PossibleMovesNode *PossibleMovesList_GetFirst(PossibleMovesList *l){
    if (l == NULL) return NULL;

    return l->first;
}
PossibleMovesNode *PossibleMovesList_GetLast(PossibleMovesList *l){
    if (l == NULL) return NULL;

    return l->last;
}
void PossibleMovesList_SetLast(PossibleMovesList *l, PossibleMovesNode *node){
    if (l == NULL) return;

    l->last = node;
}

//Nodes
int PossibleMovesNode_GetX(PossibleMovesNode *node){
    if (node == NULL) return -1;

    return node->x;
}
int PossibleMovesNode_GetY(PossibleMovesNode *node){
    if (node == NULL) return -1;

    return node->y;
}
void PossibleMovesNode_SetPosition(PossibleMovesNode *node, int x, int y){
    if (node == NULL) return;

    if (x != -1) node->x = x;
    if (y != -1) node->y = y;
}

int PossibleMovesNode_GetMoveType(PossibleMovesNode *node){
    if (node == NULL) return -1;

    return node->moveType;
}
void PossibleMovesNode_SetMoveType(PossibleMovesNode *node, int t){
    if (node == NULL) return;

    node->moveType = t;
}

PossibleMovesNode *PossibleMovesNode_GetNextNode(PossibleMovesNode *node){
    if (node == NULL) return NULL;

    return node->next;
}
void PossibleMovesNode_SetNextNode(PossibleMovesNode *node, PossibleMovesNode *next){
    if (node == NULL) return;

    node->next = next;
}

int PossibleMovesList_IsEmpty(PossibleMovesList *l){
    if (l == NULL) return -1;
    return l->isEmpty;
}
void PossibleMovesList_SetIsEmpty(PossibleMovesList *l, int a){
    if (l == NULL) return;

    l->isEmpty = a;
}
