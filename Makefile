CC=/usr/bin/gcc
LD=/usr/bin/gcc

all: k16-emulator 

k16-emulator: bin/emulator.o
	$(LD) bin/emulator.o -L/usr/lib/x86_64-linux-gnu -lSDL2 -o k16-emulator

bin/emulator.o: src/emulator.c
	$(CC) -c src/emulator.c -lSDL2 -o bin/emulator.o

clean:
	rm bin/*
	rm k16-emulator