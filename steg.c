#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"
#include "bmp.h"
#include "encrypt.h"

#define MAX_EXTENSION_LENGTH 16

void embed_lsbN_bytes(char* message, int msg_size, FILE* carrier, FILE* output, int n);
void extract_lsbN_bytes(uint8_t* dest, FILE* carrier, uint32_t nbytes, int n);

void embed_lsb1(char* buffer, int size, FILE* carrier, FILE* output) {
    embed_lsbN_bytes(buffer, size, carrier, output, 1);
}
void embed_lsb4(char* buffer, int size, FILE* carrier, FILE* output) {
    embed_lsbN_bytes(buffer, size, carrier, output, 4);
}

void extract_lsb1(uint8_t* dest, FILE* carrier, uint32_t nbytes) {
    extract_lsbN_bytes(dest, carrier, nbytes, 1);
}
void extract_lsb4(uint8_t* dest, FILE* carrier, uint32_t nbytes) {
    extract_lsbN_bytes(dest, carrier, nbytes, 4);
}

uint8_t get_bit(char* message, int bit_index) {
    int byte_index = bit_index / 8;
    int bit_offset = bit_index % 8;
    return (message[byte_index] >> (7 - bit_offset)) & 0x01;
}

char pattern[4];

void extract_lsbi(uint8_t* dest, FILE* carrier, uint32_t nbytes) {
    uint8_t bytes[8];
    int bit;
    int pattern_idx;

    for (uint32_t i = 0; i < nbytes; i++) {
        fread(bytes, 1, 8, carrier);
        uint8_t byte = 0;

        for (int j = 0; j < 8; j++) {
            pattern_idx = (bytes[j] & 0x06) >> 1;
            bit = (bytes[j] & 0x01);
            if (pattern[pattern_idx] > 0)
                bit = bit ^ 0x01;
            byte = (byte << 1) | bit;
        }
        dest[i] = byte;
    }
}

void embed_lsbi(char* message, int msg_size, FILE* carrier, FILE* output) {
    int changed[4] = {0, 0, 0, 0};  // 00,01,10,11
    int count[4] = {0, 0, 0, 0};    // 00,01,10,11
    uint8_t byte;
    int bit_index = 0;
    int max_bit = msg_size * 8;

    // save file offset
    int offset = ftell(carrier);
    
    //jump pattern
    fseek(carrier, offset+4, SEEK_SET);

    while (bit_index < max_bit && fread(&byte, 1, 1, carrier)) {
        uint8_t bit_carrier = 0;
        uint8_t bit_message = 0;

        // Apply lsb1
        // bit from carrier
        bit_carrier = get_bit((char*)&byte, 7) & 0x01;
        // bit from message
        bit_message = get_bit(message, bit_index);
        bit_index++;

        byte = (byte & 0xFE) | bit_message;

        // Check if has changed and Increase or decrease changed
        if (bit_carrier != bit_message) {
            changed[(byte & 0x06) >> 1]++;
        } else {
            changed[(byte & 0x06) >> 1]--;
        }
        count[(byte & 0x06) >> 1]++;
    }

    // check if write is complete
    if (bit_index != max_bit) {
        printf("Error: embed_lsbi: write incomplete\n");
        return;
    }

    // Restore file offset
    fseek(carrier, offset, SEEK_SET);

    printf("Embedding...\n");

    // save pattern
    char pattern[4] = {0, 0, 0, 0};
    for (int i = 0; i < 4; i++) {
        pattern[i] = changed[i] > 0 ? 1 : 0;
    }
    printf("count: %d %d %d %d\n", count[0], count[1], count[2], count[3]);
    printf("changed: %d %d %d %d\n", changed[0], changed[1], changed[2], changed[3]);
    printf("pattern %d %d %d %d\n", pattern[0], pattern[1], pattern[2], pattern[3]);

    char bytes[4];
    fread(bytes, 1, 4, carrier);
    for (int i = 0; i < 4; i++) {
        bytes[i] = (bytes[i] & 0xFE) | pattern[i];
    }
    fwrite(bytes, 1, 4, output);

    byte = 0;
    bit_index = 0;
    while (bit_index < max_bit && fread(&byte, 1, 1, carrier)) {
        uint8_t bit = get_bit(message, bit_index);
        bit_index++;
        byte = (byte & (0xFF << 1)) | bit;
        bit = (byte & 0x06) >> 1;
        if (changed[bit] > 0) {
            byte = byte ^ 0x01;
        }

        fwrite(&byte, 1, 1, output);
    }
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

    // check if write is complete
    if (bit_index != max_bits) {
        printf("Error: embed_lsbi: write incomplete\n");
        return;
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

    char* buffer = malloc(4 + msg_size + MAX_EXTENSION_LENGTH);
    unsigned int index = 0;

    // embed size
    char big_endian_size[4] = {msg_size >> 24, msg_size >> 16, msg_size >> 8, msg_size};
    memcpy(buffer, big_endian_size, 4);
    index += 4;
    // embed_function(big_endian_size, 4, carrier, output);

    // embed message
    // embed_function(message, msg_size, carrier, output);
    memcpy(buffer + index, message, msg_size);
    index += msg_size;

    if (args.enc_method == NONE) {
        // embed extension
        char* extension = strrchr(args.input_file, '.');
        if (!extension) {
            extension = "";
        }
        printf("Extension: %s\n", extension);

        // embed_function(extension, strlen(extension) + 1, carrier, output);  //+1 for null terminated
        memcpy(buffer + index, extension, strlen(extension) + 1);
        index += strlen(extension) + 1;
    }

    embed_function(buffer, index, carrier, output);

    // finish copying
    uint8_t byte;
    while (fread(&byte, 1, 1, carrier)) {
        fwrite(&byte, 1, 1, output);
    }
    free(buffer);
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
    free(message);
    free(header);
}

void extract_lsbN_bytes(uint8_t* dest, FILE* carrier, uint32_t nbytes, int n) {
    uint8_t bytes[8 / n];
    for (uint32_t i = 0; i < nbytes; i++) {
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
    void (*extract_function)(uint8_t*, FILE*, uint32_t);
    char pat[4];
    switch (args.steg_method) {
        case LSB1:
            extract_function = extract_lsb1;
            break;
        case LSB4:
            extract_function = extract_lsb4;
            break;
        case LSBI:
            // extract pattern

            fread(pat, 1, 4, carrier);
            pattern[0] = pat[0] & 0x01;
            pattern[1] = pat[1] & 0x01;
            pattern[2] = pat[2] & 0x01;
            pattern[3] = pat[3] & 0x01;
            printf("Pattern: %x %x %x %x\n", pattern[0], pattern[1], pattern[2], pattern[3]);
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
    for (uint32_t i = 0; i < *size; i++) {
        extract_function((uint8_t*)(output + i), carrier, 1);
    }

    if (args.enc_method != NONE)
        return output;

    printf("extracting extension\n");

    // extract extension
    int i = 0;
    byte = -1;
    while (byte != 0 && i < MAX_EXTENSION_LENGTH) {
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
    printf("offset: %d\n", bmp_file_header.offset);
    fseek(carrier, bmp_file_header.offset, SEEK_SET);

    char extension[MAX_EXTENSION_LENGTH];
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
    free(output);
    free(output_filename);
}
