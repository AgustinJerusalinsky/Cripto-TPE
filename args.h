#ifndef ARGUMENTS_H
#define ARGUMENTS_H
#include <stdint.h>
#include <stdbool.h>

enum MAX_SIZES{
    MAX_FILENAME_SIZE = 64,
    MAX_PASSWORD_SIZE = 64,
};

typedef enum STEG_METHOD{
    LSB1,
    LSB4,
    LSBI
} STEG_METHOD;

typedef enum ENC_METHOD{
    NONE,
    AES128,
    AES192,
    AES256,
    DES,
} ENC_METHOD;

typedef enum ENC_MODE{
    ECB,
    CFB,
    OFB,
    CBC,    
} ENC_MODE;

typedef enum ACTION{
    EMBED,
    EXTRACT,
} ACTION;

struct arguments{
    char input_file[MAX_FILENAME_SIZE];
    char output_file[MAX_FILENAME_SIZE];
    char carrier[MAX_FILENAME_SIZE];
    char pass[MAX_PASSWORD_SIZE];
    STEG_METHOD steg_method;
    ENC_METHOD enc_method;
    ENC_MODE mode;
    ACTION action;
};

extern struct arguments args;

void parse_arguments(int argc, char ** argv);

#endif