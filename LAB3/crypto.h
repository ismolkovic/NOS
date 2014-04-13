/*
    Zaglavlje glavnog modula za kriptiranje
*/

#ifndef CRYPTO_HEADER
#define CRYPTO_HEADER 

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include <time.h>
using namespace std;

#include "common.h"
#include "mode.h"

// generiranje kljuceva
void generateKey(int, char*, long);
void generateRandomByte(byte**, long);

// za parsiranje ulaaz
long readData(char*, byte**);
void parse(int, int, char*, char*, char*);
long getSubData(string, string, string, byte**);
long getKeyIV(string, string, string, byte**);

// glavna funkcija koja raspoređuje posao
void operation(byte*, byte*, byte*, int, int, long, long, char*, char*);

// funkcije koje provode bas64 kodiranje i dekodiranje
long base64encode(byte*, byte**, long);
long base64decode(byte*, byte**, long);


// zapisivanje sadržaja datoteka
void writeCryptoFile(byte*, long, char*, long, char*);
void writePlainFile(byte*, long, char*);
void writeKeyFile(byte*, long, byte*, long, char*);
void write60chars(byte*, long, FILE *f, int);

#endif