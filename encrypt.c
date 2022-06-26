#include <openssl/err.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"

void get_key_iv(const EVP_CIPHER *cipher, char *pwd, unsigned char *key, unsigned char *iv);

void handleErrors(void) {
    ERR_print_errors_fp(stderr);
    abort();
}

int handler(unsigned char *input, int input_len, unsigned char *output, int mode);

const EVP_CIPHER *get_cypher() {
    const EVP_CIPHER *aes128[] = {EVP_aes_128_ecb(), EVP_aes_128_cfb8(), EVP_aes_128_ofb(), EVP_aes_128_cbc()};
    const EVP_CIPHER *aes192[] = {EVP_aes_192_ecb(), EVP_aes_192_cfb8(), EVP_aes_192_ofb(), EVP_aes_192_cbc()};
    const EVP_CIPHER *aes256[] = {EVP_aes_256_ecb(), EVP_aes_256_cfb8(), EVP_aes_256_ofb(), EVP_aes_256_cbc()};
    const EVP_CIPHER *des[] = {EVP_des_ecb(), EVP_des_cfb8(), EVP_des_ofb(), EVP_des_cbc()};
    const EVP_CIPHER **chain_list;

    switch (args.enc_method) {
        case AES128:
            chain_list = aes128;
            break;
        case AES192:
            chain_list = aes192;
            break;
        case AES256:
            chain_list = aes256;
            break;
        case DES:
            chain_list = des;
            break;
        default:
            chain_list = aes128;
            break;
    }
    switch (args.mode) {
        case ECB:
            return chain_list[0];
        case CFB:
            return chain_list[1];
        case OFB:
            return chain_list[2];
        case CBC:
            return chain_list[3];
        default:
            return NULL;
    }
}

int encrypt(
    unsigned char *plaintext, int plaintext_len,
    unsigned char *ciphertext) {
    return handler(plaintext, plaintext_len, ciphertext, 1);
}

int decrypt(
    unsigned char *ciphertext, int ciphertext_len,
    unsigned char *plaintext) {
    return handler(ciphertext, ciphertext_len, plaintext, 0);
}

int handler(unsigned char *input, int input_len, unsigned char *output, int mode) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int output_len;
    int ret;
    char *pwd = args.pass;

    const EVP_CIPHER *cipher = get_cypher();

    printf("cipher: %s\n", EVP_CIPHER_name(cipher));

    int iv_len = EVP_CIPHER_iv_length(cipher);
    int key_len = EVP_CIPHER_key_length(cipher);

    unsigned char iv[iv_len];  // = malloc(iv_len);

    unsigned char key[key_len];  // = malloc(key_len);

    if (iv == NULL || key == NULL) {
        printf("error creating iv and key buffer\n");
        abort();
    }

    get_key_iv(cipher, pwd, key, iv);

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /* Initialise the decryption operation. */
    if (!EVP_CipherInit_ex(ctx, cipher, NULL, key, iv, mode))
        handleErrors();

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */
    if (!EVP_CipherUpdate(ctx, output, &len, input, input_len))
        handleErrors();
    output_len = len;

    /*
     * Finalise the decryption. A positive return value indicates success,
     * anything else is a failure - the plaintext is not trustworthy.
     */
    ret = EVP_CipherFinal_ex(ctx, output + len, &len);

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    if (ret > 0) {
        /* Success */
        output_len += len;
        return output_len;
    } else {
        /* Verify failed */
        return -1;
    }
}

void get_key_iv(const EVP_CIPHER *cipher, char *pwd, unsigned char *key, unsigned char *iv) {
    printf("pwd: %s\n", pwd);
    EVP_BytesToKey(cipher, EVP_sha256(), NULL, (unsigned char *)pwd, strlen(pwd), 1, key, iv);
}
