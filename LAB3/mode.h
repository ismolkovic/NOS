#ifndef MODE_HEADER
#define MODE_HEADER 

#include "common.h"
#include "aes.h"
#include <cstdlib>
using namespace std;
#include <iostream>

// metode koje se koriste za padding
long padding(byte*, byte**, long);
long unpadd(byte*, byte**, long);

// ECB
long ECB_ENCRYPT(byte*, long, byte*, long, byte**);
long ECB_DECRYPT(byte*, long, byte*, long, byte**);


//CTR
// metode za incijalizacijski vektor
void ivAddOne(byte**);

// metode za kriptiranje/dekriptiranje
long CTR_ENCRYPT(byte*, long, byte*, long, byte*, byte**);
long CTR_DECRYPT(byte*, long, byte*, long, byte*, byte**);

#endif