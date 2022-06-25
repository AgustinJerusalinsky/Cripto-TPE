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
    EVP_CIPHER_CTX *ctx;

    int len;
    char *pwd = args.pass;
    int ciphertext_len;
    const EVP_CIPHER *cipher = get_cypher();

    int iv_len = EVP_CIPHER_iv_length(cipher);
    int key_len = EVP_CIPHER_key_length(cipher);

    unsigned char *iv = malloc(iv_len);

    unsigned char *key = malloc(key_len);

    unsigned char *aad = (unsigned char *)"";
    int aad_len = 0;
    unsigned char *tag = NULL;

    if (iv == NULL || key == NULL) {
        printf("error creating iv and key buffer\n");
        abort();
    }

    get_key_iv(cipher, pwd, key, iv);

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /* Initialise the encryption operation. */
    if (1 != EVP_EncryptInit_ex(ctx, cipher, NULL, NULL, NULL))
        handleErrors();

    /*
     * Set IV length if default 12 bytes (96 bits) is not appropriate
     */
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL))
        handleErrors();

    /* Initialise key and IV */
    if (1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv))
        handleErrors();

    /*
     * Provide any AAD data. This can be called zero or more times as
     * required
     */
    if (1 != EVP_EncryptUpdate(ctx, NULL, &len, aad, aad_len))
        handleErrors();

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    /*
     * Finalise the encryption. Normally ciphertext bytes may be written at
     * this stage, but this does not occur in GCM mode
     */
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    /* Get the tag */
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag))
        handleErrors();

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int decrypt(
    unsigned char *ciphertext, int ciphertext_len,
    unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    int ret;
    char *pwd = args.pass;

    const EVP_CIPHER *cipher = get_cypher();

    printf("cipher: %s\n", EVP_CIPHER_name(cipher));

    int iv_len = EVP_CIPHER_iv_length(cipher);
    int key_len = EVP_CIPHER_key_length(cipher);

    unsigned char *iv = malloc(iv_len);

    unsigned char *key = malloc(key_len);

    unsigned char *aad = (unsigned char *)"";
    int aad_len = 0;
    unsigned char *tag = NULL;

    if (iv == NULL || key == NULL) {
        printf("error creating iv and key buffer\n");
        abort();
    }

    get_key_iv(cipher, pwd, key, iv);

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /* Initialise the decryption operation. */
    if (!EVP_DecryptInit_ex(ctx, cipher, NULL, NULL, NULL))
        handleErrors();

    /* Set IV length. Not necessary if this is 12 bytes (96 bits) */
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL))
        handleErrors();

    /* Initialise key and IV */
    if (!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv))
        handleErrors();

    /*
     * Provide any AAD data. This can be called zero or more times as
     * required
     */
    if (!EVP_DecryptUpdate(ctx, NULL, &len, aad, aad_len))
        handleErrors();

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */
    if (!EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    /* Set expected tag value. Works in OpenSSL 1.0.1d and later */
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag))
        handleErrors();

    /*
     * Finalise the decryption. A positive return value indicates success,
     * anything else is a failure - the plaintext is not trustworthy.
     */
    ret = EVP_DecryptFinal_ex(ctx, plaintext + len, &len);

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    if (ret > 0) {
        /* Success */
        plaintext_len += len;
        return plaintext_len;
    } else {
        /* Verify failed */
        return -1;
    }
}

void get_key_iv(const EVP_CIPHER *cipher, char *pwd, unsigned char *key, unsigned char *iv) {
    printf("pwd: %s\n", pwd);
    EVP_BytesToKey(cipher, EVP_sha256(), NULL, (unsigned char *)pwd, strlen(pwd), 1, key, iv);
}
