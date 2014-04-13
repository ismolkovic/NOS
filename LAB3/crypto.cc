#include "crypto.h"
#include "base64table.cc"

/*
    Metoda koja parsira ulaz
    MODE 0-ECB, 1-CTR
    0-kritpiraje, 1-dekriptiranje
*/
void parse(int mode, int enc, char *inFile, char *keyFile, char *outFile) {
  byte *input = NULL, *key = NULL, *iv = NULL;
  long iSize, kSize, ivSize;

  // ako kriptiramo onda imamo samo podatke za kriptiranje zapisane
  if( enc == 0 ) {
    iSize = readData(inFile, &input);
  }

  //inace treba skuziti di su podaci
  else {
    ifstream ifs(inFile, ios::binary);
    string inData((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close(); // procitamo datoteku


    iSize = getSubData(inData, "Data:", "---END", &input);
  }

  // citanje kljuca
  ifstream kfs(keyFile, ios::in);
  string keyData((std::istreambuf_iterator<char>(kfs)), std::istreambuf_iterator<char>());
  kfs.close(); // procitamo datoteku

  if (keyData.length() == 0) return;

  if( mode == 0)
    kSize = getKeyIV(keyData, "Secret key:", "---END", &key); // procitamo kljuc
  else {

    ivSize = getKeyIV(keyData, "Initialization vector:", "Secret key:", &iv);
    kSize = getKeyIV(keyData, "Secret key:", "---END", &key); // procitamo kljuc
  }

  //cout << kSize << " " << ivSize << endl;

  operation(input, key, iv, mode, enc, iSize, kSize, outFile, inFile);
  if(input != NULL) delete[] input;
  if(key   != NULL) delete[] key;
  if(iv    != NULL) delete[] iv;
}

/*
    Metoda koja cita binarne podatke iz datoteke
*/
long readData(char* fileName, byte** output) {
  FILE *file = fopen(fileName, "rb");
  long fSize;

  fseek(file, 0, SEEK_END);
  fSize = ftell(file);
  rewind(file);

  *output = new byte[ fSize ];
  fread(*output , sizeof(byte), fSize, file);
  fclose(file);
  return fSize;
}

/*
    Metoda koja uzima kriptirane podatke u base64 formatu iz datoteke
*/
long getSubData(string input, string start, string end, byte **output) {
  long ret;
  int sPos = input.find(start) + start.length();
  int ePos  = input.find(end);
  vector< byte > tmpData;

  for(int i = sPos; i < ePos; i++) {
    if(input[i] != '\n' && input[i] != '\t' && input[i] != ' ')
      tmpData.push_back((byte)input[i]);
  }
  ret = tmpData.size();

  *output = new byte[ ret ];
  copy(tmpData.begin(), tmpData.end(), *output);
  return ret;
}

/*
    Metoda koja cita inicijalizacijski vektor ili kljuc iz datoteke
*/
long getKeyIV(string input, string start, string end, byte **output) {
  int sPos = input.find(start) + start.length();
  int ePos  = input.find(end);
  string tmpString = "";

  for(int i = sPos; i < ePos; i++) {
    if(input[i] != '\n' && input[i] != '\t' && input[i] != ' ')
      tmpString += input[ i ];
  }
  
  *output = new byte[ tmpString.length() /2 ];
  
  for(int i = 0; i < tmpString.length(); i += 2) {
    string tmp = tmpString.substr(i,2);
    (*output)[i/2] = strtol(tmp.data(), NULL , 16);
  }

  return tmpString.length() / 2;
}

/*  
    Metoda koja poziva operaciju kriptiranja ili dekriptiranja 
    za određeni mod, bilo ECB, bilo CTR
*/
void operation(byte *in, byte *key, byte *iv, int mode, int encDec, long iSize, long kSize, char *outFile, char* inFile) {
  long oSize;//, oSize1;
  byte *output = NULL;

  if(mode == 0 ) { // radimo ECB
    if( encDec == 0) { // kriptiramo
        byte *tmpEncrypt;
        oSize = ECB_ENCRYPT(in, iSize, key, kSize, &tmpEncrypt);

        oSize =  base64encode(tmpEncrypt, &output, oSize);
        if(tmpEncrypt != NULL)  delete[] tmpEncrypt;
    } 

    else { // inace ecb dekriptiranje
        byte *tmpDecrypt = NULL;
        oSize = base64decode(in, &tmpDecrypt, iSize);

        oSize = ECB_DECRYPT(tmpDecrypt, oSize, key, kSize, &output);
        if(tmpDecrypt != NULL) delete[] tmpDecrypt;
    }
  }


  if( mode == 1 ) { // radimo ctr
    if( encDec == 0) {
      byte *tmpEncrypt = NULL;
      oSize = CTR_ENCRYPT(in, iSize, key, kSize, iv, &tmpEncrypt);


      oSize =  base64encode(tmpEncrypt, &output, oSize);
      if(tmpEncrypt != NULL) delete[] tmpEncrypt;
    }
    else { // ctr dekriptiranje
      byte *tmpDecrypt;
      oSize = base64decode(in, &tmpDecrypt, iSize);  

      if(oSize == 0 || tmpDecrypt == NULL) return;

      oSize = CTR_DECRYPT(tmpDecrypt, oSize, key, kSize, iv, &output);
      if(tmpDecrypt != NULL) delete[] tmpDecrypt;
    }
  }

  
  if (encDec == 0) 
    writeCryptoFile(output, oSize, outFile, kSize, inFile);
  else
    writePlainFile(output, oSize, outFile);

  if(output != NULL) delete[] output; 
}

/*
    Metoda koja obavlja base64 kodiranje
    @param  in - ulazni bajtovi
    @param  out - odrediste
    @param iSize - velicina ulaznih podataka
*/
long base64encode(byte *in, byte **out, long iSize) {
  long newSize;

  if(iSize % 3 == 0) newSize = iSize/3 * 4;
  else newSize = ((iSize/3)+1) * 4;
  
  int k = 0;
  *out = new byte[ newSize ];

  for(int i = 0; i < (iSize/3) * 3; i += 3 ) {
    unsigned long int w;
    w = in[ i ]; w <<= 8;
    w = w | in[ i + 1 ] ; w <<= 8;
    w = w | in[ i + 2 ];
    byte d = w & 0x3f; w >>= 6;
    byte c = w & 0x3f; w >>= 6;
    byte b = w & 0x3f; w >>= 6;
    byte a = w & 0x3f;

    (*out)[ k++ ] = table[ a ];
    (*out)[ k++ ] = table[ b ];
    (*out)[ k++ ] = table[ c ];
    (*out)[ k++ ] = table[ d ];
  }

  if(iSize % 3 == 1){
    unsigned long int w; 
    w = in[ iSize - 1]; w <<= 4;
    byte b = w & 0x3f; w >>= 6;
    byte a = w & 0x3f;

    (*out)[ newSize - 1 ] = '=';
    (*out)[ newSize - 2 ] = '=';
    (*out)[ newSize - 3 ] = table[ b ];
    (*out)[ newSize - 4 ] = table[ a ];
  }

  if(iSize % 3 == 2) {
    unsigned long int w; 
    w = in[ iSize - 2]; w <<= 8;
    w = w | in[ iSize - 1]; w <<= 2;

    byte c = w & 0x3f; w >>= 6;
    byte b = w & 0x3f; w >>= 6;
    byte a = w & 0x3f; 

    (*out)[ newSize-1 ] = '=';
    (*out)[ newSize-2 ] = table[ c ];
    (*out)[ newSize-3 ] = table[ b ];
    (*out)[ newSize-4 ] = table[ a ];

  }
  return newSize; 
}

/*  
    Metoda koja vrsi base64 dekodiranje
*/
long base64decode(byte *in, byte **out, long iSize) {
  long newSize = iSize * 3 / 4;
  int len, itSize; 
  int n = 0;

  if(in[iSize-1] == '=' && in[iSize-2] == '=') len = 2;
  if(in[iSize-1] == '=' && in[iSize-2] != '=') len = 1;
  if(in[iSize-1] != '=' && in[iSize-2] != '=') len = 0;

  newSize = newSize - len;
  itSize = iSize - len;
  *out = new byte[ newSize ];

  for(int i = 0; i < itSize; i += 4) {
    unsigned long int w;
    w = (byte)invTable[ in[ i ] ]; w &= 0x3f; w <<= 6;
    byte tmp = (byte)invTable[ in[ i+1 ] ]; tmp &= 0x3f; 
    w |= tmp; w <<= 6; 
    tmp = (byte)invTable[ in[ i + 2 ]]; tmp &= 0x3f;
    w |= tmp; w <<= 6;
    tmp = (byte)invTable[ in[i + 3]]; tmp &= 0x3f;
    w |= tmp;

    byte c = w & 0xff; w >>= 8;
    byte b = w & 0xff; w >>= 8;
    byte a = w & 0xff;

    (*out)[n++] = a;
    (*out)[n++] = b;
    (*out)[n++] = c;
  }

  if(len == 1) { // imamo samo jedan jednako
    unsigned long int w;
    w = (byte)invTable[ in[iSize-4] ]; w &= 0x3f; w <<= 6;
    byte tmp = (byte)invTable[ in[iSize-3] ]; tmp &= 0x3f;
    w |= tmp; 
    tmp = (byte)invTable[ in[iSize-2] ]; tmp &= 0x3f; tmp >>=2; // izbcimo najniza 2
    w <<= 4 ;w |= tmp;

    // sada imamo zapisana 2 byta
    (*out)[n++] = w & 0xff;  w >>= 8;
    (*out)[n] = w & 0xff;
  }

  if(len == 2) { // imamo 2 jednako na kraju, dobivamo jos 1 bajt
    byte last = (byte)invTable[ in[iSize-4 ] ]; last <<= 2;
    byte tmp  = (byte)invTable[ in[iSize-3 ] ]; tmp &= 0xc0; tmp >>= 6;
    (*out)[n] = last | tmp;
  }

  return newSize;
}

/*
    Funkcija koja generira AES ključeve i inicijalizacijski 
    vektor ako se radi CTR mode
*/
void generateKey(int type, char *fileName, long size) {
  srand ((unsigned)time(NULL)); // za generator
  byte *key = NULL, *iv = NULL; 
  if(type == ECB_KEY){
    generateRandomByte(&key, size);
  }
  
  if(type == CTR_KEY) {
    generateRandomByte(&key, size);
    generateRandomByte(&iv, AES_BLOCK);
  }

  // zapisemo sadržaj ključa u datoteku
  writeKeyFile(key, size, iv, AES_BLOCK, fileName);
  if(key != NULL) delete[] key;
  if(iv  != NULL) delete[] iv;
}

void generateRandomByte(byte** output, long size) {
  *output = new byte[ size ];

  for(int i = 0; i < size; i++) {
    (*output)[ i ] = (byte) rand() % BYTE_MAX;
  }
}
//
// Ključne riječi u datotekama
  
  char header[] = "---BEGIN OS2 CRYPTO DATA---";
  char footer[] = "---END OS2 CRYPTO DATA---";

//

/*
    Funkcija koja zapisuje sadržaj kriptirane datoteke
    @param in - bajtovi koji se zapisuju
    @param size - velicina tih bajtova
    @param fileName - naziv datoteke
    @param kSize - duljina kljuca
    @param inFile - putanja do ulazne datoteke/ uzme se samo basename
*/
void writeCryptoFile(byte *in, long size, char *fileName, long kSize, char *inFile) {
  FILE *f = fopen(fileName, "wb");

  string tmp = inFile;
  int pos = tmp.find_last_of('/');

  fprintf(f, "%s\n", header);

  fprintf(f, "Description:\n\tCrypted file\n\n");

  fprintf(f, "Method:\n\tAES\n\n");
  

  if(kSize == 32)
    fprintf(f, "Key length:\n\t0%0X\n\n", (unsigned int)kSize * 8);

  else
    fprintf(f, "Key length:\n\t%X\n\n", (unsigned int)kSize * 8);

  fprintf(f, "File name:\n\t%s\n\n", inFile + pos + 1);

  fprintf(f, "Data:");
  
  write60chars(in, size, f, 0);

  fprintf(f, "\n\n%s", footer);
  fclose(f);
}

/*
    Metoda koja zapisuje sadržaj dekriptirane datoteke
*/
void writePlainFile(byte *input, long size, char *fileName) {
  FILE *f = fopen(fileName , "wb");

  for(int i = 0; i < size; i++) {
    fprintf(f, "%c", input[ i ]);
  }

  fclose(f); 
}

/*  
    Funkcija koja zapisuje sadržaj datoteke u koju se zapisuje ključ
*/
void writeKeyFile(byte *key, long kSize, byte* iv, long ivSize, char *outFile) {  
  FILE *f = fopen(outFile, "wb");

  fprintf(f, "%s\n", header);
  fprintf(f,"Description:\n\tSecret key\n\n");
  fprintf(f,"Method:\n\tAES\n\n" );

   if(kSize == 32)
    fprintf(f, "Key length:\n\t0%0X\n\n", (unsigned int)kSize * 8);

  else
    fprintf(f, "Key length:\n\t00%0X\n\n", (unsigned int)kSize * 8);


  if(iv != NULL) {
    fprintf(f, "Initialization vector:");
    write60chars(iv, ivSize, f, 1);
    fprintf(f, "\n\n");
  }

  fprintf(f, "Secret key:");
  write60chars(key, kSize, f, 1);

  fprintf(f, "\n\n%s\n", footer);
  fclose(f);
}

/*
    Funkcija formatirano zapisuje u svaki red 60 znakova
    @param mode - oznacava da li pisemo hex ili pisemo char
*/
void write60chars(byte *data, long size, FILE *f, int mode) {
  int n = 0;
  for(int i = 0; i < size; i++) {
    if(n % 60 == 0) {
      fprintf(f, "\n\t" );
      n = 0;
    }
    if (mode == 0) {
      fprintf(f, "%c", data[ i ] );
      n++;
    }
    if (mode == 1) {
      fprintf(f, "%02x", data[i]);
      n += 2;
    }
  }
}
