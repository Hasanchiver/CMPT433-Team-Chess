#include "udpStockfish.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define MOVE "move"
#define USERMOVED "usermoved"

#define PORTNUMBER 54321
#define MAX_MESSAGE_LENGTH 1024
#define STOCKFISH_MOVE_STRING_SIZE 10

static pthread_t threadId;
static char msgOut[MAX_MESSAGE_LENGTH];
static _Bool stopUdp = false;
static _Bool lastmove = false;
static _Bool stockfishMoved = false;
char* stockfishMove = NULL; // Bad design. Need to change

_Bool userCommandStop() {
	return stopUdp;
}
// char* separateMoveFromString(char *line) {
// 	char *s1;
// 	char *s2;
// 	char *sp;
//
// 	sp = strchr(line, ' ');
// 	if (!sp) { exit(EXIT_FAILURE); }
//
// 	s1 = strndup(line, sp-line); /* Copy chars until space */
// 	s2 = sp+1; /* Skip the space */
//
// 	free(s1);
// }

// Exposed functions for stockfish
/*************************************************************
*************************************************************/
static _Bool compareCommand(char* msgIn, char* acceptedCommand) {
	return strncmp(msgIn, acceptedCommand, strlen(acceptedCommand)) == 0;
}

void Stockfish_setLastMoveToTrue(){
	lastmove = true;
}

_Bool Stockfish_checkIfStockfishMoved(){
	return stockfishMoved;
}

void Stockfish_setStockfishMovedToFalse(){
	stockfishMoved = false;
}

void Stockfish_getStockfishMove(char **buffer, int buffersize){
	*buffer = stockfishMove;
}

/*************************************************************
*************************************************************/


// Process input recieved from stockfish socket
/*************************************************************
*************************************************************/

static void processInMsg(char* msgIn, int socketDescriptor,
		struct sockaddr_in *sin) {
	msgOut[0] = 0;

	// parse msgIn. Could contain move from stockfish.Expected stockfish string form "move e2 e4"
	char *s1,*s2,*sp;
	sp = strchr(msgIn, ' ');
	if (sp) {
		s1 = strndup(msgIn, sp-msgIn); /* Copy chars until space */
		msgIn = s1;
		s2 = sp+1; /* Skip the space */
	}

	if (compareCommand(msgIn, USERMOVED)) {
		if(lastmove){ // If user has moved since last time, send new move
			lastmove = false;
			char* buffer = NULL;
			//NetworkAPI_getMostRecentMove(&buffer, 10); TODO!! uncomment after merge with networkapi
			sprintf(msgOut, "%s",
					buffer); // Send last move by user. Expected form "e2 e4"
		}
		else{ // If user has not moved, send 0
			sprintf(msgOut,
					"0");
		}
	}
	else if (compareCommand(msgIn, MOVE)) {
		stockfishMove = s2; // store move locally. can be accessed through exposed function
		stockfishMoved = true;
		sprintf(msgOut,
	 			"\n");
	}
	else {
		sprintf(msgOut,
				"Command not accepted. Type help for available commands.\n");
	}
	free(s1);
}

/*************************************************************
*************************************************************/


// Main UDP thread for stockfish
/*************************************************************
*************************************************************/
static void *startUdp(void *args) {
	char msgIn[MAX_MESSAGE_LENGTH];

	struct sockaddr_in sin;
	unsigned int sin_len;
	memset(&sin, 0, sizeof(sin));

	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORTNUMBER);

	int socket_descriptor = socket(PF_INET, SOCK_DGRAM, 0);

	bind(socket_descriptor, (struct sockaddr*) &sin, sizeof(sin));

	while (!stopUdp) {
		sin_len = sizeof(sin);
		int bytesRx = recvfrom(socket_descriptor, msgIn, MAX_MESSAGE_LENGTH, 0,
				(struct sockaddr *) &sin, &sin_len);

		msgIn[bytesRx] = 0;

		processInMsg(msgIn, socket_descriptor, &sin);

		if (strnlen(msgOut, MAX_MESSAGE_LENGTH) > 0) {
			sin_len = sizeof(sin);
			sendto(socket_descriptor, msgOut,
					strnlen(msgOut, MAX_MESSAGE_LENGTH), 0,
					(struct sockaddr *) &sin, sin_len);
		}
	}
	close(socket_descriptor);
	return NULL;
}
/*************************************************************
*************************************************************/

void UdpStart(void) {
	pthread_create(&threadId, NULL, *startUdp, NULL);
}
void UdpStop(void) {
	stopUdp = true;
	pthread_join(threadId, NULL);
}
