#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"
#include "bmp.h"
#include "encrypt.h"

void embed_lsbN_bytes(char* message, int msg_size, FILE* carrier, FILE* output, int n);
void extract_lsbN_bytes(uint8_t* dest, FILE* carrier, int nbytes, int n);

void embed_lsb1(char* buffer, int size, FILE* carrier, FILE* output) {
    embed_lsbN_bytes(buffer, size, carrier, output, 1);
}
void embed_lsb4(char* buffer, int size, FILE* carrier, FILE* output) {
    embed_lsbN_bytes(buffer, size, carrier, output, 4);
}
void embed_lsbi(char* buffer, int size, FILE* carrier, FILE* output) {
}

void extract_lsb1(uint8_t* dest, FILE* carrier, int nbytes) {
    extract_lsbN_bytes(dest, carrier, nbytes, 1);
}
void extract_lsb4(uint8_t* dest, FILE* carrier, int nbytes) {
    extract_lsbN_bytes(dest, carrier, nbytes, 4);
}
void extract_lsbi(uint8_t* dest, FILE* carrier, int nbytes) {
}

uint8_t get_bit(char* message, int bit_index) {
    int byte_index = bit_index / 8;
    int bit_offset = bit_index % 8;
    return (message[byte_index] >> (7 - bit_offset)) & 0x01;
}

void embed_lsbN_bytes(char* message, int msg_size, FILE* carrier, FILE* output, int n) {
    int bit_index = 0;
    int max_bits = msg_size * 8;
    uint8_t byte;
    while (bit_index < max_bits && fread(&byte, 1, 1, carrier)) {
        uint8_t bits = 0;
        for (int i = 0; i < n; i++) {
            bits = (bits << 1) | get_bit(message, bit_index);
            bit_index++;
        }
        byte = (byte & (0xFF << n)) | bits;

        fwrite(&byte, 1, 1, output);
    }
}

void embed_into_carrier(char* message, int msg_size, FILE* carrier, FILE* output) {
    void (*embed_function)(char*, int, FILE*, FILE*);

    switch (args.steg_method) {
        case LSB1:
            embed_function = embed_lsb1;
            break;
        case LSB4:
            embed_function = embed_lsb4;
            break;
        case LSBI:
            embed_function = embed_lsbi;
            break;
    }

    // embed size
    char big_endian_size[4] = {msg_size >> 24, msg_size >> 16, msg_size >> 8, msg_size};

    embed_function(big_endian_size, 4, carrier, output);

    // embed message
    embed_function(message, msg_size, carrier, output);

    if (args.enc_method != NONE) {
        // embed extension
        char* extension = strrchr(args.input_file, '.');
        if (!extension) {
            extension = "";
        }
        printf("Extension: %s\n", extension);

        embed_function(extension, strlen(extension) + 1, carrier, output);  //+1 for null terminated
    }
    // finish copying
    uint8_t byte;
    while (fread(&byte, 1, 1, carrier)) {
        fwrite(&byte, 1, 1, output);
    }
}

void embed() {
    // copy input file to memory
    FILE* input = fopen(args.input_file, "rb");
    if (input == NULL) {
        printf("input file not found\n");
        return;
    }
    fseek(input, 0, SEEK_END);
    int fs = ftell(input);
    fseek(input, 0, SEEK_SET);
    printf("fs: %d\n", fs);
    char* message = malloc(fs + 1);
    fread(message, 1, fs, input);

    // Encrypt if necessary
    if (args.enc_method != NONE) {
        unsigned char* ciphertext = (unsigned char*)malloc(fs + MAX_BLOCK_SIZE);

        char* extension = strrchr(args.input_file, '.');
        if (!extension) {
            extension = "";
        }

        unsigned char* plaintext = (unsigned char*)malloc(4 + fs + strlen(extension) + 1);

        plaintext[0] = fs >> 24;
        plaintext[1] = fs >> 16;
        plaintext[2] = fs >> 8;
        plaintext[3] = fs;
        memcpy(plaintext + 4, message, fs);
        memcpy(plaintext + 4 + fs, extension, strlen(extension) + 1);

        fs = encrypt(plaintext, fs + 4 + strlen(extension) + 1, ciphertext);
        if (fs == -1) {
            printf("Encryption failed\n");
            exit(1);
        }
        free(message);
        free(plaintext);
        message = (char*)ciphertext;
    }

    // Open input file
    FILE* carrier = fopen(args.carrier, "rb");
    if (carrier == NULL) {
        printf("carrier file not found\n");
        return;
    }

    BITMAPFILEHEADER bmp_file_header;

    // Read bitmap file header
    fread(&bmp_file_header, sizeof(BITMAPFILEHEADER), 1, carrier);

    // copy all bytes before pixel map
    char* header = malloc(bmp_file_header.offset);
    fseek(carrier, 0, SEEK_SET);
    fread(header, bmp_file_header.offset, 1, carrier);

    // jump to pixel map
    fseek(carrier, bmp_file_header.offset, SEEK_SET);

    FILE* output = fopen(args.output_file, "wb");
    fwrite(header, bmp_file_header.offset, 1, output);
    // Insert into carrier file
    embed_into_carrier(message, fs, carrier, output);
    fclose(output);
    fclose(carrier);
    fclose(input);
}

void extract_lsbN_bytes(uint8_t* dest, FILE* carrier, int nbytes, int n) {
    uint8_t bytes[8 / n];
    for (int i = 0; i < nbytes; i++) {
        fread(bytes, 1, 8 / n, carrier);
        uint8_t byte = 0;

        for (int j = 0; j < 8 / n; j++) {
            byte = (byte << n) | (bytes[j] & (0xFF >> (8 - n)));
        }
        dest[i] = byte;
    }
}

char* extract_from_carrier(FILE* carrier, char* extension, uint32_t* size) {
    // extract function mapping
    void (*extract_function)(uint8_t*, FILE*, int);
    switch (args.steg_method) {
        case LSB1:
            extract_function = extract_lsb1;
            break;
        case LSB4:
            extract_function = extract_lsb4;
            break;
        case LSBI:
            extract_function = extract_lsbi;
            break;
    }

    // extract size
    uint8_t big_endian_size[4];
    extract_function(big_endian_size, carrier, 4);
    *size = big_endian_size[0] << 24 | big_endian_size[1] << 16 | big_endian_size[2] << 8 | big_endian_size[3];
    printf("size: %d\n", *size);

    char* output = malloc(*size + 1);

    uint8_t byte;

    // extract data
    for (int i = 0; i < *size; i++) {
        extract_function((uint8_t*)(output + i), carrier, 1);
    }

    if (args.enc_method != NONE)
        return output;

    printf("extracting extension\n");

    // extract extension
    int i = 0;
    byte = -1;
    while (byte != 0) {
        extract_function(&byte, carrier, 1);
        extension[i] = byte;
        i++;
    }

    return output;
}

void extract() {
    FILE* carrier = fopen(args.carrier, "rb");
    if (carrier == NULL) {
        printf("carrier file not found\n");
        return;
    }

    BITMAPFILEHEADER bmp_file_header;
    // Read bitmap file header
    fread(&bmp_file_header, sizeof(BITMAPFILEHEADER), 1, carrier);
    // jump to pixel map
    fseek(carrier, bmp_file_header.offset, SEEK_SET);

    char extension[8];
    uint32_t size;
    char* output;

    printf("Extracting from carrier file...\n");
    output = extract_from_carrier(carrier, extension, &size);

    // Decrypt if necessary
    if (args.enc_method != NONE) {
        printf("decrypting... \n");
        unsigned char* plaintext = (unsigned char*)malloc(size + MAX_BLOCK_SIZE);
        int decr_size = decrypt((unsigned char*)output, size, plaintext);
        printf("cipher size: %d\n", decr_size);
        if (decr_size < 0) {
            printf("decryption failed\n");
            exit(1);
        }
        size = plaintext[0] << 24 | plaintext[1] << 16 | plaintext[2] << 8 | plaintext[3];
        printf("decrypted size: %d\n", size);
        free(output);
        output = malloc(size + 1);
        memcpy(output, (char*)plaintext + 4, size);
        strcpy(extension, (char*)plaintext + 4 + size);
        free(plaintext);
    }
    printf("dumping output...");
    // Open output file
    char* output_filename = malloc(strlen(args.output_file) + strlen(extension) + 1);
    strcpy(output_filename, args.output_file);
    strcat(output_filename, extension);
    FILE* output_file = fopen(output_filename, "wb");
    fwrite(output, size, 1, output_file);
    fclose(output_file);
    fclose(carrier);
}
