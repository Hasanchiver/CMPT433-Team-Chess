
#include "global.h"
#include "spi.h"
#include "lcd.h"
#include "gpio.h"


#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

static int _width = 800;
static int _height = 480;
static struct timespec time_delay;


tsMatrix_t cali_matrix;


int board_shiftx = 160;
int board_cellSize = 60;

//NOTHING : 100
//WHITE : 0 Pawn, 1 Knight, 2 Bishop, 3 Rock, 4 King, 5 Queen
//BLACK : 10 Pawn, 11 Knight, 12 Bishop, 13 Rock, 14 King, 15 Queen
uint8_t chesspieceTable[8][8];
uint8_t board_color_table[8][8];




static void delay(int miliseconds);
int gpioInit(int port, int dir, int val);


int lcdInit(){



	// GPIO INIT

	gpioInit(RST_PIN, GPIO_OUT, 0);

	//Power cycle the LCD by writing 1, 0, 1 to its RST pin
	gpio_write(RST_PIN, 1);
	delay(10);
	gpio_write(RST_PIN, 0);
	delay(10);
	gpio_write(RST_PIN, 1);
	
	delay(1000);

	gpioInit(INT_PIN, GPIO_IN, 0);
	delay(30);




	//SPI INIT


	

	//Reading the register address 0 and wait for a 0x75 reply
	uint8_t reply = spi_read(0);

	if(reply != 0x75){
		
		printf("Wrong reply, %d\n", reply);
		exit(EXIT_FAILURE);
	}

	/*printf("pass\n");
	while(1);*/


	uint8_t pixclk          = RA8875_PCSR_PDATL | RA8875_PCSR_2CLK;
    uint8_t hsync_nondisp   = 26;
    uint8_t hsync_start     = 32;
    uint8_t hsync_pw        = 96;
    uint8_t hsync_finetune  = 0;
    uint16_t vsync_nondisp   = 32;
    uint16_t vsync_start     = 23;
    uint8_t vsync_pw        = 2;



 spi_write(RA8875_PCSR, pixclk);
  delay(1);
  
  /* Horizontal settings registers */
  spi_write(RA8875_HDWR, (_width / 8) - 1);                          // H width: (HDWR + 1) * 8 = 480
  spi_write(RA8875_HNDFTR, RA8875_HNDFTR_DE_HIGH + hsync_finetune);
  spi_write(RA8875_HNDR, (hsync_nondisp - hsync_finetune - 2)/8);    // H non-display: HNDR * 8 + HNDFTR + 2 = 10
  spi_write(RA8875_HSTR, hsync_start/8 - 1);                         // Hsync start: (HSTR + 1)*8 
  spi_write(RA8875_HPWR, RA8875_HPWR_LOW + (hsync_pw/8 - 1));        // HSync pulse width = (HPWR+1) * 8
  
  /* Vertical settings registers */
  spi_write(RA8875_VDHR0, (uint16_t)(_height - 1) & 0xFF);
  spi_write(RA8875_VDHR1, (uint16_t)(_height - 1) >> 8);
  spi_write(RA8875_VNDR0, vsync_nondisp-1);                          // V non-display period = VNDR + 1
  spi_write(RA8875_VNDR1, vsync_nondisp >> 8);
  spi_write(RA8875_VSTR0, vsync_start-1);                            // Vsync start position = VSTR + 1
  spi_write(RA8875_VSTR1, vsync_start >> 8);
  spi_write(RA8875_VPWR, RA8875_VPWR_LOW + vsync_pw - 1);            // Vsync pulse width = VPWR + 1
  
  /* Set active window X */
  spi_write(RA8875_HSAW0, 0);                                        // horizontal start point
  spi_write(RA8875_HSAW1, 0);
  spi_write(RA8875_HEAW0, (uint16_t)(_width - 1) & 0xFF);            // horizontal end point
  spi_write(RA8875_HEAW1, (uint16_t)(_width - 1) >> 8);
  
  /* Set active window Y */
  spi_write(RA8875_VSAW0, 0);                                        // vertical start point
  spi_write(RA8875_VSAW1, 0);  
  spi_write(RA8875_VEAW0, (uint16_t)(_height - 1) & 0xFF);           // horizontal end point
  spi_write(RA8875_VEAW1, (uint16_t)(_height - 1) >> 8);
  
  //setcolor65();

  
  /* Clear the entire window */
  spi_write(RA8875_MCLR, RA8875_MCLR_START | RA8875_MCLR_FULL);
  delay(500); 



	


  	// Display on
  	spi_write(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);

  	// Enable GPIOX
	spi_write(RA8875_GPIOX, 1);


	// PWM Config
	spi_write(RA8875_P1CR, RA8875_P1CR_ENABLE | (RA8875_PWM_CLK_DIV1024 & 0xF));



	setBrightness(255);



	//use 8 bit color for graphic accerleration
	setColor8();





	//Enable Calibrate Alogrithm
	initCaliMatrix();

	initChessPieceTable();

	//Enable touch panel
	touchPanelEnable(1);





	set_SPISpeed(1500000);

	printf("Initialization Success\n");


	fillScreen8( BOARD_OUTER_COLOR );

	


	selectLayer(1);
	drawChessBoard();


	set_SPISpeed(4000000);
	selectLayer(2);
	drawChessPiece();
	set_SPISpeed(1500000);



	layer_show(1);




	printf("Reach Touch Code\n");

	touchBegin();
	while(1);







/*	int cnt, shifter = 0;
	

	struct PPMImage *img = readImage("bg.ppm");

	int loc_x = 0, loc_y = 0;

	uint8_t *img_col = malloc(sizeof(uint8_t) * img->x * 2);

	
	for(int j = loc_y; j < loc_y + img->y; ++j){

		cnt = 0;
		for(int i = loc_x; i < loc_x + img->x; ++i){
				
			uint16_t col = color565( ((img->data)[cnt]).red,  ((img->data)[cnt]).green, ((img->data)[cnt]).blue  );
			img_col[cnt++] = col >> 8;
			img_col[cnt++] = col;
			
		}

		spi_transfer(img_col, sizeof(uint8_t) * 2 * img->x, loc_x + (shifter++), j);

	}


	free(img);
	free(img_col);*/















/*	struct PPMImage *img = readImage("bg.ppm");

	int cnt = 0;
	for(int j = 0; j < img->y; ++j){
		for(int i = 0; i < img->x; ++i){
				
			uint16_t col = color565( ((img->data)[cnt]).red,  ((img->data)[cnt]).green, ((img->data)[cnt]).blue  );
			++cnt;

			drawPixel(i, j , col);
		}
	}*/
	




	return 0;

}

















void touchBegin(){
	
	uint16_t x, y;

	tsPoint_t raw, cali;

	while(1){


		readTouch(&x, &y);
		delay(1);



		while(gpio_read(INT_PIN) == 1);

		if(touched() == 1){

			memset(&raw, 0, sizeof(tsPoint_t));
			readTouch(&x, &y);
			raw.x = x;
			raw.y = y;


		}else{
			raw.x = 0;
			raw.y = 0;
		}

		transformPoint( &cali , &raw);

		printf("Touch\t\tx: %d\ty: %d\n", cali.x, cali.y);
		fillCircle(cali.x, cali.y, 3, RA8875_BLACK);
		delay(500);
		

	}


}





int touched( void ){

	uint8_t ret = spi_read(RA8875_INTC2);

	if( ret &  RA8875_INTC2_TP ){
		return 1;
	}
	
	return 0;
}




void readTouch(uint16_t *x, uint16_t *y){

  uint16_t tx, ty;
  uint8_t temp;
  
  tx = spi_read(RA8875_TPXH);
  ty = spi_read(RA8875_TPYH);
  temp = spi_read(RA8875_TPXYL);
  tx <<= 2;
  ty <<= 2;
  tx |= temp & 0x03;
  ty |= (temp >> 2) & 0x03;

  *x = tx;
  *y = ty;

	spi_write(RA8875_INTC2, RA8875_INTC2_TP);
 

}




void transformPoint(tsPoint_t* displayPtr,  tsPoint_t * screenPtr){

  
  if( cali_matrix.Divider != 0 )
  {
    displayPtr->x = ( (cali_matrix.An * screenPtr->x) + 
                      (cali_matrix.Bn * screenPtr->y) + 
                       cali_matrix.Cn 
                    ) / cali_matrix.Divider ;

    displayPtr->y = ( (cali_matrix.Dn * screenPtr->x) + 
                      (cali_matrix.En * screenPtr->y) + 
                       cali_matrix.Fn 
                    ) / cali_matrix.Divider ;
  }
  else
  {
  	fprintf(stderr, "must init calibrate matrix before reading touch panel\n");
    exit(-1);
  }


}










void initCaliMatrix(){

	/*
	An : -320640
	Bn : -320
	Cn : 10675600
	Dn : -1728
	En : -218688
	Fn : 28104048
	Divide : -380423
	*/

	cali_matrix.An = -320640;
	cali_matrix.Bn = -320;
	cali_matrix.Cn = 10675600;
	cali_matrix.Dn = -1728;
	cali_matrix.En = -218688;
	cali_matrix.Fn = 28104048;
	cali_matrix.Divider = -380423;



}











void initChessPieceTable( void ){

	for(int i = 0; i < 8; ++i){

		for(int j = 0; j < 8; ++j){
			chesspieceTable[i][j] = 100;

		}

	}



	chesspieceTable[0][0] = 5;

	chesspieceTable[5][1] = 5;




}



void drawChessPiece( void ){

	for(int i = 0; i < 8; ++i){

		for(int j = 0; j < 8; ++j){


			if( chesspieceTable[i][j] == 5){
				//printf("I need to draw\n");
				drawChessPieceImage(i, j, 5);

			}





		}



	}


}














void drawChessPieceImage(int ii, int jj, int type){

	char *s;

	switch(type){
		case 5:
			s = "img/WhiteQueen.ppm";
			break;



			default:

			printf("program should not reach here\n");
			exit(-1);

	};

	struct PPMImage *img = readImage(s);

	int cnt = 0;

	int base_x = 160 + ii * 60;
	int base_y = jj *60;



	
	for(int j = base_y; j < base_y + img->y && j < 480; ++j){
		for(int i = base_x; i < base_x + img->x; ++i){

			//uint16_t col = color565( ((img->data)[cnt]).red,  ((img->data)[cnt]).green, ((img->data)[cnt]).blue  );
			uint8_t col = color332( ((img->data)[cnt]).red,  ((img->data)[cnt]).green, ((img->data)[cnt]).blue );


			if(col == SPECIAL_COLOR){
				//printf("back color\n");
				//col = (board_color_table[ii][jj] == 1) ? BOARD_CELL_COLOR_1 : BOARD_CELL_COLOR_2;
				++cnt;
				continue;
			}else{
				//printf("I am my color\n");
			}

			++cnt;
			drawPixel(i, j , col);
		}
	}







}



































































void drawChessBoard(){

	uint8_t prev_color = BOARD_CELL_COLOR_2;
	uint8_t curr_color = 0;


	for( int i = board_shiftx; i < board_shiftx + 8 * board_cellSize; i += board_cellSize){

		for(int j = 0; j < 480; j += board_cellSize){

			

			curr_color = (prev_color == BOARD_CELL_COLOR_1) ? BOARD_CELL_COLOR_2 : BOARD_CELL_COLOR_1;

			rectHelper8(i, j, i + board_cellSize, j + board_cellSize, curr_color, 1);
			

			prev_color = curr_color;

		}


		prev_color = (prev_color == BOARD_CELL_COLOR_2) ? BOARD_CELL_COLOR_1 : BOARD_CELL_COLOR_2;

	}

	int curr_val = 2;

	for(int i = 0; i < 8; ++i){
		for(int j = 0; j < 8; ++j){
			
			if(curr_val == 1){
				curr_val = 2;
			}else{
				curr_val = 1;
			}
			board_color_table[i][j] = curr_val;

		}

		if(curr_val == 1){
				curr_val = 2;
			}else{
				curr_val = 1;
			}

	}


	/*for(int i = 0; i < 8; ++i){
			for(int j = 0; j < 8; ++j){
				printf("%d\t", board_color_table[j][i]);
			}
			printf("\n");
		}
	printf("\n");*/






}




























void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color){
	circleHelper(x0, y0, r, color, 1);
}	

void circleHelper(int16_t x0, int16_t y0, int16_t r, uint16_t color, int filled){

  /* Set X */
  writeCommand(0x99);
  writeData(x0);
  writeCommand(0x9a);
  writeData(x0 >> 8);
  
  /* Set Y */
  writeCommand(0x9b);
  writeData(y0); 
  writeCommand(0x9c);	   
  writeData(y0 >> 8);
  
  /* Set Radius */
  writeCommand(0x9d);
  writeData(r);  
  
  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));
  
  /* Draw! */
  writeCommand(RA8875_DCR);
  if (filled)
  {
    writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_FILL);
  }
  else
  {
    writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_NOFILL);
  }
  
  /* Wait for the command to finish */
  waitPoll(RA8875_DCR, RA8875_DCR_CIRCLE_STATUS);





}







void setBrightness(uint8_t bright){

	spi_write(RA8875_P1DCR, bright);

}





void setColor8( void ){
	uint8_t ret = spi_read(0x10);
	ret &= ~(0xC);
	spi_write(0x10, ret);
}




void fillScreen8(uint8_t color){
	rectHelper8(0, 0, _width-1, _height-1, color, 1);
}




void rectHelper8(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color, int filled)
{
  /* Set X */
  writeCommand(0x91);
  writeData(x);
  writeCommand(0x92);
  writeData(x >> 8);
  
  /* Set Y */
  writeCommand(0x93);
  writeData(y); 
  writeCommand(0x94);	   
  writeData(y >> 8);
  
  /* Set X1 */
  writeCommand(0x95);
  writeData(w);
  writeCommand(0x96);
  writeData((w) >> 8);
  
  /* Set Y1 */
  writeCommand(0x97);
  writeData(h); 
  writeCommand(0x98);
  writeData((h) >> 8);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xE0) >> 5);
  writeCommand(0x64);
  writeData((color & 0x1C) >> 2);
  writeCommand(0x65);
  writeData((color & 0x03));



  /* Draw! */
  writeCommand(RA8875_DCR);
  if (filled)
  {
    writeData(0xB0);
  }
  else
  {
    writeData(0x90);
  }
  
  
  waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);


}




uint8_t color332(int R, int G, int B){


	uint8_t red = (R * 8) / 256;
	uint8_t green = (G * 8) / 256;
	uint8_t blue = (B * 4) / 256;


	return (red << 5) | (green << 2) | blue;

}























































void fillScreen(uint16_t color){
	rectHelper(0, 0, _width-1, _height-1, color, 1);
}




void rectHelper(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, int filled)
{
  /* Set X */
  writeCommand(0x91);
  writeData(x);
  writeCommand(0x92);
  writeData(x >> 8);
  
  /* Set Y */
  writeCommand(0x93);
  writeData(y); 
  writeCommand(0x94);	   
  writeData(y >> 8);
  
  /* Set X1 */
  writeCommand(0x95);
  writeData(w);
  writeCommand(0x96);
  writeData((w) >> 8);
  
  /* Set Y1 */
  writeCommand(0x97);
  writeData(h); 
  writeCommand(0x98);
  writeData((h) >> 8);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));



  /* Draw! */
  writeCommand(RA8875_DCR);
  if (filled)
  {
    writeData(0xB0);
  }
  else
  {
    writeData(0x90);
  }
  
  
  waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);


}



int waitPoll(uint8_t regname, uint8_t waitflag) {

	while (1)
  	{
    	uint8_t temp = spi_read(regname);
    	if (!(temp & waitflag)){
      		return 1;
    	}
  }  
  

  return 0;

}

void graphicsMode(void) {

	
  	uint8_t ret = spi_read(RA8875_MWCR0);
	ret &= ~RA8875_MWCR0_TXTMODE;
  	spi_write(RA8875_MWCR0, ret);


}





















void textMode( void ){

  uint8_t ret = spi_read(RA8875_MWCR0);


  ret |= RA8875_MWCR0_TXTMODE;
  spi_write( RA8875_MWCR0, ret );
  

  /* Select the internal (ROM) font */
  
  ret = spi_read(0x21);

  ret &= ~((1<<7) | (1<<5)); // Clear bits 7 and 5
  spi_write(0x21, ret);

}




void setcolor65( void ){

	uint8_t ret = spi_read(0x10);
	spi_write(0x10, ret | 0xC );

}























static void delay(int miliseconds){
	time_delay.tv_sec = 0;
	time_delay.tv_nsec = miliseconds * 1000000L;

	nanosleep(&time_delay, NULL);
}

void microdelay(int microseconds){
	time_delay.tv_sec = 0;
	time_delay.tv_nsec = microseconds * 1000L;

	nanosleep(&time_delay, NULL);
}













void drawPixel(int16_t x, int16_t y, uint16_t color){

  spi_write(RA8875_CURH0, x);
  spi_write(RA8875_CURH1, x >> 8);
  spi_write(RA8875_CURV0, y);
  spi_write(RA8875_CURV1, y >> 8);  

  writeCommand(RA8875_MRWC);


  uint8_t tx[2] = {RA8875_DATAWRITE, color};

  spi_transfer(tx, 2);



}






/*
struct ppm_image* readImage(char *filename){


	struct ppm_image *img = malloc(sizeof(struct ppm_image));

	FILE *f = fopen(filename, "r");

	if(f == NULL){
		return 0;
	}
	uint8_t buff[255];

	fscanf(f, "%s\n", buff);		//P6

	int x;

	fscanf(f, "%d", &x);
	img->width = x;

	fscanf(f, "%d", &x);
	img->height = x;


	fscanf(f, "%d", &x);		//depth 255

	uint8_t y;


	img->data = malloc(img->width * img->height);

	for(int i = 0; i < img->width * img->height; ++i){

		fscanf(f, "%c", &y);
		(img->data)[i] = (uint8_t)y;

	}


	return img;


}
*/




uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}









struct PPMImage *readImage(char *filename)
{
         char buff[16];
         struct PPMImage *img;
         FILE *fp;
         int c, rgb_comp_color;


         //open PPM file for reading
         fp = fopen(filename, "rb");
         if (!fp) {
              fprintf(stderr, "Unable to open file '%s'\n", filename);
              exit(1);
         }

         //read image format
         if (!fgets(buff, sizeof(buff), fp)) {
              perror(filename);
              exit(1);
         }

    //check the image format
    if (buff[0] != 'P' || buff[1] != '6') {
         fprintf(stderr, "Invalid image format (must be 'P6')\n");
         exit(1);
    }

    //alloc memory form image
    img = (struct PPMImage *)malloc(sizeof(struct PPMImage));
    if (!img) {
         fprintf(stderr, "Unable to allocate memory\n");
         exit(1);
    }

    //check for comments
    c = getc(fp);
    while (c == '#') {
    	while (getc(fp) != '\n');
        c = getc(fp);
    }

    ungetc(c, fp);
    //read image size information
    if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {
         fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
         exit(1);
    }

    //read rgb component
    if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
         fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
         exit(1);
    }

    //check rgb component depth
    if (rgb_comp_color!= 255) {
         fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
         exit(1);
    }

    while (fgetc(fp) != '\n') ;
    //memory allocation for pixel data
    img->data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));

    if (!img) {
         fprintf(stderr, "Unable to allocate memory\n");
         exit(1);
    }

    //read pixel data from file
    if (fread(img->data, 3 * img->x, img->y, fp) != img->y) {
         fprintf(stderr, "Error loading image '%s'\n", filename);
         exit(1);
    }

    fclose(fp);
    return img;
}






































void touchPanelEnable(int i){



	uint8_t   adcClk = (uint8_t) RA8875_TPCR0_ADCCLK_DIV16;


	if (i == 1)
	{
		/* Enable Touch Panel (Reg 0x70) */
		spi_write(RA8875_TPCR0, RA8875_TPCR0_ENABLE        |
		                       RA8875_TPCR0_WAIT_4096CLK  |
		                       RA8875_TPCR0_WAKEENABLE   |
		                       adcClk); 

		/* Set Auto Mode      (Reg 0x71) */
		spi_write(RA8875_TPCR1, RA8875_TPCR1_AUTO    |
		                       // RA8875_TPCR1_VREFEXT |
		                       RA8875_TPCR1_DEBOUNCE);
		/* Enable TP INT */
		uint8_t ret = spi_read(RA8875_INTC1);
		spi_write(RA8875_INTC1, ret | RA8875_INTC1_TP);



	} else {

		/* Disable TP INT */
		uint8_t ret1 = spi_read(RA8875_INTC1);
		spi_write(RA8875_INTC1, ret1 & (~RA8875_INTC1_TP) );
		/* Disable Touch Panel (Reg 0x70) */
		spi_write(RA8875_TPCR0, RA8875_TPCR0_DISABLE);
	}

}


































void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color){


	writeCommand(0x91);
  writeData(x0);
  writeCommand(0x92);
  writeData(x0 >> 8);
  
  /* Set Y */
  writeCommand(0x93);
  writeData(y0); 
  writeCommand(0x94);
  writeData(y0 >> 8);
  
  /* Set X1 */
  writeCommand(0x95);
  writeData(x1);
  writeCommand(0x96);
  writeData((x1) >> 8);
  
  /* Set Y1 */
  writeCommand(0x97);
  writeData(y1); 
  writeCommand(0x98);
  writeData((y1) >> 8);
  
  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(RA8875_DCR);
  writeData(0x80);
  
  
  waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);


}





































































void bmpDraw(char *filename, int x, int y) {

/*
  FILE     *bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  _BOOL  goodBmp = false;       // Set to true on valid header parse
  _BOOL  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  uint8_t  lcdidx = 0;
  _BOOL  first = true;

  if((x >= _width()) || (y >= _height())) return;


  // Open requested file on SD card
  if ((bmpFile = fopen(filename) == NULL) {
  	fprintf(stderr, "no image file error\n");
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.println(F("File size: ")); 
    Serial.println(read32(bmpFile));

    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print(F("Image Offset: ")); 
    Serial.println(bmpImageoffset, DEC);

    // Read DIB header
    Serial.print(F("Header size: ")); 
    Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);

    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); 
      Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed
        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds

        for (row=0; row<h; row++) { // For each scanline...
          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
          pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }









          for (col=0; col<w; col++) { // For each column...


            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              // Push LCD buffer to the display first
              if(lcdidx > 0) {
                tft.drawPixel(col+x, row+y, lcdbuffer[lcdidx]);
                lcdidx = 0;
                first  = false;
              }

              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            lcdbuffer[lcdidx] = color565(r,g,b);
            tft.drawPixel(col+x, row+y, lcdbuffer[lcdidx]);
          } // end pixel





        } // end scanline




        // Write any remaining data to LCD
        if(lcdidx > 0) {
          tft.drawPixel(col+x, row+y, lcdbuffer[lcdidx]);
        } 



        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");

      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println(F("BMP format not recognized."));*/

}































//Layer Conctrol ----------------------------------

void selectLayer(int i){


	uint8_t ret = spi_read(0x41);

	if(i == 1){
		ret &= ~1;
	}else{
		ret |= 1;
	}

	spi_write(0x41, ret);

}


// 0 only layer 1, 1 only layer 2, 2 both
void layer_show(int mode){

	uint8_t ret = spi_read(0x52);

	ret &= ~(0x7);

	if( mode == 0 ){
		

	}else if(mode == 1){
		ret |= 0x1;
	}else if(mode == 2){
		ret |= 0x4;
	}
	spi_write(0x52, ret);

}













































