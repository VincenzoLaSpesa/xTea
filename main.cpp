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
    char *input="/home/darshan/Codice/xTea/amleto.txt";
    char *output="/home/darshan/Codice/xTea/output.hex";
    char *replain="/home/darshan/Codice/xTea/replain.txt";
    char *outputcbc="/home/darshan/Codice/xTea/output_cbc.hex";
    char *replaincbc="/home/darshan/Codice/xTea/replain_cbc.txt";
    char *chiavestr="anticlericalismo";//128 bit

    uint32_t *k=(uint32_t *)(&chiavestr[0]);
//encodo
    xTea *coder= new xTea();
    coder->setup(input, output,k);
    coder->encode();
//decodo
    coder= new xTea();
    coder->setup(output, replain,k);
    coder->decode();
//encodocbc
    coder= new xTea();
    coder->setup(input, outputcbc,k);
    coder->CBCencode();
//decodocbc
    coder= new xTea();
    coder->setup(outputcbc,replaincbc,k);
    coder->CBCdecode();

#else
    if(argc<3){
	fprintf(stderr, "%s: operando mancante\n", argv[0]);
	return -1;
	}

	//questo è ancora da implementare

#endif
    return EXIT_SUCCESS;
};
