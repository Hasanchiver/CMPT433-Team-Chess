
# Makefile for building and debuging chess program
# Author: Joe --- TeamChess

DEPOLY_TARGET = chessProgram
DEBUGGING_TARGET = test
PUBDIR = $(HOME)/cmpt433/public/project

SOURCES= main.c spi.c lcd.c gpio.c unicodeboard.c chesslogic.c 
DEBUG_SOURCE= test.c

DEPLOY_DIR = $(HOME)/Documents/Courses/2018-summer/cmpt-433/workspace/public/chess

CC_C = arm-linux-gnueabihf-gcc


CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L #-Werror



all: node
	$(CC_C) $(CFLAGS) $(SOURCES) -o $(DEPLOY_DIR)/$(DEPOLY_TARGET) -lpthread -lm
	
test:
	$(CC_C) $(CFLAGS) $(DEBUG_SOURCE) -o $(DEPLOY_DIR)/$(DEBUGGING_TARGET) -lpthread -lm

cpptest:
	arm-linux-gnueabihf-gcc $(CFLAGS) test.cpp BusDevice.cpp -o $(DEPLOY_DIR)/cpptest -lpthread -lm

node:
	mkdir -p $(PUBDIR)/chess-server-copy/
	cp -R chess-server/* $(PUBDIR)/chess-server-copy/

clean:
	rm -f $(DEPLOY_DIR)/$(DEPOLY_TARGET)
	rm -f $(DEPLOY_DIR)/$(DEBUGGING_TARGET)
