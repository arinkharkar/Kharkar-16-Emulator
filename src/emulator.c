#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include "emulator.h"




void fib();

int main(int argc, char** argv) {
    fib();

    if (argc < 2) {
        printf("USAGE: \n"
              "emulator [path-to-binary]\n");
        return 1;
    }
    
    size_t file_len = 0;
    uint16_t* data = read_file(argv[1], &file_len);
    void* pdata = data;
    if (!data) {
        printf("Error, invalid path: %s\n", argv[1]);
        return 1;
    }

    CPU* cpu = malloc(sizeof(CPU));
    if (!cpu) {
        printf("Error initailizng CPU\n");
        return 1;
    }

    // Go through each byte in the file and execute it
    size_t i = 0;
    while (i < file_len) {
        switch(*data) {
        case HLT:
            printf("HLT called\n");
            goto end;
        case LDA:
            printf("LDA called\n");
            cpu->REGISTER_A = cpu->BUS;
            goto next;
        case LDB:
            printf("LDB called\n");
            cpu->REGISTER_B = cpu->BUS;
            goto next;
        case LDC:
            printf("LDC called\n");
            cpu->REGISTER_C = cpu->BUS;
            goto next;
        case LDPC:
            printf("LDPC called\n");
            cpu->PC = cpu->BUS;
            goto next;
        case LDPNZ:
            printf("LDPNZ called\n");
        // if the zero flag is clear
            if (!(cpu->FLAGS & 0b10)) {
                cpu->PC = cpu->BUS;
            }
            goto next;
        case LDF:
            printf("LDF called\n");
            cpu->FLAGS = cpu->BUS;
            goto next;
        case PUSH:
            printf("PUSH called\n");
            cpu->BUS = cpu->memory[cpu->REGISTER_A];
            goto next;
        case PUSHI:
            printf("PUSHI called\n");
            // put the next value into the bus
            data++;
            cpu->BUS = *(data);
            goto next;
        case ADD:
            printf("ADD called\n");
            // Set the overflow and zero bit if necessary
            if (cpu->REGISTER_A + cpu->REGISTER_B == 0) cpu->FLAGS |= 0b10;
            if ((size_t)cpu->REGISTER_A + (size_t)cpu->REGISTER_B > (size_t)UINT16_MAX) cpu->FLAGS |= 0b1;
            cpu->BUS = cpu->REGISTER_A + cpu->REGISTER_B;
            
            goto next;
        case LOADM:
            printf("LOADM called\n");
            cpu->memory[cpu->REGISTER_A] = cpu->BUS;
            goto next;
        case LOADVM:
            printf("LOADVM called\n");
            cpu->video_memory[cpu->REGISTER_A] = cpu->BUS;
            goto next;
        }
        next:
        i++;
        cpu->PC++;
        data++;
    }
    end:

    dump_cpu(cpu);

    free(cpu);
    free(pdata);
    return 0;
}

void dump_cpu(CPU* cpu) {
    printf("Register A: %u\nRegister B: %u\nRegister C: %u\nPC: %u\nFLAGS: %u\nBUS: %u\n", cpu->REGISTER_A, cpu->REGISTER_B, cpu->REGISTER_C, cpu->PC, cpu->FLAGS, cpu->BUS);
}

void print_mem(CPU* cpu, uint16_t location) {
    printf("%u\n", cpu->memory[location]);
}

void print_vmem(CPU* cpu, uint16_t location) {
    printf("%u\n", cpu->video_memory[location]);
}

uint16_t* read_file(char* path, size_t* file_len) {
    FILE* file = fopen(path, "r");
    if (!file) return NULL;
    fseek (file, 0, SEEK_END);
    size_t length = ftell (file);
    *file_len = length;
    fseek (file, 0, SEEK_SET);
    uint8_t* buffer = malloc (length);
    if (!buffer) return NULL;
    fread (buffer, 1, length, file);
    fclose (file);
    return (uint16_t*)buffer;
}

void fib() {
    int a = 0;
    int b = 1;
    int c = 10;

    for (int i = 0; i < c / 2; i++) {
        a = a + b;
        b = a + b;
    }
    printf("%d", b);
}
