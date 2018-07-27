
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


//uint8_t *tx;






int spiInit(){

	//Setting up the linux SPI environment
	//system("echo BB-SPIDEV0 > /sys/devices/platform/bone_capemgr/slots");
	//system("echo cape_enable=bone_capemgr.enable_partno=BB-SPIDEV0 >> /boot/uEnv.txt");


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
		

	/*
	 * bits per word
	 */
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
	
	//tx = malloc(sizeof(uint8_t) * 4);


	gpioInit(SPI_CS, GPIO_OUT, 1);



	printf("SPI MODE:\t\t%d\n", spi_mode);
	printf("SPI WORD:\t\t%d\n", spi_wordbit);
	printf("SPI SPEED:\t\t%d KHz\n", spi_speed / 1000);

	printf("\nSPI module init success\n--------------------------------------\n\n");


    return 0;
	
}




int spi_write(uint8_t reg, uint8_t data){

	int ret;
	uint8_t tx[] = {
		RA8875_CMDWRITE, reg, RA8875_DATAWRITE, data
		
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

		fprintf(stderr, "write reg : send spi message error\n");
		exit(EXIT_FAILURE);
	}

	//nanosleep(&trans_delay, NULL);

	return 0;


	



/*
int ret;


	tx[0] = RA8875_CMDWRITE;
	tx[1] = reg;
	tx[2] = RA8875_DATAWRITE;
	tx[3] = data;	



	uint8_t rx[4] = {0, };

	struct spi_ioc_transfer trans = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = 1,
		.delay_usecs = 0,
		.speed_hz = spi_speed,
		.bits_per_word = spi_wordbit,
	};

	gpio_write(SPI_CS, 0);

	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &trans);

	if (ret < 1){

		fprintf(stderr, "write reg : send spi message error\n");
		exit(EXIT_FAILURE);
	}

	trans.tx_buf = (unsigned long)(tx + 1);
	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &trans);

	if (ret < 1){

		fprintf(stderr, "write reg : send spi message error\n");
		exit(EXIT_FAILURE);
	}

	gpio_write(SPI_CS, 1);


	//microdelay(1);

	gpio_write(SPI_CS, 0);

	trans.tx_buf = (unsigned long)(tx + 2);
	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &trans);

	if (ret < 1){
		fprintf(stderr, "write reg : send spi message error\n");
		exit(EXIT_FAILURE);
	}


	trans.tx_buf = (unsigned long)(tx + 3);
	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &trans);

	if (ret < 1){

		fprintf(stderr, "write reg : send spi message error\n");
		exit(EXIT_FAILURE);
	}

	gpio_write(SPI_CS, 1);

	//microdelay(1);

	

	return 0;*/





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


/*

	tx[0] = RA8875_CMDWRITE;
	tx[1] = reg;
	tx[2] = RA8875_DATAREAD;
	tx[3] = 0x00;


	int ret;



	

	uint8_t rx[4] = {0, };

	struct spi_ioc_transfer trans = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = 1,
		.delay_usecs = 0,
		.speed_hz = spi_speed,
		.bits_per_word = spi_wordbit,
	};

	gpio_write(SPI_CS, 0);

	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &trans);

	if (ret < 1){

		fprintf(stderr, "write reg : send spi message error\n");
		exit(EXIT_FAILURE);
	}

	trans.tx_buf = (unsigned long)(tx + 1);
	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &trans);

	if (ret < 1){

		fprintf(stderr, "write reg : send spi message error\n");
		exit(EXIT_FAILURE);
	}

	gpio_write(SPI_CS, 1);


	//microdelay(1);

	gpio_write(SPI_CS, 0);

	trans.tx_buf = (unsigned long)(tx + 2);
	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &trans);

	if (ret < 1){
		fprintf(stderr, "write reg : send spi message error\n");
		exit(EXIT_FAILURE);
	}


	trans.tx_buf = (unsigned long)(tx + 3);
	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &trans);

	if (ret < 1){

		fprintf(stderr, "write reg : send spi message error\n");
		exit(EXIT_FAILURE);
	}

	gpio_write(SPI_CS, 1);


	return rx[0];*/


}
























void spi_transfer(uint8_t* data, int size){

	//setWordBit(size * 8);

	int ret;


	uint8_t tx[size];

	for(int i = 0; i < size; ++i){
		tx[i] = data[i];
	}

	uint8_t rx[size];


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
		fprintf(stderr, "write_cmd : send spi message error\n");
		exit(EXIT_FAILURE);
	}

	//setWordBit(8);







/*
	spi_write(RA8875_CURH0, x);
  spi_write(RA8875_CURH1, x >> 8);
  spi_write(RA8875_CURV0, y);
  spi_write(RA8875_CURV1, y >> 8);  
  writeCommand(RA8875_MRWC);


	uint8_t *outdata = malloc(sizeof(uint8_t) * size + sizeof(uint8_t));
	
	int ret;
	uint8_t rx[size];

	outdata[0] = RA8875_DATAWRITE;
	for(int i = 0; i < size; ++i){
		outdata[i + 1] = data[i];
	}



	struct spi_ioc_transfer trans = {
		.tx_buf = (unsigned long)outdata,
		.rx_buf = (unsigned long)rx,
		.len = size,
		.delay_usecs = 0,
		.speed_hz = spi_speed,
		.bits_per_word = spi_wordbit,
	};
	gpio_write(SPI_CS, 0);

	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &trans);

	if (ret < 1){
		fprintf(stderr, "write_cmd : send spi message error\n");
		exit(EXIT_FAILURE);
	}


	gpio_write(SPI_CS, 1);


*/














}


void setWordBit(int i ){
	spi_wordbit = i;
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
	

/*
	int ret;

	tx[0] = RA8875_CMDWRITE;
	tx[1] = d;


	uint8_t rx[2] = {0, };

	struct spi_ioc_transfer trans = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = 1,
		.delay_usecs = 0,
		.speed_hz = spi_speed,
		.bits_per_word = spi_wordbit,
	};

	gpio_write(SPI_CS, 0);

	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &trans);

	if (ret < 1){

		fprintf(stderr, "write reg : send spi message error\n");
		exit(EXIT_FAILURE);
	}

	trans.tx_buf = (unsigned long)(tx + 1);
	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &trans);

	if (ret < 1){

		fprintf(stderr, "write reg : send spi message error\n");
		exit(EXIT_FAILURE);
	}

	gpio_write(SPI_CS, 1);*/

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
	







/*
	int ret;
	tx[0] = RA8875_DATAWRITE;
	tx[1] = d;


	uint8_t rx[2] = {0, };

	struct spi_ioc_transfer trans = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = 1,
		.delay_usecs = 0,
		.speed_hz = spi_speed,
		.bits_per_word = spi_wordbit,
	};

	gpio_write(SPI_CS, 0);

	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &trans);

	if (ret < 1){

		fprintf(stderr, "write reg : send spi message error\n");
		exit(EXIT_FAILURE);
	}

	trans.tx_buf = (unsigned long)(tx + 1);
	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &trans);

	if (ret < 1){

		fprintf(stderr, "write reg : send spi message error\n");
		exit(EXIT_FAILURE);
	}

	gpio_write(SPI_CS, 1);

*/

}








void set_SPISpeed( int speed ){
	spi_speed = speed;
}

