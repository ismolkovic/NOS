#include "mode.h"

/*
     Metoda koja radi ECB kriptiranje nad ulaznim podacima
     @param input - ulazni podaci, kojima se doda padding
     @param iSize - velicina padotaka za kriptiranje
     @param key - kljuc s kojim se kriptira
     @param kSize - duljina kljuca
     @param output - lokacija gdje se premaju podaci

     return - metoda vraca novu velicinu podataka
*/
long ECB_ENCRYPT(byte *input, long iSize, byte *key, long kSize, byte** output) {

  // najprije napravimo padding;
  byte *data;
  long dSize = padding(input, &data, iSize); // dobili smo poravnate podatke

  byte *block = new byte[ AES_BLOCK ];
  AES algo(key, kSize);

  for(int i = 0; i < dSize; i += AES_BLOCK) {
    byte *outBlock;
    memcpy(block, data + i, AES_BLOCK * sizeof(byte));
    
    algo.encrypt(block, &outBlock);
    
    memcpy(data + i, outBlock, AES_BLOCK * sizeof(byte));
    delete[] outBlock;
  }

  *output = data;
  delete[] block;
  return dSize;
}

/*
     Metoda koja radi ECB dekriptiranje nad ulaznim podacima
     @param input - ulazni podaci
     @param iSize - velicina padotaka za dekriptiranje
     @param key - kljuc s kojim se dekriptira
     @param kSize - duljina kljuca
     @param output - lokacija gdje se premaju podaci
  
     return - metoda vraca novu velicinu podataka
*/
long ECB_DECRYPT(byte *input, long iSize, byte *key, long kSize, byte** output) {

  byte *data = new byte[ iSize ]; // kopiramo ulazne podatke
  memcpy(data, input, iSize * sizeof(byte));

  byte *block = new byte[ AES_BLOCK ];
  AES algo(key, kSize);
  
  for(int i = 0; i < iSize; i += AES_BLOCK) {
    byte *outBlock;
    memcpy(block, data + i, AES_BLOCK * sizeof(byte));
    
    algo.decrypt(block, &outBlock);

    memcpy(data + i, outBlock, AES_BLOCK * sizeof(byte));
    delete[] outBlock;
  }

  byte *tOut;
  long dSize = unpadd(data, &tOut, iSize);
  
  delete[] data;
  delete[] block;
  *output = tOut;

  return dSize;
}
/*
    Metoda koja radi CTR kriptiranje nad ulaznim podacima
    @param in - ulazni podaci
    @param iSize - velicina ulaznih podatak
    @parame key - kljuc kojim se kriptira
    @param kSize - velicina kljuca
    @param IV - vraca iv kako bi se kasnije mogao zapisati
    @param out - lokacija podataka gdje se zapise izlaz
*/
long CTR_ENCRYPT(byte *in, long iSize, byte *key, long kSize, byte *IV, byte **out) {
  // najprije napravimo padding;
  byte *plainText;
  long dSize = padding(in, &plainText, iSize); // dobili smo poravnate podatke
  *out = new byte[ dSize ]; // isto toliko mjesta i za izlaz


  byte *currentIV = new byte[ AES_BLOCK ];
  memcpy(currentIV, IV, AES_BLOCK * sizeof(byte));

  AES algo(key, kSize);

  for(int i = 0; i < dSize/AES_BLOCK; i++) {
    byte *outBlock;

    algo.encrypt(currentIV, &outBlock); // kriptiramo vektor
    memcpy((*out) + i * AES_BLOCK, outBlock, AES_BLOCK * sizeof(byte));

    delete[] outBlock;
    ivAddOne(&currentIV);
  }

  // sadda jos treba napraviti xor podataka
  for(int i = 0; i < dSize; i++) {
    (*out)[ i ] = (*out)[ i ] ^ plainText[ i ];
  }

  delete[] currentIV;
  delete[] plainText;
  return dSize;
}

/*
    Metoda koja provid CTR dekriptiranje
    @param in - ulazni podaci
    @param iSize - velicina ulaznih podataka
    @param key - kljuc koji se koristi za dekriptiranje
    @param kSize - velicina kljuca
    @param IV - inicijalizacijski vektor koji se koristi
    @param out - lokacija odredista
*/
long CTR_DECRYPT(byte *in, long iSize, byte *key, long kSize, byte *IV, byte **out) {
  byte *data = new byte[ iSize ]; // napravimo dovoljno mjesta

  byte *currentIV = new byte[ AES_BLOCK ];
  memcpy(currentIV, IV, AES_BLOCK * sizeof(byte)); // kopiramo iv
  AES algo(key, kSize); // napravimo AES objekt

  for(int i = 0; i < iSize/AES_BLOCK; i++) {
    byte *outBlock;
    
    algo.encrypt(currentIV, &outBlock); // kriptiramo vektor
    memcpy(data + i * AES_BLOCK, outBlock, AES_BLOCK * sizeof(byte));
  
    delete[] outBlock;
    ivAddOne(&currentIV);
  }  

  // sada smo generirali podatke još treba napraviti xor
  for(int i = 0; i < iSize; i++) {
    data[ i ] = data[ i ] ^ in[ i ];
  }

  // sada još treba maknuti nadopunu
  byte *tOut;
  long dSize = unpadd(data, &tOut, iSize);

  *out = tOut;
  delete[] currentIV;
  delete[] data; // pobrisemo podatke

  return dSize;
}


/*
    Metoda koja radi +1 na trenutni vektor
*/
void ivAddOne(byte **iv) {
  ivType a;
  memcpy(&a, (*iv) + 8, 8 *sizeof(byte));
  a++;
  memcpy((*iv) + 8, &a, 8 * sizeof(byte));
}

/*  
    Metoda koja radi padding nad ulaznim podacima
    @param input - podaci koji se prosiruju
    @param size - duljina ulaznih podataka
    @param output - odrediste podataka
*/
long padding(byte *input, byte** output, long size) {
  int len; // koliko treba nadopuniti;
  if (size < AES_BLOCK) 
    len = AES_BLOCK - size;
  else if( size % AES_BLOCK == 0)
    len = AES_BLOCK;
  else
    len = (((int)size/AES_BLOCK) + 1) * AES_BLOCK - size;

  *output = new byte[ size + len ];
  memcpy(*output, input, sizeof(byte) * size);

  for(int i = size; i < size +  len ; i++){
    (*output)[i] = (byte)len; 
  }

  return size + len;
}

/*  
    Metoda koja makne nadopunu nad ulaznim podacima
    @param input - ulazni podaci
    @param size - duljina ulaznih podataka
    @param output - odrediste podataka
*/
long unpadd(byte* input, byte** output, long size) {
  int len = (int)input[ size -1 ];

  if(len > 16) {
    *output = input;
    return size;
  }

  *output = new byte[ size - len ];
  memcpy(*output, input, (size-len) * sizeof(byte));
  return size - len;
}

