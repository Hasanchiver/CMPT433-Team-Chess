
/*
	WHITE : 0 Pawn, 1 Knight, 2 Bishop, 3 Rock, 4 King, 5 Queen
	BLACK : 10 Pawn, 11 Knight, 12 Bishop, 13 Rock, 14 King, 15 Queen
*/


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
#include <math.h>


static int _width = 800;
static int _height = 480;
static struct timespec time_delay;

tsMatrix_t cali_matrix;

int board_shiftx = 160;
int board_cellSize = 60;


uint8_t chesspieceTable[8][8];
uint8_t board_color_table[8][8];




/*--------------------------------------- Public API ---------------------------------------*/

int lcdInit(){

	/* GPIO INIT */
	gpioInit(RST_PIN, GPIO_OUT, 0);

	//Power cycle LCD
	gpio_write(RST_PIN, 1);
	delay(10);
	gpio_write(RST_PIN, 0);
	delay(10);
	gpio_write(RST_PIN, 1);
	delay(500);

	gpioInit(INT_PIN, GPIO_IN, 0);
	delay(30);



	/* SPI INIT */

	//Reading STSR and wait for a 0x75 reply
	uint8_t reply = spi_read(0);

	if(reply != 0x75){
		
		printf("Initialize failed, wrong reply, check SPI setting\n");
		exit(EXIT_FAILURE);
	}	

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

	/* Clear the entire window */
	spi_write(RA8875_MCLR, RA8875_MCLR_START | RA8875_MCLR_FULL);
	delay(500); 

  	// Display on
  	spi_write(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);

  	// Enable GPIOX
	spi_write(RA8875_GPIOX, 1);

	// PWM Config
	spi_write(RA8875_P1CR, RA8875_P1CR_ENABLE | (RA8875_PWM_CLK_DIV1024 & 0xF));
	spi_write(0x8c, RA8875_P2CR_ENABLE | (RA8875_PWM_CLK_DIV1024 & 0xF));

	// Brightness
	setBrightness(255);

	// Enable touch panel
	touchPanelEnable(1);

	// Enable 2 layers
	spi_write(0x20, spi_read(0x20) | 0x80);


	//Layer 1 trans color set
	spi_write(0x67, (TRANS_COLOR & 0xE0) >> 5 );
	spi_write(0x68, (TRANS_COLOR & 0x1C) >> 2 );
	spi_write(0x69, (TRANS_COLOR & 0x03) );

	printf("LCD initialization success\n");


	/* Game logic set up */

	setColor8();

	initCaliMatrix();

	initChessPieceTable();

	initChessBoardColorTable();


	/* Post action*/
	set_SPISpeed(4000000);
	



	/*--------------------------- TESTING ---------------------------*/
	
	/*--------------------------- SCENE TESTING ---------------------------*/
	//loadNewGameScene();
	//loadRestartScene();


	/*--------------------------- MOVE TEST ---------------------------*/
	//chesspiece_move(1, 0, 3, 0, 0);
	//chesspiece_move(7, 6, 3, 3, 11);


	/*--------------------------- TOUCH SCREEN TEST ---------------------------*/
	
	/*int mx, my;
	int rett;

	while(1){

		rett = touchPanelListen(&mx, &my);
		printf("return %d\n", rett);

		if(rett == 1){
			printf("row: %d, col : %d\n", mx, my);
		}
		delay(500);
	}*/
	


	/*--------------------------- TEST END ---------------------------*/
	
	//while(1);

	return 0;
}









void loadNewGameScene( void ){

	/* Pre Condition */

	// Fill trans color
	selectLayer(1);
	c8_drawRect(0, 0, _width - 1, _height - 1, TRANS_COLOR, 1);

	// Set layer 2 background
	c8_setBackgroundColour(2, BOARD_BACKGROUND_COLOR);


	/* Load Elements */

	paintChessBoard();

	paintInitChessPiece();



	/* Load Background Control Elements */
	//ADD-TO...



	// Show 
	setLayerDisplayMode(2);

}


void loadRestartScene( char *msg ){

	// Background Render
	setGraphicsMode();
	c8_setBackgroundColour( 2, RESTART_SCENE_BACKGROUND_COLOR );


	
	// BTN Draw
	//c8_drawRect();


	//setTextMode();



	// Text Draw

	// Show
	setLayerDisplayMode(0);

	// Post
	setGraphicsMode();
}















































/**
	@param: row -> if return 1, row set to the row user click on board
			col -> if return 1, col set to the row user click on board

	@return: 	0 -> user click outside of board, and not on control btn
				1 -> user click on the board
				2 -> user click a btn on background(not implement yet)


				-1 -> error, should ignore it and recall the function

	@note: 	- MUST HAVE A PROPER DELAY BETWEEN TWO CALLS, SUGGEST 500 ms
			-it is blocked call
*/
int gameboard_touchPanelListen( int *row, int *col ){

	uint16_t x, y;
	tsPoint_t raw, cali;

	
	set_SPISpeed(1500000);

	// Clear the INT pending 
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

	if(cali.x < 160 || cali.x > 640){

		if( 0 /* Check the btn click here */ ){

			set_SPISpeed(4000000);
			return 2;	
		}
		set_SPISpeed(4000000);
		return 0;
	}

	int rx = 0, ry = 0;

	printf("cali -----------------------------%d %d\n", cali.x, cali.y);

	rx = ceil((cali.x - 160) / 60.0);
	ry = ceil(cali.y / 60.0);

	*row = rx - 1;
	*col = ry - 1;

	set_SPISpeed(4000000);
	
	return 1;
}



void selectChessPiece( uint8_t table[8][8] ){
	paintEligibleMove(table);
}


void deselectChessPiece( void ){
	paintChessBoard();
}


void moveChessPiece(int src_row, int src_col, int dst_row, int dst_col, int type){

	clearChessPieceLoc(src_row, src_col);
	clearChessPieceLoc(dst_row, dst_col);
	updateChessPieceLoc(dst_row, dst_col, type);

	paintChessBoard();	//This is extremely fast, no worry

	selectLayer(2);
	c8_drawRect( src_row * 60 + 160, src_col * 60, src_row * 60 + 60 + 160, src_col * 60 + 60, MOVED_COLOR, 1 );
	c8_drawRect( dst_row * 60 + 160, dst_col * 60, dst_row * 60 + 60 + 160, dst_col * 60 + 60, MOVED_COLOR, 1 );

}














int restart_touchPanelListen( void ){






	return 0;
}







































































/*
	@note: always blue side(user) move first, do not ask me why
*/
void paintInitChessPiece( void ){

	selectLayer(1);

	for(int i = 0; i < 8; ++i){

		for(int j = 0; j < 8; ++j){

			if( chesspieceTable[i][j] != 100 ){
				drawChessPieceImage(i, j, chesspieceTable[i][j], 1);
			}
		}
	}
}


void updateChessPieceLoc(int row, int col, int type) {

	selectLayer(1);
	drawChessPieceImage(row, col, type, 1);
}

void clearChessPieceLoc(int row, int col){

	selectLayer(1);
	c8_drawRect(row * 60 + 160, col * 60, (row + 1) * 60 + 160, (col + 1) * 60, TRANS_COLOR, 1);

}

void paintEligibleMove(uint8_t table[8][8]){

	selectLayer(2);

	for(int i = 0; i < 8; ++i){

		for(int j = 0; j < 8; ++j){

			if( table[i][j] != 0 ){
				c8_drawRect(i * 60 + 160, j * 60, (i + 1) * 60 + 160, (j + 1) * 60, ELIGIBLE_MOVE_COLOR, 1);
			}
		}
	}
}






























































































































































































/* ---------------------Layer Control ---------------------*/

void selectLayer(int i){

	spi_write(0x41, (i == 1) ? (spi_read(0x41) & ~1) : (spi_read(0x41) | 1) );
}

void setLayerDisplayMode(int mode){

	uint8_t ret = spi_read(0x52) & (~(0x7));

	if( mode == 0 ){			//	Only layer 1 visible

	}else if(mode == 1){		//	Only layer 2 visible
		ret |= 0x1;
	}else if(mode == 2){		//	Both visible, with transparent
		ret |= 0x3;
	}

	spi_write(0x52, ret);
}

void setLayerTransparency(int layer, uint8_t val){

	spi_write(0x53, (layer == 1) ? ((spi_read(0x53) & (~0x0F)) | val)
								:	(((spi_read(0x53) & (~0xF0)) | val) << 4) );
}



/* ---------------------Mode Control ---------------------*/

void setGraphicsMode(void) {

  	spi_write(RA8875_MWCR0, spi_read(RA8875_MWCR0) & (~RA8875_MWCR0_TXTMODE));
}

void setTextMode( void ){

  spi_write( RA8875_MWCR0, spi_read(RA8875_MWCR0) | RA8875_MWCR0_TXTMODE );
  spi_write(0x21, spi_read(0x21) & ~((1<<7) | (1<<5)) );
}



/* ---------------------LCD Setting ---------------------*/

void c8_setBackgroundColour( int layer, uint8_t color ){

	selectLayer(layer);
	c8_drawRect(0, 0, _width - 1, _height - 1, color, 1);
}

void setBrightness(uint8_t bright){

	spi_write(RA8875_P1DCR, bright);
	spi_write(RA8875_P2DCR, bright);
}


void textSetCursor(uint16_t x, uint16_t y) 
{
  
  writeCommand(0x2A);
  writeData(x & 0xFF);
  writeCommand(0x2B);
  writeData(x >> 8);
  writeCommand(0x2C);
  writeData(y & 0xFF);
  writeCommand(0x2D);
  writeData(y >> 8);
}

void textTransparent(uint8_t foreColor)
{
  /* Set Fore Color */
/*  writeCommand(0x63);
  writeData((foreColor & 0xE0) >> 5);
  writeCommand(0x64);
  writeData((foreColor & 0x1C) >> 2);
  writeCommand(0x65);
  writeData((foreColor & 0x3));

  // Set transparency flag
  writeCommand(0x22);
  uint8_t temp = readData();
  temp |= (1<<6);

  writeData(temp);  */
}


void textWrite(char* buffer) 
{

/*	uint32_t len = strlen(buffer);
		
	writeCommand(RA8875_MRWC);

	for ( uint16_t i = 0; i < len; ++i) {
		
		writeData(buffer[i]);
		delay(1);
  	}*/
}













/* ---------------------Timing Control ---------------------*/

void delay(int miliseconds){
	time_delay.tv_sec = 0;
	time_delay.tv_nsec = miliseconds * 1000000L;

	nanosleep(&time_delay, NULL);
}

void microdelay(int microseconds){
	time_delay.tv_sec = 0;
	time_delay.tv_nsec = microseconds * 1000L;

	nanosleep(&time_delay, NULL);
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



/* --------------------- Game Setting ---------------------*/

void initChessBoardColorTable(void){

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
}


void initCaliMatrix(){

	// Data come from experiments on Arduino 

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

	chesspieceTable[0][0] = 3;
	chesspieceTable[0][1] = 1;
	chesspieceTable[0][2] = 2;
	chesspieceTable[0][3] = 5;
	chesspieceTable[0][4] = 4;
	chesspieceTable[0][5] = 2;
	chesspieceTable[0][6] = 1;
	chesspieceTable[0][7] = 3;
	
	chesspieceTable[1][0] = 0;
	chesspieceTable[1][1] = 0;
	chesspieceTable[1][2] = 0;
	chesspieceTable[1][3] = 0;
	chesspieceTable[1][4] = 0;
	chesspieceTable[1][5] = 0;
	chesspieceTable[1][6] = 0;
	chesspieceTable[1][7] = 0;

	chesspieceTable[6][0] = 10;
	chesspieceTable[6][1] = 10;
	chesspieceTable[6][2] = 10;
	chesspieceTable[6][3] = 10;
	chesspieceTable[6][4] = 10;
	chesspieceTable[6][5] = 10;
	chesspieceTable[6][6] = 10;
	chesspieceTable[6][7] = 10;
	
	chesspieceTable[7][0] = 13;
	chesspieceTable[7][1] = 11;
	chesspieceTable[7][2] = 12;
	chesspieceTable[7][3] = 15;
	chesspieceTable[7][4] = 14;
	chesspieceTable[7][5] = 12;
	chesspieceTable[7][6] = 11;
	chesspieceTable[7][7] = 13;
}


struct PPMImage *readImage(char *filename) {

         char buff[16];
         struct PPMImage *img;
         FILE *fp;
         int c, rgb_comp_color;

         fp = fopen(filename, "rb");
         if (!fp) {
              fprintf(stderr, "Unable to open file '%s'\n", filename);
              exit(1);
         }

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



void setColor8( void ){

	spi_write(0x10, (spi_read(0x10) & ~(0xC)));
}









/* --------------------- Utils ---------------------*/

uint8_t color332(int R, int G, int B){

	uint8_t red = (R * 8) / 256;
	uint8_t green = (G * 8) / 256;
	uint8_t blue = (B * 4) / 256;

	return (red << 5) | (green << 2) | blue;
}



void transformPoint(tsPoint_t* displayPtr,  tsPoint_t * screenPtr){

  if( cali_matrix.Divider != 0 ) {
    displayPtr->x = ( (cali_matrix.An * screenPtr->x) + 
                      (cali_matrix.Bn * screenPtr->y) + 
                       cali_matrix.Cn 
                    ) / cali_matrix.Divider ;

    displayPtr->y = ( (cali_matrix.Dn * screenPtr->x) + 
                      (cali_matrix.En * screenPtr->y) + 
                       cali_matrix.Fn 
                    ) / cali_matrix.Divider ;
  }
  else{
  	fprintf(stderr, "must init calibrate matrix before reading touch panel\n");
    exit(-1);
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

	// Clear the INT bit
	spi_write(RA8875_INTC2, RA8875_INTC2_TP);
}



void drawChessPieceImage(int ii, int jj, int type, int layer){

	selectLayer(1);

	char *s;
	int white = 0;

	switch(type){

		case 0:
			s = "img/WhitePawn.ppm";
			white = 1;
			break;
		case 1:
			s = "img/WhiteKnight.ppm";
			white = 1;
			break;
		case 2:
			s = "img/WhiteBishop.ppm";
			white = 1;
			break;
		case 3:
			s = "img/WhiteRook.ppm";
			white = 1;
			break;
		case 4:
			s = "img/WhiteKing.ppm";
			white = 1;
			break;
		case 5:
			s = "img/WhiteQueen.ppm";
			white = 1;
			break;
		case 10:
			s = "img/BlackPawn.ppm";
			break;
		case 11:
			s = "img/BlackKnight.ppm";
			break;
		case 12:
			s = "img/BlackBishop.ppm";
			break;
		case 13:
			s = "img/BlackRook.ppm";
			break;
		case 14:
			s = "img/BlackKing.ppm";
			break;
		case 15:
			s = "img/BlackQueen.ppm";
			break;
		default:
			printf("program should not reach here\n");
			exit(-1);
	};

	struct PPMImage *img = readImage(s);

	int cnt = 0;

	int base_x = 160 + ii * 60;
	int base_y = jj *60;

	//uint8_t line[60];
	
	for(int j = base_y; j < base_y + img->y && j < 480; ++j){
		for(int i = base_x; i < base_x + img->x; ++i){

			uint8_t col = color332( ((img->data)[cnt]).red,  ((img->data)[cnt]).green, ((img->data)[cnt]).blue );

			if(col == TRANS_COLOR){
				++cnt;
				continue;
			}else{
				
				if(white == 1){
					col = CHESS_COLOR1;
				}else{
					col = CHESS_COLOR2;
				}

			}

			++cnt;
			drawPixel(i, j , col);
		}
	}
}














/* --------------------- Drawing Tools ---------------------*/

void c8_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color, int filled){

	//printf("%d %d %d %d\n",x, y, w, h);
	//x += 60;

	writeCommand(0x91);
	writeData(x);
	writeCommand(0x92);
	writeData(x >> 8);

	writeCommand(0x93);
	writeData(y); 
	writeCommand(0x94);	   
	writeData(y >> 8);

	writeCommand(0x95);
	writeData(w);
	writeCommand(0x96);
	writeData((w) >> 8);

	writeCommand(0x97);
	writeData(h); 
	writeCommand(0x98);
	writeData((h) >> 8);

	writeCommand(0x63);
	writeData((color & 0xE0) >> 5);
	writeCommand(0x64);
	writeData((color & 0x1C) >> 2);
	writeCommand(0x65);
	writeData((color & 0x03));

	writeCommand(RA8875_DCR);
	writeData( (filled) ? 0xB0 : 0x90);

	waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

void drawPixel(int16_t x, int16_t y, uint8_t color){

  spi_write(RA8875_CURH0, x);
  spi_write(RA8875_CURH1, x >> 8);
  spi_write(RA8875_CURV0, y);
  spi_write(RA8875_CURV1, y >> 8);  

  writeCommand(RA8875_MRWC);
  
  uint8_t tx[2] = {RA8875_DATAWRITE, color};

  spi_transfer(tx, 2);
}


/* --------------------- Game Controls ---------------------*/
void paintChessBoard(){

	selectLayer(2);

	uint8_t prev_color = BOARD_CELL_COLOR_2;
	uint8_t curr_color = 0;

	for( int i = board_shiftx; i < board_shiftx + 8 * board_cellSize; i += board_cellSize){

		for(int j = 0; j < 480; j += board_cellSize){

			curr_color = (prev_color == BOARD_CELL_COLOR_1) ? BOARD_CELL_COLOR_2 : BOARD_CELL_COLOR_1;
			c8_drawRect(i, j, i + board_cellSize, j + board_cellSize, curr_color, 1);
			prev_color = curr_color;
		}

		prev_color = (prev_color == BOARD_CELL_COLOR_2) ? BOARD_CELL_COLOR_1 : BOARD_CELL_COLOR_2;
	}
}

void touchPanelEnable(int i){

	uint8_t   adcClk = (uint8_t) RA8875_TPCR0_ADCCLK_DIV16;
	if (i == 1) {
		spi_write(RA8875_TPCR0, RA8875_TPCR0_ENABLE        |
		                       RA8875_TPCR0_WAIT_4096CLK  |
		                       RA8875_TPCR0_WAKEENABLE   |
		                       adcClk); 

		spi_write(RA8875_TPCR1, RA8875_TPCR1_AUTO    |
		                       RA8875_TPCR1_DEBOUNCE);

		uint8_t ret = spi_read(RA8875_INTC1);
		spi_write(RA8875_INTC1, ret | RA8875_INTC1_TP);

	} else {

		uint8_t ret1 = spi_read(RA8875_INTC1);
		spi_write(RA8875_INTC1, ret1 & (~RA8875_INTC1_TP) );
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





























