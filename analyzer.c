#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_EXTENSION_LENGTH 2024

int main(int argc, char *argv[]){
    // 2 files arguments, 1st file is original, 2nd file is a carrier
    if(argc != 3){
        printf("Usage: %s <original file> <carrier file>\n", argv[0]);
        return 1;
    }
    // open files
    FILE *original = fopen(argv[1], "r");
    FILE *carrier = fopen(argv[2], "r");
    if(original == NULL || carrier == NULL){
        printf("Error opening files\n");
        return 1;
    }
    
    int differ_bits[8] = {0};

    uint8_t byte_img1;
    uint8_t byte_img2;
    int n = 8;
    while (fread(&byte_img1, 1, 1, original) && fread(&byte_img2, 1, 1, carrier)) {
        
        
        uint8_t bit_img1= 0;
        uint8_t bit_img2= 0;
        for (int i = 0; i < n; i++) {
            bit_img1 = byte_img1 & 0x01;
            bit_img2 = byte_img2 & 0x01;
            if (bit_img1 != bit_img2)
               differ_bits[7-i]++;
                
            byte_img1 = (byte_img1 >> 1);
            byte_img2 = (byte_img2 >> 1);
        }

    }


    // while (fread(&byte_img2, 1, 1, carrier))
    //     differ_bits++;

    //printf("Total bit difference %d\n", differ_bits);
    int sum = 0;
    for (int i = 0; i < 8; i++) {
        sum += differ_bits[i];
        printf("%d ", differ_bits[i]);
    }
    printf("Total bit difference %d\n", sum);

}
