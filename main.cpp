/*
 * File:   Main.cpp
 * Author: darshan
 *
 * Created on 24 febbraio 2011, 13.09
 */

#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <stdio.h>

#define DEBUG

#include "functions.h"
#include "xTea.h"
//using namespace std;


//http://stackoverflow.com/a/868894

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;

}
//


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
    delete coder;

//decodo
    coder= new xTea();
    coder->setup(output, replain,k);
    coder->decode();
    delete coder;

//encodocbc
    coder= new xTea();
    coder->setup(input, outputcbc,k);
    coder->CBCencode();
    delete coder;

//decodocbc
    coder= new xTea();
    coder->setup(outputcbc,replaincbc,k);
    coder->CBCdecode();
    delete coder;


#else
if(!(cmdOptionExists(argv, argv+argc, "-i") && cmdOptionExists(argv, argv+argc, "-o")))
    {
	fprintf(stderr, "Parametri non validi:\n");
	fprintf(stderr, "	xTea.bin -i input -o output opzioni\n");
	fprintf(stderr, "opzioni può essere: \n");
	fprintf(stderr, "	-e per codificare \n");
	fprintf(stderr, "	-d per decodificare \n");
	fprintf(stderr, "	-cbc per attivare cbc \n");
	return -1;
    }

	//questo è ancora da implementare

    xTea *coder= new xTea();

#endif
    return EXIT_SUCCESS;
};
