#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

int createChild(const char* szCommand, char* const aArguments[], const char* szMessage);

int main(){
    printf("Starting read and write\n");
    createChild("./stockfish", NULL, "uci\n");
}

int createChild(const char* szCommand, char* const aArguments[], const char* szMessage) {
  int aStdinPipe[2];
  int aStdoutPipe[2];
  int nChild;
  char nChar;
  int nResult;

  if (pipe(aStdinPipe) < 0) {
    perror("allocating pipe for child input redirect");
    return -1;
  }
  if (pipe(aStdoutPipe) < 0) {
    close(aStdinPipe[PIPE_READ]);
    close(aStdinPipe[PIPE_WRITE]);
    perror("allocating pipe for child output redirect");
    return -1;
  }

  nChild = fork();
  if (0 == nChild) {
    // stockfish continues here

    // redirect stdin
    if (dup2(aStdinPipe[PIPE_READ], STDIN_FILENO) == -1) {
      exit(errno);
    }

    // redirect stdout
    if (dup2(aStdoutPipe[PIPE_WRITE], STDOUT_FILENO) == -1) {
      exit(errno);
    }

    // redirect stderr
    if (dup2(aStdoutPipe[PIPE_WRITE], STDERR_FILENO) == -1) {
      exit(errno);
    }

    // all these are for use by the chess program only
    close(aStdinPipe[PIPE_READ]);
    close(aStdinPipe[PIPE_WRITE]);
    close(aStdoutPipe[PIPE_READ]);
    close(aStdoutPipe[PIPE_WRITE]);

    // run stockfish process image
    nResult = execv(szCommand, aArguments);

    // if we get here at all, an error occurred, but we are in the stockfish
    // process, so just exit
    exit(nResult);
  }
  else if (nChild > 0) {
    // chess program continues here

    // close unused file descriptors, these are for stockfish only only
    close(aStdinPipe[PIPE_READ]);
    close(aStdoutPipe[PIPE_WRITE]);

    if (NULL != szMessage) {
      write(aStdinPipe[PIPE_WRITE], szMessage, strlen(szMessage));
    }


    // Just a char by char read here, can be changed accordingly
    while (read(aStdoutPipe[PIPE_READ], &nChar, 1) == 1) {
        printf("%c", nChar);
      //write(STDOUT_FILENO, &nChar, 1);
    }
    printf("\n");

    // char* szMessage;
    // for(int i = 0;i < 5;i++){
    //
    //     //printf("The message is %s\n",szMessage);
    //     // Include error check here
    //
    //
    // }

    // done with these, close only when no more coommunication is required
    // between the chess program and stockfish
    close(aStdinPipe[PIPE_WRITE]);
    close(aStdoutPipe[PIPE_READ]);
  }
  else {
    // failed to create child
    close(aStdinPipe[PIPE_READ]);
    close(aStdinPipe[PIPE_WRITE]);
    close(aStdoutPipe[PIPE_READ]);
    close(aStdoutPipe[PIPE_WRITE]);
  }
  return nChild;
}
