#include "aes.h"
#include "aestable.cc"

/*
  @param key - kljuc s kojim se kriptira
  @param size - duljina kljuca 16,24,32 
*/
AES::AES(byte* key, long size) {
  keySize = size;
  keyExpansion(key, size);
}

/*
  @param input - blok za kriptiranje
  @param output - kriptirani blok
*/
void AES::encrypt(byte *in, byte **out) {
  byte *state = new byte[ AES_BLOCK ];
  memcpy(state, in, sizeof(byte) * AES_BLOCK);

  int Nr = numberOfRounds();
  addRoundKey(&state, 0); // prvo se dodaje prvi podkljuc

  // idemo po predvidenom broju rundi
  for(int rnd = 1; rnd < Nr ; rnd++) {
    subBytes(&state);
    shiftRows(&state);    
    mixColumns(&state);
    addRoundKey(&state, rnd);
  }

  // zadnja runda 
  subBytes(&state);
  shiftRows(&state);
  addRoundKey(&state, Nr);

  *out = state;
}

/*
  @param input - blok za dekriptiranje
  @param output - dekriptirani blok
*/
void AES::decrypt(byte *in, byte **out) {
  byte *state = new byte[ AES_BLOCK ];
  memcpy(state, in, AES_BLOCK * sizeof(byte));

  int Nr = numberOfRounds();
  addRoundKey(&state, Nr);

  // idemo po rundama od kraja prema pocetku
  for(int rnd = Nr -1; rnd > 0; rnd--) {
    invShiftRows(&state);
    invSubBytes(&state);    
    addRoundKey(&state, rnd);    
    invMixColumns(&state);
  }

  // zadnja runda
  invShiftRows(&state);
  invSubBytes(&state);
  addRoundKey(&state, 0);

  *out = state; // brise se tamo odakle se poziva
}

/*
    Metoda koja sluzi za prosirenje kljuca
*/
void AES::keyExpansion(byte *key, long size) {
  long newSize = numberOfSubKeys();
  roundKey = new byte[ newSize ];
  memset(roundKey, 0, newSize);

  // kopiramo stari kljuc
  memcpy(roundKey, key, size * sizeof(byte)); 
  int i = size; // trenutno kolko imamo generirano
  int rconIn = 1; // index u rcon tablici
 
  while(i < newSize) {
    byte *temp = new byte[ 4 ];
    memcpy(temp, roundKey + (i-4) , 4); //uzmemo zadnju rijec

    if(i % size == 0)
      RotWordXCon(&temp, rconIn++);

    // ako je AES_256 kljuc
    if((size == AES_256) && ((i % size) == 16)) {
      for(int j = 0; j < 4; j++) 
        temp[ j ] = sbox[ temp[ j ] ];
    }

    // prosirimo kljuc
    for(int j = 0; j < 4; j++) {
      roundKey[ i ] = roundKey[i - size] ^ temp[ j ];
      i++;
    }
    delete[] temp;
  }
}

/*
   Metoda koja sluzi kod expanzije kljuca kada se generira prva 
   rijec kljuca
*/
void AES::RotWordXCon(byte **input, int index) {
  RotateWord(input, 1);

  for(int i = 0; i < 4; i++) {
    (*input)[ i ] = sbox[ (*input)[ i ] ];
  }

  (*input)[ 0 ] = (*input)[ 0 ] ^ rcon[ (byte) index ];
}

/*
  Metoda koja sluzi za rotaciju za s okteta ulijevo
  @param s - korak 
*/
void AES::RotateWord(byte **input, int s) {
  byte first = (*input)[ 0 ];
  
  for(int i = 0; i < 3; i++) 
    (*input)[ i * s] = (*input)[ (i+1) * s];

  (*input)[ 3 * s ] = first;
}

/*
    Metoda koja vraca broj rundi
*/
long AES::numberOfRounds() {
  long numRound = 0;
  if(keySize == AES_128) numRound = 10;
  if(keySize == AES_192) numRound = 12;
  if(keySize == AES_256) numRound = 14;

  return numRound;
}

/*
    Metoda koja vraca broj riječi u proširenom ključu
*/
long AES::numberOfSubKeys() {
  long newSize = 0;
  if(keySize == AES_128) newSize = (10+1) * 16;
  if(keySize == AES_192) newSize = (12+1) * 16;
  if(keySize == AES_256) newSize = (14+1) * 16;

  return newSize;
}

/*
    Metoda koja dodaje podkljuc trenutnom bloku 
    @param r - broj podkljuca koji se dodaje bloku
*/
void AES::addRoundKey(byte **input, int r) {
  for(int i = 0; i < 16; i++) {
    (*input)[ i ] = (*input)[ i ] ^ roundKey[ r * AES_BLOCK + i];
  }
}

void AES::subBytes(byte **input) {
  for(int i = 0; i < AES_BLOCK; i++) {
    (*input)[ i ] = sbox[ (*input)[ i ] ];
  }
}

void AES::shiftRows(byte **in) {
  rotateRow(in, 1); // drugi red
  rotateRow(in, 2); // treci red
  rotateRow(in, 2); //
  rotateRow(in, 3); // cetvrti red
  rotateRow(in, 3);
  rotateRow(in, 3);
}

/*
    Metoda koja sluzi za rotaciju retka za jedno mjesto 
    ulijevo
*/
void AES::rotateRow(byte **in, int r) {
  byte first = (*in)[ r ];

  for(int i = 0 ; i < 3; i++)
    (*in)[ r + i * 4 ] = (*in)[r +(i+1) * 4];

  (*in)[ r + 3 * 4 ] = first; 
}


/*
    Metoda koja sluzi za mjesanje stupaca
*/
void AES::mixColumns(byte** input) {
  for(int i = 0; i < 4; i++) {
    byte *temp = new byte[ 4 ];
    byte *temp1 = new byte[ 4 ];
    memcpy(temp, (*input) + i *4, 4 * sizeof(byte));
    computeColumn(&temp);
    memcpy((*input) + i * 4, temp, 4 * sizeof(byte));
    delete[] temp;
  }
}

/*
    Metoda koja obavlja potrebno mnozenje za metodu
    mixColumns koja radi mjesanje stupaca
*/
void AES::computeColumn(byte** s) {
  byte *temp = new byte[ 4 ];
  memcpy(temp, *s, 4 * sizeof(byte));

  (*s)[ 0 ] = mulGF(0x02, temp[0]) ^ mulGF(0x03, temp[1]) ^ temp[2] ^ temp[3];
  (*s)[ 1 ] = temp[0] ^ mulGF(0x02, temp[1]) ^ mulGF(0x03, temp[2]) ^ temp[3];
  (*s)[ 2 ] = temp[0] ^ temp[1] ^ mulGF(0x02, temp[2]) ^ mulGF(0x03, temp[3]);
  (*s)[ 3 ] = mulGF(0x03, temp[0]) ^ temp[1] ^ temp[2] ^ mulGF(0x02, temp[3]);

  delete[] temp;
}

/*
  Inverzna zamjena bajtova
*/
void AES::invSubBytes(byte **input) {
  for(int i = 0; i < AES_BLOCK; i++) {
    (*input)[ i ] = invSbox[ (*input)[ i ] ];
  }
}

/*
    Inverzna zamjena stupaca
*/
void AES::invShiftRows(byte **in) {
  invRotRow(in, 1); // drugi red
  invRotRow(in, 2); // treci red
  invRotRow(in, 2); //
  invRotRow(in, 3); // cetvrti red
  invRotRow(in, 3);
  invRotRow(in, 3);
}

/*
    Metoda koja rotira redak u desno
*/
void AES::invRotRow(byte **in, int r) {
  byte first = (*in)[ r + 3*4 ];

  for(int i = 3 ; i > 0; i--)
    (*in)[ r + i * 4 ] = (*in)[r + (i-1) * 4];

  (*in)[ r ] = first; 
}

/*
    Inverzno mijesanje stupaca
*/
void AES::invMixColumns(byte **input) {
  for(int i = 0; i < 4; i++) {
    byte *temp = new byte[ 4 ];
    byte *temp1 = new byte[ 4 ];
    memcpy(temp, (*input) + i *4, 4 * sizeof(byte));
    computeInvColumn(&temp);
    memcpy((*input) + i * 4, temp, 4 * sizeof(byte));
    delete[] temp;
  }
}

/*  
    Metoda koja 
    racuna nove vrijednosti elemenata stupaca
*/
void AES::computeInvColumn(byte **s) {
  byte *temp = new byte[ 4 ];
  memcpy(temp, *s, 4 * sizeof(byte));

  // samo zapisana matrica
  (*s)[ 0 ] = mulGF(0x0e, temp[0]) ^ mulGF(0x0b, temp[1]) ^ mulGF(0x0d, temp[2]) ^ mulGF(0x09, temp[3]);
  (*s)[ 1 ] = mulGF(0x09, temp[0]) ^ mulGF(0x0e, temp[1]) ^ mulGF(0x0b, temp[2]) ^ mulGF(0x0d, temp[3]);
  (*s)[ 2 ] = mulGF(0x0d, temp[0]) ^ mulGF(0x09, temp[1]) ^ mulGF(0x0e, temp[2]) ^ mulGF(0x0b, temp[3]);
  (*s)[ 3 ] = mulGF(0x0b, temp[0]) ^ mulGF(0x0d, temp[1]) ^ mulGF(0x09, temp[2]) ^ mulGF(0x0e, temp[3]);

  delete[] temp;
}

/*
    Mnozenje u Galoisovom polju GF(2^8)
*/
byte AES::mulGF(byte a, byte b) {
   byte mul = 0, h; // rezultat i da li imamo bit najvece tezine
   for (int i = 0; i < 8; i++) {
      if (b & 1) mul ^= a; 
      h = a & 0x80;
      a <<= 1; // pomnozi s 2 
      if (h) a ^= 0x1b; // ako je preljev xor 0x11b
      b >>= 1; // sljedeci bit vece tezine 
   }
   return mul;
}
