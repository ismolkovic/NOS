/*
    Datoteka koja sadrzi sve potrebne konstante
    i tipove podataka koji se koriste
*/

#ifndef LAB3_HEADER
#define LAB3_HEADER 

#include <cstdio>
#include <cstdlib>
#include <cstring>

typedef unsigned char byte;
typedef unsigned long long int ivType;

#define AES_BLOCK 16 // koliko bajtova
#define AES_WORD 4 // koliko ima bajtova u stupcu

#define AES_128 16
#define AES_192 24
#define AES_256 32
 
#define BYTE_MAX 256
#define ECB_KEY 0
#define CTR_KEY 1


#endif