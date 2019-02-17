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

#include "functions.h"
#include "xTea.h"

//http://stackoverflow.com/a/868894

char* getCmdOption(char** begin, char** end, const std::string & option) {
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}
//

int main(int argc, char *argv[]) {
#ifdef DEBUG
    printf("Debug Mode!\n");
    const char *input = "./in.txt";
    const char *output = "./out.hex";
    const char *replain = "./replain.txt";
    const char *outputcbc = "./output_cbc.hex";
    const char *replaincbc = "./replain_cbc.txt";
    const char *chiavestr = "anticlericalismo"; //128 bit

    uint32_t *k = (uint32_t *) (&chiavestr[0]);
    //encodo
    xTea coder = xTea();
    coder.setup(input, output, k);
    coder.encode();

    //decodo
    coder = xTea();
    coder.setup(output, replain, k);
    coder.decode();

    //encodocbc
    coder = xTea();
    coder.setup(input, outputcbc, k);
    coder.CBCencode();

    //decodocbc
    coder = xTea();
    coder.setup(outputcbc, replaincbc, k);
    coder.CBCdecode();

#else
    if (!(cmdOptionExists(argv, argv + argc, "-i") && cmdOptionExists(argv, argv + argc, "-o"))) {
        fprintf(stderr, "Parametri non validi:\n");
        fprintf(stderr, "	xTea.bin -i input -o output opzioni\n");
        fprintf(stderr, "opzioni può essere: \n");
        fprintf(stderr, "	-e per codificare \n");
        fprintf(stderr, "	-d per decodificare \n");
        fprintf(stderr, "	-cbc per attivare cbc \n");
        return -1;
    }

    //questo è ancora da implementare

    xTea *coder = new xTea();

#endif
    return EXIT_SUCCESS;
};