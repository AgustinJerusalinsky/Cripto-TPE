#ifndef ENCRYPT_H
#define ENCRYPT_H

#define MAX_BLOCK_SIZE  512

int encrypt(
    unsigned char *plaintext, int plaintext_len,
    unsigned char *ciphertext);

int decrypt(
    unsigned char *ciphertext, int ciphertext_len,
    unsigned char *plaintext);

#endif