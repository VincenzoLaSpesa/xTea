/*
 * File:   xTea.h
 * Author: darshan
 *
 * Created on 24 febbraio 2011, 13.17
 */

#ifndef _XTEA_H
#define	_XTEA_H

#include <stdint.h>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <stdio.h>

class xTea {
public:
    static const uint64_t iv=0x1234567890abcdef;
    static const int round=64;

    uint32_t *chiave;
    xTea();
    xTea(const xTea& orig);
    virtual ~xTea();

    bool setup(const char *input, const char*output, uint32_t *chiave);
    //uint32_t* getkey(char *string);
    bool encode();
    bool decode();
    bool CBCencode();
    bool CBCdecode();

private:
    int readNextBlock(uint64_t &blocco, const bool doPad=false);
    int pos;
    int size;
    uint64_t oldblock;
    FILE *input;
    FILE *output;
};

class DataLayer{
public:
    DataLayer(const char *filename, const bool intentWrite);
    ~DataLayer();
    bool writeBlock(const uint64_t blocco);
    bool readBlock(uint64_t &blocco);
    void close();
private:
    FILE *file;
    short pad = -1;
};

#endif	/* _XTEA_H */

