#include "udpStockfish.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define MOVE "move"

#define PORTNUMBER 54321
#define MAX_MESSAGE_LENGTH 1024
#define MAXVOL 100
#define MAXTEMPO 300
#define MINTEMPO 40

static pthread_t threadId;
static char msgOut[MAX_MESSAGE_LENGTH];
static _Bool stopUdp = false;

_Bool userCommandStop() {
	return stopUdp;
}
// static int getIndexFromString(char *string) {
// 	int value = -1;
// 	sscanf(string, "%*s %d", &value);
// 	return value;
// }
static _Bool compareCommand(char* msgIn, char* acceptedCommand) {
	return strncmp(msgIn, acceptedCommand, strlen(acceptedCommand)) == 0;
}

static void processInMsg(char* msgIn, int socketDescriptor,
		struct sockaddr_in *sin) {
	msgOut[0] = 0;
	//int number = getIndexFromString(msgIn);
	int feedback = -1;
	if (compareCommand(msgIn, MOVE)) {
		sprintf(msgOut,
				"Command not accepted. Type help for available commands.\n");
	} else {
		sprintf(msgOut,
				"Command not accepted. Type help for available commands.\n");
	}
	if (feedback != -1) {
		sprintf(msgOut, "%d\n", feedback);
	}
}

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

void UdpStart(void) {
	pthread_create(&threadId, NULL, *startUdp, NULL);
}
void UdpStop(void) {
	stopUdp = true;
	pthread_join(threadId, NULL);
}
