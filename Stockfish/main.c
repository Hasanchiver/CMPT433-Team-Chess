#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include<arpa/inet.h>
#include<sys/socket.h>
 
#define BUFLEN 5000
#define PORT 16888



#define STOCKFISH_PATH      "/home/duke/cmpt433/public/chess/Stockfish/stockfish\n"
 
#define NUM_PIPES          2
 
#define PARENT_WRITE_PIPE  0
#define PARENT_READ_PIPE   1
 
int pipes[NUM_PIPES][2];
 

#define READ_FD  0
#define WRITE_FD 1
 
#define PARENT_READ_FD  ( pipes[PARENT_READ_PIPE][READ_FD]   )
#define PARENT_WRITE_FD ( pipes[PARENT_WRITE_PIPE][WRITE_FD] )
 
#define CHILD_READ_FD   ( pipes[PARENT_WRITE_PIPE][READ_FD]  )
#define CHILD_WRITE_FD  ( pipes[PARENT_READ_PIPE][WRITE_FD]  )
 
void main()
{
    int outfd[2];
    int infd[2];
     
    // pipes for parent to write and read
    pipe(pipes[PARENT_READ_PIPE]);
    pipe(pipes[PARENT_WRITE_PIPE]);
     
    if(!fork()) {

        char *argv[]={ "/bin/bash", 0};
 
        dup2(CHILD_READ_FD, STDIN_FILENO);
        dup2(CHILD_WRITE_FD, STDOUT_FILENO);
 
        /* Close fds not required by child. Also, we don't
           want the exec'ed program to know these existed */
        close(CHILD_READ_FD);
        close(CHILD_WRITE_FD);
        close(PARENT_READ_FD);
        close(PARENT_WRITE_FD);
          
        execv(argv[0], argv);
    } else {

        char buffer[1024];
        char cmd[30];

        int count;

       
        close(CHILD_READ_FD);
        close(CHILD_WRITE_FD);
 
        
        write(PARENT_WRITE_FD, STOCKFISH_PATH, 
				strlen(STOCKFISH_PATH) );


        count = read(PARENT_READ_FD, buffer, sizeof(buffer) - 1);
	        
	    if (count < 0 ) {
            printf("IO Error, should not come here, restart\n");
            exit(-1);
	    }

        write(PARENT_WRITE_FD, "uci\n", strlen("uci\n") );


        //Experiment code, I just simply too tired to rewrite today, sorry
        while(1){

        	count = read(PARENT_READ_FD, buffer, sizeof(buffer) - 1);
	        if (count >= 0 ) {
	            if(count != (sizeof(buffer) - 1) ){
	            	break;
	            }
	        } else {
	            printf("IO Error, should not come here, restart\n");
	            exit(-1);
	        }
        }



        //UDP init part

		struct sockaddr_in si_me, si_other;
		 
		int s, i, slen = sizeof(si_other) , recv_len;
		char buf[BUFLEN];
		 

		if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		    printf("socket creation error\n");
		    exit(-1);
		}
		 
		memset((char *) &si_me, 0, sizeof(si_me));
		 
		si_me.sin_family = AF_INET;
		si_me.sin_port = htons(PORT);
		si_me.sin_addr.s_addr = htonl(INADDR_ANY);
		

		if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
		{
		    printf("binding error\n");
		    exit(-1);
		}

		char uci_cmd[] = "position fen ";


		printf("Start listening ... \n");


		char *msg_move;

  		while(1){


  			/*	------------UDP Receive Phase ------------*/
  			if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        	{
            	printf("UDP receive error, should not occur, just restart everything\n");
		    	exit(-1);
        	}

			//uci_cmd = "position fen ";

        	// Will copy 18 characters from array1 to array2
			strncpy(uci_cmd, "position fen ", sizeof("position fen "));
        	
        	strcat(uci_cmd, buf);

        	//printf("%s\n", uci_cmd);

        	/*	------------ Write Phase ------------*/

        	write(PARENT_WRITE_FD, uci_cmd, strlen( uci_cmd ) );

        	//write(PARENT_WRITE_FD, "go depth 20\n", strlen( "go depth 20\n" ) );

        	write(PARENT_WRITE_FD, "go movetime 5000\n", strlen( "go movetime 5000\n" ));

  			
        	/*	------------ Read Phase ------------*/

        	while(1){

        		count = read(PARENT_READ_FD, buffer, sizeof(buffer) - 1);
		    
			    if (count < 0 ) {
		            printf("IO Error, should not come here, restart\n");
		            exit(-1);
			    }
			    buffer[count] = 0;

			    if( (msg_move = strstr(buffer, "bestmove")) != NULL ){
			    	//printf("%s\n", msg_move);
			    	break;
			    }


        	}

        	/*	------------ UDP Send Phase ------------*/

        	if (sendto(s, msg_move, strlen(msg_move) , 0 , (struct sockaddr *) &si_other, slen) == -1) {
    	        printf("udp send error, restart everything\n");
    	        exit(-1);
        	}


  		}

    }

}
