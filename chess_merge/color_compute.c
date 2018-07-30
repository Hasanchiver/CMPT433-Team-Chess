#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


int main(int argc, char *argv[]){

	int r,g,b;


	r = 244;
	g = 167;
	b = 66;


	uint8_t red = (r * 8) / 256;
	uint8_t green = (g * 8) / 256;
	uint8_t blue = (b * 4) / 256;

	uint8_t all =  (red << 5) | (green << 2) | blue;

	printf("result : 0x%x\n", all);

	return 0;

}