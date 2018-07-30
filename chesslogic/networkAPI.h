#ifndef _NETWORKAPI_H_
#define _NETWORKAPI_H_

// output example: rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR 
void NetworkAPI_getBoardString(char *buffer, int buffersize); // must give at least size 100

// Ensure you have a buffer size greater than 6
// output example: moveArray = "e4 e5";
void NetworkAPI_getMostRecentMove(char *buffer, int buffersize);

#endif