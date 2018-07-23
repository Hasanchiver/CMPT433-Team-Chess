

#ifndef LIST_H
#define LIST_H

#include "ChessPiece.h"

typedef struct list List;

List* CreateList();
void DeleteList(List **list);

void List_AddPiece(List* list, ChessPiece* piece);
void List_RemovePiece(List* list, int x, int y);
void List_RemovePieceAt(List *list, int index);
ChessPiece* List_GetKing(List* list);
ChessPiece* List_GetPieceAt(List* list, int index);
ChessPiece* List_GetPieceByPosition(List *l, int x, int y);

int List_GetSize(List *l);
void List_SetPieceAt(List* list, int index, ChessPiece *piece);

#endif // LIST_H
