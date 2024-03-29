/*
 * File:   xTea.h
 * Author: darshan
 *
 * Created on 24 febbraio 2011, 13.17
 */

#pragma once

#ifdef _WIN32
#define xTeaLib_EXPORT __declspec(dllexport)
#else
#define xTeaLib_EXPORT
#endif

#include <fstream>
#include <iostream>

//! This method is just used for testing the library integration with conan. it does nothing usefull, just print some compiler info
void xTeaLib_EXPORT dummy();

/**
 * PCKS5 padding (https://www.ietf.org/rfc/rfc1423.txt https://www.di-mgt.com.au/cryptopad.html )
 */
class StreamPadder
{
public:
  StreamPadder(std::ifstream *stream);
  ~StreamPadder();
  int ReadBlockPad(uint64_t *blocco);
  int ReadBlock(uint64_t *blocco);
  void Close();
  short UnPad(const char *data);

private:
  std::ifstream *reader_;
  short pad_ = 0;
};

class xTea
{
public:
  static const uint64_t iv = 0x1234567890abcdef;
  static const int round = 64;

  xTea();
  ~xTea();

  int Setup(const char *inputFile, const char *outputFile, const uint32_t *chiave);
  bool Encode(bool cbc = false);
  bool Decode(bool cbc = false);
  bool Dup(bool pad);

private:
  void encipher(uint64_t *blocco, bool cbc);
  void decipher(uint64_t *blocco, bool cbc);
  uint32_t chiave_[4];
  uint64_t oldblock_;
  std::ifstream reader_;
  std::ofstream writer_;
};
