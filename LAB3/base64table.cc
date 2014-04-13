 
#ifndef BASE_TABLES
#define BASE_TABLES

#define INV (-1)
#define PAD (-2)


//za base64 kodiranje
const char* table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// inverzna tablica za base64 kriptiranje
static signed char invTable[256] = {
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV, 62,    INV,INV,INV, 63,
     52, 53, 54, 55,     56, 57, 58, 59,
     60, 61,INV,INV,    INV,PAD,INV,INV,
    INV,  0,  1,  2,      3,  4,  5,  6,
      7,  8,  9, 10,     11, 12, 13, 14,
     15, 16, 17, 18,     19, 20, 21, 22,
     23, 24, 25,INV,    INV,INV,INV,INV,
    INV, 26, 27, 28,     29, 30, 31, 32,
     33, 34, 35, 36,     37, 38, 39, 40,
     41, 42, 43, 44,     45, 46, 47, 48,
     49, 50, 51,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
    INV,INV,INV,INV,    INV,INV,INV,INV,
};


#endif