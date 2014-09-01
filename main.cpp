/*
 * File:   Main.cpp
 * Author: darshan
 *
 * Created on 24 febbraio 2011, 13.09
 */

#include <cstdlib>
#include <iostream>
#include <cstring>
#include <stdio.h>

#define DEBUG

#include "functions.h"
#include "xTea.h"
//using namespace std;

int main(int argc, char *argv[])
{


#ifdef DEBUG
    printf("Debug Mode!\n");
    char *chiavestr="anticlericalismo";//128 bit
    char *input="/home/darshan/Codice/xTea/amleto.txt";
    char *output="/home/darshan/Codice/xTea/output.hex";
    char *replain="/home/darshan/Codice/xTea/replain.txt";

    uint32_t *k=(uint32_t *)(&chiavestr[0]);
    xTea *coder= new xTea();
    coder->setup(input, output,k);
    coder->encode();
//
    coder= new xTea();
    coder->setup(output, replain,k);
    coder->decode();

#else
    FILE *input,*output;
    char* patternstr=argv[1];
    input =stdin;
    output =stdout;
    if(argc<2){
	fprintf(stderr, "%s: operando mancante\n", argv[0]);
	return -1;
	}
#endif
    return EXIT_SUCCESS;
};
