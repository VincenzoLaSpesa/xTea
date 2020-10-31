/*
 * File:   Main.cpp
 * Author: darshan
 *
 * Created on 24 febbraio 2011, 13.09
 */

#include <stdio.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>


#if __has_include(<direct.h>)
	#include <direct.h>
	#define GetCurrentDir _getcwd
#else
	#include <unistd.h>
	#define GetCurrentDir getcwd
#endif

//#define DEBUG

#include "xTea.h"

//http://stackoverflow.com/a/868894

char *getCmdOption(char **begin, char **end, const std::string &option) {
  char **itr = std::find(begin, end, option);
  if (itr != end && ++itr != end) {
    return *itr;
  }
  return 0;
}

bool cmdOptionExists(char **begin, char **end, const std::string &option) {
  return std::find(begin, end, option) != end;
}

std::string GetCurrentWorkingDir(void) {
  char buff[FILENAME_MAX];
  GetCurrentDir(buff, FILENAME_MAX);
  return std::string{buff};
}
//

int main(int argc, char *argv[]) {
#ifdef DEBUG
  std::cout << "Debug Mode!\n"
            << "running from " << GetCurrentWorkingDir().c_str() << std::endl;
  const char *input = "in.txt";
  const char *dup = "pad.hex";
  const char *dup2 = "unpad.txt";
  const char *output = "output.hex";
  const char *replain = "replain.txt";
  const char *outputcbc = "output_cbc.hex";
  const char *replaincbc = "replain_cbc.txt";
  const char *chiavestr = "incomprehensible";  //128 bit

  uint32_t *k = (uint32_t *)(&chiavestr[0]);
  xTea xtea{};
  bool flag;
  //dup (testo il padding)
  printf("Dup1 \n");
  flag = xtea.Setup(input, dup, k);
  if (!flag)
    return -1;
  xtea.Dup(true);
  printf("Dup2 \n");
  flag = xtea.Setup(dup, dup2, k);
  if (!flag)
    return -1;
  xtea.Dup(false);
  //encodo
  printf("Encode plain \n");
  flag = xtea.Setup(input, output, k);
  if (!flag)
    return -1;
  xtea.Encode(false);
  //decodo
  printf("Decode plain\n");
  flag = xtea.Setup(output, replain, k);
  if (!flag)
    return -1;
  xtea.Decode(false);
  //encodocbc
  printf("Encode cbc\n");
  flag = xtea.Setup(input, outputcbc, k);
  if (!flag)
    return -1;
  xtea.Encode(true);
  //decodocbc
  printf("Decode cbc\n");
  flag = xtea.Setup(outputcbc, replaincbc, k);
  if (!flag)
    return -1;
  xtea.Decode(true);

#else
  if (!(cmdOptionExists(argv, argv + argc, "-i") &&
        cmdOptionExists(argv, argv + argc, "-o") &&
        cmdOptionExists(argv, argv + argc, "-k"))) {
    fprintf(stderr, "Unrecognized syntax:\n");
    fprintf(stderr, "	xTea -i input -o output -k password options\n");
    fprintf(stderr, "supported options are: \n");
    fprintf(stderr, "	-e for encoding \n");
    fprintf(stderr, "	-d for decoding \n");
    fprintf(stderr, "	-cbc for toggling cbc mode \n");
    return -1;
  }

  std::string input_file = getCmdOption(argv, argv + argc, "-i");
  std::string output_file = getCmdOption(argv, argv + argc, "-o");
  std::string password = getCmdOption(argv, argv + argc, "-k");
  uint32_t *k = (uint32_t *)(password.data());
  bool cbc = cmdOptionExists(argv, argv + argc, "-cbc");
  bool encode = cmdOptionExists(argv, argv + argc, "-e");
  bool decode = cmdOptionExists(argv, argv + argc, "-d");

  if (encode && decode) {
    std::cerr << "You can either encode or decode" << std::endl;
    return -1;
  }
  if (!encode && !decode) {
    std::cerr << "No action provided, you have to encode or decode" << std::endl;
    return -1;
  }

  xTea xtea{};
  int flag = xtea.Setup(input_file.c_str(), output_file.c_str(), k);
  if (flag == -1) {
    std::cerr << "Unable to open the input file" << std::endl;
    return -1;
  }
  if (flag == -2) {
    std::cerr << "Unable to open the output file" << std::endl;
    return -1;
  }
  if (encode) {
    if (!xtea.Encode(cbc))
      std::cerr << "Error while encoding" << std::endl;
    return false;
  }
  if (decode) {
    if (!xtea.Decode(cbc))
      std::cerr << "Error while decoding" << std::endl;
    return false;
  }
#endif
  return true;
};
