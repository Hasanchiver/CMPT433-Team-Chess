TARGET= unicodeboard
SOURCES= unicodeboard.c chesslogic.c 
TESTTARGET= test
TESTSOURCES= test.c chesslogic.c networkAPI.c

PUBDIR = $(HOME)/cmpt433/public/project
WRKDIR = $(HOME)/cmpt433/work/project
OUTDIR = $(PUBDIR)
NODEPATH = $(PUBDIR)/beatbox-server-copy
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc
CPP = g++
C = gcc

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror
LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB

all: 
	$(C) $(CFLAGS) $(SOURCES) -o $(WRKDIR)/$(TARGET)

test: 
	$(C) $(CFLAGS) $(TESTSOURCES) -o $(WRKDIR)/$(TESTTARGET)

cross:
	$(CC_C) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET)
	#  $(LFLAGS) -lpthread -lasound

clean:
	rm -f $(OUTDIR)/$(TARGET)
	rm -f $(WRKDIR)/$(TESTTARGET)
	rm -f $(WRKDIR)/$(TARGET)

#valgrind --leak-check=full \--show-reachable=yes ./main