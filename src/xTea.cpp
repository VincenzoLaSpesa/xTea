/*
 * File:   xTea.cpp
 * Author: darshan
 *
 * Created on 24 febbraio 2011, 13.17
 */

#include "xTea.h"
#include <array>
#include <cassert>
#include <fstream>
#include <ios>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <sstream>

const uint32_t delta = 0x9E3779B9;
/**
 * Implementazione originale di David Wheeler e Roger Needham corretta in modo da utilizzare interi da 32bit su ogni architettura
 * @param num_rounds
 * @param v
 * @param k
 */
inline void Encipher(unsigned int num_rounds, uint64_t *blocco, uint32_t const k[4]) {
  unsigned int i;
  uint32_t *v = (uint32_t *)blocco;
  uint32_t v0 = v[0], v1 = v[1], sum = 0;
  for (i = 0; i < num_rounds; i++) {
    v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
    sum += delta;
    v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum >> 11) & 3]);
  }
  v[0] = v0;
  v[1] = v1;
}

/**
 * Implementazione originale di David Wheeler e Roger Needham corretta in modo da utilizzare interi da 32bit su ogni architettura
 * @param num_rounds
 * @param v
 * @param k
 */
inline void Decipher(unsigned int num_rounds, uint64_t *blocco, uint32_t const k[4]) {
  unsigned int i;
  uint32_t *v = (uint32_t *)blocco;
  uint32_t v0 = v[0], v1 = v[1], sum = delta * num_rounds;
  for (i = 0; i < num_rounds; i++) {
    v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum >> 11) & 3]);
    sum -= delta;
    v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
  }
  v[0] = v0;
  v[1] = v1;
}

xTea::xTea() : reader_(nullptr), writer_(nullptr) {
}

xTea::~xTea() {
}

/**
 * Configura l'algoritmo impostando gli stream di input e output e la chiave da 128bit
 */
int xTea::Setup(std::istream &input, std::ostream &output, const uint32_t *chiave) {
  oldblock_ = ((uint64_t)chiave[0] << 32) | chiave[1];
  this->reader_ = &input;
  this->writer_ = &output;
  memcpy(chiave_, chiave, 4 * sizeof(uint32_t));
  return 0;
}

int xTea::Setup(const uint32_t *chiave) {
  oldblock_ = ((uint64_t)chiave[0] << 32) | chiave[1];
  this->reader_ = nullptr;
  this->writer_ = nullptr;
  memcpy(chiave_, chiave, 4 * sizeof(uint32_t));
  return 0;
}

xTeaFileProcessor::xTeaFileProcessor() {}

xTeaFileProcessor::~xTeaFileProcessor() {
  if (reader_file_.is_open()) reader_file_.close();
  if (writer_file_.is_open()) writer_file_.close();
}

int xTeaFileProcessor::Setup(const char *input, const char *output, const uint32_t *chiave) {
  if (reader_file_.is_open()) reader_file_.close();
  if (writer_file_.is_open()) writer_file_.close();
  this->reader_file_.open(input, std::ios::binary | std::ios::in);
  this->writer_file_.open(output, std::ios::binary | std::ios::out);
  
  if (reader_file_.fail()) return -1;
  if (writer_file_.fail()) return -2;

  return engine_.Setup(reader_file_, writer_file_, chiave);
}

bool xTeaFileProcessor::Encode(bool cbc) {
  return engine_.Encode(cbc);
}

bool xTeaFileProcessor::Decode(bool cbc) {
  return engine_.Decode(cbc);
}

bool xTea::Encode(const std::vector<uint8_t> &input, std::vector<uint8_t> &output, bool cbc) {
  std::string in_str(input.begin(), input.end());
  std::istringstream iss(in_str);
  std::ostringstream oss;

  std::istream *old_reader = reader_;
  std::ostream *old_writer = writer_;

  reader_ = &iss;
  writer_ = &oss;

  bool result = Encode(cbc);

  if (result) {
    std::string out_str = oss.str();
    output.assign(out_str.begin(), out_str.end());
  }

  reader_ = old_reader;
  writer_ = old_writer;

  return result;
}

bool xTea::Decode(const std::vector<uint8_t> &input, std::vector<uint8_t> &output, bool cbc) {
  std::string in_str(input.begin(), input.end());
  std::istringstream iss(in_str);
  std::ostringstream oss;

  std::istream *old_reader = reader_;
  std::ostream *old_writer = writer_;

  reader_ = &iss;
  writer_ = &oss;

  bool result = Decode(cbc);

  if (result) {
    std::string out_str = oss.str();
    output.assign(out_str.begin(), out_str.end());
  }

  reader_ = old_reader;
  writer_ = old_writer;

  return result;
}

bool xTea::Encode(bool cbc) {
  if (!reader_ || !writer_) return false;
  uint64_t blocco;
  char *data = (char*)&blocco;
  uint32_t *pezzi;
  pezzi = (uint32_t*)&blocco;
  StreamPadder padder{reader_};
  short blockSize = padder.ReadBlockPad(&blocco);
  while (blockSize > 0) {
    encipher(&blocco, cbc);
    writer_->write(data, sizeof(uint64_t));
    blockSize = padder.ReadBlockPad(&blocco);
  }
  return true;
}

bool xTea::Decode(bool cbc) {
  if (!reader_ || !writer_) return false;
  uint64_t blocco;
  char *data = (char*)&blocco;
  StreamPadder padder{reader_};
  short blockSize = padder.ReadBlock(&blocco);
  while (blockSize > 0) {
    assert(blockSize >= sizeof(uint64_t));
    if (blockSize < sizeof(uint64_t))
      return false;  // This is bad! this file should be padded
    decipher(&blocco, cbc);
    short size = padder.UnPad(data);
    if (size < 0) // the password is wrong
      return false;
    
    writer_->write(data, size);
    blockSize = padder.ReadBlock(&blocco);
  }
  return true;
}

bool xTea::Dup(bool pad) {
  if (!reader_ || !writer_) return false;
  uint64_t blocco;
  char *data = (char*)&blocco;
  uint32_t *pezzi;
  pezzi = (uint32_t*)&blocco;
  StreamPadder padder{reader_};
  if (pad) {
    short blockSize = padder.ReadBlockPad(&blocco);
    while (blockSize > 0) {
      writer_->write(data, sizeof(uint64_t));
      blockSize = padder.ReadBlockPad(&blocco);
    }
  } else {
    short blockSize = padder.ReadBlock(&blocco);
    while (blockSize > 0) {
      std::cout << blockSize << std::endl;
      assert(blockSize >= sizeof(uint64_t));
      if (blockSize < sizeof(uint64_t))
        return false;  // This is bad! this file should be padded
      short size = padder.UnPad(data);
      writer_->write(data, size);
      blockSize = padder.ReadBlock(&blocco);
    }
  }
  return true;
}

void xTea::encipher(uint64_t *blocco, bool cbc) {
  Encipher(this->round, blocco, this->chiave_);
  if (cbc) {
    *blocco = *blocco ^ oldblock_;
    oldblock_ = *blocco;
  }
}

void xTea::decipher(uint64_t *blocco, bool cbc) {
  if (cbc) {
    uint64_t buffer = *blocco;
    *blocco = *blocco ^ oldblock_;
    Decipher(this->round, blocco, this->chiave_);
    oldblock_ = buffer;
  } else
    Decipher(this->round, blocco, this->chiave_);
}

StreamPadder::StreamPadder(std::istream *stream) {
  reader_ = stream;
}

StreamPadder::~StreamPadder() {
  Close();
}

void StreamPadder::Close() {
  pad_ = 0;
}

short StreamPadder::UnPad(const char *data) {
  reader_->peek();
  if (reader_->eof()) {
    short stub = data[sizeof(uint64_t) - 1];
    //assert(sizeof(uint64_t) >= stub);
    return sizeof(uint64_t) - stub;
  }
  return sizeof(uint64_t);
}

int StreamPadder::ReadBlockPad(uint64_t *blocco) {
  if (pad_ < 0) {
    return -1;
  }
  char *data = (char*)blocco;
  short blockSize = ReadBlock(blocco);
  pad_ = sizeof(uint64_t) - blockSize;
  if (!reader_->eof())
    return blockSize;
  while (blockSize < sizeof(uint64_t)) {
    data[blockSize] = pad_;
    blockSize++;
  }
  pad_ = -1;
  return blockSize;
}

int StreamPadder::ReadBlock(uint64_t *blocco) {
  char *data = (char *)blocco;
  reader_->read(data, sizeof(uint64_t));
  return reader_->gcount();
}

void dummy()
{
#ifdef NDEBUG
    std::cout << "xTeaLib/0.1: Hello World Release!\n";
#else
    std::cout << "xTeaLib/0.1: Hello World Debug!\n";
#endif

// ARCHITECTURES
#ifdef _M_X64
    std::cout << "  xTeaLib/0.1: _M_X64 defined\n";
#endif

#ifdef _M_IX86
    std::cout << "  xTeaLib/0.1: _M_IX86 defined\n";
#endif

#if __i386__
    std::cout << "  xTeaLib/0.1: __i386__ defined\n";
#endif

#if __x86_64__
    std::cout << "  xTeaLib/0.1: __x86_64__ defined\n";
#endif

// Libstdc++
#if defined _GLIBCXX_USE_CXX11_ABI
    std::cout << "  xTeaLib/0.1: _GLIBCXX_USE_CXX11_ABI " << _GLIBCXX_USE_CXX11_ABI << "\n";
#endif

// COMPILER VERSIONS
#if _MSC_VER
    std::cout << "  xTeaLib/0.1: _MSC_VER" << _MSC_VER << "\n";
#endif

#if _MSVC_LANG
    std::cout << "  xTeaLib/0.1: _MSVC_LANG" << _MSVC_LANG << "\n";
#endif

#if __cplusplus
    std::cout << "  xTeaLib/0.1: __cplusplus" << __cplusplus << "\n";
#endif

#if __INTEL_COMPILER
    std::cout << "  xTeaLib/0.1: __INTEL_COMPILER" << __INTEL_COMPILER << "\n";
#endif

#if __GNUC__
    std::cout << "  xTeaLib/0.1: __GNUC__" << __GNUC__ << "\n";
#endif

#if __GNUC_MINOR__
    std::cout << "  xTeaLib/0.1: __GNUC_MINOR__" << __GNUC_MINOR__ << "\n";
#endif

#if __clang_major__
    std::cout << "  xTeaLib/0.1: __clang_major__" << __clang_major__ << "\n";
#endif

#if __clang_minor__
    std::cout << "  xTeaLib/0.1: __clang_minor__" << __clang_minor__ << "\n";
#endif

#if __apple_build_version__
    std::cout << "  xTeaLib/0.1: __apple_build_version__" << __apple_build_version__ << "\n";
#endif

    // SUBSYSTEMS

#if __MSYS__
    std::cout << "  xTeaLib/0.1: __MSYS__" << __MSYS__ << "\n";
#endif

#if __MINGW32__
    std::cout << "  xTeaLib/0.1: __MINGW32__" << __MINGW32__ << "\n";
#endif

#if __MINGW64__
    std::cout << "  xTeaLib/0.1: __MINGW64__" << __MINGW64__ << "\n";
#endif

#if __CYGWIN__
    std::cout << "  xTeaLib/0.1: __CYGWIN__" << __CYGWIN__ << "\n";
#endif
}
