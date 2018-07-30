
#include "spi.h"
//#include "global.h"

#include <stdio.h>
#include <stdlib.h>


int proc_arg(int argc, char* argv[]);
int spiInit();
int lcdInit();
int gpioInit(int port, int dir, int val);




int dd_main(int argc, char *argv[]){

	proc_arg(argc, argv);		//Not implemented yet


	if( spiInit() < 0 ){
		fprintf(stderr, "%s", "sys_err: SPI environment setting failure\n\nexit\n");
		exit(EXIT_FAILURE);
	}



	if( lcdInit() < 0 ){
		fprintf(stderr, "%s", "sys_err: LCD initializing failure\n\nexit\n");
		exit(EXIT_FAILURE);
	}



	return 0;
}


int proc_arg(int argc, char* argv[]){

	return 0;
}