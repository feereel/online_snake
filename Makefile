CC=gcc
CFLAGS=-Wall -pedantic -Werror -g
SER_BIN=server.exe
CLI_BIN=client.exe

src=src
cli=client
ser=server

SER_OBJ=$(src)/engine.o $(src)/handler.o $(src)/render.o $(ser)/server.o
CLI_OBJ=$(src)/engine.o $(src)/handler.o $(src)/render.o $(cli)/client.o

all: tsrc tclient tserver cclient cserver

tsrc:
	make -C $(src)

tserver:
	make -C $(ser)

tclient:
	make -C $(cli)

cclient:
	$(CC) $(CFLAGS) -lncurses -o $(CLI_BIN) $(CLI_OBJ)

cserver:
	$(CC) $(CFLAGS) -lncurses -o $(SER_BIN) $(SER_OBJ)

clean:
	make clean -C $(src)
	make clean -C $(ser)
	make clean -C $(cli)
	rm -rf *.o *.exe *.log

