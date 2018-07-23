// Shared includes and functions

#ifndef _GENERAL_H_
#define _GENERAL_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


// Trigger an orderly shutdown
void General_shutdown(void);
_Bool General_isShuttingDown(void);

void die_on_failed(char *msg);
void sleep_usec(long usec);
void sleep_msec(long msec);


#endif
