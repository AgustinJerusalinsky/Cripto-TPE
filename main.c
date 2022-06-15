#include <stdio.h>
#include "args.h"
#include "steg.h"

struct arguments args = {0};

int main(int argc, char * argv[]){
    
    //print all args
    parse_arguments(argc,argv);
    printf("%d\n", argc);
    printf("input: %s\n", args.input_file);
    printf("output: %s\n", args.output_file);
    printf("carrier: %s\n", args.carrier);
    printf("pass: %s\n", args.pass);
    printf("action: %d\n", args.action);
    printf("mode: %d\n", args.mode);
    printf("enc: %d\n", args.enc_method);
    printf("steg: %d\n", args.steg_method);

    //check if all args are set
    if(args.input_file[0]==0 && args.action == EMBED){
        printf("input file not set\n");
        return 1;
    }
    if(args.output_file[0]==0){
        printf("output file not set\n");
        return 1;
    }
    if(args.carrier[0]==0){
        printf("carrier file not set\n");
        return 1;
    }
    if(args.pass[0]==0 && args.enc_method != NONE){
        printf("password not set\n");
        return 1;
    }


    switch (args.action)
    {
    case EMBED:
        embed();
        break;
    case EXTRACT:
        extract();
        break;
    
    default:
        break;
    }

    
}