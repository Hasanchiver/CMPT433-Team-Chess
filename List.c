

#include "List.h"

#include <stdlib.h>

struct list{
    ChessPiece **pieces;
    int lSize;
};

List* CreateList(){
    List* l = malloc(sizeof(List));
    if (l == NULL) return NULL;

    l->lSize = 16;
    l->pieces = malloc((l->lSize)*sizeof(ChessPiece*));

    if (l->pieces == NULL){
        DeleteList(&l);
        return NULL;
    }

    int i;
    for (i = 0; i<l->lSize; i++){
        l->pieces[i] = NULL;
    }

    return l;
}

void DeleteList(List **list){
    if (*list != NULL){
        if ((*list)->pieces != NULL){
            int i;
            for (i = 0; i<(*list)->lSize; i++){
                if ((*list)->pieces[i] != NULL)
                    DeletePiece(&((*list)->pieces[i]));
            }
        }
        free(*list);
        *list = NULL;
    }
}

void List_AddPiece(List* list, ChessPiece* piece){
    if (piece == NULL) return;
    int i;
    for (i = 0; i < list->lSize; i++){
        if (list->pieces[i] == NULL){
            list->pieces[i] = piece;
            return;
        }
    }
}

void List_RemovePiece(List* list, int x, int y){
    int i;
    for (i = 0; i < list->lSize; i++){
        if (list->pieces[i] != NULL){
            if (ChessPiece_GetX(list->pieces[i]) == x && ChessPiece_GetY(list->pieces[i]) == y){
                DeletePiece(&(list->pieces[i]));
                list->pieces[i] = NULL;
                return;
            }
        }
    }
}

void List_RemovePieceAt(List *list, int index){
    DeletePiece(&(list->pieces[index]));
    list->pieces[index] = NULL;
}

ChessPiece* List_GetKing(List* list){
    int i;
    for (i = 0; i<list->lSize; i++){
        if (list->pieces[i] != NULL && ChessPiece_GetType(list->pieces[i]) == King)
            return list->pieces[i];
    }
    return NULL;
}

ChessPiece* List_GetPieceAt(List* list, int index){
    return list->pieces[index];
}
void List_SetPieceAt(List* list, int index, ChessPiece *piece){
    if (list == NULL) return;

    list->pieces[index] = piece;
}

ChessPiece* List_GetPieceByPosition(List *list, int x, int y){
    int i;
    for (i = 0; i<list->lSize; i++){
        if (list->pieces[i] != NULL){
            if (ChessPiece_GetX(list->pieces[i]) == x && ChessPiece_GetY(list->pieces[i]) == y)
                return list->pieces[i];
        }
    }
    return NULL;
}

int List_GetSize(List *l){
    if (l == NULL) return -1;

    return l->lSize;
}
