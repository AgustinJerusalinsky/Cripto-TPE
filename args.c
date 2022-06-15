#include "args.h"

#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    IN = EXTRACT + 1,
    OUT,
    STEG,
    PASS,
};

enum {
    CARRIER = 'p',
    METHOD = 'a',
    MODE = 'm',

};

char* opts = "p:a:m:";  // -p bitmap.bmp [-a aes128] [-m ecb]

static struct option long_options[] =
    {
        {"embed", no_argument, 0, EMBED},      //--embed
        {"in", required_argument, 0, IN},      // --in
        {"out", required_argument, 0, OUT},    // --out
        {"steg", required_argument, 0, STEG},  // --steg
        {"pass", required_argument, 0, PASS},  // --pass
        {"extract", no_argument, 0, EXTRACT},  // --extract
        {0, 0, 0, 0}};

void load_defaults() {
    args.mode = CBC;
    args.enc_method = NONE;
    args.steg_method = LSB1;
}

char* modes[] = {"ECB", "CFB", "OFB", "CBC", 0};
int modes_ids[] = {ECB, CFB, OFB, CBC};

char* steg_methods[] = {"LSB1", "LSB4", "LSBI", 0};
int steg_methods_ids[] = {LSB1, LSB4, LSBI};

char* enc_methods[] = {"AES128", "AES192", "AES256", "DES", 0};
int enc_methods_ids[] = {ECB, CFB, OFB, CBC};

int map(char* names[], int* ids, char* key) {
    int i = 0;
    while (names[i] != 0) {
        if (strcmp(names[i], key) == 0)
            return ids[i];
        i++;
    }
    return -1;
}

void parse_arguments(int argc, char** argv) {
    load_defaults();
    int index = 0, option;
    while (true) {
        option = getopt_long(argc, argv, opts,
                             long_options, &index);

        if (option == -1) return;

        switch (option) {
            case EMBED:
                args.action = EMBED;
                break;
            case EXTRACT:
                args.action = EXTRACT;
                break;
            case IN:
                strcpy(args.input_file, optarg);
                break;
            case OUT:
                strcpy(args.output_file, optarg);
                break;
            case CARRIER:
                strcpy(args.carrier, optarg);
                break;
            case STEG:
                args.steg_method = map(steg_methods, steg_methods_ids, optarg);
                break;
            case PASS:
                strcpy(args.pass, optarg);
                break;
            case METHOD:
                args.enc_method = map(enc_methods, enc_methods_ids, optarg);
                break;
            case MODE:
                args.mode = map(modes, modes_ids, optarg);
                break;
        }
    }

    return;
}