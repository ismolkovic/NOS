
#include <iostream>
#include <cstdlib>
using namespace std;

#include "crypto.h"

char programName[4] = "AES";

int main(int argc, const char **argv)
{
  if( argc != 4 && argc !=6 ) {
    printf("Pokretanje:\n");
    printf("MODE 0-ECB, 1-CTR\n");
    printf("0-kritpiraje, 1-dekriptiranje\n");
    printf("\t./%s <inputDat> <kljucDat> <mode> <enc/dec> <izlazDat>\n", programName);
    printf("\nZa generiranje ključeva:\n");
    printf("TYPE 0-ECB, 1-CTR (generira se i IV) \n");
    printf("Duljina kljuca 128, 192 ili 256\n");
    printf("\t./AES <type> <duljina> <kljucDat>\n");

    exit(0);
  }

  // inace se obavlja generiranje kljuceva
  if (argc == 4) {
    char kFile[ 1024 ];
    int ecb_ctr_key = atoi(argv[ 1 ]);

    if(ecb_ctr_key != 0 && ecb_ctr_key != 1) return 0;

    long size = atol(argv[ 2 ]);

    sscanf(argv[ 3 ], "%s", kFile);

    if( size == 128 || size == 192 || size == 256)
      generateKey(ecb_ctr_key, kFile, size/8);
    else {
      printf("Nije unesena ispravna duljina kljuca...\n");
      exit(1);
    }
  }

  // obavlja se kriptiranje ili dekriptiranje teksta
  if( argc == 6) {
    int mode, encDec;
    char iFile[ 1024 ], kFile[ 1024 ], oFile[ 1024 ];

    encDec = atoi(argv[4]); 
    mode = atoi(argv[ 3 ]); 
    sscanf(argv[2], "%s", kFile);
    sscanf(argv[1], "%s", iFile);
    sscanf(argv[5], "%s", oFile);

    parse(mode, encDec, iFile, kFile, oFile);
  }

  return 0;
}