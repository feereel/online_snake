CC=gcc
CFLAGS=-Wall -pedantic -Werror -g

all: client.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o *.exe
