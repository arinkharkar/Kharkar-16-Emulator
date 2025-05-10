CC=/usr/bin/gcc
LD=/usr/bin/gcc

all: emulator

emulator: bin/emulator.o
	$(LD) bin/emulator.o -o emulator

bin/emulator.o: src/emulator.c
	$(CC) -c src/emulator.c -o bin/emulator.o

