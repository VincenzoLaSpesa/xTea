/* 
 * File:   functions.h
 * Author: darshan
 *
 * Created on 24 febbraio 2011, 13.12
 */

#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

#define BUFFER (1024*64)*sizeof(char)
//possibilmente 64k è multiplo della dimensione di una pagina.
//così mi evito di usare getpagesize() che non è ANSI
int hextoary(char* hexstring, int* buffer);


#endif /* _FUNCTIONS_H */

