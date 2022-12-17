CC = gcc
MAIN = src/tictactoe.c

ifeq ($(OS), Windows_NT)
	DELETE = del
	EXT=.exe
else 
	UNAME_S:=$(shell uname -s)
	ifeq ($(UNAME_S), Darwin)
		DELETE = rm -f
		EXT=.out
	endif
endif

all: program

program:

	$(CC) $(MAIN) -o bin/program$(EXT) -lpaho-mqtt3c

run: bin/program$(EXT)
	bin/program$(EXT)

clean: 
	$(DELETE) bin\*$(EXT)