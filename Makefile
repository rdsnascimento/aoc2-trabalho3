CC=gcc

CFLAGS=-Wall -Wextra -Werror -std=c99 -O1 -g
LDFLAGS=-lm

all: grade student

sim: io.c test.c
	$(CC) $(CFLAGS) -o sim io.c test.c $(LDFLAGS)

teste: io.c mytests.c
	$(CC) $(CFLAGS) -o teste io.c mytests.c $(LDFLAGS)

grade: sim
	./sim

student: teste
	./teste

clean:
	rm -rf *.o sim teste
