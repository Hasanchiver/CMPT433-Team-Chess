
#ifndef _SPI_H
#define _SPI_H

#include <stdint.h>



#define SPI_DEFAULT_SPEED											300000		// 300 kHz
#define SPI_DEFAULT_MODE											0
#define SPI_DEFAULT_WORDBIT											8
#define SPI_DEFAULT_DEVICE											"/dev/spidev1.0"




#define SPI_CS														22








extern int spi_fd;

int spi_write(uint8_t reg, uint8_t data);
uint8_t spi_read(uint8_t reg);
void set_SPISpeed( int speed );


void writeCommand(uint8_t d);
void writeData(uint8_t d);


void spi_transfer(uint8_t* data, int size);




void setWordBit(int i );





#endif

