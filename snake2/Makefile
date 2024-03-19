CC=gcc
CFLAGS=-Wall -pedantic -Werror
BIN=server.exe

all: clean $(BIN)

%.exe:server.c
	@echo "Building $@..."
	@$(CC) $(CFLAGS) -o $@ $<

clean:
	@echo "Cleaning"
	@rm -fv *.exe
