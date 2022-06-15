#include <stdio.h>
#include <stdlib.h>

#include "args.h"
#include "bmp.h"

uint8_t get_bit(char* message, int bit_index) {
    int byte_index = bit_index / 8;
    int bit_offset = bit_index % 8;
    return (message[byte_index] >> (7 - bit_offset)) & 0x01;
}

void lsbN_embed(char* message, int msg_size, FILE* carrier, FILE* output, int n) {
    int bit_index = 0;
    int max_bits = msg_size * 8;
    uint8_t byte;
    while (fread(&byte, 1, 1, carrier)) {
        if (bit_index >= max_bits) {
            fwrite(&byte, 1, 1, output);
        } else {
            uint8_t bits = 0;
            for (int i = 0; i < n; i++) {
                bits = (bits << 1) | get_bit(message, bit_index);
                bit_index++;
            }
            byte = (byte & (0xFF << n)) | bits;
        }
    }
}


void embed_into_carrier(char* message, int msg_size, FILE* carrier, FILE* output) {
    switch (args.steg_method) {
        case LSB1:
            // lsb1(message, carrier, output);
            lsbN_embed(message, msg_size, carrier, output, 1);
            break;
        case LSB4:
            lsbN_embed(message, msg_size, carrier, output, 4);
            // lsb4();
            break;
        case LSBI:
            // lsbi();
            break;
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
    printf("message: %s\n", message);

    // Encrypt if necessary

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


void extract_lsbN_bytes(uint8_t * dest, FILE * carrier, int nbytes, int n){
    uint8_t bytes[8/n];
    for (int i = 0; i < nbytes; i++) {
        fread(bytes, 1, 8/n, carrier);
        uint8_t byte = 0;

        for (int j = 0; j < 8/n; j++) {
            byte = (byte<<n) | (bytes[j] & (0xFF >> (8-n)));
        }
        dest[i] = byte;
    }
}

void extract_lsbN(FILE * carrier, FILE * output, int n) {
    uint8_t size[4];
    extract_lsbN_bytes(size, carrier, 4, n);
    uint32_t real_size = size[0] << 24 | size[1] << 16 | size[2] << 8 | size[3];
    printf("size: %ul\n", real_size);
    for (int i = 0; i < real_size; i++) {
        uint8_t byte;
        extract_lsbN_bytes(&byte, carrier, 1, n);
        fwrite(&byte, 1, 1, output);
    }



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

    FILE* output = fopen(args.output_file, "wb");
    
    
    
    if (args.steg_method == LSB1) {
        extract_lsbN(carrier, output, 1);
    }

    fclose(output);
    fclose(carrier);

    // Decrpyt if necessary
}
