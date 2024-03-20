CC=gcc
CFLAGS=-Wall -pedantic -Werror
BIN=server.exe

all: clean $(BIN)

server.exe:
	@echo "Building $@..."
	$(CC) server.c engine.c handler.c $< -o $@ 

clean:
	@echo "Cleaning"
	@rm -fv *.exe
