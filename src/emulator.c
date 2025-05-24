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

#define WINDOW_WIDTH 1000

SDL_mutex* mutex;
SDL_cond* cond;

int main(int argc, char** argv) {
    // Ensure the correct parameters are passed
    if (argc < 2) {
        printf("USAGE: \n"
              "emulator [path-to-binary]\n");
        exit(1);
    }
    // copy the path passed as argv[1] into path
    char* path = malloc (MAX_PATH);
    strncpy(path, argv[1], MAX_PATH);

    // Initialize SDL
    SDL_Event event;
    SDL_Renderer* renderer;
    SDL_Window* window;
    int i;

    init_sdl(event, renderer, window);


    // read the file 
    size_t file_len = 0;
    uint16_t* data = read_file(path, &file_len);
    if (!data) {
        fprintf(stderr, "Error, invalid path: %s\n", (char*)path); 
        exit(1);
    }

    CPU* cpu = malloc(sizeof(CPU));

    if (file_len >= MEMORY_SIZE) {
        fprintf(stderr, "Error, binary bigger than memory!");
        exit(1);
    }

    // copy the binary to memory
    memcpy(cpu->memory, data, file_len);

    if (!cpu) {
        printf("Error initailizng CPU\n");
        exit(1);
    }

    // clear the screen
    for (int i = 0; i < VIDEO_MEMORY_SIZE; i++) {
        cpu->video_memory[i] = 0;
    }
       
    // update the SDL screen with the render data
    SDL_RenderPresent(renderer);
    while (1) {
        
        // Go through each byte in the file and execute it
        size_t i = 0;
        while (i < file_len) {
            switch(cpu->memory[cpu->PC]) {
                case HLT:
                    goto end;
                case LDAI:
                    cpu->PC++;
                    cpu->REGISTER_A = cpu->memory[cpu->PC];
                    break;
                case LDAM:
                    cpu->PC++;
                    cpu->REGISTER_A = cpu->memory[cpu->memory[cpu->PC]];
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
                    cpu->PC++;
                    cpu->memory[cpu->memory[cpu->PC]] = cpu->REGISTER_A;
                    break;
                case LDVMA:
                    cpu->PC++;
                    cpu->video_memory[cpu->memory[cpu->PC]] = cpu->REGISTER_A;
                    break;
                case STMA:
                    cpu->PC++;
                    cpu->video_memory[cpu->REGISTER_A] = cpu->memory[cpu->PC];
                    break;
                case STVMA:
                    cpu->PC++;
                    cpu->video_memory[cpu->REGISTER_A] = cpu->memory[cpu->PC];
                    break;
            }

            i++;
            // Check if quit has been called
            if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
                break;
            
            // go through the cpu vram to paint the screen
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                for (int y = 0; y < SCREEN_HEIGHT; y++) {
                    if (cpu->video_memory[x+y*SCREEN_HEIGHT]) {
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                }
            } 
            SDL_RenderPresent(renderer);
        }   
    
    
   
    }
    end:
    printf("CPU Terminated!\n");
    dump_cpu(cpu);

    while (1) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    }

    free(cpu);
    free(data);
    free(path);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
    
}

int init_sdl(SDL_Event event, SDL_Renderer* renderer, SDL_Window* window) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    // since the video will only be 1 bit, either full white or black, set the color to be white
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}



void dump_cpu(CPU* cpu) {
    char message[1024];
    sprintf(message, "Register A: %u\nRegister B: %u\nRegister Result: %u\nPC: %u\nFLAGS: %u\n IP: %u\n", cpu->REGISTER_A, cpu->REGISTER_B, cpu->REGISTER_RESULT, cpu->PC, cpu->FLAGS, cpu->IR); 
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "CPU Dump", message, NULL);
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
