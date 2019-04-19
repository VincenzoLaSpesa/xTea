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
#include <cassert>
/**
 * Implementazione originale di David Wheeler e Roger Needham corretta in modo da utilizzare interi da 32bit su ogni architettura
 * @param num_rounds
 * @param v
 * @param k
 */
inline void Encipher(unsigned int num_rounds, uint64_t *blocco, uint32_t const k[4]) {
    unsigned int i;
    uint32_t* v = (uint32_t*)blocco;
    uint32_t v0 = v[0], v1 = v[1], sum = 0, const delta = 0x9E3779B9;
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
    uint32_t* v = (uint32_t*)blocco;
    uint32_t v0 = v[0], v1 = v[1], const delta = 0x9E3779B9, sum = delta * num_rounds;
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

xTea::~xTea() {
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
    this->reader_.open(input, std::ios::binary | std::ios::in);
    this->writer_.open(output, std::ios::binary | std::ios::out);
    memcpy(chiave_, chiave, 4 * sizeof(uint32_t));
    return !(writer_.bad() || reader_.bad());
}

bool xTea::Encode(bool cbc) {
    uint64_t blocco;
    char *data = (char*)&blocco;
    uint32_t *pezzi;
    pezzi = (uint32_t *)&blocco;
    StreamPadder padder{ &reader_ };
    short blockSize = padder.ReadBlockPad(&blocco);
    while (blockSize > 0) {
        encipher(&blocco, cbc);
        writer_.write(data, sizeof(uint64_t));
        blockSize = padder.ReadBlockPad(&blocco);
    }
    writer_.close();
    return true;
}

bool xTea::Decode(bool cbc) {
    uint64_t blocco;
    char *data = (char*)&blocco;
    StreamPadder padder{ &reader_ };
    short blockSize = padder.ReadBlock(&blocco);
    while (blockSize > 0) {
        assert(blockSize >= sizeof(uint64_t));
        if (blockSize < sizeof(uint64_t))
            return false; // This is bad! this file should be padded
        decipher(&blocco, cbc);
        short size = padder.UnPad(data);
        writer_.write(data, size);
        blockSize = padder.ReadBlock(&blocco);
    }
    writer_.close();
    return true;
}

bool xTea::Dup(bool pad) {
    uint64_t blocco;
    char *data = (char*)&blocco;
    uint32_t *pezzi;
    pezzi = (uint32_t *)&blocco;
    StreamPadder padder{ &reader_ };
    if (pad) {
        short blockSize = padder.ReadBlockPad(&blocco);
        while (blockSize > 0) {
            writer_.write(data, sizeof(uint64_t));
            blockSize = padder.ReadBlockPad(&blocco);
        }
    } else {
        short blockSize = padder.ReadBlock(&blocco);
        while (blockSize > 0) {
            std::cout << blockSize << std::endl;
            assert(blockSize >= sizeof(uint64_t));
            if (blockSize < sizeof(uint64_t))
                return false; // This is bad! this file should be padded
            short size = padder.UnPad(data);
            writer_.write(data, size);
            blockSize = padder.ReadBlock(&blocco);
        }
    }
    writer_.close();
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

StreamPadder::StreamPadder(std::ifstream *stream) {
    reader_ = stream;
}

StreamPadder::~StreamPadder() {
    Close();
}

void StreamPadder::Close() {
    pad_ = 0;
    reader_->close();
}

short StreamPadder::UnPad(const char * data) {
    reader_->peek();
    if (reader_->eof()) {
        short stub = data[sizeof(uint64_t) - 1];
        std::cout << data << std::endl;
        std::cout << stub << std::endl;
        assert(sizeof(uint64_t) >= stub);
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
    if(!reader_->eof())
        return blockSize;
    while (blockSize < sizeof(uint64_t)) {
        data[blockSize] = pad_;
        blockSize++;
    }
    pad_ = -1;
    return blockSize;
}

int StreamPadder::ReadBlock(uint64_t * blocco) {
    char *data = (char*)blocco;
    reader_->read(data, sizeof(uint64_t));
    return reader_->gcount();
}
