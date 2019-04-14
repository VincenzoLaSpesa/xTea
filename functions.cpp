#include "functions.h"
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdio.h>

inline int hexchar(char carattere){
    if(carattere=='0')return 0;
    if(carattere=='1')return 1;
    if(carattere=='2')return 2;
    if(carattere=='3')return 3;
    if(carattere=='4')return 4;
    if(carattere=='5')return 5;
    if(carattere=='6')return 6;
    if(carattere=='7')return 7;
    if(carattere=='8')return 8;
    if(carattere=='9')return 9;
    if(carattere=='A' || carattere=='a')return 10;
    if(carattere=='B' || carattere=='b')return 11;
    if(carattere=='C' || carattere=='c')return 12;
    if(carattere=='D' || carattere=='d')return 13;
    if(carattere=='E' || carattere=='e')return 14;
    if(carattere=='F' || carattere=='f')return 15;
    return -100;
};

int hextoary(char* hexstring,int* buffer){
    int len=strlen(hexstring);

    for(int a=0;a<len;a=a+2){
        buffer[a/2]=hexchar(hexstring[a])*16+hexchar(hexstring[a+1]);
	if (buffer[a/2]<0) return -1;
    }
    return 1;
};