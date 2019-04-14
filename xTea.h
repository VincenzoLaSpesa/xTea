/*
 * File:   xTea.h
 * Author: darshan
 *
 * Created on 24 febbraio 2011, 13.17
 */

#pragma once

#include <iostream>
#include <fstream>

/**
* PCKS5 padding (https://www.ietf.org/rfc/rfc1423.txt https://www.di-mgt.com.au/cryptopad.html )
*/
class DataLayer{
public:
    DataLayer();
    DataLayer(const char *filename);
    ~DataLayer();
    bool Setup(const char *filename);
    int ReadBlock(uint64_t *blocco);
	void SetPaddingStatus(bool isSourcePadded);
    void close();
private:
	std::ifstream file_;
    short pad_ = 0;
	bool isSourcePadded_ = false;
};

class xTea {
public:
    static const uint64_t iv=0x1234567890abcdef;
    static const int round=64;

    xTea();
	~xTea();

    bool Setup(const char *input, const char*output,const uint32_t *chiave);
    bool Encode();
    bool Decode();
    bool CBCEncode();
    bool CBCDecode();
	bool Dup(bool isInputPadded);
private:
	//! Apply PCKS5 padding (https://www.ietf.org/rfc/rfc1423.txt https://www.di-mgt.com.au/cryptopad.html )
	short pad(uint64_t *blocco, short blockSize);
	//! Remove PCKS5 padding 
	short unPad(const uint64_t *blocco) const;
	uint32_t chiave_[4];
	uint64_t oldblock_;
	std::ifstream reader_;
	std::ofstream writer_;
};

