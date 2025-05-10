CC=/usr/bin/gcc
LD=/usr/bin/gcc

all: k16-assembler

k16-assembler: bin/assembler.o
	$(LD) bin/assembler.o -o k16-assembler

bin/assembler.o: src/assembler.c
	$(CC) -c src/assembler.c -o bin/assembler.o

clean:
	rm k16-assembler bin/assembler.o