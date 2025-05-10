#ifndef MAIN_H
#define MAIN_H
#include <stdint.h>
#include <stddef.h>

// The bit length of the CPU
#define CPU_BITS 16

// change the size of each part of the cpu depending on the bit length of the cpu
#if CPU_BITS == 16
#define CPU_BITS_UNIT uint16_t
#elif CPU_BITS == 8
#define CPU_BITS_UNIT uint8_t
#endif

#define SCREEN_WIDTH 80
#define SCREEN_LENGTH 25



#define HLT    0x0
#define LDA    0x1
#define LDB    0x2
#define LDC    0x3
#define LDPC   0x4
#define LDPNZ  0x5
#define LDF    0x6
#define PUSH   0x7
#define PUSHI  0x8
#define ADD    0x9
#define LOADM  0x10
#define LOADVM 0x11

typedef struct {
    CPU_BITS_UNIT REGISTER_A;
    CPU_BITS_UNIT REGISTER_B;
    CPU_BITS_UNIT REGISTER_C;

    CPU_BITS_UNIT PC;

    CPU_BITS_UNIT FLAGS;
    CPU_BITS_UNIT BUS;

    CPU_BITS_UNIT memory[UINT16_MAX/(sizeof(CPU_BITS_UNIT)/sizeof(char))];
    CPU_BITS_UNIT video_memory[SCREEN_WIDTH*SCREEN_LENGTH*sizeof(char)/((sizeof(CPU_BITS_UNIT)/sizeof(char)))];
} CPU;

uint16_t* read_file(char* path, size_t* len);

void dump_cpu(CPU* cpu);

void print_mem(CPU* cpu, uint16_t location);

void print_vmem(CPU* cpu, uint16_t location);

#endif