#ifndef AES_HEADER
#define AES_HEADER 

#include "common.h"
#include <iostream>
#include <cstring>
using namespace std;

// osnovna klasa AES
class AES {
  private:
    byte *roundKey;
    long keySize;

    // metode za ekspanziju kljuca 
    void keyExpansion(byte *key, long);
    void RotWordXCon(byte**, int);
    void RotateWord(byte**, int);
    long numberOfSubKeys();

    // metode za kriptiranje
    long numberOfRounds();
    void addRoundKey(byte**, int);
    void subBytes(byte**);
    void shiftRows(byte**);
    void mixColumns(byte**);
    void rotateRow(byte**, int);
    void computeColumn(byte**);


    // metode za dekriptiranje
    void invSubBytes(byte**);
    void invShiftRows(byte**);
    void invMixColumns(byte**);
    void invRotRow(byte**, int);
    void computeInvColumn(byte**);
    byte mulGF(byte, byte);

  public:
    AES(byte* key, long);
    ~AES() { delete[] roundKey; }
    void encrypt(byte*, byte**);
    void decrypt(byte*, byte**);
};

#endif