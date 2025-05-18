/*
    EXPLANATION: 
        This program runs on 2 threads, the CPU loop and the window loop, keeping the logic entirely seperate between the two
        In order to work multiplatformly, the window loop is done through SDL (Simple Direct Media Layer), it simply loops through the video memory array
        The CPU loop is extraordinarily simple, just switch statement through the 15 instructions

*/



#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "emulator.h"


// bad practice but to synchronize the threads, we need to have the CPU thread set the video memory pointer
uint16_t* screen_memory = NULL;

int main(int argc, char** argv) {
    // Ensure the correct parameters are passed
    if (argc < 2) {
        printf("USAGE: \n"
              "emulator [path-to-binary]\n");
        exit(1);
    }

    // Create the emulate CPU thread
    //SDL_Thread* thread = SDL_CreateThread(emulate_cpu, "cpu_thread", argv[1]); 
        

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_Window* window = SDL_CreateWindow("K-16 Emulator Screen",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_LENGTH, 0);
    if (!window) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        exit(1);
    }

    while (screen_memory == NULL) {}

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_Texture* texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             SCREEN_WIDTH, SCREEN_LENGTH);

    // the array of pixels to fill the screen with
    uint32_t* pixels = malloc(SCREEN_WIDTH * SCREEN_LENGTH * sizeof(uint32_t));
    if (!pixels) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    // Main loop
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = 0;
        }

        for (int y = 0; y < SCREEN_LENGTH; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                    pixels[x+y*SCREEN_LENGTH] = 0xFFFFFFFF;
            }
        }

        SDL_UpdateTexture(texture, NULL, pixels, SCREEN_LENGTH * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    free(pixels);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
    
}

// file path is a void* as the arg is passed as a function
int emulate_cpu(void* file_path) {
    size_t file_len = 0;
    uint16_t* data = read_file(file_path, &file_len);
    void* pdata = data;
    if (!data) {
        printf("Error, invalid path: %s\n", (char*)file_path); 
        exit(1);
    }

    CPU* cpu = malloc(sizeof(CPU));
    if (!cpu) {
        printf("Error initailizng CPU\n");
        exit(1);
    }
    // set the video memory pointer
    screen_memory = cpu->video_memory;
    // Go through each byte in the file and execute it
    size_t i = 0;
    while (i < file_len) {
        switch(*data) {
        case HLT:
            goto end;
        case LDAI:
            data++;
            cpu->REGISTER_A = *data;
            break;
        case LDAM:
            data++;
            cpu->REGISTER_A = cpu->memory[*data];
            break;
        case LDAB:
            cpu->REGISTER_A = cpu->REGISTER_B;
            break;
        case LDARES:
            cpu->REGISTER_A = cpu->REGISTER_RESULT;
            break;
        case LDAFLAG:
            cpu->REGISTER_A = cpu->FLAGS;
            break;
        case LDBA:
            cpu->REGISTER_B = cpu->REGISTER_A;
            break;
        case ADD:
            if ((uint64_t)cpu->REGISTER_A + (uint64_t)cpu->REGISTER_B > UINT16_MAX)
                cpu->FLAGS |= 0b1;
            else 
                cpu->FLAGS &= 0b1111111111111110;
            if ((uint64_t)cpu->REGISTER_A + (uint64_t)cpu->REGISTER_B == 0)
                cpu->FLAGS |= 0b10;
            else 
                cpu->FLAGS &= 0b1111111111111101;
            cpu->REGISTER_RESULT = cpu->REGISTER_A + cpu ->REGISTER_B;
            break;
        case LDPCA:
            cpu->PC = cpu->REGISTER_A - 1;
            break;
        case LDPCNZA:
            if (cpu->FLAGS & 0b10)
                cpu->PC = cpu->REGISTER_A - 1;
            break;
        case LDMA:
            data++;
            cpu->memory[*data] = cpu->REGISTER_A;
            break;
        case LDVMA:
            data++;
            cpu->video_memory[*data] = cpu->REGISTER_A;
            break;
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
    printf("Register A: %u\nRegister B: %u\nRegister Result: %u\nPC: %u\nFLAGS: %u\n IP: %u\n", cpu->REGISTER_A, cpu->REGISTER_B, cpu->REGISTER_RESULT, cpu->PC, cpu->FLAGS, cpu->IR); 
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

void display_err(char* message) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "k16-emulator ERROR", message, NULL);
}
