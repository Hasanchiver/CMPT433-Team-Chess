
TARGET= chess

SOURCES= main.c ChessBoard.c ChessPiece.c Game.c General.c List.c uci.c Player.c PossibleMovesList.c Rules.c

PUBDIR = $(HOME)/cmpt433/public/myApps
OUTDIR = $(PUBDIR)
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = gcc

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror



chess:
	$(CC_C) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET) -lpthread


clean:
	rm -f $(OUTDIR)/$(TARGET)
