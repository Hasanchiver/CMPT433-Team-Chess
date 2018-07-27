#include <stdio.h>
#include <stdint.h>

int main(){


	int r,g,b;

	r = 0xB8;
	g = 0x98;
	b = 0xFa;


	uint8_t red = (r * 8) / 256;
	uint8_t green = (g * 8) / 256;
	uint8_t blue = (b * 4) / 256;

	uint8_t all =  (red << 5) | (green << 2) | blue;

	printf("result : 0x%x\n", all);

	return 0;

}