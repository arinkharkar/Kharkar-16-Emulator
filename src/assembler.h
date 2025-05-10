#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#ifdef __linux__
    #include <linux/limits.h>
    #define MAX_PATH PATH_MAX
#else 
    #ifndef MAX_PATH
        #error "Unsuported Operating System"
    #endif
#endif



typedef enum {
    HLT,
    LDA,
    LDB,
    LDC,
    LDPC,
    LDPCNZ,
    LDF,
    PUSH,
    PUSHI,
    ADD,
    LOADM,
    LOADVM
} token;

char* read_file(char* path, size_t* file_len);

int write_file(char* path, size_t file_len, uint8_t* write_contents);

uint8_t* assemble(char* input_file, size_t* output_file_size);

int get_input_output_file_names(char* src_file, char* out_file, int argc, char** argv);

token* tokenize_input(char* input, size_t* token_amount);

bool is_number(const char* str);

#endif