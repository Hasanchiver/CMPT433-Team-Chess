

#include "Rules.h"
#include "List.h"
#include "PossibleMovesList.h"

#include <stdlib.h>
#include <stdbool.h>

int MovePiece(ChessPiece *piece, int newX, int newY, Game *game){
    if (piece == NULL || game == NULL) return 0;

    ChessBoard *board = Game_GetBoard(game);

    int isValidMove = GetPossibleMoveType(ChessPiece_GetPossibleMovesList(piece), newX, newY);
    if (isValidMove == None) return 0;

    //En Passant
    if (isValidMove == EnPassant){
        if (ChessPiece_GetColor(piece) == White) KillPiece(board, newX, newY+1, Black);
        else KillPiece(board, newX, newY-1, White);
    }
    //Castling
    if (isValidMove == Castling){
        if (newX == 2){
			ChessBoard_SetChessPieceAtPosition(board, 3, newY, ChessBoard_GetChessPieceAtPosition(board, 0, newY));
            //board->pieces[3][newY] = board->pieces[0][newY];
			ChessBoard_SetChessPieceAtPosition(board, 0, newY, NULL);
            //board->pieces[0][newY] = NULL;

			ChessPiece_SetPosition(ChessBoard_GetChessPieceAtPosition(board, 3, newY), 3, -1);
            //board->pieces[3][newY]->x = 3;
			ChessPiece_AddNumberOfMoves(ChessBoard_GetChessPieceAtPosition(board, 3, newY), 1);
            //board->pieces[3][newY]->numberOfMoves++;
        } else {
			ChessBoard_SetChessPieceAtPosition(board, 5, newY, ChessBoard_GetChessPieceAtPosition(board, 7, newY));
            //board->pieces[5][newY] = board->pieces[7][newY];
			ChessBoard_SetChessPieceAtPosition(board, 7, newY, NULL);
            //board->pieces[7][newY] = NULL;

			ChessPiece_SetPosition(ChessBoard_GetChessPieceAtPosition(board, 5, newY), 5, -1);
            //board->pieces[5][newY]->x = 5;
			ChessPiece_AddNumberOfMoves(ChessBoard_GetChessPieceAtPosition(board, 5, newY), 1);
            //board->pieces[5][newY]->numberOfMoves++;
        }
    }
    //Normal kill
    if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
        KillPiece(board, newX, newY, ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)));
    }

    //Remove from board
    ChessBoard_SetChessPieceAtPosition(board, ChessPiece_GetX(piece), ChessPiece_GetY(piece), NULL);
    //Set new position
    ChessPiece_SetPosition(piece, newX, newY);
    //piece->x = newX;
    //piece->y = newY;
    //Update board
    ChessBoard_SetChessPieceAtPosition(board, newX, newY, piece);
    //board->pieces[newX][newY] = piece;
    ChessPiece_AddNumberOfMoves(piece, 1);
    //piece->numberOfMoves++;

    //Check if it's promotion
    if (ChessPiece_GetType(piece) == Pawn && (ChessPiece_GetY(piece) == 0 || ChessPiece_GetY(piece) == 7)){
        return 2;
    }

    //Update last moved piece
    Player_SetLastMovedPiece(GetPlayer(board, ChessPiece_GetColor(piece)), piece);

    return 1;


    /*
    If it's promotion, returns 2.
    Then, in MouseInputManager, treat it
    accordingly, switching to Promotion Game State.
    */
}

int IsValidMoveAndKingNotInCheck(ChessPiece *piece, int newX, int newY, ChessBoard *board){
    int flag = IsValidMove(piece, newX, newY, board);

    if (flag == None) return None;

    if (WillLetKingInCheck(piece, newX, newY, board)) return None;

    return flag;
}

int IsValidMove(ChessPiece *piece, int newX, int newY, ChessBoard *board){
    if (piece == NULL) return None;
    switch(ChessPiece_GetType(piece)){
        case Pawn:
            return (IsValidPawnMove(piece, newX, newY, board));
        case Rook:
            return (IsValidRookMove(piece, newX, newY, board));
        case Knight:
            return (IsValidKnightMove(piece, newX, newY, board));
        case Bishop:
            return (IsValidBishopMove(piece, newX, newY, board));
        case Queen:
            return (IsValidQueenMove(piece, newX, newY, board));
        case King:
            return (IsValidKingMove(piece, newX, newY, board));
        default: return None;
    }
    return None;
}

int IsValidPawnMove(ChessPiece *piece, int newX, int newY, ChessBoard *board){
    //Out Of Bounds
    if ((newX > 7) || (newX < 0) || (newY > 7) || (newY < 0)) return None;
    int pieceY = ChessPiece_GetY(piece);
    int pieceX = ChessPiece_GetX(piece);
    //Current Position
    if (newX == pieceX && newY == pieceY) return None;

    //WHITE
    if (ChessPiece_GetColor(piece) == White){
        //Forward
        if ((newY == pieceY - 1) && (newX == pieceX)){
            if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) == NULL) return Normal;    //If has another piece in front
            else return None;
        }
        //Diagonal/En Passant
        if ((newY == pieceY - 1) && ((newX == pieceX + 1) || (newX == pieceX - 1))){
            if ((ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL) &&
                (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece))){
                return Attack;
            }
            else if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) == NULL){
                //En Passant
                if (ChessBoard_GetChessPieceAtPosition(board, newX, pieceY) != NULL){
                    if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, pieceY)) != ChessPiece_GetColor(piece)
                            && ChessPiece_GetType(ChessBoard_GetChessPieceAtPosition(board, newX, pieceY)) == Pawn){
                        if (ChessPiece_GetNumberOfMoves(ChessBoard_GetChessPieceAtPosition(board, newX, pieceY)) == 1){
                            if (ChessBoard_GetChessPieceAtPosition(board, newX, pieceY) == Player_GetLastMovedPiece(GetPlayer(board, White))
                                    || ChessBoard_GetChessPieceAtPosition(board, newX, pieceY) == Player_GetLastMovedPiece(GetPlayer(board, Black))){
                                //FINALLY
                                //En passant, needs to KillPiece the other pawn
                                return EnPassant;
                            }
                        }
                    }
                }
            }
        }
        //Double Move
        if ((pieceY == 6) && (newY == pieceY - 2) && (newX == pieceX)){
            if (ChessBoard_GetChessPieceAtPosition(board, newX, pieceY-1) == NULL
                    && ChessBoard_GetChessPieceAtPosition(board, newX, pieceY-2) == NULL){
                return Normal;
            }
            else return None;
        }
    }
    else{   //BLACK
        //Forward
        if ((newY == pieceY + 1) && (newX == pieceX)){
            if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) == NULL) return Normal;    //If has another piece in front
            else return None;
        }
        //Diagonal/En Passant
        if ((newY == pieceY + 1) && ((newX == pieceX + 1) || (newX == pieceX - 1))){
            if ((ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL)
                    && (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece))){
                return Attack;
            }
            else if ((ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL)){
                //En Passant
                if (ChessBoard_GetChessPieceAtPosition(board, newX, pieceY) != NULL){
                    if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, pieceY)) != ChessPiece_GetColor(piece)
                                            && ChessPiece_GetType(ChessBoard_GetChessPieceAtPosition(board, newX, pieceY)) == Pawn){
                        if (ChessPiece_GetNumberOfMoves(ChessBoard_GetChessPieceAtPosition(board, newX, pieceY)) == 1){
                            if (ChessBoard_GetChessPieceAtPosition(board, newX, pieceY) == Player_GetLastMovedPiece(GetPlayer(board, White))
                                            || ChessBoard_GetChessPieceAtPosition(board, newX, pieceY) == Player_GetLastMovedPiece(GetPlayer(board, Black))){
                                //En passant, needs to KillPiece the other pawn
                                return EnPassant;
                            }
                        }
                    }
                }
            }
        }
        //Double Move
        if ((pieceY == 1) && (newY == pieceY + 2) && (newX == pieceX)){
            if (ChessBoard_GetChessPieceAtPosition(board, newX, pieceY + 1) == NULL
                        && ChessBoard_GetChessPieceAtPosition(board, newX, pieceY + 2) == NULL){
                return Normal;
            }
            else return None;
        }
    }
    return None;
}

int IsValidRookMove(ChessPiece *piece, int newX, int newY, ChessBoard *board){
    //Out Of Bounds
    if ((newX > 7) || (newX < 0) || (newY > 7) || (newY < 0)) return None;

    int pieceX = ChessPiece_GetX(piece);
    int pieceY = ChessPiece_GetY(piece);

    //Current Position
    if (newX == pieceX && newY == pieceY) return None;

    //Vertical
    int i = -1;
    if (newX == pieceX){
        //Up
        if (newY > pieceY){
            for (i = pieceY+1; i < newY; i++){
                //If there's a piece in between
                if (ChessBoard_GetChessPieceAtPosition(board, newX, i) != NULL) return None;
            }
        }
        //Down
        if (newY < pieceY){
            for (i = pieceY-1; i > newY; i--){
                //If there's a piece in between
                if (ChessBoard_GetChessPieceAtPosition(board, newX, i) != NULL) return None;
            }
        }
    }
    //If got to newY
    if (i == newY){
        //Check if there's a piece in the new position
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)){
                return Attack;
            } else return None;
        } else return Normal;
    }

    //Horizontal
    if (newY == pieceY){
        //Right
        if (newX > pieceX){
            for (i = pieceX+1; i < newX; i++){
                //If there's a piece in between
                if (ChessBoard_GetChessPieceAtPosition(board, i, newY) != NULL) return None;
            }
        }
        //Left
        if (newX < pieceX){
            for (i = pieceX-1; i > newX; i--){
                //If there's a piece in between
                if (ChessBoard_GetChessPieceAtPosition(board, i, newY) != NULL) return None;
            }
        }
    }
    //If got to newX
    if (i == newX){
        //Check if there's a piece in the new position
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)){
                return Attack;
            } else return None;
        } else return Normal;
    }

    return None;
}

int IsValidKnightMove(ChessPiece *piece, int newX, int newY, ChessBoard *board){
    //Out Of Bounds
    if ((newX > 7) || (newX < 0) || (newY > 7) || (newY < 0)) return None;
    int pieceX = ChessPiece_GetX(piece);
    int pieceY = ChessPiece_GetY(piece);
    //Current Position
    if (newX == pieceX && newY == pieceY) return None;

    //Up-Right
    if (newX == pieceX + 1 && newY == pieceY - 2){
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)) return Attack;
            else return None;
        }
        else return Normal;
    }
    //Right-Up
    if (newX == pieceX + 2 && newY == pieceY - 1){
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)) return Attack;
            else return None;
        }
        else return Normal;
    }
    //Up-Left
    if (newX == pieceX - 1 && newY == pieceY - 2){
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)) return Attack;
            else return None;
        }
        else return Normal;
    }
    //Left-Up
    if (newX == pieceX - 2 && newY == pieceY - 1){
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)) return Attack;
            else return None;
        }
        else return Normal;
    }
    //Down-Right
    if (newX == pieceX + 1 && newY == pieceY + 2){
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)) return Attack;
            else return None;
        }
        else return Normal;
    }
    //Right-Down
    if (newX == pieceX + 2 && newY == pieceY + 1){
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)) return Attack;
            else return None;
        }
        else return Normal;
    }
    //Down-Left
    if (newX == pieceX - 1 && newY == pieceY + 2){
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)) return Attack;
            else return None;
        }
        else return Normal;
    }
    //Left-Down
    if (newX == pieceX - 2 && newY == pieceY + 1){
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)) return Attack;
            else return None;
        }
        else return Normal;
    }

    return None;
}

int IsValidBishopMove(ChessPiece *piece, int newX, int newY, ChessBoard *board){
    //Out Of Bounds
    if ((newX > 7) || (newX < 0) || (newY > 7) || (newY < 0)) return None;
    int pieceX = ChessPiece_GetX(piece);
    int pieceY = ChessPiece_GetY(piece);
    //Current Position
    if (newX == pieceX && newY == pieceY) return None;

    int i;
    bool noPieceBetween = false;
    //Down-Right

    for (i = 1; i < 8; i++){
        if (newX > pieceX){
            if (newY > pieceY){
                //Down-Right
                //If out of bounds
                if (pieceX + i > 8 || pieceY + i > 8) return None;
                //If reached position
                if (newX == pieceX + i && newY == pieceY + i){
                    noPieceBetween = true;
                    break;
                }
                //If there's a piece in between
                if (ChessBoard_GetChessPieceAtPosition(board, pieceX + i, pieceY + i) != NULL) return None;
            }
            else if (newY < pieceY){
                //Up-Right
                //If out of bounds
                if (pieceX + i > 8 || pieceY - i < 0) return None;
                //If reached position
                if (newX == pieceX + i && newY == pieceY - i){
                    noPieceBetween = true;
                    break;
                }
                //If there's a piece in between
                if (ChessBoard_GetChessPieceAtPosition(board, pieceX + i, pieceY - i) != NULL) return None;
            }
        }
        else if (newX < pieceX){
            if (newY > pieceY){
                //Down-Left
                //If out of bounds
                if (pieceX - i < 0 || pieceY + i > 8) return None;
                //If reached position
                if (newX == pieceX - i && newY == pieceY + i){
                    noPieceBetween = true;
                    break;
                }
                //If there's a piece in between
                if (ChessBoard_GetChessPieceAtPosition(board, pieceX - i, pieceY + i) != NULL) return None;
            }
            else if (newY < pieceY){
                //Up-Left
                //If out of bounds
                if (pieceX - i < 0 || pieceY - i < 0) return None;
                //If reached position
                if (newX == pieceX - i && newY == pieceY - i){
                    noPieceBetween = true;
                    break;
                }
                //If there's a piece in between
                if (ChessBoard_GetChessPieceAtPosition(board, pieceX - i, pieceY - i) != NULL) return None;
            }
        }
    }

    if (noPieceBetween){
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)) return Attack;
            else return None;
        } else return Normal;
    }

    return None;
}

int IsValidQueenMove(ChessPiece *piece, int newX, int newY, ChessBoard *board){
    //Out Of Bounds
    if ((newX > 7) || (newX < 0) || (newY > 7) || (newY < 0)) return None;
    int pieceX = ChessPiece_GetX(piece);
    int pieceY = ChessPiece_GetY(piece);
    //Current Position
    if (newX == pieceX && newY == pieceY) return None;

    //Lazy way: Queen = Bishop + Rook
    return (IsValidBishopMove(piece, newX, newY, board) || IsValidRookMove(piece, newX, newY, board));
}

int IsValidKingMove(ChessPiece *piece, int newX, int newY, ChessBoard *board){
    //Out Of Bounds
    if ((newX > 7) || (newX < 0) || (newY > 7) || (newY < 0)) return None;
    int pieceX = ChessPiece_GetX(piece);
    int pieceY = ChessPiece_GetY(piece);
    //Current Position
    if (newX == pieceX && newY == pieceY) return None;

    //Up
    if (newX == pieceX && newY == pieceY - 1){
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)) return Attack;
            else return None;
        }
        else return Normal;
    }
    //Down
    if (newX == pieceX && newY == pieceY + 1){
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)) return Attack;
            else return None;
        }
        else return Normal;
    }
    //Left
    if (newX == pieceX - 1 && newY == pieceY){
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)) return Attack;
            else return None;
        }
        else return Normal;
    }
    //Right
    if (newX == pieceX + 1 && newY == pieceY){
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)) return Attack;
            else return None;
        }
        else return Normal;
    }
    //Up-Left
    if (newX == pieceX - 1 && newY == pieceY - 1){
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)) return Attack;
            else return None;
        }
        else return Normal;
    }
    //Up-Right
    if (newX == pieceX + 1 && newY == pieceY - 1){
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)) return Attack;
            else return None;
        }
        else return Normal;
    }
    //Down-Left
    if (newX == pieceX - 1 && newY == pieceY + 1){
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)) return Attack;
            else return None;
        }
        else return Normal;
    }
    //Down-Right
    if (newX == pieceX + 1 && newY == pieceY + 1){
        if (ChessBoard_GetChessPieceAtPosition(board, newX, newY) != NULL){
            if (ChessPiece_GetColor(ChessBoard_GetChessPieceAtPosition(board, newX, newY)) != ChessPiece_GetColor(piece)) return Attack;
            else return None;
        }
        else return Normal;
    }

    //Castling
    if (newY  == pieceY && (newY == 0 || newY == 7)){
        if (ChessPiece_GetNumberOfMoves(piece) > 0) return None;
        ChessPiece* leftRook = ChessBoard_GetChessPieceAtPosition(board, 0, newY);
        ChessPiece* rightRook = ChessBoard_GetChessPieceAtPosition(board, 7, newY);
        if (newX == 2 && leftRook != NULL){
            if (ChessPiece_GetType(leftRook) == Rook && ChessPiece_GetNumberOfMoves(leftRook) == 0){
                //If none in between (1, 2, 3), and king can't be in check there
                if (ChessBoard_GetChessPieceAtPosition(board, 1, newY) != NULL
                    || ChessBoard_GetChessPieceAtPosition(board, 2, newY) != NULL
                    || ChessBoard_GetChessPieceAtPosition(board, 3, newY) != NULL)
                        return None;
                if (WillLetKingInCheck(piece, 1, newY, board)) return None;
                if (WillLetKingInCheck(piece, 2, newY, board)) return None;
                if (WillLetKingInCheck(piece, 3, newY, board)) return None;
                if (IsKingInCheck(board, ChessPiece_GetColor(piece))) return None;

                return Castling;
            }
        } else if (newX == 6 && rightRook != NULL){
            if (ChessPiece_GetType(rightRook) == Rook && ChessPiece_GetNumberOfMoves(rightRook) == 0){
                //If none in between (5, 6), and king can't be in check there
                if (ChessBoard_GetChessPieceAtPosition(board, 5, newY) != NULL
                    || ChessBoard_GetChessPieceAtPosition(board, 6, newY) != NULL)
                        return None;
                if (WillLetKingInCheck(piece, 5, newY, board)) return None;
                if (WillLetKingInCheck(piece, 6, newY, board)) return None;
                if (IsKingInCheck(board, ChessPiece_GetColor(piece))) return None;

                return Castling;
            }
        }
    }

    return None;
}

ChessPiece *Promotion(ChessPiece *piece, Game *game, Type t){
    ChessBoard *board = Game_GetBoard(game);
    int pieceX = ChessPiece_GetX(piece);
    int pieceY = ChessPiece_GetY(piece);

    //Create new ChessPiece for type
    ChessPiece *newPiece = CreateChessPiece(pieceX, pieceY, ChessPiece_GetColor(piece), t);
    ChessPiece_AddNumberOfMoves(newPiece, ChessPiece_GetNumberOfMoves(piece));
    //Remove Piece from board and player list
    ChessBoard_SetChessPieceAtPosition(board, pieceX, pieceY, NULL);
    List_RemovePiece(Player_GetList(GetPlayer(board, ChessPiece_GetColor(piece))), pieceX, pieceY);
    //Add piece to player list
    List_AddPiece(Player_GetList(GetPlayer(board, ChessPiece_GetColor(piece))), newPiece);
    //Add piece to board
    ChessBoard_SetChessPieceAtPosition(board, ChessPiece_GetX(newPiece), ChessPiece_GetY(newPiece), newPiece);

    return newPiece;
}

void KillPiece(ChessBoard *board, int x, int y, ChessColor color){
    //Check if piece was last moved piece
    ChessPiece *lastMoved = Player_GetLastMovedPiece(GetPlayer(board, color));
    if (lastMoved != NULL){
        if (ChessPiece_GetX(lastMoved) == x && ChessPiece_GetY(lastMoved) == y)
            Player_SetLastMovedPiece(GetPlayer(board, color), NULL);
    }

    //Remove piece from board
    ChessBoard_SetChessPieceAtPosition(board, x, y, NULL);

    //Remove piece from player's list
    List_RemovePiece(Player_GetList(GetPlayer(board, color)), x, y);
}

int WillLetKingInCheck(ChessPiece *piece, int newX, int newY, ChessBoard *board){
    int pieceX = ChessPiece_GetX(piece);
    int pieceY = ChessPiece_GetY(piece);

    //Save current state
    ChessPiece *tempPiece = ChessBoard_GetChessPieceAtPosition(board, newX, newY);

    //Simulate new state
    ChessBoard_SetChessPieceAtPosition(board, pieceX, pieceY, NULL);
    ChessBoard_SetChessPieceAtPosition(board, newX, newY, piece);

    //Check if king is in check
    //Get enemy's piece list
    List *l;
    if (ChessPiece_GetColor(piece) == White) l = Player_GetList(GetPlayer(board, Black));
    else l = Player_GetList(GetPlayer(board, White));

    //Get friendly king
    ChessPiece *king = List_GetKing(Player_GetList(GetPlayer(board, ChessPiece_GetColor(piece))));

    //Search for a piece in enemy's list that can kill the king
    int i;
    for (i = 0; i < List_GetSize(l); i++){
        ChessPiece* p = List_GetPieceAt(l, i);
        if (p != NULL){
            if (p != tempPiece){
                if (ChessPiece_GetType(piece) == King){
                    if (IsValidMove(p, newX, newY, board)) break;
                } else if (IsValidMove(p, ChessPiece_GetX(king), ChessPiece_GetY(king), board) > 0) break;
            }
        }
    }

    //End state simulation
    ChessBoard_SetChessPieceAtPosition(board, newX, newY, tempPiece);
    ChessBoard_SetChessPieceAtPosition(board, pieceX, pieceY, piece);

    if (i == List_GetSize(l)) return 0; //Didn't find a piece which could check king
    else return 1;
}

int IsKingInCheck(ChessBoard *board, ChessColor color){
    //Get enemy player and friendly king
    Player *enemyPlayer;
    ChessPiece *king;

    enemyPlayer = GetPlayer(board, !color);
    king = List_GetKing(Player_GetList(GetPlayer(board, color)));

    //Run through enemy piece list
    int i;
    for (i = 0; i<PLAYER_MAX_PIECES; i++){
        ChessPiece *piece = List_GetPieceAt(Player_GetList(enemyPlayer), i);
        if (piece != NULL){
            if (GetPossibleMoveType(ChessPiece_GetPossibleMovesList(piece), ChessPiece_GetX(king), ChessPiece_GetY(king)) != None)
                return 1;
        }
    }

    return 0;
}

int Checkmate(ChessPiece *king, ChessBoard *board){
    //If king has a move available
    if (!PossibleMovesList_IsEmpty(ChessPiece_GetPossibleMovesList(king))) return 0;

    //Check if a piece can protect the king
    Player *player = GetPlayer(board, ChessPiece_GetColor(king));

    int i;
    for (i = 0; i<PLAYER_MAX_PIECES; i++){
        ChessPiece *temp = List_GetPieceAt(Player_GetList(player), i);
        if (temp != NULL && ChessPiece_GetType(temp) != King){
            //If such piece has a move available
            if (!PossibleMovesList_IsEmpty(ChessPiece_GetPossibleMovesList(temp))) return 0;
        }
    }

    return 1;
}
