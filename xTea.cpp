/*
 * File:   xTea.cpp
 * Author: darshan
 *
 * Created on 24 febbraio 2011, 13.17
 */

#include "xTea.h"
#include <sys/stat.h>

void encipher(unsigned int num_rounds, uint32_t v[2], uint32_t const k[4]) {
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], sum=0, delta=0x9E3779B9;
    for (i=0; i < num_rounds; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
        sum += delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum>>11) & 3]);
    }
    v[0]=v0; v[1]=v1;
}

void decipher(unsigned int num_rounds, uint32_t v[2], uint32_t const k[4]) {
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], delta=0x9E3779B9, sum=delta*num_rounds;
    for (i=0; i < num_rounds; i++) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum>>11) & 3]);
        sum -= delta;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
    }
    v[0]=v0; v[1]=v1;
}


xTea::xTea() {
}

xTea::xTea(const xTea& orig) {
}

xTea::~xTea() {
	//nothing to do

    //fclose (input) ;
    //fclose (output) ;
}

bool xTea::setup(char *input, char *output,uint32_t *chiave){
    pos=0;
    //oldblock=iv;

    oldblock=chiave[0]+ (chiave[1] >> 32);

    struct stat results;

    if (stat(input, &results) == 0){
        this->size=results.st_size;
    }
    else
        return false;
    this->input=fopen  ( input  , "rb" );
    this->output=fopen  ( output  , "wb" );
    this->chiave=new uint32_t[4];
    this->chiave[0]=chiave[0];
    this->chiave[1]=chiave[1];
    this->chiave[2]=chiave[2];
    this->chiave[3]=chiave[3];

    return true;
}


int xTea::nextblock(uint64_t &blocco){
    blocco=0x5f5f5f5f5f5f5f5f; // carattere _
    int len = fread (&blocco , 1 , sizeof(uint64_t) , input );
    return len;
}

bool xTea::encode(){
    uint64_t blocco;
    uint32_t *pezzi;
    pezzi=(uint32_t *)&blocco;
    while(this->nextblock(blocco)>0){
        encipher(this->round,pezzi,chiave);
        fwrite ((void*)&blocco , 1 , sizeof(blocco) , output );
    }
    fclose(input);
    fclose(output);
    return true;
}

bool xTea::decode(){
    uint64_t blocco;
    uint32_t *pezzi;
    pezzi=(uint32_t *)&blocco;
    while(this->nextblock(blocco)>0){
        decipher(this->round,pezzi,chiave);
        fwrite ((void*)&blocco , 1 , sizeof(blocco) , output );
    }
    fclose(input);
    fclose(output);
    return true;
}

bool xTea::CBCencode(){
    uint64_t blocco;
    uint32_t *pezzi;
    pezzi=(uint32_t *)&blocco;
    while(this->nextblock(blocco)>0){
        encipher(this->round,pezzi,chiave);
        blocco=blocco^oldblock;
        fwrite ((void*)&blocco , 1 , sizeof(blocco) , output );
        oldblock=blocco;
    }
    fclose(input);
    fclose(output);
    return true;
}

bool xTea::CBCdecode(){
    uint64_t blocco;
    uint64_t buffer;
    uint32_t *pezzi;
    pezzi=(uint32_t *)&blocco;

    while(this->nextblock(blocco)>0){
        buffer=blocco;
        blocco=blocco^oldblock;
        decipher(this->round,pezzi,chiave);
        fwrite ((void*)&blocco , 1 , sizeof(blocco) , output );
        oldblock=buffer;
    }
    fclose(input);
    fclose(output);
    return true;
}
