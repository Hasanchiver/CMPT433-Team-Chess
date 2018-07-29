
#ifndef _UDP_LISTENER_H_
#define _UDP_LISTENER_H_

void UdpStart(void);

void UdpStop(void);

_Bool userCommandStop(void);

void setLastMoveToTrue(); // call this when the user has moved
_Bool checkIfStockfishMoved(); // check if staockfish
void setStockfishMovedToFalse(); 
char* getStockfishMove();

#endif
