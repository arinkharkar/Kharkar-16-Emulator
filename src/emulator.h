#ifndef MAIN_H
#define MAIN_H
#include <stdint.h>
#include <stddef.h>


#ifdef __linux__
    #include <linux/limits.h>
    #define MAX_PATH PATH_MAX
#else
    // MAX_PATH will be defined on Windows
    #ifndef MAX_PATH
        #error "Unsuported Operating System"
    #endif
#endif

// The bit length of the CPU
#define CPU_BITS 16

// change the size of each part of the cpu depending on the bit length of the cpu
#if CPU_BITS == 16
#define CPU_BITS_UNIT uint16_t
#elif CPU_BITS == 8
#define CPU_BITS_UNIT uint8_t
#endif

#define SCREEN_WIDTH  (80*21)
#define SCREEN_HEIGHT (25*21)



#define HLT     0x0
#define LDAI    0x1
#define LDAM    0x2
#define LDAB    0x3
#define LDARES  0x4
#define LDAFLAG 0x5
#define LDBA    0x6
#define ADD     0x7
#define LDPCA   0x8
#define LDPCNZA 0x10
#define LDMA    0x11
#define LDVMA   0x12
#define STVMA   0x13
#define STMA    0x14


#define VIDEO_MEMORY_SIZE (SCREEN_WIDTH*SCREEN_HEIGHT)
#define MEMORY_SIZE (UINT16_MAX)

typedef struct {
    CPU_BITS_UNIT REGISTER_A;
    CPU_BITS_UNIT REGISTER_B;
    CPU_BITS_UNIT REGISTER_RESULT;

    CPU_BITS_UNIT PC;
    CPU_BITS_UNIT IR;
    CPU_BITS_UNIT FLAGS;

    CPU_BITS_UNIT memory[MEMORY_SIZE];
    CPU_BITS_UNIT video_memory[VIDEO_MEMORY_SIZE];
} CPU;


// passed to the cpu thread
typedef struct {
    char* path;
    uint16_t* screen_memory;
} cpu_thread_data;

// read file in path, returns a buffer and the length of the file is passed in len
// returned buffer must be free'd
uint16_t* read_file(char* path, size_t* len);

// dump the values of the cpu 
void dump_cpu(CPU* cpu);

void print_mem(CPU* cpu, uint16_t location);

void print_vmem(CPU* cpu, uint16_t location);

void display_err(char* message);

int emulate_cpu(cpu_thread_data* data);

int init_sdl(SDL_Event event, SDL_Renderer* renderer, SDL_Window* window) ;

#endif