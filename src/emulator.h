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

#define SCREEN_WIDTH 80*15
#define SCREEN_LENGTH 25*15



#define HLT     0x0
#define LDAI    0x1
#define LDAM    0x2
#define LDAB    0x3
#define LDARES  0x4
#define LDAFLAG 0x5
#define LDBA    0x6
#define ADD     0x7
#define LDPCA   0x8
#define LDPCNZA  0x10
#define LDMA    0x11
#define LDVMA   0x12

typedef struct {
    CPU_BITS_UNIT REGISTER_A;
    CPU_BITS_UNIT REGISTER_B;
    CPU_BITS_UNIT REGISTER_RESULT;

    CPU_BITS_UNIT PC;
    CPU_BITS_UNIT IR;
    CPU_BITS_UNIT FLAGS;

    CPU_BITS_UNIT memory[UINT16_MAX/(sizeof(CPU_BITS_UNIT)/sizeof(char))];
    CPU_BITS_UNIT video_memory[SCREEN_WIDTH*SCREEN_LENGTH*sizeof(char)/((sizeof(CPU_BITS_UNIT)/sizeof(char)))];
} CPU;

uint16_t* read_file(char* path, size_t* len);

void dump_cpu(CPU* cpu);

void print_mem(CPU* cpu, uint16_t location);

void print_vmem(CPU* cpu, uint16_t location);

void display_err(char* message);

int emulate_cpu(void*);

#endif