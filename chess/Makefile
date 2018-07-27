TARGET= chessgame
SOURCES= unicodeboard.c chesslogic.c udpListener.c networkAPI.c

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


all: node
	$(CC_C) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET) -lpthread
node:
	mkdir -p $(PUBDIR)/chess-server-copy/
	cp -R chess-server/* $(PUBDIR)/chess-server-copy/


clean:
	rm -f $(OUTDIR)/$(TARGET)
	rm -f $(WRKDIR)/$(TARGET)


