#ifndef _GPIO_H
#define _GPIO_H






int gpioInit(int port, int dir, int val);
int gpio_write(int port, int val);
int gpio_setDirection(int port, int dir);
int gpio_read(int port);








#endif

