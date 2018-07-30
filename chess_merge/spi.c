
#include "spi.h"
#include "global.h"
#include "lcd.h"
#include "gpio.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>


#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


int spi_fd = 0;
int spi_speed = SPI_DEFAULT_SPEED;
int spi_mode = SPI_DEFAULT_MODE;
int spi_wordbit = SPI_DEFAULT_WORDBIT;

char *device = SPI_DEFAULT_DEVICE;

int spiInit(){

	/* LINUX environment set up*/
	//system("echo BB-SPIDEV0 > /sys/devices/platform/bone_capemgr/slots");
	//system("echo cape_enable=bone_capemgr.enable_partno=BB-SPIDEV0 >> /boot/uEnv.txt");


	/* SPI set up*/
	int ret = 0;

	spi_fd = open(device, O_RDWR);
	if (spi_fd < 0){

		fprintf(stderr, "cannot open the device, exit\n");
		exit(EXIT_FAILURE);
	}


	ret = ioctl(spi_fd, SPI_IOC_WR_MODE, &spi_mode);
	if (ret == -1){

		fprintf(stderr, "cannot set 1, exit\n");
		exit(EXIT_FAILURE);
	}

	ret = ioctl(spi_fd, SPI_IOC_RD_MODE, &spi_mode);
	if (ret == -1){
		fprintf(stderr, "cannot read 1, exit\n");
		exit(EXIT_FAILURE);
	}
		

	ret = ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_wordbit);
	if (ret == -1){
		fprintf(stderr, "cannot set 2, exit\n");
		exit(EXIT_FAILURE);
	}
		

	ret = ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &spi_wordbit);
	if (ret == -1){
		fprintf(stderr, "cannot read 2, exit\n");
		exit(EXIT_FAILURE);
	}
		

	ret = ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
	if (ret == -1){
		fprintf(stderr, "cannot set 3, exit\n");
		exit(EXIT_FAILURE);
	}
		

	ret = ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
	if (ret == -1){
		fprintf(stderr, "cannot read 3, exit\n");
		exit(EXIT_FAILURE);
	}


	/* Report */
	printf("SPI MODE:\t\t%d\n", spi_mode);
	printf("SPI WORD:\t\t%d\n", spi_wordbit);
	printf("SPI SPEED:\t\t%d KHz\n", spi_speed / 1000);
	printf("SPI inititialization success\n");

    return 0;
	
}


int spi_write(uint8_t reg, uint8_t data){

	writeCommand(reg);
	writeData(data);

	return 0;
}



uint8_t spi_read(uint8_t reg){

	int ret;
	uint8_t tx[] = {
		RA8875_CMDWRITE, reg, RA8875_DATAREAD, 0x00
		
	};

	uint8_t rx[ARRAY_SIZE(tx)] = {0, };

	struct spi_ioc_transfer trans = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.delay_usecs = 0,
		.speed_hz = spi_speed,
		.bits_per_word = spi_wordbit,
	};

	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &trans);

	if (ret < 1){
		fprintf(stderr, "read_reg : send spi message error\n");
		exit(EXIT_FAILURE);
	}
		

	return rx[ARRAY_SIZE(tx) - 1];

}



void spi_transfer(uint8_t* data, int size){

	int ret;
	
	uint8_t rx[size];

	struct spi_ioc_transfer trans = {
		.tx_buf = (unsigned long)data,
		.rx_buf = (unsigned long)rx,
		.len = size,
		.delay_usecs = 0,
		.speed_hz = spi_speed,
		.bits_per_word = spi_wordbit,
	};

	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &trans);

	if (ret < 1){
		fprintf(stderr, "block transfer : send spi message error\n");
		exit(EXIT_FAILURE);
	}

}



void writeCommand(uint8_t d){

	int ret;
	uint8_t tx[] = {
		RA8875_CMDWRITE, d
		
	};
	uint8_t rx[ARRAY_SIZE(tx)] = {0, };

	struct spi_ioc_transfer trans = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.delay_usecs = 0,
		.speed_hz = spi_speed,
		.bits_per_word = spi_wordbit,
	};

	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &trans);

	if (ret < 1){
		fprintf(stderr, "write_cmd : send spi message error, cmd : %d\n", d);
		exit(EXIT_FAILURE);
	}

}


void writeData(uint8_t d){

	int ret;
	uint8_t tx[] = {
		RA8875_DATAWRITE, d
	};

	uint8_t rx[ARRAY_SIZE(tx)] = {0, };

	struct spi_ioc_transfer trans = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.delay_usecs = 0,
		.speed_hz = spi_speed,
		.bits_per_word = spi_wordbit,
	};

	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &trans);

	if (ret < 1){
		fprintf(stderr, "write_data : send spi message error, data : %d\n", d);
		exit(EXIT_FAILURE);
	}
}


void set_SPISpeed( int speed ){
	spi_speed = speed;
}

