// udpListener.c
#include "udpListener.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			// for strncmp()
#include <unistd.h>			// for close()
#include <pthread.h>
#include <stdbool.h>

#define PORTNUMBER 12345
#define MAX_MESSAGE_LENGTH 1024

#define COMMAND_MOVE     "move"
#define COMMAND_PROMOTE  "promote"
#define COMMAND_TURN     "turn"

static pthread_t threadId;
static char msgOut[MAX_MESSAGE_LENGTH];
static _Bool stopUdp = false; 


static _Bool compareCommand(char* msgIn, char* acceptedCommand)
{
	return strncmp(msgIn, acceptedCommand, strlen(acceptedCommand)) == 0;
}
static void processInMsg(char* msgIn, int socketDescriptor, struct sockaddr_in *sin)
{
	msgOut[0] = 0;
	int feedback = -1;
	if (compareCommand(msgIn,COMMAND_TURN))
	{
		feedback = 3;
		sprintf(msgOut, "%d\n", feedback);
		 //feedback = ChessGame_getTurnNumber();
	}
	else if(compareCommand(msgIn,COMMAND_MOVE))
	{
		sprintf(msgOut,"25 50 \n"); //feedback = ChessGame_getCurrentMove();
	}
	else if(compareCommand(msgIn,COMMAND_PROMOTE))
	{
		sprintf(msgOut,"hello \n"); //feedback = ChessGame_getPromotionAtTurn(number);
	}
	
}

static void *startUdp(void *args)
{
	char msgIn[MAX_MESSAGE_LENGTH];
	
	struct sockaddr_in sin;     
	unsigned int sin_len;
	memset(&sin, 0, sizeof(sin));
	
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(PORTNUMBER);	

	int socket_descriptor = socket(PF_INET, SOCK_DGRAM, 0);

	bind(socket_descriptor, (struct sockaddr*) &sin, sizeof(sin));

	while (!stopUdp)
	{
		sin_len = sizeof(sin);
		int bytesRx = recvfrom(socket_descriptor, msgIn, MAX_MESSAGE_LENGTH, 0,(struct sockaddr *) &sin, &sin_len);
		
		msgIn[bytesRx] = 0;

		processInMsg(msgIn, socket_descriptor, &sin);

		if (strnlen(msgOut, MAX_MESSAGE_LENGTH) > 0) {
			sin_len = sizeof(sin);
			sendto( socket_descriptor,
				msgOut, strnlen(msgOut, MAX_MESSAGE_LENGTH), 0,
				(struct sockaddr *) &sin, sin_len);
		}
	}
	close(socket_descriptor);
	return NULL;
}

void UdpListener_startListening(void)
{
	pthread_create(&threadId, NULL, &startUdp, NULL);
}
void UdpListener_cleanup(void)
{
	stopUdp = true;
	pthread_join(threadId, NULL);
}

