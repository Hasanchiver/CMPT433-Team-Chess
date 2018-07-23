#include "General.h"
#include <time.h>


static _Bool isShuttingDown = false;
void General_shutdown(void)
{
	isShuttingDown = true;
}
_Bool General_isShuttingDown(void)
{
	return isShuttingDown;
}



void sleep_usec(long usec)
{
	struct timespec sleep_time;
	sleep_time.tv_sec = (usec / 1000000);
	sleep_time.tv_nsec = (usec % 1000000) * 1000;
	nanosleep(&sleep_time, NULL);
}

void sleep_msec(long msec)
{
	sleep_usec(msec * 1000);
}
