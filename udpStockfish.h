
#ifndef _UDP_LISTENER_H_
#define _UDP_LISTENER_H_

void UdpStart(void);

void UdpStop(void);

_Bool userCommandStop(void);

void Stockfish_setLastMoveToTrue(); // call this when the user has moved
_Bool Stockfish_checkIfStockfishMoved(); // check if stockfish moved in response to user's move
void Stockfish_getStockfishMove(char **buffer, int buffersize); // read stockfish move(call only if stockfishMoved boolea is true)
void Stockfish_setStockfishMovedToFalse(); // set bool stockfishMoved to false(call only after reading stockfishmove)

#endif
