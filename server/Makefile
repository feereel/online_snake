CC=gcc
CFLAGS=-Wall -pedantic -Werror -g

all: server.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o *.exe
