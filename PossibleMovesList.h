

#ifndef POSSIBLE_MOVES_LIST_H
#define POSSIBLE_MOVES_LIST_H

//#include "ChessBoard.h"

typedef struct possibleMovesNode PossibleMovesNode;
typedef struct possibleMovesList PossibleMovesList;

PossibleMovesList *CreatePossibleMovesList();
PossibleMovesNode *CreatePossibleMovesNode(int x, int y, int t);

void DeletePossibleMovesList(PossibleMovesList **l);
void DeletePossibleMovesNode(PossibleMovesNode **node);

//void UpdatePossibleMovesLists(ChessBoard *board);
//void UpdatePossibleMovesList(ChessBoard *board, ChessPiece *piece);

int GetPossibleMoveType(PossibleMovesList *l, int x, int y);

void AddPossibleMoveOrReplace(PossibleMovesList *l, int x, int y, int t);

PossibleMovesNode *PossibleMovesList_GetFirst(PossibleMovesList *l);
PossibleMovesNode *PossibleMovesList_GetLast(PossibleMovesList *l);
void PossibleMovesList_SetLast(PossibleMovesList *l, PossibleMovesNode *node);

int PossibleMovesList_IsEmpty(PossibleMovesList *l);
void PossibleMovesList_SetIsEmpty(PossibleMovesList *l, int a);

//Nodes
int PossibleMovesNode_GetX(PossibleMovesNode *node);
int PossibleMovesNode_GetY(PossibleMovesNode *node);
void PossibleMovesNode_SetPosition(PossibleMovesNode *node, int x, int y);

int PossibleMovesNode_GetMoveType(PossibleMovesNode *node);
void PossibleMovesNode_SetMoveType(PossibleMovesNode *node, int t);

PossibleMovesNode *PossibleMovesNode_GetNextNode(PossibleMovesNode *node);
void PossibleMovesNode_SetNextNode(PossibleMovesNode *node, PossibleMovesNode *next);

#endif // POSSIBLE_MOVES_LIST_H
