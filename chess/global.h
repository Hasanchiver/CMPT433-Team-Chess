
#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "stdint.h"




typedef struct {
     unsigned char red,green,blue;
} PPMPixel;




struct PPMImage{

	int x;
	int y;
	PPMPixel *data;

};

#endif