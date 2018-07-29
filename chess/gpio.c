
#include "global.h"
#include "spi.h"
#include "lcd.h"
#include "gpio.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/**
	@param : 0 -> in, 1 -> out
*/
int gpioInit(int port, int dir, int val){

	FILE *f1 = fopen("/sys/class/gpio/export", "w");
	if(f1 == NULL){
		fprintf(stderr, "unable to open export\n");
		exit(-1);
	}

	fprintf(f1, "%d", port);
	fclose(f1);

	char s[40];
	sprintf(s, "/sys/class/gpio/gpio%d/direction", port);
	f1 = fopen( s, "w" );
	if(f1 == NULL){
		fprintf(stderr, "unable to open direction\n");
		exit(-1);
	}
	fprintf(f1, "%s", (dir == 0) ? "in" : "out");
	fclose(f1);

	sprintf(s, "/sys/class/gpio/gpio%d/value", port);
	f1 = fopen(s, "w");
	if(f1 == NULL){
		fprintf(stderr, "unable to open value\n");
		exit(-1);
	}
	fprintf(f1, "%d", (val == 0) ? 0 : 1);
	fclose(f1);

	return 0;
}


int gpio_write(int port, int val){

	char s[40];
	sprintf(s, "/sys/class/gpio/gpio%d/value", port);
	FILE *f = fopen(s, "w");
	if(f == NULL){
		fprintf(stderr, "gpio write cannot open file, port %d, val %d\n", port, val);
		exit(-1);
	}
	fprintf(f, "%d", (val == 0) ? 0 : 1);
	fclose(f);

	return 0;
}


int gpio_read(int port){

	char s[40];
	int ret;
	sprintf(s, "/sys/class/gpio/gpio%d/value", port);
	FILE *f = fopen(s, "r");
	if(f == NULL){
		fprintf(stderr, "gpio read cannot open file, port %d\n", port);
		exit(-1);
	}
	fscanf(f, "%d", &ret);
	fclose(f);
	
	return ret;
}

