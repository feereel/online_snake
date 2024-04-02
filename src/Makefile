CC=gcc
CFLAGS=-Wall -pedantic -Werror -g

all: engine.o handler.o render.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o *.exe
