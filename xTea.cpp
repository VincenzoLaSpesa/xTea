/*
 * File:   xTea.cpp
 * Author: darshan
 *
 * Created on 24 febbraio 2011, 13.17
 */

#include "xTea.h"
#include <fstream>
#include <iostream>
#include <ios>
#include <array>
/**
 * Implementazione originale di David Wheeler e Roger Needham corretta in modo da utilizzare interi da 32bit su ogni architettura
 * @param num_rounds
 * @param v
 * @param k
 */
void encipher(unsigned int num_rounds, uint32_t v[2], uint32_t const k[4]) {
    unsigned int i;
    uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x9E3779B9;
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
void decipher(unsigned int num_rounds, uint32_t v[2], uint32_t const k[4]) {
    unsigned int i;
    uint32_t v0 = v[0], v1 = v[1], delta = 0x9E3779B9, sum = delta*num_rounds;
    for (i = 0; i < num_rounds; i++) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum >> 11) & 3]);
        sum -= delta;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
    }
    v[0] = v0;
    v[1] = v1;
}

xTea::xTea() {
}

xTea::~xTea()
{
	reader_.close();
	writer_.close();
}

/**
 * Configura l'algoritmo impostando i file di input e output ( nomi dei file come stringa) e la chiave da 128bit
 * @param input
 * @param output
 * @param chiave
 * @return
 */
bool xTea::Setup(const char *input, const char *output, const uint32_t *chiave) {
    oldblock_ = chiave[0]+ (chiave[1] >> 32);
	bool flag = this->reader_.Setup(input);
	this->writer_.open(output, std::ios::binary | std::ios::out);
	flag &= !writer_.bad();
	memcpy(chiave_, chiave, 4 * sizeof(uint32_t));
    return flag;
}

bool xTea::Encode() {
	uint64_t blocco;
	char *data = (char*)&blocco;
    uint32_t *pezzi;
    pezzi = (uint32_t *)&blocco;
	reader_.SetPaddingStatus(false);
	short blockSize;
	while(blockSize = reader_.ReadBlock(&blocco) , blockSize > 0){
		encipher(this->round, pezzi, chiave_);
		writer_.write(data, blockSize);
	}
	writer_.close();
	reader_.close();
    return true;
}

bool xTea::Decode() {
	uint64_t blocco;
	char *data = (char*)&blocco;
	uint32_t *pezzi;
    pezzi = (uint32_t *) &blocco;
	reader_.SetPaddingStatus(true);
	short blockSize = 0;
	while (blockSize = reader_.ReadBlock(&blocco), blockSize > 0) {
		decipher(this->round, pezzi, chiave_);
		writer_.write(data, blockSize);
		std::cout << data;
	}
	writer_.close();
	reader_.close();
    return true;
}

bool xTea::CBCEncode() {
	uint64_t blocco;
	char *data = (char*)&blocco;
    uint32_t *pezzi;
    pezzi = (uint32_t *) & blocco;
	reader_.SetPaddingStatus(false);
	short blockSize;
	while (blockSize = reader_.ReadBlock(&blocco), blockSize > 0) {
		encipher(this->round, pezzi, chiave_);
		blocco = blocco ^ oldblock_;
		writer_.write(data, blockSize);
		oldblock_ = blocco;
	}
	writer_.close();
	reader_.close();
    return true;
}

bool xTea::CBCDecode() {
	uint64_t blocco;
	char *data = (char*)&blocco;
    uint64_t buffer;
    uint32_t *pezzi;
    pezzi = (uint32_t *)&blocco;
	reader_.SetPaddingStatus(true);
	short blockSize = 0;
	while (blockSize = reader_.ReadBlock(&blocco), blockSize > 0) {
		buffer = blocco;
		blocco = blocco ^ oldblock_;
		decipher(this->round, pezzi, chiave_);
		writer_.write(data, blockSize);
		oldblock_ = buffer;
	}
	writer_.close();
	reader_.close();
    return true;
}

bool xTea::Dup(bool isInputPadded)
{
	uint64_t blocco=0;
	char *data = (char*)&blocco;
	reader_.SetPaddingStatus(isInputPadded);
	short blockSize;
	while (blockSize = reader_.ReadBlock(&blocco), blockSize > 0) {
		writer_.write(data, blockSize);
		std::cout.write(data, blockSize);
		std::cout << std::endl;
	}
	writer_.close();
	reader_.close();
	return true;
}

DataLayer::DataLayer(){
}

bool DataLayer::Setup(const char * filename)
{
	file_.open(filename, std::ios::binary | std::ios::in);
	isSourcePadded_ = false;
	return file_.is_open();
}

DataLayer::DataLayer(const char * filename){
    Setup(filename);
}

DataLayer::~DataLayer()
{
	close();
}

void DataLayer::close()
{
	pad_ = 0;
	file_.close();
}

int DataLayer::ReadBlock(uint64_t *blocco)
{
	*blocco = 0;
	char *data = (char*)blocco;
	file_.read(data, sizeof(uint64_t));
	std::streamsize len = file_.gcount();
    if(file_.eof()){// Reached the end of the file
		if (pad_ < 0)
			return 0;
		if (!isSourcePadded_){ // Source is not padded, create the filling block
			pad_ = sizeof(uint64_t) - len;
			if (pad_ == 0) {
				*blocco = 0x0808080808080808;
			}
			else {
				while (len < sizeof(uint64_t)) {
					data[len] = pad_;
					len++;
				}
			}
			pad_ = -1;
		}
	}
	else if(isSourcePadded_){ // What if we are just before the eof?
		file_.peek();
		if(file_.eof()) { //source is already padded, cut the padding block
			int stub = data[sizeof(uint64_t) - 1];
			pad_ = -1;
			return sizeof(uint64_t) - stub;
		}
	}
	return sizeof(uint64_t);
}

void DataLayer::SetPaddingStatus(bool isSourcePadded)
{
	isSourcePadded_ = isSourcePadded;
}
