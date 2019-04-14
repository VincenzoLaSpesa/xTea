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
void Encipher(unsigned int num_rounds, uint32_t v[2], uint32_t const k[4]) {
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
void Decipher(unsigned int num_rounds, uint32_t v[2], uint32_t const k[4]) {
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
	this->reader_.open(output, std::ios::binary | std::ios::in);
	this->writer_.open(output, std::ios::binary | std::ios::out);	
	memcpy(chiave_, chiave, 4 * sizeof(uint32_t));
    return !(writer_.bad() || reader_.bad());
}

bool xTea::Encode(bool cbc) {
	uint64_t blocco;
	char *data = (char*)&blocco;
    uint32_t *pezzi;
    pezzi = (uint32_t *)&blocco;
	short blockSize;
	while (reader_.read(data, sizeof(uint64_t))) {
		blockSize = reader_.gcount();
		if (reader_.eof()) //Pad last block
			blockSize=pad(data, blockSize);
		encipher(pezzi);
		if (cbc) {
			blocco = blocco ^ oldblock_;
			oldblock_ = blocco;
		}
		writer_.write(data, sizeof(uint64_t));
		if (blockSize < 0) { //source is not paddable, add a dummy block
			blocco = 0x0808080808080808;
			writer_.write(data, sizeof(uint64_t));
		}
	}
	writer_.close();
	reader_.close();
    return true;
}

bool xTea::Decode(bool cbc) {
	uint64_t blocco;
	char *data = (char*)&blocco;
	uint32_t *pezzi;
    pezzi = (uint32_t *) &blocco;
	short blockSize;
	while (reader_.read(data, sizeof(uint64_t))) {
		if (reader_.gcount() < sizeof(uint64_t))
			return false; // This is bad! this file should be padded
		decipher(pezzi);
		reader_.peek();
		if (reader_.eof()) // Remove padding from last block
			blockSize = unPad(data);
		writer_.write(data, blockSize);
	}
	writer_.close();
	reader_.close();
    return true;
}

bool xTea::Dup(bool isInputPadded){
	uint64_t blocco;
	char *data = (char*)&blocco;
	uint32_t *pezzi;
	pezzi = (uint32_t *)&blocco;
	short blockSize;
	while (reader_.read(data, sizeof(uint64_t))) {
		blockSize = reader_.gcount();
		if (!isInputPadded) {
			if(reader_.eof())//Pad last block
				blockSize = pad(data, blockSize);
		}
		else {
			if (blockSize < sizeof(uint64_t))
				return false; // This is bad! this file should be padded
			reader_.peek();
			if (reader_.eof()) // Remove padding from last block
				blockSize = unPad(data);
		}
		writer_.write(data, sizeof(uint64_t));
		if (blockSize < 0) { //source is not paddable, add a dummy block
			blocco = 0x0808080808080808;
			writer_.write(data, sizeof(uint64_t));
		}
	}
	writer_.close();
	reader_.close();
	return true;
}

short xTea::pad(char* data, short blockSize)
{
	short pad = sizeof(uint64_t) - blockSize;
	if (pad == 0) {
		return -1;
	}
	else {
		while (blockSize < sizeof(uint64_t)) {
			data[blockSize] = pad;
			blockSize++;
		}
	}
	return pad;
}

short xTea::unPad(const char *data) const
{
	int stub = data[sizeof(uint64_t) - 1];
	return sizeof(uint64_t) - stub;
}

void xTea::encipher(uint32_t v[2])
{
	Encipher(this->round, v, this->chiave_);
}

void xTea::decipher(uint32_t v[2])
{
	Decipher(this->round, v, this->chiave_);
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
