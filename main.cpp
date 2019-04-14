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
#include <direct.h>

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

std::string GetCurrentWorkingDir(void) {
	char buff[FILENAME_MAX];
	_getcwd(buff, FILENAME_MAX);
	return std::string{ buff };
}
//


int main(int argc, char *argv[])
{


#ifdef DEBUG
	std::cout << "Debug Mode!\n" << "running from " << GetCurrentWorkingDir().c_str() << std::endl;
    const char *input="in.txt";
	const char *dup = "dup.hex";
	const char *dup2 = "dup2.txt";
	const char *output="output.hex";
    const char *replain="replain.txt";
    const char *outputcbc="output_cbc.hex";
    const char *replaincbc="replain_cbc.txt";
    const char *chiavestr="incomprehensible";//128 bit

    uint32_t *k=(uint32_t *)(&chiavestr[0]);
	xTea xtea{};
	bool flag;
//dup (testo il padding)
	printf("Dup1 \n");
	flag = xtea.Setup(input, dup, k);
	if (!flag) return -1;
	xtea.Dup(false);
	printf("Dup2 \n");
	flag = xtea.Setup(dup,dup2, k);
	if (!flag) return -1;
	xtea.Dup(true);
//encodo
	printf("Encode plain \n");
    flag=xtea.Setup(input, output,k);
	if (!flag) return -1;
    xtea.Encode();
//decodo
	printf("Decode plain\n");
	flag = xtea.Setup(output, replain,k);
	if (!flag) return -1;
	xtea.Decode();
//encodocbc
	printf("Encode cbc\n");
	flag = xtea.Setup(input, outputcbc,k);
	if (!flag) return -1;
	xtea.CBCEncode();
//decodocbc
	printf("Decode cbc\n");
	flag = xtea.Setup(outputcbc,replaincbc,k);
	if (!flag) return -1;
	xtea.CBCDecode();


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
